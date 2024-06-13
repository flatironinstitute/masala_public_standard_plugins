/*
    Masala
    Copyright (C) 2022 Vikram K. Mulligan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/// @file src/optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizer.cc
/// @brief Implementation for a CostFunctionNetworkOptimizer that refines a cost function network
/// optimization problem by greedy descent.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/annealing/AnnealingScheduleBase_API.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/threads/MasalaThreadedWorkRequest.hh>
#include <base/managers/threads/MasalaThreadedWorkExecutionSummary.hh>
#include <base/managers/random/MasalaRandomNumberGenerator.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <iostream>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
GreedyCostFunctionNetworkOptimizer::GreedyCostFunctionNetworkOptimizer(
    GreedyCostFunctionNetworkOptimizer const & src
) :
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer( src )
{
    std::lock_guard< std::mutex > lock( src.optimizer_mutex_ );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
GreedyCostFunctionNetworkOptimizer &
GreedyCostFunctionNetworkOptimizer::operator=( GreedyCostFunctionNetworkOptimizer const & src ) {
    std::lock( optimizer_mutex_, src.optimizer_mutex_ );
    std::lock_guard< std::mutex > lock1( optimizer_mutex_, std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.optimizer_mutex_, std::adopt_lock );
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::operator=( src );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
    return *this;
}

/// @brief Make a copy of this object that's wholly independent.
GreedyCostFunctionNetworkOptimizerSP
GreedyCostFunctionNetworkOptimizer::deep_clone() const {
    GreedyCostFunctionNetworkOptimizerSP new_optimizer( masala::make_shared< GreedyCostFunctionNetworkOptimizer >(*this) );
    new_optimizer->make_independent();
    return new_optimizer;
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
GreedyCostFunctionNetworkOptimizer::make_independent() {
	// GNDN
}


////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outher vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
GreedyCostFunctionNetworkOptimizer::get_categories() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return CostFunctionNetworkOptimizer::get_categories();
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "simulated_annealing", "stochastic" }
std::vector< std::string >
GreedyCostFunctionNetworkOptimizer::get_keywords() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	std::vector< std::string > keywords( CostFunctionNetworkOptimizer::get_keywords() );
    keywords.push_back( "greedy" );
    keywords.push_back( "deterministic" );
    return keywords;
}

/// @brief Get the category that this MasalaEngine fits into.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }.
std::vector< std::vector< std::string > >
GreedyCostFunctionNetworkOptimizer::get_engine_categories() const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return CostFunctionNetworkOptimizer::get_engine_categories();
}

/// @brief Get the class name.
/// @returns "GreedyCostFunctionNetworkOptimizer".
std::string
GreedyCostFunctionNetworkOptimizer::class_name() const {
    return class_name_static();
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
std::string
GreedyCostFunctionNetworkOptimizer::class_namespace() const {
    return class_namespace_static();
}

/// @brief Get the class name (static version).
/// @returns "GreedyCostFunctionNetworkOptimizer".
//static
std::string
GreedyCostFunctionNetworkOptimizer::class_name_static() {
    return "GreedyCostFunctionNetworkOptimizer";
}

/// @brief Get the class namespace (static version).
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
//static
std::string
GreedyCostFunctionNetworkOptimizer::class_namespace_static() {
    return "standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
GreedyCostFunctionNetworkOptimizer::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using namespace masala::base::api::getter;
    using namespace masala::base::api::work_function;
    using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    if( api_description_ == nullptr ) {
        MasalaObjectAPIDefinitionSP api_description(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this,
                "An optimizer for cost function network problems that uses a Monte Carlo search to optimize.  This "
                "performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to "
                "pick a node at random and change its setting at random, compute the change in overall energy or score, "
                "and accept or reject the move based on the difference in energy and the Metropolis criterion.",
                false, false
            )
        );

        // Constructors:
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( GreedyCostFunctionNetworkOptimizer, api_description );

        // Setters:
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_cpu_threads_to_request", "Sets the number of threads to request when running problems in parallel.",
				"threads_in", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_cpu_threads_to_request, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"cpu_threads_to_request", "Gets the number of threads to request when running problems in parallel.",
				"cpu_threads_to_request", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::cpu_threads_to_request, this )
			)
		);

        // Work functions:
        api_description->add_work_function(
            masala::make_shared<
                MasalaObjectAPIWorkFunctionDefinition_OneInput<
                    std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >,
                    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const &
                >
            >(
                "run_cost_function_network_optimizer", "Run the optimizer on a cost function network optimization problem, and produce a solution.",
                true, false, true, false,
                "problems", "A set of problems to run.",
                "solutions", "A vector of solution sets.  Each CostFunctionNetworkOptimizationSolutions object contains the set of solutions for the problem "
                "in the input vector with the corresponding index.  There may be multiple solutions, depending on settings.",
                std::bind( &GreedyCostFunctionNetworkOptimizer::run_cost_function_network_optimizer, this, std::placeholders::_1 )
            )
        );

        // Convert nonconst to const:
        api_description_ = api_description;
    }
    return api_description_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the number of threads to request.
/// @details The default setting of 0 means "request all available".
void
GreedyCostFunctionNetworkOptimizer::set_cpu_threads_to_request(
    masala::base::Size const threads_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    cpu_threads_to_request_ = threads_in;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the number of threads to request.
/// @details The default setting of 0 means "request all available".
masala::base::Size
GreedyCostFunctionNetworkOptimizer::cpu_threads_to_request() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return cpu_threads_to_request_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
/// the problem with the same index.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
GreedyCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
) const {
    using namespace masala::base::managers::threads;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );

	TODO TODO TODO:
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
