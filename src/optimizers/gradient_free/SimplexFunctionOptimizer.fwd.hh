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

/// @file src/numeric_api/base_classes/optimization/gradient_free/SimplexFunctionOptimizer.fwd.hh
/// @brief Forward declarations for the SimplexFunctionOptimizer.
/// @details The SimplexFunctionOptimizer carries out local minimization of an arbitrary function
/// without using gradient information.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_free_SimplexFunctionOptimizer_fwd_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_free_SimplexFunctionOptimizer_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_free {

	class SimplexFunctionOptimizer;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using SimplexFunctionOptimizerSP = MASALA_SHARED_POINTER< SimplexFunctionOptimizer >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using SimplexFunctionOptimizerCSP = MASALA_SHARED_POINTER< SimplexFunctionOptimizer const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using SimplexFunctionOptimizerWP = MASALA_WEAK_POINTER< SimplexFunctionOptimizer >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using SimplexFunctionOptimizerCWP = MASALA_WEAK_POINTER< SimplexFunctionOptimizer const >;

} // namespace gradient_free
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_gradient_free_SimplexFunctionOptimizer_fwd_hh