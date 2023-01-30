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
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_hh

// Forward declarations:
#include <optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/CostFunctionNetworkOptimizer.hh>

// Numeric API headers:
#include <numeric_api/types.hh>
#include <numeric_api/base_classes/optimization/annealing/AnnealingSchedule.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.fwd.hh>

// STL headers:
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

/// @brief A CostFunctionNetworkOptimizer that solves a cost function network problem using Monte Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
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
	/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "stochastic" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the class name.
	/// @returns "MonteCarloCostFunctionNetworkOptimizer".
	std::string class_name() const override;

	/// @brief Get the class namespace.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	std::string class_namespace() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the API definition for this object.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the number of threads to request.
	/// @details The default setting of 0 means "request all available".
	void set_cpu_threads_to_request( masala::numeric_api::Size const threads_in );

	/// @brief Set the number of times to try each problem.
	/// @details Minimum is 1.
	void set_attempts_per_problem( masala::numeric_api::Size const attempts_in );

	/// @brief Set the annealing schedule to use for annealing.
	/// @details Cloned on input.
	void set_annealing_schedule( masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule const & schedule_in );

	/// @brief Set the numer of Monte Carlo moves to make in each attempt.
	void set_annealing_steps_per_attempt( masala::numeric_api::Size const steps_in );

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the number of threads to request.
	/// @details The default setting of 0 means "request all available".
	masala::numeric_api::Size cpu_threads_to_request() const;

	/// @brief Get the number of times to try each problem.
	/// @details Minimum is 1.
	masala::numeric_api::Size attempts_per_problem() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
	/// @details Must be implemented by derived classes.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP
	run_cost_function_network_optimizer(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
	) const override;

private:

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
	run_mc_trajectory(
		masala::numeric_api::Size const annealing_steps,
		masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule const & annealing_schedule,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem,
		std::mutex & solutions_mutex,
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions
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
	masala::numeric_api::Size cpu_threads_to_request_ = 0;

	/// @brief The number of times to attempt each problem.
	masala::numeric_api::Size attempts_per_problem_ = 1;

	/// @brief The number of Monte Carlo steps to make per attempt.
	masala::numeric_api::Size annealing_steps_per_attempt_ = 100000;

	/// @brief The annealing schedule to use for annealing.
	masala::numeric_api::base_classes::optimization::annealing::AnnealingScheduleSP annealing_schedule_;

}; // class MonteCarloCostFunctionNetworkOptimizer

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins

#endif // StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_hh