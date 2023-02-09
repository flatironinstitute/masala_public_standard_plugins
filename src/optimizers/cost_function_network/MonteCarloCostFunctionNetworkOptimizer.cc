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
#include <numeric_api/auto_generated_api/optimization/annealing/AnnealingScheduleBase_API.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/threads/MasalaThreadedWorkRequest.hh>
#include <base/managers/threads/MasalaThreadedWorkExecutionSummary.hh>
#include <base/managers/random/MasalaRandomNumberGenerator.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <vector>
#include <string>
#include <utility>

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
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer( src )
{
    std::lock_guard< std::mutex > lock( src.optimizer_mutex_ );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
    attempts_per_problem_ = src.attempts_per_problem_;
    n_solutions_to_store_per_problem_ = src.n_solutions_to_store_per_problem_;
    annealing_steps_per_attempt_ = src.annealing_steps_per_attempt_;
    annealing_schedule_ = ( src.annealing_schedule_ == nullptr ? nullptr : src.annealing_schedule_->deep_clone() );

    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->reset_call_count();
    }
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer &
MonteCarloCostFunctionNetworkOptimizer::operator=( MonteCarloCostFunctionNetworkOptimizer const & src ) {
    std::lock( optimizer_mutex_, src.optimizer_mutex_ );
    std::lock_guard< std::mutex > lock1( optimizer_mutex_, std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.optimizer_mutex_, std::adopt_lock );
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::operator=( src );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
    attempts_per_problem_ = src.attempts_per_problem_;
    annealing_steps_per_attempt_ = src.annealing_steps_per_attempt_;
    n_solutions_to_store_per_problem_ = src.n_solutions_to_store_per_problem_;
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
/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "simulated_annealing", "stochastic" }
std::vector< std::string >
MonteCarloCostFunctionNetworkOptimizer::get_keywords() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	std::vector< std::string > keywords( CostFunctionNetworkOptimizer::get_keywords() );
    keywords.push_back( "monte_carlo" );
    keywords.push_back( "simulated_annealing" );
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
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( MonteCarloCostFunctionNetworkOptimizer, api_description );

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
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_n_solutions_to_store_per_problem", "Sets the maximum number of solutions to return for each problem.",
				"n_solutions_in", "The maximum number solutions to return for each problem.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_n_solutions_to_store_per_problem, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< AnnealingScheduleBase_API const & > > (
				"set_annealing_schedule", "Sets the annealing schedule to use for the problem.",
				"annealing_schedule_in", "The annealing schedule to use.  Cloned on input.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_annealing_steps_per_attempt", "Sets the length of the Monte Carlo trajectory performed for each attempt of each problem.",
				"steps_in", "The number of steps in the Monte Carlo trajectory.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_annealing_steps_per_attempt, this, std::placeholders::_1 )
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
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"n_solutions_to_store_per_problem", "Gets the maximum number of solutions that will be returned for each problem.",
				"n_solutions_to_store_per_problem", "The maximum number of solutions that will be returned for each problem.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::n_solutions_to_store_per_problem, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"annealing_steps_per_attempt", "Gets the length of the Monte Carlo trajectory performed for each attempt of each problem.",
				"steps", "The number of steps in the Monte Carlo trajectory.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::annealing_steps_per_attempt, this )
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
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer, this, std::placeholders::_1 )
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
    masala::base::Size const threads_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    cpu_threads_to_request_ = threads_in;
}

/// @brief Set the number of times to try each problem.
/// @details Minimum is 1.
void
MonteCarloCostFunctionNetworkOptimizer::set_attempts_per_problem(
    masala::base::Size const attempts_in
) {
    CHECK_OR_THROW_FOR_CLASS( attempts_in > 0, "set_attempts_per_problem", "The number of attempts per problem must be greater than zero." );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    attempts_per_problem_ = attempts_in;
}

/// @brief Set the number of solutions to return for each problem.
void
MonteCarloCostFunctionNetworkOptimizer::set_n_solutions_to_store_per_problem(
    masala::base::Size const n_solutions_in
) {
    CHECK_OR_THROW_FOR_CLASS( n_solutions_in > 0, "set_n_solutions_to_store_per_problem", "The number of solutions to return per problem must be greater than zero." );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    n_solutions_to_store_per_problem_ = n_solutions_in;
}

