/*
    Standard Masala Plugins
    Copyright (C) 2024 Vikram K. Mulligan

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

/// @file src/scoring/scoring_terms/molecular_system/ConstantScoringTerm.fwd.hh
/// @brief Forward declarations for a scoring term that returns a constant value.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_scoring_scoring_terms_molecular_system_ConstantScoringTerm_fwd_hh
#define Standard_Masala_Plugins_src_scoring_scoring_terms_molecular_system_ConstantScoringTerm_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace scoring {
namespace scoring_terms {
namespace molecular_system {

	class ConstantScoringTerm;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using ConstantScoringTermSP = MASALA_SHARED_POINTER< ConstantScoringTerm >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using ConstantScoringTermCSP = MASALA_SHARED_POINTER< ConstantScoringTerm const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using ConstantScoringTermWP = MASALA_WEAK_POINTER< ConstantScoringTerm >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using ConstantScoringTermCWP = MASALA_WEAK_POINTER< ConstantScoringTerm const >;

} // namespace molecular_system
} // namespace scoring_terms
} // namespace scoring
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_scoring_scoring_terms_molecular_system_ConstantScoringTerm_fwd_hh