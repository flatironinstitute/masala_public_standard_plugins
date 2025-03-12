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

/// @file src/optimizers/cost_function_network/cost_function/GraphBasedCostFunction.fwd.hh
/// @brief Forward declarations for a pure virtual base class for GraphBasedCostFunctions.
/// @details GraphBasedCostFunctions define a penalty function which is based on the following:
/// - A graph of interacting choices at all nodes is computed.
/// - During an optimization trajectory, the set of edges based on the currently-selected choices is maintained and updated.
/// - Derived classes compute something from this graph.  (The initial application is to compute the size of islands, and then
/// to sum some function of each island's size; this is used to promote mutually-connected structures like hydrogen bond networks
/// when designing peptides and proteins, and is analogous to Rosetta's hbnet scoring term.)
/// @note This class has been updated to be a template class.  Originally, it stored a boolean graph, but it makes
/// sense to allow other types of values to occupy edges (for instance, to support Rosetta's interdigitation_bonus
/// scoring term).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphBasedCostFunction_fwd_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphBasedCostFunction_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

	class GraphBasedCostFunction;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using GraphBasedCostFunctionSP = MASALA_SHARED_POINTER< GraphBasedCostFunction >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using GraphBasedCostFunctionCSP = MASALA_SHARED_POINTER< GraphBasedCostFunction const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using GraphBasedCostFunctionWP = MASALA_WEAK_POINTER< GraphBasedCostFunction >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using GraphBasedCostFunctionCWP = MASALA_WEAK_POINTER< GraphBasedCostFunction const >;

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphBasedCostFunction_fwd_hh