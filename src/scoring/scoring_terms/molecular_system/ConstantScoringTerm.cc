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

/// @file src/scoring/scoring_terms/molecular_system/ConstantScoringTerm.cc
/// @brief A scoring term that returns a constant value.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit headers:
#include <scoring/scoring_terms/molecular_system/ConstantScoringTerm.hh>

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
namespace scoring {
namespace scoring_terms {
namespace molecular_system {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION, DESTRUCTION, COPYING, AND CLONING
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the copy.
ConstantScoringTermSP
ConstantScoringTerm::clone() const {
	ConstantScoringTermSP new_copy( masala::make_shared< ConstantScoringTerm >( *this ) );
	return new_copy;
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
ConstantScoringTermSP
ConstantScoringTerm::deep_clone() const {
	ConstantScoringTermSP my_copy( clone() );
	my_copy->make_independent();
	return my_copy;
}

/// @brief Make this object independent by calling deep_clone on all contained objects.
/// @details All that this version does is to make the api description null.
void
ConstantScoringTerm::make_independent() {
	Parent::make_independent();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Every class can name itself.
/// @returns "ConstantScoringTerm".
std::string
ConstantScoringTerm::class_name() const {
	return "ConstantScoringTerm";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::scoring::scoring_terms::molecular_system".
std::string
ConstantScoringTerm::class_namespace() const {
	return "standard_masala_plugins::scoring::scoring_terms::molecular_system";
}

/// @brief Get a list of categories that this object could be sorted into.
/// @returns { { "ScoringTerm", "WholeMolecularSystemScoringTerm" } }
std::vector< std::vector< std::string > >
ConstantScoringTerm::get_categories() const {
	return Parent::get_categories();
}

/// @brief Get a list of keywords associated with this object.
/// @returns { "scoring_term", "whole_molecular_system", "constant" }
std::vector< std::string >
ConstantScoringTerm::get_keywords() const {
	std::vector< std::string > keywords( Parent::get_keywords() );
	keywords.push_back( "constant" );
	return keywords;
}

/// @brief Get an object describing the API for this object.
/// @note This is a weak pointer rather than a shared pointer since the
/// original object is expected to hold on to its API definition (which includes
/// funciton pointers to the functions of the instance).  Querying whether the
/// weak pointer can be converted to a shared pointer serves on a check as to
/// whether it is safe to use the function pointers.  Not ideal, but better than
/// nothing.
masala::base::api::MasalaObjectAPIDefinitionCWP
ConstantScoringTerm::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::constructor;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::work_function;
	using namespace masala::core_api::auto_generated_api::molecular_system;
	using namespace masala::core_api::auto_generated_api::selection::atom_selection;

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_description(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"A scoring term that returns a constant value.",
				false, false
			)
		);
		TODO TODO TODO;
		api_description->add_constructor(
			masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< ConstantScoringTerm > >(
				"ConstantScoringTerm", "Default constructor."
			)
		);
		api_description->add_constructor(
			masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< ConstantScoringTerm, ConstantScoringTerm const & > >(
				"ConstantScoringTerm", "Copy constructor.",
				"src", "The other instance of an ConstantScoringTerm that we are copying."
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< AtomSelection_APICSP, MolecularSystem_API const & > >(
				"generate_atom_selection", "Given a molecular system, generate a selection of atoms, by element type.",
				true, false, true, false,
				"molecular_system", "An input molecular system, for which a selection will be generated.",
				"atom_selection", "A selection of atoms generated from the input molecular system, by element type.",
				std::bind( &ConstantScoringTerm::generate_atom_selection, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_element_type", "Sets the element type, by abbreviation string.  Elements should be expressed with proper case (e.g. \"Na\" for sodium, not \"NA\").",
				"element_name", "The abbreviated name of the element, with proper capitalization.", false, false,
				std::bind( static_cast< void(ConstantScoringTerm::*)(std::string const &) >( &ConstantScoringTerm::set_element_type ), this, std::placeholders::_1 )
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
ConstantScoringTerm::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Selector", "AtomSelector" }
	};
}

/// @brief Get the keywords for this plugin class.
/// @returns { "standard_masala_plugins", "selector", "atom_selector",
///            "element", "elements" }
std::vector< std::string >
ConstantScoringTerm::get_keywords() const {
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