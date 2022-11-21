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

namespace standard_masala_plugins {
namespace selectors {
namespace atom_selectors {


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

	if( api_description_ == nullptr ) {
		MasalaObjectAPIDefinitionSP api_description(
			utility::pointer::make_shared< MasalaObjectAPIDefinition >(
				class_name(), class_namespace(),
				"An atom selector that selects atoms by element type.",
				false
			)
		);
	}
	//TODO TODO TODO CONTINUE HERE
	return api_description_;
}

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins