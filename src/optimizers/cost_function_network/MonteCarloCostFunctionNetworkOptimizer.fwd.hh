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

/// @file src/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.fwd.hh
/// @brief Forward declarations for a CostFunctionNetworkOptimizer that solves a cost function network problem
/// using Monte Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_fwd_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

	class MonteCarloCostFunctionNetworkOptimizer;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using MonteCarloCostFunctionNetworkOptimizerSP = MASALA_SHARED_POINTER< MonteCarloCostFunctionNetworkOptimizer >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using MonteCarloCostFunctionNetworkOptimizerCSP = MASALA_SHARED_POINTER< MonteCarloCostFunctionNetworkOptimizer const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using MonteCarloCostFunctionNetworkOptimizerWP = MASALA_WEAK_POINTER< MonteCarloCostFunctionNetworkOptimizer >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using MonteCarloCostFunctionNetworkOptimizerCWP = MASALA_WEAK_POINTER< MonteCarloCostFunctionNetworkOptimizer const >;

	/// @brief An enum class for the modes for storing solutions.
	/// @details CHECK_AT_EVERY_STEP promotes diversity at the expense of slower computation,
	/// checking every solution considered to see whether it should be stored. CHECK_ON_ACCEPTANCE
	/// only checks whether to store a solution when it is accepted.
	/// @note If you add to this, update the function
	/// MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_string_from_enum().
	enum class MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode {
		INVALID_MODE=0, // Keep this first.
		CHECK_AT_EVERY_STEP,
		CHECK_ON_ACCEPTANCE, // Keep this second-to-last.
		NUM_SOLUTION_STORAGE_MODES = CHECK_ON_ACCEPTANCE // Keep this last.
	};

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins

#endif //StandardMaslaPlugins_src_optimizers_cost_function_network_MonteCarloCostFunctionNetworkOptimizer_fwd_hh