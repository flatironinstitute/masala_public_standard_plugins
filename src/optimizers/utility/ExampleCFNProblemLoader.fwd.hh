/*
    Masala
    Copyright (C) 2026 Vikram K. Mulligan

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

/// @file src/optimizers/utility/ExampleCFNProblemLoader.fwd.hh
/// @brief Forward declarations for a utility class that loads 100 example CFN problems, which are small enough to solve
/// using the Toulbar2 deterministic solver.
/// @details These problems are intended for unit-testing new CFN solvers.  Pretty much any CFN solver
/// should be able to find the optimal solution for these.  The solutions are also produced by the loader.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_utility_ExampleCFNProblemLoader_fwd_hh
#define Standard_Masala_Plugins_src_optimizers_utility_ExampleCFNProblemLoader_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace utility {

	class ExampleCFNProblemLoader;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using ExampleCFNProblemLoaderSP = MASALA_SHARED_POINTER< ExampleCFNProblemLoader >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using ExampleCFNProblemLoaderCSP = MASALA_SHARED_POINTER< ExampleCFNProblemLoader const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using ExampleCFNProblemLoaderWP = MASALA_WEAK_POINTER< ExampleCFNProblemLoader >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using ExampleCFNProblemLoaderCWP = MASALA_WEAK_POINTER< ExampleCFNProblemLoader const >;

} // namespace utility
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_utility_ExampleCFNProblemLoader_fwd_hh