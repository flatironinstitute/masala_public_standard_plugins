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

/// @file src/optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction.fwd.hh
/// @brief Forward declarations for a class for FunctionOfIntegerPenaltySumCostFunctions.
/// @details FunctionOfIntegerPenaltySumCostFunctions define a penalty function which is based on the following:
/// - A signed integer penalty is assigned to each choice.
/// - The selected choices' penalties are summed, and a constant is added.
/// - An arbitrary function (I->R) is applied to the sum, and this is returned as the penalty.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_FunctionOfIntegerPenaltySumCostFunction_fwd_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_FunctionOfIntegerPenaltySumCostFunction_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

	/// @brief Penalty function behaviour outside of the range specified.
	/// @details If you add to this list, update penalty_behaviour_string_from_enum().
	enum class PenaltyFunctionBehaviourOutsideRange {
		UNDEFINED_BEHAVIOUR = 0, // Keep first
		CONSTANT = 1, // Keep second
		LINEAR,
		QUADRATIC, // Keep second to last
		NUM_BEHAVIOURS = QUADRATIC // Keep last
	};

	class FunctionOfIntegerPenaltySumCostFunction;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using FunctionOfIntegerPenaltySumCostFunctionSP = MASALA_SHARED_POINTER< FunctionOfIntegerPenaltySumCostFunction >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using FunctionOfIntegerPenaltySumCostFunctionCSP = MASALA_SHARED_POINTER< FunctionOfIntegerPenaltySumCostFunction const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using FunctionOfIntegerPenaltySumCostFunctionWP = MASALA_WEAK_POINTER< FunctionOfIntegerPenaltySumCostFunction >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using FunctionOfIntegerPenaltySumCostFunctionCWP = MASALA_WEAK_POINTER< FunctionOfIntegerPenaltySumCostFunction const >;

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_FunctionOfIntegerPenaltySumCostFunction_fwd_hh