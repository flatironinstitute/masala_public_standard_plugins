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

/// @file src/optimizers/cost_function_network/cost_function/graph_island_based/LogOfGraphIslandCountCostFunction.fwd.hh
/// @brief Forward declarations for a class for LogOfGraphIslandCountCostFunctions.
/// @details LogOfGraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph, sum the counts in the islands over a minimum, and
/// return the natural log of the sum.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_LogOfGraphIslandCountCostFunction_fwd_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_LogOfGraphIslandCountCostFunction_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace graph_island_based {

	class LogOfGraphIslandCountCostFunction;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using LogOfGraphIslandCountCostFunctionSP = MASALA_SHARED_POINTER< LogOfGraphIslandCountCostFunction >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using LogOfGraphIslandCountCostFunctionCSP = MASALA_SHARED_POINTER< LogOfGraphIslandCountCostFunction const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using LogOfGraphIslandCountCostFunctionWP = MASALA_WEAK_POINTER< LogOfGraphIslandCountCostFunction >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using LogOfGraphIslandCountCostFunctionCWP = MASALA_WEAK_POINTER< LogOfGraphIslandCountCostFunction const >;

} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_LogOfGraphIslandCountCostFunction_fwd_hh