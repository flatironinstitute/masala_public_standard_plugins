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

/// @file src/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.cc
/// @brief Implementation for a CostFunctionNetworkOptimizer that solves a cost function network problem using Monte
/// Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/base_classes/optimization/annealing/AnnealingSchedule.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_ZeroInput.tmpl.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/threads/MasalaThreadedWorkRequest.hh>
#include <base/managers/threads/MasalaThreadedWorkExecutionSummary.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer::MonteCarloCostFunctionNetworkOptimizer(
    MonteCarloCostFunctionNetworkOptimizer const & src
) :
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer( src ),
    cpu_threads_to_request_(src.cpu_threads_to_request_),
    attempts_per_problem_(src.attempts_per_problem_),
    annealing_steps_per_attempt_( src.annealing_steps_per_attempt_ ),
    annealing_schedule_( src.annealing_schedule_ == nullptr ? nullptr : src.annealing_schedule_->deep_clone() )
{
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->reset_call_count();
    }
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer &
MonteCarloCostFunctionNetworkOptimizer::operator=( MonteCarloCostFunctionNetworkOptimizer const & src ) {
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::operator=( src );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
    attempts_per_problem_ = src.attempts_per_problem_;
    annealing_steps_per_attempt_ = src.annealing_steps_per_attempt_;
    annealing_schedule_ = ( src.annealing_schedule_ == nullptr ? nullptr : src.annealing_schedule_->deep_clone() );
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->reset_call_count();
    }
    return *this;
}

/// @brief Make a copy of this object that's wholly independent.
MonteCarloCostFunctionNetworkOptimizerSP
MonteCarloCostFunctionNetworkOptimizer::deep_clone() const {
    MonteCarloCostFunctionNetworkOptimizerSP new_optimizer( masala::make_shared< MonteCarloCostFunctionNetworkOptimizer >(*this) );
    new_optimizer->make_independent();
    return new_optimizer;
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
MonteCarloCostFunctionNetworkOptimizer::make_independent() {
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_ = annealing_schedule_->deep_clone();
    }
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
MonteCarloCostFunctionNetworkOptimizer::get_categories() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return CostFunctionNetworkOptimizer::get_categories();
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "stochastic" }
std::vector< std::string >
MonteCarloCostFunctionNetworkOptimizer::get_keywords() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	std::vector< std::string > keywords( CostFunctionNetworkOptimizer::get_keywords() );
    keywords.push_back( "monte_carlo" );
    keywords.push_back( "stochastic" );
    return keywords;
}

/// @brief Get the class name.
/// @returns "MonteCarloCostFunctionNetworkOptimizer".
std::string
MonteCarloCostFunctionNetworkOptimizer::class_name() const {
    return "MonteCarloCostFunctionNetworkOptimizer";
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
std::string
MonteCarloCostFunctionNetworkOptimizer::class_namespace() const {
    return "standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
MonteCarloCostFunctionNetworkOptimizer::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using namespace masala::base::api::getter;
    using masala::numeric_api::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    if( api_description_ == nullptr ) {
        MasalaObjectAPIDefinitionSP api_description(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this,
                "An optimizer for cost function network problems that uses a Monte Carlo search to optimize.  This "
                "performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to "
                "pick a node at random and change its setting at random, compute the change in overall energy or score, "
                "and accept or reject the move based on the difference in energy and the Metropolis criterion.",
                false
            )
        );

        // Constructors:

        api_description->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< MonteCarloCostFunctionNetworkOptimizer > >(
                class_name(), "Create an instance of this optimizer, and initialize it with default options."
            )
        );
        api_description->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< MonteCarloCostFunctionNetworkOptimizer, MonteCarloCostFunctionNetworkOptimizer const & > >(
                class_name(), "Copy constructor.",
                "src", "The optimizer to copy.  Not changed by this operation."
            )
        );

        // Setters:

		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_cpu_threads_to_request", "Sets the number of threads to request when running problems in parallel.",
				"threads_in", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_cpu_threads_to_request, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_attempts_per_problem", "Sets the number of times to try each problem.",
				"attempts_in", "The number of times to try each problem.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_attempts_per_problem, this, std::placeholders::_1 )
			)
		);

		// Getters:

		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"cpu_threads_to_request", "Gets the number of threads to request when running problems in parallel.",
				"cpu_threads_to_request", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::cpu_threads_to_request, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"attempts_per_problem", "Gets the number of times to try each problem.",
				"attempts_per_problem", "The number of times to try each problem.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::attempts_per_problem, this )
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
MonteCarloCostFunctionNetworkOptimizer::set_cpu_threads_to_request(
    masala::numeric_api::Size const threads_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    cpu_threads_to_request_ = threads_in;
}

