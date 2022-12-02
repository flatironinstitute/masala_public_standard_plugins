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
/// @brief A creator for an atom selector that selects atoms by element type.
/// @details Creators are needed to allow the Masala plugin system to use objects
/// defined in plugin libraries.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Header:
#include <selectors/atom_selectors/ElementTypeAtomSelectorCreator.hh>

// Project headers:
#include <selectors/atom_selectors/ElementTypeAtomSelector.hh>

// STL headers:
#include <string>

namespace standard_masala_plugins {
namespace selectors {
namespace atom_selectors {


////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Create an object of the desired type (in this case, an ElementTypeAtomSelector).
/// @returns A new instance of an ElementTypeAtomSelector.
/// @details Must be implemented by derived classes.
masala::base::managers::plugin_module::MasalaPluginSP
ElementTypeAtomSelectorCreator::create_plugin_object() const {
	return masala::make_shared< ElementTypeAtomSelector >();
}

/// @brief Return the names of the categories for this type of plugin object.
/// @returns In this case, returns { {"Selector", "AtomSelector"} }.
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector).
/// A plugin can be in more than one hierarchical category, but must be in at least one.  The
/// first one is used as the primary key.
std::vector< std::vector< std::string > >
ElementTypeAtomSelectorCreator::get_plugin_object_categories() const {
	return std::vector< std::vector< std::string > >{
		{ "Selector", "AtomSelector" }
	};
}

/// @brief Return keywords associated with this plugin module.
/// @returns This override returns { "standard_masala_plugins", "selector",
/// "atom_selector", "element", "elements" }.
std::vector< std::string >
ElementTypeAtomSelectorCreator::get_plugin_object_keywords() const {
	return std::vector< std::string >{
		"standard_masala_plugins",
		"selector",
		"atom_selector",
		"element",
		"elements"
	};
}

/// @brief Get The name of the class of object that this creator creates.
/// @returns This override returns "ElementTypeAtomSelector".
std::string
ElementTypeAtomSelectorCreator::get_plugin_object_name() const {
	return "ElementTypeAtomSelector";
}

/// @brief Get the namespace of the class of object that this creator creates.
/// @returns This override returns "standard_masala_plugins::selectors::atom_selectors".
std::string
ElementTypeAtomSelectorCreator::get_plugin_object_namespace() const {
	return "standard_masala_plugins::selectors::atom_selectors";
}

/// @brief Get the class name.
/// @returns Returns "ElementTypeAtomSelectorCreator".
std::string
ElementTypeAtomSelectorCreator::class_name() const {
	return "ElementTypeAtomSelectorCreator";
}

/// @brief Get the class namespace.
/// @returns Returns "standard_masala_plugins::selectors::atom_selectors".
std::string
ElementTypeAtomSelectorCreator::class_namespace() const {
	return "standard_masala_plugins::selectors::atom_selectors";
}


} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins
