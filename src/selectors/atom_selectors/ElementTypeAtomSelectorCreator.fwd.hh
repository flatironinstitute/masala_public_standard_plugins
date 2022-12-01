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

/// @file src/selectors/atom_selectors/ElementTypeAtomSelectorCreator.cc
/// @brief Forward declarations for a creator for an atom selector that selects
/// atoms by element type.
/// @details Creators are needed to allow the Masala plugin system to use objects
/// defined in plugin libraries.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelectorCreator_fwd_hh
#define Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelectorCreator_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace selectors {
namespace atom_selectors {

	class ElementTypeAtomSelectorCreator;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using ElementTypeAtomSelectorCreatorSP = MASALA_SHARED_POINTER< ElementTypeAtomSelectorCreator >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using ElementTypeAtomSelectorCreatorCSP = MASALA_SHARED_POINTER< ElementTypeAtomSelectorCreator const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using ElementTypeAtomSelectorCreatorWP = MASALA_WEAK_POINTER< ElementTypeAtomSelectorCreator >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using ElementTypeAtomSelectorCreatorCWP = MASALA_WEAK_POINTER< ElementTypeAtomSelectorCreator const >;

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelectorCreator_fwd_hh