/// @brief Set the annealing schedule to use for annealing.
/// @details Cloned on input.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule(
    masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & schedule_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    annealing_schedule_ = schedule_in.deep_clone();
    annealing_schedule_->set_final_time_index( annealing_steps_per_attempt_ );
    annealing_schedule_->reset_call_count();
}

/// @brief Set the numer of Monte Carlo moves to make in each attempt.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_steps_per_attempt(
    masala::base::Size const steps_in
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
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::cpu_threads_to_request() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return cpu_threads_to_request_;
}

/// @brief Get the number of times to try each problem.
/// @details Minimum is 1.
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::attempts_per_problem() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return attempts_per_problem_;
}

/// @brief Get the number of solutions to return for each problem.
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::n_solutions_to_store_per_problem() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return n_solutions_to_store_per_problem_;
}

/// @brief Get the numer of Monte Carlo moves to make in each attempt.
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::annealing_steps_per_attempt() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return annealing_steps_per_attempt_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
/// the problem with the same index.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
) const {
    using namespace masala::base::managers::threads;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );

    CHECK_OR_THROW_FOR_CLASS( annealing_schedule_ != nullptr, "run_cost_function_network_optimizer", "An annealing schedule must be set before calling this function." );
    annealing_schedule_->reset_call_count();
    annealing_schedule_->set_final_time_index( annealing_steps_per_attempt_ );

    // Create storange for solutions.
    Size const nproblems( problems.n_problems() );
    std::vector< CostFunctionNetworkOptimizationSolutions_APISP > solutions_by_problem;
    std::vector< std::mutex > solution_mutexes( nproblems );
    solutions_by_problem.reserve( nproblems );
    for( Size i(0); i<nproblems; ++i ) {
        solutions_by_problem.push_back( masala::make_shared< CostFunctionNetworkOptimizationSolutions_API >() );
    }
    solutions_by_problem.shrink_to_fit();

    // Create work vector.
    MasalaThreadedWorkRequest work_request( cpu_threads_to_request_ );
    work_request.reserve( nproblems * attempts_per_problem_ );
    for( Size i(0); i<nproblems; ++i ) {
#ifndef NDEBUG
        // Redundant check that this is a cost function network optimization problem in debug mode.
        CostFunctionNetworkOptimizationProblem_APICSP problem_cast( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >(problems.problem(i)) );
        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( problem_cast != nullptr, "run_cost_function_network_optimizer", "Program error: problem "
            + std::to_string(i) + " is not enclosed in a CostFunctionNetworkOptimizationProblem_API!  It is a " + problems.problem(i)->inner_class_name() + " encapsulated in a "
            + problems.problem(i)->class_name() + "."
        );
#else
        // Just assume that this is the right problem type in release mode.
        CostFunctionNetworkOptimizationProblem_APICSP problem_cast( std::static_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >(problems.problem(i)) );
#endif
        for( Size j(0); j<attempts_per_problem_; ++j ) {
            work_request.add_job(
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::run_mc_trajectory, this,
                    j, // replicate index
                    i, // problem index
                    annealing_steps_per_attempt_, // Steps in the MC search.
                    n_solutions_to_store_per_problem_, // Solutions per problem.
                    std::cref( *annealing_schedule_ ), // A copy of the annealing schedule.
                    problem_cast, // The problem description.
                    std::ref( *(solutions_by_problem[i]) ), // The storage for the collection of solutions.
                    std::ref( solution_mutexes[i] ) // A mutex for locking the solution storage for the problem.
                )
            );
        }
    }

    // Do the work.
    MasalaThreadedWorkExecutionSummary const threading_summary( MasalaThreadManager::get_instance()->do_work_in_threads( work_request ) );
    threading_summary.write_summary_to_tracer();

    // Nonconst to const requires a silly extra step:
    std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > const_solutions_by_problem( nproblems );
    for( Size i(0); i<nproblems; ++i ) {
        const_solutions_by_problem[i] = solutions_by_problem[i];
    }

    return const_solutions_by_problem;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run a single Monte Carlo trajectory.
