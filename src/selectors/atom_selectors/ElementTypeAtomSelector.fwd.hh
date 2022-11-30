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

/// @file src/selectors/atom_selectors/ElementTypeAtomSelector.fwd.hh
/// @brief Forward declarations for an atom selector that selects atoms by element type.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_fwd_hh
#define Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_fwd_hh

#include <memory> // For std::shared_ptr

namespace standard_masala_plugins {
namespace selectors {
namespace atom_selectors {

	class ElementTypeAtomSelector;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a std::shared_ptr for objects of that class.
	using ElementTypeAtomSelectorSP = std::shared_ptr< ElementTypeAtomSelector >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a std::shared_ptr for const objects of that class.
	using ElementTypeAtomSelectorCSP = std::shared_ptr< ElementTypeAtomSelector const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a std::weak_ptr for objects of that class.
	using ElementTypeAtomSelectorWP = std::weak_ptr< ElementTypeAtomSelector >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a std::weak_ptr for const objects of that class.
	using ElementTypeAtomSelectorCWP = std::weak_ptr< ElementTypeAtomSelector const >;

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_fwd_hh