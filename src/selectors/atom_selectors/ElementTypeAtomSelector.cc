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

	AtomSelectionSP selection( std::make_shared< AtomSelection >() );
	selection->reserve( pose.total_atoms() );

	for( auto const & atom( pose.atoms_begin() ); atom != pose.atoms_end(); ++atom ) {
		if( atom->element() == element_ ) {
			selection->add_atom( atom );
		}
	}

	selection->shrink_to_fit();
	return selection;
}

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins