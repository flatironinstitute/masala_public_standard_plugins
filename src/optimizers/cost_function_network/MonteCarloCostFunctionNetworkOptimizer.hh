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

/// @file src/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.hh
/// @brief Headers for a CostFunctionNetworkOptimizer that solves a cost function network problem using Monte
/// Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_hh

// Forward declarations:
#include <optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/CostFunctionNetworkOptimizer.hh>

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

/// @brief A CostFunctionNetworkOptimizer that solves a cost function network problem using Monte Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class MonteCarloCostFunctionNetworkOptimizer : public masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	MonteCarloCostFunctionNetworkOptimizer() = default;

	/// @brief Copy constructor.
	/// @details Needed since we define a mutex.
	MonteCarloCostFunctionNetworkOptimizer( MonteCarloCostFunctionNetworkOptimizer const & );

	/// @brief Assignment operator.
	/// @details Needed since we define a mutex.
	MonteCarloCostFunctionNetworkOptimizer &
	operator=( MonteCarloCostFunctionNetworkOptimizer const & );

	/// @brief Make a copy of this object that's wholly independent.
	MonteCarloCostFunctionNetworkOptimizerSP
	deep_clone() const;

	/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
	void
	make_independent();

	/// @brief Destructor.
	~MonteCarloCostFunctionNetworkOptimizer() override = default;

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
	/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "simulated_annealing", "stochastic" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the category that this MasalaEngine fits into.
	/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
	std::vector< std::vector< std::string > >
	get_engine_categories() const override;

	/// @brief Get the class name.
	/// @returns "MonteCarloCostFunctionNetworkOptimizer".
	std::string class_name() const override;

	/// @brief Get the class namespace.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	std::string class_namespace() const override;

	/// @brief Get the class name (static version).
	/// @returns "MonteCarloCostFunctionNetworkOptimizer".
	static std::string class_name_static();

	/// @brief Get the class namespace (static version).
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC STATIC ENUM FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the storage modes, as a list separated by a delimiter.
	/// @param delimiter The delimiter to use.  (For instance, ", " for a comma-separated list).
	/// @param include_and If true, the string "and " is inserted before the last entry.
	static std::string solution_storage_mode_strings( std::string const & delimiter, bool const include_and );

	/// @brief Given a solution storage mode as an enum, get the corresponding string.
	/// @param mode_enum The solution storage mode.
	/// @note Throws if the mode is INVALID.
	static std::string solution_storage_mode_string_from_enum( MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const mode_enum );

	/// @brief Given a solution storage mode as a string, get the corresponding enum.
	/// @param mode_string The solution storage mode, expressed as a string.
	/// @returns The corresponding enum, or INVALID if the string could not be parsed.
	static MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode solution_storage_mode_enum_from_string( std::string const & mode_string );

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

	/// @brief Set the number of times to try each problem.
	/// @details Minimum is 1.
	void set_attempts_per_problem( masala::base::Size const attempts_in );

	/// @brief Set the number of solutions to return for each problem.
	void set_n_solutions_to_store_per_problem( masala::base::Size const n_solutions_in );

	/// @brief Set the annealing schedule to use for annealing.
	/// @details Cloned on input.
	void set_annealing_schedule( masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & schedule_in );

	/// @brief Set the annealing schedule by name.
	/// @details Namespace is not required unless the name is not unique.  Throws if
	/// the name is not found at all in the plugin manager.
	void set_annealing_schedule_by_name( std::string const & schedule );

	/// @brief Set the numer of Monte Carlo moves to make in each attempt.
	void set_annealing_steps_per_attempt( masala::base::Size const steps_in );

	/// @brief Set the solution storage mode, by enum.
	void set_solution_storage_mode( MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const solution_storage_mode_in );

	/// @brief Set the solution storage mode, by string.
	void set_solution_storage_mode( std::string const & solution_storage_mode_string_in );

	/// @brief Set whether we're using multimutations.
	/// @details If true, we select the number of mutation positions from a Poisson distribution.  If false, we only
	/// mutate one node at a time.  True by default.
	/// @note We actually take a Poisson distribution and add 1, since we don't want 0 mutations.
	void set_use_multimutation( bool const setting );

	/// @brief Set the probability of having 1 mutation.  Must be a value between 0 and 1.  Default 0.75.
	/// @details Used to find the value of lambda for the Poisson distribution.  Since we add 1 to the value
	/// that comes out of the Poisson distribution, the value of P(0) is set to this value:
	/// P(k) = lambda^k exp(-lambda) / k!
	/// P(0) = exp(-lambda)
	/// -ln( P(0) ) = lambda
	/// @note Throws if outside of the range (0, 1].
	void
	set_multimutation_probability_of_one_mutation(
		masala::base::Real const probability_in
	);

	/// @brief Set whether we do greedy optimization at the end on each solution found by the Monte Carlo search.
	/// False by default.
	void set_do_greedy_refinement( bool const do_greedy_refinement_in );

	/// @brief Set whether we keep both the original solutions and the ones found by greedy refinement if doing
	/// greedy optimization (for greater diversity).  If false, we just keep the greedy solutions (for lower
	/// scores).  True by default.  Not used if do_greedy_refinement_ is false.
	void set_keep_original_mc_solutions_alongside_greedy_refinement_solutions( bool const keep_original_solutions_in );

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the number of threads to request.
	/// @details The default setting of 0 means "request all available".
	masala::base::Size cpu_threads_to_request() const;

	/// @brief Get the number of times to try each problem.
	/// @details Minimum is 1.
	masala::base::Size attempts_per_problem() const;

	/// @brief Get the number of solutions to return for each problem.
	masala::base::Size n_solutions_to_store_per_problem() const;

	/// @brief Get the numer of Monte Carlo moves to make in each attempt.
	masala::base::Size annealing_steps_per_attempt() const;

	/// @brief Const access to the annealing schedule (to allow its configuration to be examined).
	/// @details The annealing schedule must be set before this is called.  Throws otherwise.
	masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & annealing_schedule() const;

	/// @brief Get the solution storage mode, by enum.
	MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode solution_storage_mode_enum() const;

	/// @brief Get the solution storage mode, by string.
	std::string solution_storage_mode_string() const;

	/// @brief Get whether we're using multimutations.
	/// @details If true, we select the number of mutation positions from a Poisson distribution.  If false, we only
	/// mutate one node at a time.  True by default.
	/// @note We actually take a Poisson distribution and add 1, since we don't want 0 mutations.
	bool use_multimutation() const;

	/// @brief Get the probability of having 1 mutation.  Must be a value between 0 and 1.  Default 0.75.
	masala::base::Real multimutation_probability_of_one_mutation() const;

	/// @brief Get whether we do greedy optimization at the end on each solution found by the Monte Carlo search.
	/// False by default.
	bool do_greedy_refinement() const;

	/// @brief Get whether we keep both the original solutions and the ones found by greedy refinement if doing
	/// greedy optimization (for greater diversity).  If false, we just keep the greedy solutions (for lower
	/// scores).  True by default.  Not used if do_greedy_refinement_ is false.
	bool keep_original_mc_solutions_alongside_greedy_refinement_solutions() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Access the annealing schedule by nonconst reference (to allow it to be configured).
	/// @details The annealing schedule must be set before this is called.  Throws otherwise.
	masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API & annealing_schedule_nonconst();

	/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
	/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
	/// the problem with the same index.
	std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
	run_cost_function_network_optimizer(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
	) const override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Perform greedy refinement on all solutions found.
	void
	carry_out_greedy_refinement(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems,
		std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP > & solutions_by_problem,
		bool keep_original_mc_solutions_alongside_greedy
	) const;

	/// @brief Run a single Monte Carlo trajectory.
	/// @details This function runs in threads.
	/// @param[in] replicate_index The index of this replicate for this problem.
	/// @param[in] problem_index The index of this problem.
	/// @param[in] annealing_steps The number of steps in the trajectory.
	/// @param[in] n_solutions_to_store The number of solutions to store.
	/// @param[in] annealing_schedule The temperature generator (already configured with the number of steps).
	/// @param[in] problem The description of the problem.  This may or may not be a specialized problem like a
	/// PrecomputedPairwiseCostFunctionNetworkOptimizationProblem.
	/// @param[in] solutions Storage for a collection of solutions.  Should be unique to problem.
	/// @param[in] solution_storage_mode The mode for storing solutions.
	/// @param[in] use_multimutation If true, we do N mutations, where N is chosen from a Poisson distribution.
	/// If false, we do one mutation at a time.
	/// @param[in] multimutation_probability_of_one_mutation The probability of just doing one mutation in
	/// multimutation mode.
	/// @param solutions_mutex A mutex for the collection of solutions.
	void
	run_mc_trajectory(
		masala::base::Size const replicate_index,
		masala::base::Size const problem_index,
		masala::base::Size const annealing_steps,
		masala::base::Size const n_solutions_to_store,
		masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & annealing_schedule,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP problem,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions,
		MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const solution_storage_mode,
		bool const use_multimutation,
		masala::base::Real const multimutation_probability_of_one_mutation,
		std::mutex & solutions_mutex
	) const;

	/// @brief Make a Monte Carlo move.
	/// @param current_solution The current solution, as a vector of choice indices for all variable positions.  Changed by this operation.
	/// @param n_choices_per_variable_node Number of choices per variable node, in the same order as current_solution.  The pairs are
	/// (node index, number of choices).  The node index is ABSOLUTE.
	/// @param randgen The handle of the Masala random generator.
	static
	void
	make_mc_move(
		std::vector< masala::base::Size > & current_solution,
		std::vector< std::pair< masala::base::Size, masala::base::Size > > const & n_choices_per_variable_node,
		masala::base::managers::random::MasalaRandomNumberGeneratorHandle const randgen
	);

	/// @brief Make a Monte Carlo move that introduces many mutations, where the number of mutations is sampled from a
	/// Poisson distribution.
	/// @param current_solution The current solution, as a vector of choice indices for all variable positions.  Changed by this operation.
	/// @param n_choices_per_variable_node Number of choices per variable node, in the same order as current_solution.  The pairs are
	/// (node index, number of choices).  The node index is based on VARIABLE nodes.
	/// @param poisson_lambda The parameter lambda in the Poisson distribution of the number of mutations to introduce.
	/// @param randgen The handle of the Masala random generator.
	static
	void
	make_mc_multimove(
		std::vector< masala::base::Size > & current_solution,
		std::vector< std::pair< masala::base::Size, masala::base::Size > > const & n_choices_per_variable_node,
		masala::base::Real const poisson_lambda,
		masala::base::managers::random::MasalaRandomNumberGeneratorHandle const randgen
	);

	/// @brief Determine whether to add the current solution to the set of solutions stored for this replicate attempt.
	/// @details We maintain a list of N solutions for each replicate attempt.  If this solution has been encountered before
	/// and is already in the list, we increment the count for the number of times it has been seen.  If not, and if fewer
	/// than N solutions have been stored, we append the solution in a CostFunctionNetworkOptimizationSolution container.  If
	/// the solution has not been seen, N solutions are stored, and this solution is lower-energy than the highest-energy
	/// solution, we replace the highest-energy solution with this one.
	/// @param[in] current_solution The solution that we are considering, represented as a vector of choice indices where each
	/// entry in the vector corresponds to a variable node (in order).
	/// @param[in] current_absolute_score The absolute score of this solution.
	/// @param[inout] solutions The container of solutions.  This should be a thread-local copy.  This is a vector of tuples, where
	/// each tuple is ( solution vector for variable nodes, solution score, number of times solution was seen ).
	/// @param[in] n_solutions_to_store The number of solutions to store.
	/// @param[in] force_store If true, we always store this solution.  If false, we use conditional logic.
	static
	void
	determine_whether_to_store_solution(
		std::vector< masala::base::Size > const & current_solution,
		masala::base::Real const current_absolute_score,
		std::vector< std::tuple < std::vector< masala::base::Size >, masala::base::Real, masala::base::Size > > & solutions,
		masala::base::Size const n_solutions_to_store,
		bool const force_store
	);

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

	/// @brief The number of times to attempt each problem.
	masala::base::Size attempts_per_problem_ = 1;

	/// @brief The number of solutions to store for each problem.
	masala::base::Size n_solutions_to_store_per_problem_ = 1;

	/// @brief The number of Monte Carlo steps to make per attempt.
	masala::base::Size annealing_steps_per_attempt_ = 100000;

	/// @brief If true, we select the number of mutation positions from a Poisson distribution.  If false, we only
	/// mutate one node at a time.  True by default.
	/// @details We actually take a Poisson distribution and add 1, since we don't want 0 mutations.
	bool use_multimutation_ = true;

	/// @brief If true, we do greedy optimization at the end on each solution found by the Monte Carlo search.
	/// False by default.
	bool do_greedy_refinement_ = false;

	/// @brief If true, we keep both the original solutions and the ones found by greedy refinement if doing
	/// greedy optimization (for greater diversity).  If false, we just keep the greedy solutions (for lower
	/// scores).  True by default.  Not used if do_greedy_refinement_ is false.
	bool keep_original_mc_solutions_alongside_greedy_refinement_solutions_ = true;

	/// @brief The probability of having 1 mutation.  Must be a value between 0 and 1.  Default 0.75.
	/// @details Used to find the value of lambda for the Poisson distribution.  Since we add 1 to the value
	/// that comes out of the Poisson distribution, the value of P(0) is set to this value:
	/// P(k) = lambda^k exp(-lambda) / k!
	/// P(0) = exp(-lambda)
	/// -ln( P(0) ) = lambda
	masala::base::Real multimutation_probability_of_one_mutation_ = 0.75;

	/// @brief The annealing schedule to use for annealing.
	masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_APISP annealing_schedule_;

	/// @brief The mode for checking whether to store a solution.
	/// @details CHECK_AT_EVERY_STEP (default) promotes diversity at the expense of slower computation,
	/// checking every solution considered to see whether it should be stored. CHECK_ON_ACCEPTANCE
	/// only checks whether to store a solution when it is accepted.
	MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode solution_storage_mode_ = MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::CHECK_AT_EVERY_STEP;

}; // class MonteCarloCostFunctionNetworkOptimizer

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins

#endif // StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_hh