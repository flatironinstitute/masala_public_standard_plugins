/*
    Standard Masala Plugins
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

// Optimizers headers:
#include <optimizers/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>

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
#include <base/managers/engine/data_representation_request/MasalaDataRepresentationNameRequirementCriterion.hh>
#include <base/managers/engine/MasalaDataRepresentationRequest.hh>
#include <base/managers/engine/MasalaDataRepresentationManager.hh>
#include <base/managers/engine/MasalaDataRepresentationCreator.hh>
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
	masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer( src ) // Calls protected_assign(), but only for the base class, since this is a constructor.
{
	std::lock( optimizer_mutex_, src.optimizer_mutex_ );
	std::lock_guard< std::mutex > lock( optimizer_mutex_, std::adopt_lock );
	std::lock_guard< std::mutex > lock2( src.optimizer_mutex_, std::adopt_lock );
	GreedyCostFunctionNetworkOptimizer::protected_assign(src); // Repeats call to parent class protected_assign(), but that's okay.  Needed since virtual function calls aren't possible in constructors.
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
GreedyCostFunctionNetworkOptimizer &
GreedyCostFunctionNetworkOptimizer::operator=( GreedyCostFunctionNetworkOptimizer const & src ) {
	masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::operator=( src );  // Calls protected_assign().
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
/// @returns { "optimizer", "cost_function_network", "numeric", "greedy", "deterministic" }
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

/// @brief Get the keywords that this MasalaEngine has.
/// @returns { "optimizer", "cost_function_network", "numeric", "greedy", "deterministic" }
std::vector< std::string >
GreedyCostFunctionNetworkOptimizer::get_engine_keywords() const {
	return get_keywords();
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
                "A deterministic optimizer for cost function network problems that uses a greedy algorithm to find the "
				"nearest local solution to a starting state.  If starting states are provided in the CFN problem definition, "
				"these are used; otherwise a user-specified nunmber of random starting states is used.  Each starting state "
				"can be optimized in parallel.  The optimization algorithm is to make movies that conist of conisdering "
				"every possible point mutation and accepting the one that reduces the energy the most.  This is repeated until "
				"the energy does not improve any further.",
                false, false
            )
        );

        // Constructors:
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( GreedyCostFunctionNetworkOptimizer, api_description );

        // Setters:
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::managers::engine::MasalaDataRepresentationAPICSP const & > >(
				"set_template_preferred_cfn_data_representation", "Set a template cost function network optimization problem data representation, "
				"configured by the user but with no data entered.  This can optionally be passed in, in which case the get_template_preferred_cfn_data_representation() "
				"function can be used to retrieve a deep clone.  This allows the solver to cache its preferred data representation with its setup.",
				"representation_in", "A fully configured but otherwise empty data representation object, to be cached.  Deep clones will be retrievable with the "
				"get_template_preferred_cfn_data_representation() function when calling code wants to start populating a data representation with data.",
				true, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_template_preferred_cfn_data_representation, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_cpu_threads_to_request", "Sets the number of threads to request when running problems in parallel.",
				"threads_in", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_cpu_threads_to_request, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_n_random_starting_states", "If starting states are not provided in the problem definition or in the "
				"optimizer configuration, indicate the number of random starting states to use.  Defaults to 1.",
				"n_random_starting_states_in", "The number of random starting states to use.  This number of greedy "
				"descent trajectories will be carried out for all problems that do not provide starting states.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_n_random_starting_states, this, std::placeholders::_1 )
			)
		);


		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::vector< std::vector< masala::base::Size > > const & > > (
				"set_optimizer_starting_states", "Set the starting points to use, as a vector of vectors of choice-by-node.  "
				"These replace any already stored.  Note that by setting the starting points in the optimizer rather than in the problem, "
				"an error will be thrown at apply time if the number of nodes or choices doesn't match the problem to which the optimizer "
				"is applied.",
				"starting_states_in", "The starting points to use, as a vector of vectors of choice-by-node.  These replace any already stored.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_optimizer_starting_states, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::vector< std::vector< masala::base::Size > > const & > > (
				"add_optimizer_starting_states", "Add starting points to use, as a vector of vectors of choice-by-node.  These are "
				"appended to any already stored.  Note that by setting the starting points in the optimizer rather than in the problem, "
				"an error will be thrown at apply time if the number of nodes or choices doesn't match the problem to which the optimizer "
				"is applied.",
				"additional_starting_states", "The additional starting points to use, as a vector of vectors of choice-by-node.  These will "
				"be appended to any already stored.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::add_optimizer_starting_states, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::vector< masala::base::Size > const & > > (
				"add_optimizer_starting_state", "Add a starting point to use, as a vector of choice-by-node.  This is appended "
				"to any already stored.  Note that by setting the starting points in the optimizer rather than in the problem, "
				"an error will be thrown at apply time if the number of nodes or choices doesn't match the problem to which the "
				"optimizer is applied.",
				"additional_starting_state", "The additional starting point to use, as a vector of choice-by-node.  This will "
				"be appended to any already stored.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::add_optimizer_starting_state, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput > (
				"clear_optimizer_starting_states", "Clear the starting points to use.  By setting the starting points in the "
				"optimizer rather than in the problem, an error will be thrown at apply time if the number of nodes or choices "
				"doesn't match the problem to which the optimizer is applied.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::clear_optimizer_starting_states, this )
			)
		);

		// Getters:
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::base::managers::engine::MasalaDataRepresentationAPISP > >(
				"get_template_preferred_cfn_data_representation_copy", "Get a template cost function network optimization problem data representation, configured "
				"by the user but with no data entered.  If no template CFN problem has been passed in by calling set_template_preferred_cfn_data_representation(), this "
				"function returns a default, empty PairwisePrecomputedCostFunctionNetworkOptimizationProblem.  Otherwise, it deep-clones the object that was passed in.",
				"template_preferred_cfn_data_representation_copy", "A deep clone of the configured but empty CFN problem representation that was passed in to "
				"set_template_preferred_cfn_data_representation(), or a default, empty PairwisePrecomputedCostFunctionNetworkOptimizationProblem if no template was provided.",
				true, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::get_template_preferred_cfn_data_representation_copy, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"cpu_threads_to_request", "Gets the number of threads to request when running problems in parallel.",
				"cpu_threads_to_request", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::cpu_threads_to_request, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"n_random_starting_states", "If starting states are not provided in the problem definition or in the optimizer "
				"configuration, get the number of random starting states to use.  Defaults to 1.",
				"n_random_starting_states", "The number of random starting states to use.  This number of greedy "
				"descent trajectories will be carried out for all problems that do not provide starting states.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::n_random_starting_states, this )
			)
		);
		// n_times_seen_multiplier() deliberately omitted from public interface.
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< std::vector< std::vector< masala::base::Size > > const & > > (
				"optimizer_starting_states", "Access the list of starting states that the optimizer has been configured to try.  These "
				"are provided by the user during optimizer configuration rather than by the problem.  At optimization time, these will "
				"result in a throw if the size of the state vector doesn't match the number of nodes or choices in the problem.",
				"optimizer_starting_states", "Starting points to use, provided by the user during optimizer configuration rather than "
				"by the problem.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::optimizer_starting_states, this )
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
                "run_cost_function_network_optimizer", "Run the optimizer on a set of cost function network optimization problems, and produce a vector of solutions.",
                true, false, true, false,
                "problems", "A set of problems to run.",
                "solutions", "A vector of solution sets.  Each CostFunctionNetworkOptimizationSolutions object contains the set of solutions for the problem "
                "in the input vector with the corresponding index.  There may be multiple solutions, depending on settings.",
                std::bind( &GreedyCostFunctionNetworkOptimizer::run_cost_function_network_optimizer, this, std::placeholders::_1 )
            )
        );
		api_description->add_work_function(
			masala::make_shared<
				MasalaObjectAPIWorkFunctionDefinition_OneInput<
					masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP,
					masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP const &
				>
			>(
				"run_cost_function_network_optimizer_on_one_problem", "Run the optimizer on a single cost function network optimization problem, and produce a "
				"set of one or more solutions.",
				true, false, false, false,
				"problem", "A single cost function network optimization problem to run.",
				"solutions", "A solution set.  The CostFunctionNetworkOptimizationSolutions object contains the set of solutions for the input problem.  "
				"There may be multiple solutions, depending on settings.",
				std::bind( &GreedyCostFunctionNetworkOptimizer::run_cost_function_network_optimizer_on_one_problem, this, std::placeholders::_1 )
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

/// @brief If starting states are not provided in the problem definition, indicate
/// the number of random starting states to use.  Defaults to 1.
void
GreedyCostFunctionNetworkOptimizer::set_n_random_starting_states(
	masala::base::Size const setting
) {
	CHECK_OR_THROW_FOR_CLASS( setting >= 1, "set_n_random_starting_states",
		"The number of random starting states must be at least 1.  Got " + std::to_string( setting ) + "."
	);
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	n_random_starting_states_ = setting;
}

/// @brief By default, a single greedy optimization trajectory results in a solution being seen
/// once.  This option lets other code specify that it should be higher.  Useful when other code,
/// like the MonteCaroCostFunctionNetworkOptimizer, wants to use this for greedy refinement of
/// solutions that it has produced many times.
/// @note NOT part of the public-facing interface.  Intended only for use by code within this
/// plugin sub-library.
void
GreedyCostFunctionNetworkOptimizer::set_n_times_seen_multiplier(
	masala::base::Size const setting
) {
	CHECK_OR_THROW_FOR_CLASS( setting > 0, "set_n_times_seen_multiplier", "The multiplier must be "
		"greater than or equal to 1."
	);
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	n_times_seen_multiplier_ = setting;
}

/// @brief Set the starting points to use, as a vector of vectors of choice-by-node.  These replace any already stored.
/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
void
GreedyCostFunctionNetworkOptimizer::set_optimizer_starting_states(
	std::vector< std::vector< masala::base::Size > > const & starting_states_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	optimizer_starting_states_ = starting_states_in;
}

/// @brief Add starting points to use, as a vector of vectors of choice-by-node.  These are appended to any already stored.
/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
void
GreedyCostFunctionNetworkOptimizer::add_optimizer_starting_states(
	std::vector< std::vector< masala::base::Size > > const & starting_states_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	optimizer_starting_states_.reserve( optimizer_starting_states_.size() + starting_states_in.size() );
	for( auto const & state : starting_states_in ) {
		optimizer_starting_states_.push_back( state );
	}
}

/// @brief Add a starting point to use, as a vector of choice-by-node.  This is appended to any already stored.
/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
void
GreedyCostFunctionNetworkOptimizer::add_optimizer_starting_state(
	std::vector< masala::base::Size > const & starting_state_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	optimizer_starting_states_.push_back( starting_state_in );
}

/// @brief Clear the starting points to use.
/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
void
GreedyCostFunctionNetworkOptimizer::clear_optimizer_starting_states() {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	optimizer_starting_states_.clear();
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

/// @brief If starting states are not provided in the problem definition, get
/// the number of random starting states to use.  Defaults to 1.
masala::base::Size
GreedyCostFunctionNetworkOptimizer::n_random_starting_states() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	return n_random_starting_states_;
}

/// @brief By default, a single greedy optimization trajectory results in a solution being seen
/// once.  This option lets other code specify that it should be higher.  Useful when other code,
/// like the MonteCaroCostFunctionNetworkOptimizer, wants to use this for greedy refinement of
/// solutions that it has produced many times.
/// @note NOT part of the public-facing interface.  Intended only for use by code within this
/// plugin sub-library.
masala::base::Size
GreedyCostFunctionNetworkOptimizer::n_times_seen_multiplier() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	return n_times_seen_multiplier_;
}

/// @brief Access the list of starting states that the optimizer has been configured to try.
std::vector< std::vector< masala::base::Size > > const &
GreedyCostFunctionNetworkOptimizer::optimizer_starting_states() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	return optimizer_starting_states_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a vector of cost function network optimization problems, and produce a vector of solutions.
/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
/// the problem with the same index.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
GreedyCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
) const {
    using namespace masala::base::managers::threads;
	using namespace masala::base::managers::random;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );

	// Get the random number generator:
	MasalaRandomNumberGeneratorHandle rg( MasalaRandomNumberGenerator::get_instance() );

	// Create the multithreaded work request:
	MasalaThreadedWorkRequest work_request;
	if( cpu_threads_to_request_ > 0 ) {
		work_request.set_n_threads_to_request( cpu_threads_to_request_ );
	} else {
		work_request.set_request_all_threads();
	}

	// Starting states: outer vector is by problem, middle vector is a bunch of starting states for the
	// nth problem, inner vector is a single starting state (one choice index per variable node).
	std::vector< std::vector< std::vector< Size > > > starting_states_by_problem;

	// Storage for solutions:
	std::vector< CostFunctionNetworkOptimizationSolutions_APISP > solutions_containers_by_problem;
	solutions_containers_by_problem.reserve( problems.n_problems() );

	for( Size iproblem(0); iproblem < problems.n_problems(); ++iproblem ) {
		// Check the problem:
		CostFunctionNetworkOptimizationProblem_APICSP problem( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >( problems.problem(iproblem) ) );
		CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "run_cost_function_network_optimizer", "The "
			+ class_name() + " optimizer is only compatible with CostFunctionNetworkOptimizationProblem "
			"objects.  A problem of type " + problems.problem(iproblem)->inner_class_name()
			+ " was passed to this function!"
		);

		// Create the container for the solution:
		masala::numeric_api::auto_generated_api::optimization::OptimizationSolutions_APISP new_solutions_container_uncast(
            problems.problem(iproblem)->create_solutions_container()
        );
        CostFunctionNetworkOptimizationSolutions_APISP new_solutions_container(
            std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >(
                new_solutions_container_uncast
            )
        );
        CHECK_OR_THROW_FOR_CLASS( new_solutions_container != nullptr, "run_cost_function_network_optimizer", "Problem "
            + std::to_string(iproblem) + " created a " + new_solutions_container_uncast->inner_class_name() + " container, but this function "
            "only works with CostFunctionNetworkOptimizationSolutions containers.  Program error.  Please consult a developer, as "
            "this ought not to happen."
        );
        solutions_containers_by_problem.push_back( new_solutions_container );

		Size n_replicates;
		bool has_starting_points( false );
		
		{
			// Using predefined starting states:

			// If this problem has starting states, use those:
			std::vector< std::vector< Size > > starting_states_to_use;
			if( problem->has_candidate_starting_solutions() ) {
				// From problem.
				starting_states_to_use = problem->candidate_starting_solutions();
				has_starting_points = true;
			}

			// If this optimizer has staring states, also use those:
			if( !optimizer_starting_states_.empty() ) {
				for( auto const & state : optimizer_starting_states_ ) {
					check_starting_state_against_problem( state, *problem );
				}
				if( starting_states_to_use.empty() ) {
					starting_states_to_use = optimizer_starting_states_;
				} else {
					starting_states_to_use.reserve( starting_states_to_use.size() + optimizer_starting_states_.size() );
					for( auto const & state : optimizer_starting_states_ ) {
						starting_states_to_use.push_back( state );
					}
				}
				has_starting_points = true;
			}

			if( has_starting_points ) {
				starting_states_by_problem.push_back( starting_states_to_use );
				n_replicates = starting_states_to_use.size();
			} else {
				// Otherwise, use random starting states:
				starting_states_by_problem.push_back(
					generate_random_starting_states( *problem, rg, n_random_starting_states_ )
				);
				n_replicates = n_random_starting_states_;
			}
		}

		// Make a vector of work to do:
		for( std::vector< std::vector< Size > > const & starting_states : starting_states_by_problem ) {
			for( std::vector< Size > const & starting_state : starting_states ) {
				work_request.add_job(
					std::bind(
						&GreedyCostFunctionNetworkOptimizer::do_one_greedy_optimization_job_in_threads,
						this,
						std::cref(starting_state),
						n_replicates,
						problem,
						std::ref(*solutions_containers_by_problem[iproblem]),
						n_times_seen_multiplier_
					)
				);
			}
		}
	}

	masala::base::managers::threads::MasalaThreadedWorkExecutionSummary const thread_summary(
		MasalaThreadManager::get_instance()->do_work_in_threads( work_request )
	);
    thread_summary.write_summary_to_tracer();

    // Nonconst to const requires a silly extra step:
    std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > const_solutions_containers_by_problem( solutions_containers_by_problem.size() );
    for( Size i(0); i<solutions_containers_by_problem.size(); ++i ) {
		// Comment the following out; just for debugging:
		// write_to_tracer(
		// 	"Problem " + std::to_string(i) + " returned ["
		// 	+ masala::base::utility::container::container_to_string(
		// 		std::static_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >(
		// 			solutions_containers_by_problem[i]->solution(0)
		// 		)->solution_at_variable_positions(), ","
		// 	) + "] with penalty "
		// 	+ std::to_string( solutions_containers_by_problem[i]->solution(0)->solution_score() )
		// );

        const_solutions_containers_by_problem[i] = solutions_containers_by_problem[i];
    }

    return const_solutions_containers_by_problem;
}

/// @brief Run the optimizer on a cost function network optimization problem, and produce one or more solutions.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP
GreedyCostFunctionNetworkOptimizer::run_cost_function_network_optimizer_on_one_problem(
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP const & problem
) const {
	using namespace masala::base::managers::threads;
	using namespace masala::base::managers::random;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );

	// Get the random number generator:
	MasalaRandomNumberGeneratorHandle rg( MasalaRandomNumberGenerator::get_instance() );

	// Create the multithreaded work request:
	MasalaThreadedWorkRequest work_request;
	if( cpu_threads_to_request_ > 0 ) {
		work_request.set_n_threads_to_request( cpu_threads_to_request_ );
	} else {
		work_request.set_request_all_threads();
	}

	// Starting states: outer vector is a bunch of starting states for the
	// problem, inner vector is a single starting state (one choice index per variable node).
	std::vector< std::vector< Size > > starting_states_for_problem;

	// Storage for solutions:
	CostFunctionNetworkOptimizationSolutions_APISP solutions_container_for_problem(
		std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >( problem->create_solutions_container() )
	);
	CHECK_OR_THROW_FOR_CLASS( solutions_container_for_problem != nullptr, "run_cost_function_network_optimizer_on_one_problem",
		"Could not create appropriate solutions container for " + problem->class_name() + " problem type."
	);

	Size n_replicates;
	bool has_starting_points( false );
		
	{
		// Using predefined starting states:

		// If this problem has starting states, use those:
		std::vector< std::vector< Size > > starting_states_to_use;
		if( problem->has_candidate_starting_solutions() ) {
			// From problem.
			starting_states_to_use = problem->candidate_starting_solutions();
			n_replicates = problem->candidate_starting_solutions().size();
			has_starting_points = true;
		}

		// If this optimizer has staring states, also use those:
		if( !optimizer_starting_states_.empty() ) {
			for( auto const & state : optimizer_starting_states_ ) {
				check_starting_state_against_problem( state, *problem );
			}
			if( starting_states_to_use.empty() ) {
				starting_states_to_use = optimizer_starting_states_;
			} else {
				starting_states_to_use.reserve( starting_states_to_use.size() + optimizer_starting_states_.size() );
				for( auto const & state : optimizer_starting_states_ ) {
					starting_states_to_use.push_back( state );
				}
			}
			has_starting_points = true;
		}

		if( has_starting_points ) {
			starting_states_for_problem = starting_states_to_use;
			n_replicates = starting_states_to_use.size();
		} else {
			// Otherwise, use random starting states:
			starting_states_for_problem = generate_random_starting_states( *problem, rg, n_random_starting_states_ );
			n_replicates = n_random_starting_states_;
		}
	}

	// Make a vector of work to do:{
	for( std::vector< Size > const & starting_state : starting_states_for_problem ) {
		work_request.add_job(
			std::bind(
				&GreedyCostFunctionNetworkOptimizer::do_one_greedy_optimization_job_in_threads,
				this,
				std::cref(starting_state),
				n_replicates,
				problem,
				std::ref( *solutions_container_for_problem ),
				n_times_seen_multiplier_
			)
		);
	}

	masala::base::managers::threads::MasalaThreadedWorkExecutionSummary const thread_summary(
		MasalaThreadManager::get_instance()->do_work_in_threads( work_request )
	);
    thread_summary.write_summary_to_tracer();

    return solutions_container_for_problem;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Generate a bunch of random starting vectors.
std::vector< std::vector< masala::base::Size > >
GreedyCostFunctionNetworkOptimizer::generate_random_starting_states(
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem,
	masala::base::managers::random::MasalaRandomNumberGeneratorHandle const rg,
	masala::base::Size const n_random_starting_states_
) const {
	using masala::base::Size;
	Size const n_varnodes( problem.total_variable_nodes() );
	auto const n_choices_by_varnode( problem.n_choices_at_variable_nodes() );

	std::vector< std::vector< Size > > outvec( n_random_starting_states_, std::vector< masala::base::Size >( problem.total_variable_nodes() ) );
	for( Size i(0); i<n_random_starting_states_; ++i ) {
		for( Size j(0); j<n_varnodes; ++j ) {
			outvec[i][j] = rg->uniform_size_distribution( 0, n_choices_by_varnode[j].second-1 );
		}
	}

	return outvec;
}

/// @brief Do a single greedy optimizaton job, given a starting state.
/// @details This function runs in threads.  It requires no mutex since all storage is pre-allocated and all writes are to different vector entries.
void
GreedyCostFunctionNetworkOptimizer::do_one_greedy_optimization_job_in_threads(
	std::vector< masala::base::Size > const & starting_state,
	masala::base::Size const n_replicates,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP problem_ptr,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions,
	masala::base::Size const n_times_seen_multiplier
) const {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	CHECK_OR_THROW_FOR_CLASS( problem_ptr != nullptr, "do_one_greedy_optimization_job_in_threads", "A null pointer for the problem was passed to this function." );
	CostFunctionNetworkOptimizationProblem_API const & problem( *problem_ptr );

	std::vector< std::pair< Size, Size > > const nchoices_at_varnodes( problem.n_choices_at_variable_nodes() );
	Size const n_var_pos( nchoices_at_varnodes.size() );

	// Current state: state at the start of this round.
	// Candidate state: states considered this round.  Iterates through all possible single-point changes.
	// Best candidate state: lowest-scoring state considered so far this round.
	std::vector< Size > current_state( starting_state ), candidate_state( starting_state ), best_candidate_state( starting_state );
	Real current_score( problem.compute_non_approximate_absolute_score( current_state ) );
	Real candidate_score( current_score ), best_candidate_score( current_score );
	
	do {
		//write_to_tracer( "********************" ); // DELETE ME.
		current_state = best_candidate_state;
		current_score = best_candidate_score;
		for( Size i(0); i<n_var_pos; ++i ) {
			Size const nchoice( nchoices_at_varnodes[i].second );
			candidate_state = current_state;
			candidate_score = current_score;
			for( Size j(0); j<nchoice; ++j ) {
				candidate_state[i] = j;
				candidate_score = problem.compute_non_approximate_absolute_score( candidate_state );
				//write_to_tracer( "[" + masala::base::utility::container::container_to_string( candidate_state, "," ) + "]: " + std::to_string( candidate_score) ); // DELETE ME

				if( candidate_score < best_candidate_score ) {
					best_candidate_score = candidate_score;
					best_candidate_state = candidate_state;
					//write_to_tracer( "Best this round." ); // DELETE ME.
				}
			}
		}
	} while( best_candidate_state != current_state );

	//write_to_tracer( "NTIMES: " + std::to_string( n_times_seen_multiplier ) ); // DELETE ME

	solutions.merge_in_lowest_scoring_solutions(
		std::vector< std::tuple< std::vector< unsigned long int >, double, unsigned long int > >{
			{ best_candidate_state, best_candidate_score, n_times_seen_multiplier }
		},
		n_replicates,
		problem_ptr
	);
}

/// @brief Check a candiate solution against a problemd definition, and throw if there's a mismatch in node count, or if the choice indices
/// are out of range of the problem.
void
GreedyCostFunctionNetworkOptimizer::check_starting_state_against_problem(
	std::vector< masala::base::Size > const & starting_state,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem
) const {
	using masala::base::Size;

	auto const choices_by_varnode( problem.n_choices_at_variable_nodes() );
	CHECK_OR_THROW_FOR_CLASS( starting_state.size() == choices_by_varnode.size(), "check_starting_states_against_problem",
		"The number of variable nodes in the problem is " + std::to_string( choices_by_varnode.size() ) + ", but the candidate "
		"solution has " + std::to_string( starting_state.size() ) + " entries."
	);

	for( Size i(0), imax(choices_by_varnode.size()); i<imax; ++i ) {
		CHECK_OR_THROW_FOR_CLASS( choices_by_varnode[i].second > starting_state[i], "check_starting_states_against_problem",
			"Candidate solution proposes choice index " + std::to_string( starting_state[i] ) + " at variable node " +
			std::to_string( i ) + ", but this node has only " + std::to_string( choices_by_varnode[i].second ) + " choices."
		);
	}
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assign src to this object.  Must be implemented by derived classes.  Performs no mutex-locking.  Derived classes should call their parent's protected_assign().
void
GreedyCostFunctionNetworkOptimizer::protected_assign(
	CostFunctionNetworkOptimizer const & src
) {
	GreedyCostFunctionNetworkOptimizer const * src_cast_ptr( dynamic_cast< GreedyCostFunctionNetworkOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Could not interpret source object of type " + src.class_name() + " as a GreedyCostFunctionNetworkOptimizer object." );

	cpu_threads_to_request_ = src_cast_ptr->cpu_threads_to_request_;
	n_random_starting_states_ = src_cast_ptr->n_random_starting_states_;
	n_times_seen_multiplier_ = src_cast_ptr->n_times_seen_multiplier_;
	optimizer_starting_states_ = src_cast_ptr->optimizer_starting_states_;
	
	masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::protected_assign( src );
}

/// @brief Set a template cost function network optimization problem data representation, configured by the user but with no data entered.
/// @details This can optionally be passed in, in which case the get_template_preferred_cfn_data_representation() function can be
/// used to retrieve a deep clone.  This allows the solver to cache its preferred data representation with its setup.
/// @note This version performs no mutex-locking, and is called by set_template_preferred_cfn_data_representation(), which does lock the mutex.
/// This version is virtual to allow derived classes to override it, to add checks of their own.  If overridden, the override should call the
/// base class to set the variable internally.
void
GreedyCostFunctionNetworkOptimizer::protected_set_template_preferred_cfn_data_representation(
	masala::base::managers::engine::MasalaDataRepresentationAPICSP const & representation_in
) {
	masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::protected_set_template_preferred_cfn_data_representation( representation_in );
}

/// @brief If the template preferred CFN data representation has not been set, return a default CFN data representation.
/// @details This version returns a PairwisePrecomputedCostFunctionNetworkOptimizationProblem, with default configuration.  Performs no mutex-locking.
masala::base::managers::engine::MasalaDataRepresentationAPISP
GreedyCostFunctionNetworkOptimizer::protected_get_default_template_preferred_cfn_data_representation() const {
	using namespace masala::base::managers::engine;
	using namespace masala::base::managers::engine::data_representation_request;
	using masala::base::Size;

	MasalaDataRepresentationNameRequirementCriterionSP name_criterion( masala::make_shared< MasalaDataRepresentationNameRequirementCriterion >( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" ) );
	MasalaDataRepresentationRequest request;
	request.add_data_representation_criterion( name_criterion );
	std::vector< MasalaDataRepresentationCreatorCSP > creators( MasalaDataRepresentationManager::get_instance()->get_compatible_data_representation_creators( request ) );
	CHECK_OR_THROW_FOR_CLASS( creators.size() >= 1, "protected_get_default_template_preferred_cfn_data_representation", "Could not find the PairwisePrecomputedCostFunctionNetworkOptimizationProblem class."
		"  Was it registered with the Masala data representation manager?  (Note that the register_library() function of each plugin library must be called before the library is used.)"
	);
	bool found(false);
	Size found_index(0);
	for( auto const & creator : creators ) {
		if( creator->get_plugin_object_namespace_and_name() == "standard_masala_plugins::optimizers::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem" ) {
			found = true;
			break;
		}
		++found_index;
	}
	CHECK_OR_THROW_FOR_CLASS( found, "protected_get_default_template_preferred_cfn_data_representation", "Could not find the PairwisePrecomputedCostFunctionNetworkOptimizationProblem class."
		"  Was it registered with the Masala data representation manager?  (Note that the register_library() function of each plugin library must be called before the library is used.)"
	);

	MasalaDataRepresentationAPISP pairwise_cfn_api( creators[found_index]->create_data_representation() );
	PairwisePrecomputedCostFunctionNetworkOptimizationProblemSP pairwise_cfn( std::dynamic_pointer_cast< PairwisePrecomputedCostFunctionNetworkOptimizationProblem >( pairwise_cfn_api->get_inner_data_representation_object() ) );
	CHECK_OR_THROW_FOR_CLASS( pairwise_cfn != nullptr, "protected_get_default_template_preferred_cfn_data_representation", "The created object "
		"could not be interpreted as a PairwisePrecomputedCostFunctionNetworkOptimizationProblem.  This is a program error.  Please consult a developer."
	);

	// Could configure the pairwise_cfn object here if needed.
	return pairwise_cfn_api;
}


} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
