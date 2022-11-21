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

/// @file src/selectors/atom_selectors/ElementTypeAtomSelector.hh
/// @brief Headers for an atom selector that selects atoms by element type.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_hh
#define Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_hh

// Parent class:
#include <core_api/base_classes/selectors/atom_selectors/AtomSelector.hh>

// Forward declarations:
#include <selectors/atom_selectors/ElementTypeAtomSelector.fwd.hh>

// Core API headers:
#include <core/chemistry/atoms/ElementType.fwd.hh>

namespace standard_masala_plugins {
namespace selectors {
namespace atom_selectors {


/// @brief Headers for an atom selector that selects atoms by element type.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class ElementTypeAtomSelector : public masala::core_api::base_classes::selectors::atom_selectors::AtomSelector {

public:

	/// @brief Default constructor.
	ElementTypeAtomSelector() = default;

	/// @brief Copy constructor.
	ElementTypeAtomSelector( ElementTypeAtomSelector const & ) = default;

	/// @brief Destructor.
	~ElementTypeAtomSelector() override = default;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Given the current pose, generate the atom selection.
	/// @details Pure virtual function.
	masala::core_api::base_classes::selectors::atom_selectors::AtomSelectionCSP
	generate_atom_selection(
		masala::core_api::auto_generated_api::pose::Pose_API const & pose
	) const override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief The type of atom that we will be selecting.
	masala::core::chemistry::atoms::ElementTypeEnum element_ = masala::core::chemistry::atoms::ElementTypeEnum::C;

}; // class ElementTypeAtomSelector

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_hh