/// @param replicate_index The index of this replicate for this problem.
/// @param problem_index The index of this problem.
/// @param annealing_steps The number of steps in the trajectory.
/// @param n_solutions_to_store The number of solutions to store.
/// @param annealing_schedule The temperature generator (already configured with the number of steps).
/// @param problem The description of the problem.  This may or may not be a specialized problem like a PrecomputedPairwiseCostFunctionNetworkOptimizationProblem.
/// @param solutions Storage for a collection of solutions.  Should be unique to problem.
/// @param solutions_mutex A mutex for the collection of solutions.
void
MonteCarloCostFunctionNetworkOptimizer::run_mc_trajectory(
    masala::base::Size const replicate_index,
    masala::base::Size const problem_index,
    masala::base::Size const annealing_steps,
    masala::base::Size const n_solutions_to_store,
    masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & annealing_schedule,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP problem,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions,
    std::mutex & solutions_mutex
) const {
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
    using namespace masala::base::managers::random;
    using masala::base::Real;
    using masala::base::Size;

    // Make a copy of the annealing schedule.
    AnnealingScheduleBase_APISP annealing_schedule_copy( annealing_schedule.deep_clone() );
    annealing_schedule_copy->reset_call_count();

    /// Selection for the solution:
    std::vector< std::pair< Size, Size > > const n_choices_per_variable_node( problem->n_choices_at_variable_nodes() ); // First index of each pair is node index, second is number of choices.  Only variable nodes are included.
    Size const n_variable_nodes( n_choices_per_variable_node.size() );
    std::vector< Size > current_solution( n_variable_nodes ), last_accepted_solution( n_variable_nodes );

    // Get handle to random generator.
    MasalaRandomNumberGeneratorHandle const randgen( MasalaRandomNumberGenerator::get_instance() );

    // Initialize choices randomly:
    for( Size i(0); i<n_variable_nodes; ++i ) {
        current_solution[i] = randgen->uniform_size_distribution( 0, n_choices_per_variable_node[i].second - 1 );
        last_accepted_solution[i] = current_solution[i];
    }
    // Note: these will accumulate numerical errors.
    Real current_accepted_absolute_score( problem->compute_absolute_score( current_solution ) );
    write_to_tracer( "Initial score = " + std::to_string( current_accepted_absolute_score ) ); // DELETE ME

    // Main loop over all steps of the annealing trajectory.
    for( Size step_index(0); step_index < annealing_steps; ++step_index ) {
        make_mc_move( current_solution, n_choices_per_variable_node, randgen );
        Real const deltaE( problem->compute_score_change( last_accepted_solution, current_solution ) );
        write_to_tracer( "Move " + std::to_string( step_index ) +
            + " old = [" + masala::base::utility::container::container_to_string( last_accepted_solution, "," )
            + "] new = [" + masala::base::utility::container::container_to_string( current_solution, "," )
            + "] deltaE = " + std::to_string(deltaE) ); // DELETE ME

        // Apply the Metropolis criterion to accept or reject the move:
        if( randgen->apply_metropolis_criterion( deltaE, annealing_schedule_copy->temperature() ) ) {
            last_accepted_solution = current_solution;
            current_accepted_absolute_score += deltaE,
            write_to_tracer( "Accepting move " + std::to_string( step_index ) + ".  Current score = " + std::to_string( current_accepted_absolute_score ) + "." ); // DELETE ME
            determine_whether_to_store_solution( current_solution, current_accepted_absolute_score, solutions, solutions_mutex, n_solutions_to_store, replicate_index, problem_index, problem );
        } else {
            current_solution = last_accepted_solution;
        }
    }

    { // Mutex lock scope
        std::lock_guard< std::mutex > lock( solutions_mutex );

    // Recompute energies of all solutions to correct numerical error.
#ifndef NDEBUG
        solutions.recompute_all_scores( 1.0e-6); // As a sanity check, make sure numerical errors are small.
#else
        solutions.recompute_all_scores();
#endif
        solutions.sort_by_score();

    } // End mutex lock scope.

    // Minimal output.
    write_to_tracer(
        "Completed replicate " + std::to_string( replicate_index ) +
        " of cost function network optimization problem " +
        std::to_string( problem_index ) + "."
    );
}

