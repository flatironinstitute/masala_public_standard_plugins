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

/// @file src/optimizers/cost_function_network/cost_function/feature_based/SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction.fwd.hh
/// @brief Forward declarations for SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunctions class.
/// @details SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunctions define a penalty function which is based on the following:
/// - One or more features are defined for each choice at each node.  (A feature could be a hydrogen bond
/// donor or acceptor, for instance, in a packing problem)
/// - Each feature has an expected minimum and maximum number of connections that can be set.
/// - For each pair of choices, the features that are connected can be set.
/// - At calculation time, the SQUARE OF THE number of unsatisfied features (total features minus features
/// satisfied by connections) is returned.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_feature_based_SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_fwd_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_feature_based_SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace feature_based {

	class SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunctionSP = MASALA_SHARED_POINTER< SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunctionCSP = MASALA_SHARED_POINTER< SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunctionWP = MASALA_WEAK_POINTER< SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunctionCWP = MASALA_WEAK_POINTER< SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction const >;

} // namespace feature_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_feature_based_SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_fwd_hh