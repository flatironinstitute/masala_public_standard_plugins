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
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_cpu_threads_to_request", "Sets the number of threads to request when running problems in parallel.",
				"threads_in", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_cpu_threads_to_request, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_n_random_starting_states", "If starting states are not provided in the problem definition, indicate "
				"the number of random starting states to use.  Defaults to 1.",
				"n_random_starting_states_in", "The number of random starting states to use.  This number of greedy "
				"descent trajectories will be carried out for all problems that do not provide starting states.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::set_n_random_starting_states, this, std::placeholders::_1 )
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
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"n_random_starting_states", "If starting states are not provided in the problem definition, get "
				"the number of random starting states to use.  Defaults to 1.",
				"n_random_starting_states", "The number of random starting states to use.  This number of greedy "
				"descent trajectories will be carried out for all problems that do not provide starting states.",
				false, false,
				std::bind( &GreedyCostFunctionNetworkOptimizer::n_random_starting_states, this )
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

/// @brief If starting states are not provided in the problem definition, indicate
/// the number of random starting states to use.  Defaults to 1.
void
GreedyCostFunctionNetworkOptimizer::set_n_random_starting_states(
	masala::base::Size const setting
) {
	CHECK_OR_THROW_FOR_CLASS( setting >= 1, "set_n_random_starting_states",
		"The number of random starting states must be at least 1.  Got " + std::to_string( setting ) + "."
	);
	n_random_starting_states_ = setting;
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
	return n_random_starting_states_;
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
	using namespace masala::base::managers::random;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using masala::base::Size;

	// Comment the following out; just for debugging:
	write_to_tracer( "Doing greedy refinement..." );

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

		// If this problem has starting states, use those:
		Size n_replicates;
		if( problem->has_candidate_starting_solutions() ) {
			starting_states_by_problem.push_back( problem->candidate_starting_solutions() );
			n_replicates = problem->candidate_starting_solutions().size();
		} else {
			// Otherwise, use random starting states:
			starting_states_by_problem.push_back(
				generate_random_starting_states( *problem, rg, n_random_starting_states_ )
			);
			n_replicates = n_random_starting_states_;
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
						std::ref(*solutions_containers_by_problem[iproblem])
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
		write_to_tracer(
			"Problem " + std::to_string(i) + " returned ["
			+ masala::base::utility::container::container_to_string(
				std::static_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >(
					solutions_containers_by_problem[i]->solution(0)
				)->solution_at_variable_positions(), ","
			) + "] with penalty "
			+ std::to_string( solutions_containers_by_problem[i]->solution(0)->solution_score() )
		);

        const_solutions_containers_by_problem[i] = solutions_containers_by_problem[i];
    }

    return const_solutions_containers_by_problem;
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
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions
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

	solutions.merge_in_lowest_scoring_solutions(
		std::vector< std::tuple< std::vector< unsigned long int >, double, unsigned long int > >{
			{ best_candidate_state, best_candidate_score, 1 }
		},
		n_replicates,
		problem_ptr
	);
}


} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
