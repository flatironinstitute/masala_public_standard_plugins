/*
MIT License

Copyright (c) 2022 Vikram K. Mulligan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/// @file src/selectors/atom_selectors/ElementTypeAtomSelector.cc
/// @brief Headers for an atom selector that selects atoms by element type.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit headers:
#include <selectors/atom_selectors/ElementTypeAtomSelector.hh>

// Core API headers:
#include <core_api/auto_generated_api/pose/Pose_API.hh>
#include <core_api/auto_generated_api/chemistry/Molecules_API.hh>
#include <core_api/auto_generated_api/chemistry/atoms/AtomInstance_API.hh>
#include <core_api/auto_generated_api/chemistry/atoms/AtomInstanceConstIterator_API.hh>
#include <core_api/base_classes/selectors/atom_selectors/AtomSelection.hh>

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
masala::core_api::base_classes::selectors::atom_selectors::AtomSelectorSP
ElementTypeAtomSelector::clone() const {
	ElementTypeAtomSelectorSP new_copy( std::make_shared< ElementTypeAtomSelector >( *this ) );
	new_copy->api_description_ = nullptr;
	return new_copy;
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
masala::core_api::base_classes::selectors::atom_selectors::AtomSelectorSP
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

/// @brief Given the current pose, generate the atom selection.
/// @details Pure virtual function.
masala::core_api::base_classes::selectors::atom_selectors::AtomSelectionCSP
ElementTypeAtomSelector::generate_atom_selection(
	masala::core_api::auto_generated_api::pose::Pose_API const & pose
) const {
	using masala::core_api::base_classes::selectors::atom_selectors::AtomSelection;
	using masala::core_api::base_classes::selectors::atom_selectors::AtomSelectionSP;

	AtomSelectionSP selection( std::make_shared< AtomSelection >() );
	selection->reserve( pose.molecules_shared_ptr()->total_atoms() );

	for(
		auto atom( pose.molecules_shared_ptr()->atoms_begin() );
		atom != pose.molecules_shared_ptr()->atoms_end();
		++atom
	) {
		if( atom.ptr()->element_type_enum() == element_ ) {
			selection->add_atom( atom.ptr() );
		}
	}

	selection->shrink_to_fit();
	return selection;
}

/// @brief Set the element type, by string.
void
ElementTypeAtomSelector::set_element_type(
	std::string const & element_name
) {
	using namespace masala::core::chemistry::atoms;
	ElementTypeEnum const element_type( element_enum_from_name( element_name ) );
	CHECK_OR_THROW_FOR_CLASS( element_type != ElementTypeEnum::INVALID_ELEMENT_TYPE, "set_element_type", "Could not interpret \"" + element_name + "\" as a valid element type." );
	set_element_type( element_type );
}

/// @brief Set the element type, by enum.
void
ElementTypeAtomSelector::set_element_type(
	masala::core::chemistry::atoms::ElementTypeEnum const element_type
) {
	using namespace masala::core::chemistry::atoms;
	CHECK_OR_THROW_FOR_CLASS(
		static_cast< signed long >( element_type ) > 0 &&
		element_type <= ElementTypeEnum::NUM_KNOWN_ELEMENTS,
		"set_element_type",
		"Invalid element enum passed to this function."
	);
	element_ = element_type;
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
	using namespace masala::core_api::auto_generated_api::pose;

	if( api_description_ == nullptr ) {
		MasalaObjectAPIDefinitionSP api_description(
			std::make_shared< MasalaObjectAPIDefinition >(
				class_name(), class_namespace(),
				"An atom selector that selects atoms by element type.",
				false
			)
		);
		api_description->add_constructor(
			std::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< ElementTypeAtomSelector > >(
				"ElementTypeAtomSelector", "Default constructor."
			)
		);
		api_description->add_constructor(
			std::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< ElementTypeAtomSelector, ElementTypeAtomSelector const & > >(
				"ElementTypeAtomSelector", "Copy constructor.",
				"src", "The other instance of an ElementTypeAtomSelector that we are copying."
			)
		);
		api_description->add_work_function(
			std::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< AtomSelectionCSP, Pose_API const & > >(
				"generate_atom_selection", "Given a pose, generate a selection of atoms, by element type.",
				true, false,
				"pose", "An input pose, for which a selection will be generated.",
				"atom_selection", "A selection of atoms generated from the input pose, by element type.",
				std::bind( &ElementTypeAtomSelector::generate_atom_selection, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			std::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string > >(
				"set_element_type", "Sets the element type, by abbreviation string.  Elements should be expressed with proper case (e.g. \"Na\" for sodium, not \"NA\").",
				"element_name", "The abbreviated name of the element, with proper capitalization.",
				std::bind( static_cast< void(ElementTypeAtomSelector::*)(std::string const &) >( &ElementTypeAtomSelector::set_element_type ), this, std::placeholders::_1 )
			)
		);

		api_description_ = api_description;
	}

	return api_description_;
}

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins