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

/// @file src/selectors/atom_selectors/ElementTypeAtomSelector.cc
/// @brief Headers for an atom selector that selects atoms by element type.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit headers:
#include <selectors/atom_selectors/ElementTypeAtomSelector.hh>

// Core API headers:
#include <core_api/auto_generated_api/molecular_system/MolecularSystem_API.hh>
#include <core_api/auto_generated_api/chemistry/MolecularGeometry_API.hh>
#include <core_api/auto_generated_api/chemistry/atoms/AtomInstance_API.hh>
#include <core_api/auto_generated_api/chemistry/atoms/AtomInstanceConstIterator_API.hh>
#include <core_api/auto_generated_api/selection/atom_selection/AtomSelection_API.hh>

// Base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_ZeroInput.tmpl.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>

namespace standard_masala_plugins {
namespace selectors {
namespace atom_selectors {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION, DESTRUCTION, COPYING, AND CLONING
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the copy.
ElementTypeAtomSelectorSP
ElementTypeAtomSelector::clone() const {
	ElementTypeAtomSelectorSP new_copy( masala::make_shared< ElementTypeAtomSelector >( *this ) );
	new_copy->api_description_ = nullptr;
	return new_copy;
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
ElementTypeAtomSelectorSP
ElementTypeAtomSelector::deep_clone() const {
	return clone(); // There are no shared data, so deep cloning is like cloning.
}

/// @brief Make this object independent by calling deep_clone on all contained objects.
/// @details All that this version does is to make the api description null.
void
ElementTypeAtomSelector::make_independent() {
	api_description_ = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given the current molecular system, generate the atom selection.
masala::core_api::auto_generated_api::selection::atom_selection::AtomSelection_APICSP
ElementTypeAtomSelector::generate_atom_selection(
	masala::core_api::auto_generated_api::molecular_system::MolecularSystem_API const & molecular_system
) const {
	using masala::core_api::auto_generated_api::selection::atom_selection::AtomSelection_API;
	using masala::core_api::auto_generated_api::selection::atom_selection::AtomSelection_APISP;

	AtomSelection_APISP selection( masala::make_shared< AtomSelection_API >() );

	for(
		auto atom( molecular_system.molecular_geometry_shared_ptr()->atoms_begin() );
		atom != molecular_system.molecular_geometry_shared_ptr()->atoms_end();
		++atom
	) {
		if( atom.ptr()->element_type_enum() == element_ ) {
			selection->add_atom( atom.ptr() );
		}
	}

	return selection;
}

/// @brief Set the element type, by string.
void
ElementTypeAtomSelector::set_element_type(
	std::string const & element_name
) {
	using namespace masala::base::managers::database::elements;
	ElementTypeEnum const element_type( element_enum_from_name( element_name ) );
	CHECK_OR_THROW_FOR_CLASS( element_type != ElementTypeEnum::INVALID_ELEMENT_TYPE, "set_element_type", "Could not interpret \"" + element_name + "\" as a valid element type." );
	set_element_type( element_type );
}

/// @brief Set the element type, by enum.
void
ElementTypeAtomSelector::set_element_type(
	masala::base::managers::database::elements::ElementTypeEnum const element_type
) {
	using namespace masala::base::managers::database::elements;
	CHECK_OR_THROW_FOR_CLASS(
		static_cast< signed long >( element_type ) > 0 &&
		element_type <= ElementTypeEnum::NUM_KNOWN_ELEMENTS,
		"set_element_type",
		"Invalid element enum passed to this function."
	);
	element_ = element_type;
}

/// @brief Get the element type, by string.
std::string
ElementTypeAtomSelector::get_element_type_name() const {
	return masala::base::managers::database::elements::element_name_from_enum( element_ );
}

/// @brief Get the element type, by enum.
masala::base::managers::database::elements::ElementTypeEnum
ElementTypeAtomSelector::get_element_type_enum() const {
	return element_;
}

/// @brief Every class can name itself.
/// @returns "ElementTypeAtomSelector".
std::string
ElementTypeAtomSelector::class_name() const {
	return "ElementTypeAtomSelector";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::selectors::atom_selectors".
std::string
ElementTypeAtomSelector::class_namespace() const {
	return "standard_masala_plugins::selectors::atom_selectors";
}

/// @brief Get an object describing the API for this object.
/// @note This is a weak pointer rather than a shared pointer since the
/// original object is expected to hold on to its API definition (which includes
/// funciton pointers to the functions of the instance).  Querying whether the
/// weak pointer can be converted to a shared pointer serves on a check as to
/// whether it is safe to use the function pointers.  Not ideal, but better than
/// nothing.
masala::base::api::MasalaObjectAPIDefinitionCWP
ElementTypeAtomSelector::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::constructor;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::work_function;
	using namespace masala::core_api::base_classes::selectors::atom_selectors;
	using namespace masala::core_api::auto_generated_api::molecular_system;
	using namespace masala::core_api::auto_generated_api::selection::atom_selection;

	if( api_description_ == nullptr ) {
		MasalaObjectAPIDefinitionSP api_description(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"An atom selector that selects atoms by element type.",
				false, false
			)
		);
		api_description->add_constructor(
			masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< ElementTypeAtomSelector > >(
				"ElementTypeAtomSelector", "Default constructor."
			)
		);
		api_description->add_constructor(
			masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< ElementTypeAtomSelector, ElementTypeAtomSelector const & > >(
				"ElementTypeAtomSelector", "Copy constructor.",
				"src", "The other instance of an ElementTypeAtomSelector that we are copying."
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< AtomSelection_APICSP, MolecularSystem_API const & > >(
				"generate_atom_selection", "Given a molecular system, generate a selection of atoms, by element type.",
				true, false, true, false,
				"molecular_system", "An input molecular system, for which a selection will be generated.",
				"atom_selection", "A selection of atoms generated from the input molecular system, by element type.",
				std::bind( &ElementTypeAtomSelector::generate_atom_selection, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_element_type", "Sets the element type, by abbreviation string.  Elements should be expressed with proper case (e.g. \"Na\" for sodium, not \"NA\").",
				"element_name", "The abbreviated name of the element, with proper capitalization.", false, false,
				std::bind( static_cast< void(ElementTypeAtomSelector::*)(std::string const &) >( &ElementTypeAtomSelector::set_element_type ), this, std::placeholders::_1 )
			)
		);

		api_description_ = api_description;
	}

	return api_description_;
}

////////////////////////////////////////////////////////////////////////////////
// PLUGIN PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.
/// @returns { { "Selector", "AtomSelector" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outher vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
ElementTypeAtomSelector::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Selector", "AtomSelector" }
	};
}

/// @brief Get the keywords for this plugin class.
/// @returns { "standard_masala_plugins", "selector", "atom_selector",
///            "element", "elements" }
std::vector< std::string >
ElementTypeAtomSelector::get_keywords() const {
	return std::vector< std::string > {
		"standard_masala_plugins",
		"selector",
		"atom_selector",
		"element",
		"elements"
	};
}

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins