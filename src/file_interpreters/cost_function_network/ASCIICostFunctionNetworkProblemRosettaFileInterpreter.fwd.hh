/*
    Masala
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

/// @file src/file_interpreters/cost_function_network/ASCIICostFunctionNetworkProblemRosettaFileInterpreter.fwd.hh
/// @brief Forward declarations for a file interpreter for reading and writing ASCII files that define cost function network optimization problems.
/// @author Tristan Zaborniak (tzaborniak@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_file_interpreters_cost_function_network_ASCIICostFunctionNetworkProblemRosettaFileInterpreter_fwd_hh
#define StandardMaslaPlugins_src_file_interpreters_cost_function_network_ASCIICostFunctionNetworkProblemRosettaFileInterpreter_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace file_interpreters {
namespace cost_function_network {

	class ASCIICostFunctionNetworkProblemRosettaFileInterpreter;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using ASCIICostFunctionNetworkProblemRosettaFileInterpreterSP = MASALA_SHARED_POINTER< ASCIICostFunctionNetworkProblemRosettaFileInterpreter >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using ASCIICostFunctionNetworkProblemRosettaFileInterpreterCSP = MASALA_SHARED_POINTER< ASCIICostFunctionNetworkProblemRosettaFileInterpreter const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using ASCIICostFunctionNetworkProblemRosettaFileInterpreterWP = MASALA_WEAK_POINTER< ASCIICostFunctionNetworkProblemRosettaFileInterpreter >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using ASCIICostFunctionNetworkProblemRosettaFileInterpreterCWP = MASALA_WEAK_POINTER< ASCIICostFunctionNetworkProblemRosettaFileInterpreter const >;

} // namespace cost_function_network
} // namespace file_interpreters
} // namesapce standard_masala_plugins

#endif //StandardMaslaPlugins_src_file_interpreters_cost_function_network_ASCIICostFunctionNetworkProblemRosettaFileInterpreter_fwd_hh