/// @brief Make a Monte Carlo move.
/// @param current_solution The current solution, as a vector of choice indices for all variable positions.  Changed by this operation.
/// @param n_choices_per_variable_node Number of choices per variable node, in the same order as current_solution.  The pairs are
/// (node index, number of choices).
/// @param randgen The handle of the Masala random generator.
/*static*/
void
MonteCarloCostFunctionNetworkOptimizer::make_mc_move(
    std::vector< masala::base::Size > & current_solution,
    std::vector< std::pair< masala::base::Size, masala::base::Size > > const & n_choices_per_variable_node,
    masala::base::managers::random::MasalaRandomNumberGeneratorHandle const randgen
) {
    using masala::base::Real;
    using masala::base::Size;
    Size const index_to_change( randgen->uniform_size_distribution( 0, current_solution.size() - 1 ) );
    Size new_choice( randgen->uniform_size_distribution( 0, n_choices_per_variable_node[index_to_change].second - 2 ) );
    if( new_choice >= current_solution[index_to_change] ) {
        ++new_choice;
    }
    current_solution[index_to_change] = new_choice;
}

/// @brief Determine whether to add the current solution to the set of solutions stored for this replicate attempt.
/// @details We maintain a list of N solutions for each replicate attempt.  If this solution has been encountered before
/// and is already in the list, we increment the count for the number of times it has been seen.  If not, and if fewer
/// than N solutions have been stored, we append the solution in a CostFunctionNetworkOptimizationSolution container.  If
/// the solution has not been seen, N solutions are stored, and this solution is lower-energy than the highest-energy
/// solution, we replace the highest-energy solution with this one.
/// @param current_solution The solution that we are considering, represented as a vector of choice indices where each
/// entry in the vector corresponds to a variable node (in order).
/// @param current_absolute_score The absolute score of this solution.
/// @param solutions The container of solutions.
/// @param solutions_mutex A mutex for locking and keeping locked the solutions collection.
/// @param n_solutions_to_store The number of solutions to store.
/// @param replicate_index The index of this replicate for this problem.
/// @param problem_index The index of this problem.
/// @param problem The problem description.  A const shared pointer to the problem will be embedded in the solution.
/*static*/
void
MonteCarloCostFunctionNetworkOptimizer::determine_whether_to_store_solution(
    std::vector< masala::base::Size > const & current_solution,
    masala::base::Real current_absolute_score,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions,
    std::mutex & solutions_mutex,
    masala::base::Size const n_solutions_to_store,
    masala::base::Size const replicate_index,
    masala::base::Size const problem_index,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP const & problem
) {
    using masala::base::Size;
    using masala::base::Real;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

    // Lock this solutions container.
    std::lock_guard< std::mutex > lock( solutions_mutex );

    // If the solution has already been seen, increment the number of times we have seen it.
    // Simultaneously, find the highest score solution that we have stored.
    bool first(true);
    Real highestE(0.0);
    Size highestE_index(0);
    for( Size i(0), imax(solutions.n_solutions()); i<imax; ++i ) {  
        CostFunctionNetworkOptimizationSolution_API const & solution( static_cast< CostFunctionNetworkOptimizationSolution_API const & >( *(solutions.solution(i)) ) );
        if( solution == current_solution ) {
            solutions.increment_n_times_solution_was_produced(i);
            return;
        }
        if( first || solution.solution_score() > highestE ) {
            first = false;
            highestE = solution.solution_score();
            highestE_index = i;
        }
    }

    // If we reach here, we've not yet seen this solution.  If we're supposed to store more solutions
    // than we are currently storing, store this one.
    if( solutions.n_solutions() < n_solutions_to_store ) {
        solutions.add_optimization_solution( masala::make_shared< CostFunctionNetworkOptimizationSolution_API >( problem, current_solution, current_absolute_score ) );
        return;
    }

    // If we reach here, we have a full solution vector.  We only store this solution (and kick out the highest-energy solution)
    // if this solution is lower energy than the highest energy.
    if( current_absolute_score < highestE ) {
        solutions.remove_optimization_solution( highestE_index );
        solutions.add_optimization_solution( masala::make_shared< CostFunctionNetworkOptimizationSolution_API >( problem, current_solution, current_absolute_score ) );
    }

}


} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