/// @brief Set the number of times to try each problem.
/// @details Minimum is 1.
void
MonteCarloCostFunctionNetworkOptimizer::set_attempts_per_problem(
    masala::numeric_api::Size const attempts_in
) {
    CHECK_OR_THROW_FOR_CLASS( attempts_in > 0, "set_attempts_per_problem", "The number of attempts per problem must be greater than zero." );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    attempts_per_problem_ = attempts_in;
}

/// @brief Set the annealing schedule to use for annealing.
/// @details Cloned on input.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule(
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule const & schedule_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    annealing_schedule_ = schedule_in.deep_clone();
    annealing_schedule_->set_final_time_index( annealing_steps_per_attempt_ );
    annealing_schedule_->reset_call_count();
}

/// @brief Set the numer of Monte Carlo moves to make in each attempt.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_steps_per_attempt(
    masala::numeric_api::Size const steps_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    annealing_steps_per_attempt_ = steps_in;
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->set_final_time_index( steps_in );
        annealing_schedule_->reset_call_count();
    }
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the number of threads to request.
/// @details The default setting of 0 means "request all available".
masala::numeric_api::Size
MonteCarloCostFunctionNetworkOptimizer::cpu_threads_to_request() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return cpu_threads_to_request_;
}

/// @brief Get the number of times to try each problem.
/// @details Minimum is 1.
masala::numeric_api::Size
MonteCarloCostFunctionNetworkOptimizer::attempts_per_problem() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return attempts_per_problem_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP
MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
) const {
    using namespace masala::base::managers::threads;
    using masala::numeric_api::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );

    CHECK_OR_THROW_FOR_CLASS( annealing_schedule_ != nullptr, "run_cost_function_network_optimizer", "An annealing schedule must be set before calling this function." );
    annealing_schedule_->reset_call_count();

    // Create work vector.
    MasalaThreadedWorkRequest work_request( cpu_threads_to_request_ );
    work_request.reserve( problems.n_problems() * attempts_per_problem_ );
    // for( Size i(0), imax(problems.n_problems); i<imax; ++i ) {
    //     for( Size j(0); j<attempts_per_problem_; ++j ) {
    //         work_request.add_job(
    //             std::bind( &MonteCarloCostFunctionNetworkOptimizer::run_mc_trajectory, this, TODO TODO TODO )
    //         );
    //     }
    // }

    //TODO TODO TODO
    return nullptr; // TODO TODO TODO
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run a single Monte Carlo trajectory.
/// @param annealing_steps The number of steps in the trajectory.
/// @param annealing_schedule The temperature generator (already configured with the number of steps).
/// @param problem The description of the problem.  This may or may not be a specialized problem like a PrecomputedPairwiseCostFunctionNetworkOptimizationProblem.
/// @param solutions_mutex A mutex for accessing the solutions collection.
/// @param solutions Shared storage for a collection of solutions.  The mutex must be locked for access.
void
MonteCarloCostFunctionNetworkOptimizer::run_mc_trajectory(
    masala::numeric_api::Size const annealing_steps,
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule const & annealing_schedule,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem,
    std::mutex & solutions_mutex,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions
) const {
    // TODO TODO TODO
}

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
