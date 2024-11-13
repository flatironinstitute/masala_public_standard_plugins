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

/// @file src/optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizer.hh
/// @brief Headers for a CostFunctionNetworkOptimizer that refines a cost function network
/// optimization problem by greedy descent.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_GreedyCostFunctionNetworkOptimizer_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_GreedyCostFunctionNetworkOptimizer_hh

// Forward declarations:
#include <optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/CostFunctionNetworkOptimizer.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.fwd.hh>

// Numeric API headers:
#include <base/types.hh>
#include <numeric_api/auto_generated_api/optimization/annealing/AnnealingScheduleBase_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.fwd.hh>

// Base headers:
#include <base/managers/random/MasalaRandomNumberGenerator.fwd.hh>

// STL headers:
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

/// @brief A CostFunctionNetworkOptimizer that solves a cost function network problem by greedy descent.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class GreedyCostFunctionNetworkOptimizer : public masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	GreedyCostFunctionNetworkOptimizer() = default;

	/// @brief Copy constructor.
	/// @details Needed since we define a mutex.
	GreedyCostFunctionNetworkOptimizer( GreedyCostFunctionNetworkOptimizer const & );

	/// @brief Assignment operator.
	/// @details Needed since we define a mutex.
	GreedyCostFunctionNetworkOptimizer &
	operator=( GreedyCostFunctionNetworkOptimizer const & );

	/// @brief Make a copy of this object that's wholly independent.
	GreedyCostFunctionNetworkOptimizerSP
	deep_clone() const;

	/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
	void
	make_independent();

	/// @brief Destructor.
	~GreedyCostFunctionNetworkOptimizer() override = default;

