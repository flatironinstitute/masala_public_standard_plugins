/*
    Standard Masala Plugins
    Copyright (C) 2025 Vikram K. Mulligan

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

/// @file src/optimizers/cost_function_network/RandomCostFunctionNetworkOptimizer.fwd.hh
/// @brief Headers for a CostFunctionNetworkOptimizer that solves a cost function network problem by picking a
/// solution at random.
/// @details This is intended as a control during methods development, to compare a new solver to just choosing
/// a solution at random.  This is NOT intended to be a useful production solver.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_RandomCostFunctionNetworkOptimizer_fwd_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_RandomCostFunctionNetworkOptimizer_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

	class RandomCostFunctionNetworkOptimizer;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using RandomCostFunctionNetworkOptimizerSP = MASALA_SHARED_POINTER< RandomCostFunctionNetworkOptimizer >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using RandomCostFunctionNetworkOptimizerCSP = MASALA_SHARED_POINTER< RandomCostFunctionNetworkOptimizer const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using RandomCostFunctionNetworkOptimizerWP = MASALA_WEAK_POINTER< RandomCostFunctionNetworkOptimizer >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using RandomCostFunctionNetworkOptimizerCWP = MASALA_WEAK_POINTER< RandomCostFunctionNetworkOptimizer const >;

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins

#endif //StandardMaslaPlugins_src_optimizers_cost_function_network_RandomCostFunctionNetworkOptimizer_fwd_hh