public:

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
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "optimizer", "cost_function_network", "numeric", "greedy", "deterministic" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the category that this MasalaEngine fits into.
	/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
	std::vector< std::vector< std::string > >
	get_engine_categories() const override;

	/// @brief Get the keywords that this MasalaEngine has.
	/// @returns { "optimizer", "cost_function_network", "numeric", "greedy", "deterministic" }
	std::vector< std::string >
	get_engine_keywords() const override;

	/// @brief Get the class name.
	/// @returns "GreedyCostFunctionNetworkOptimizer".
	std::string class_name() const override;

	/// @brief Get the class namespace.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	std::string class_namespace() const override;

	/// @brief Get the class name (static version).
	/// @returns "GreedyCostFunctionNetworkOptimizer".
	static std::string class_name_static();

	/// @brief Get the class namespace (static version).
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the API definition for this object.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the number of threads to request.
	/// @details The default setting of 0 means "request all available".
	void set_cpu_threads_to_request( masala::base::Size const threads_in );

	/// @brief If starting states are not provided in the problem definition or in the optimizer configuration, indicate
	/// the number of random starting states to use.  Defaults to 1.
	void set_n_random_starting_states( masala::base::Size const setting );

	/// @brief By default, a single greedy optimization trajectory results in a solution being seen
	/// once.  This option lets other code specify that it should be higher.  Useful when other code,
	/// like the MonteCaroCostFunctionNetworkOptimizer, wants to use this for greedy refinement of
	/// solutions that it has produced many times.
	/// @note NOT part of the public-facing interface.  Intended only for use by code within this
	/// plugin sub-library.
	void set_n_times_seen_multiplier( masala::base::Size const setting );

	/// @brief Set the starting points to use, as a vector of vectors of choice-by-node.  These replace any already stored.
	/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
	/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
	void
	set_optimizer_starting_states(
		std::vector< std::vector< masala::base::Size > > const & starting_states_in
	);

	/// @brief Add starting points to use, as a vector of vectors of choice-by-node.  These are appended to any already stored.
	/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
	/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
	void
	add_optimizer_starting_states(
		std::vector< std::vector< masala::base::Size > > const & starting_states_in
	);

	/// @brief Add a starting point to use, as a vector of choice-by-node.  This is appended to any already stored.
	/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
	/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
	void
	add_optimizer_starting_state(
		std::vector< masala::base::Size > const & starting_state_in
	);

	/// @brief Clear the starting points to use.
	/// @details By setting the starting points in the optimizer rather than in the problem, an error will be thrown at apply time if the
	/// number of nodes or choices doesn't match the problem to which the optimizer is applied.
	void
	clear_optimizer_starting_states();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the number of threads to request.
	/// @details The default setting of 0 means "request all available".
	masala::base::Size cpu_threads_to_request() const;

	/// @brief If starting states are not provided in the problem definition or in the optimizer configuration, get
	/// the number of random starting states to use.  Defaults to 1.
	masala::base::Size n_random_starting_states() const;

	/// @brief By default, a single greedy optimization trajectory results in a solution being seen
	/// once.  This option lets other code specify that it should be higher.  Useful when other code,
	/// like the MonteCaroCostFunctionNetworkOptimizer, wants to use this for greedy refinement of
	/// solutions that it has produced many times.
	/// @note NOT part of the public-facing interface.  Intended only for use by code within this
	/// plugin sub-library.
	masala::base::Size n_times_seen_multiplier() const;

	/// @brief Access the list of starting states that the optimizer has been configured to try.
	std::vector< std::vector< masala::base::Size > > const & optimizer_starting_states() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Run the optimizer on a vector of cost function network optimization problems, and produce a vector of solutions.
	/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
	/// the problem with the same index.
	std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
	run_cost_function_network_optimizer(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
	) const override;

	/// @brief Run the optimizer on a cost function network optimization problem, and produce one or more solutions.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP
	run_cost_function_network_optimizer_on_one_problem(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP const & problem
	) const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


	/// @brief Generate a bunch of random starting vectors.
	std::vector< std::vector< masala::base::Size > >
	generate_random_starting_states(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem,
		masala::base::managers::random::MasalaRandomNumberGeneratorHandle const rg,
		masala::base::Size const n_random_starting_states_
	) const;

	/// @brief Do a single greedy optimizaton job, given a starting state.
	/// @details This function runs in threads.  It requires no mutex since all storage is pre-allocated and all writes are to different vector entries.
	void
	do_one_greedy_optimization_job_in_threads(
		std::vector< masala::base::Size > const & starting_state,
		masala::base::Size const n_replicates,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP problem_ptr,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions,
		masala::base::Size const n_times_seen_multiplier
	) const;

	/// @brief Check a candiate solution against a problemd definition, and throw if there's a mismatch in node count, or if the choice indices
	/// are out of range of the problem.
	void
	check_starting_state_against_problem(
		std::vector< masala::base::Size > const & starting_state,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem
	) const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief A mutex for threadsafe operation.
	mutable std::mutex optimizer_mutex_;

	/// @brief The API description.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_description_;

	/// @brief The maximum number of CPU threads to request for parallel execution.
	/// @details The default of 0 means "all available".
	masala::base::Size cpu_threads_to_request_ = 0;

	/// @brief If starting states are not provided in the problem definition or in the optimizer configuration, indicate
	/// the number of random starting states to use.  Defaults to 1.
	masala::base::Size n_random_starting_states_ = 1;

	/// @brief By default, a single greedy optimization trajectory results in a solution being seen
	/// once.  This option lets other code specify that it should be higher.  Useful when other code,
	/// like the MonteCaroCostFunctionNetworkOptimizer, wants to use this for greedy refinement of
	/// solutions that it has produced many times.
	/// @note NOT part of the public-facing interface.  Intended only for use by code within this
	/// plugin sub-library.
	masala::base::Size n_times_seen_multiplier_ = 1;

	/// @brief Starting points to use, provided by the user during optimizer configuration rather than by the problem.
	/// @details At optimization time, these will result in a throw if the size of the state vector doesn't match the number of
	/// nodes or choices in the problem.
	std::vector< std::vector< masala::base::Size > > optimizer_starting_states_;

}; // class GreedyCostFunctionNetworkOptimizer

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins

#endif // StandardMaslaPlugins_src_optimizers_cost_function_network_GreedyCostFunctionNetworkOptimizer_hh