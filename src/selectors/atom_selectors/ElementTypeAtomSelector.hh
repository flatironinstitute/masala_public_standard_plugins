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

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION, DESTRUCTION, COPYING, AND CLONING
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	ElementTypeAtomSelector() = default;

	/// @brief Copy constructor.
	ElementTypeAtomSelector( ElementTypeAtomSelector const & ) = default;

	/// @brief Destructor.
	~ElementTypeAtomSelector() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the copy.
	masala::core_api::base_classes::selectors::atom_selectors::AtomSelectorSP clone() const;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	masala::core_api::base_classes::selectors::atom_selectors::AtomSelectorSP deep_clone() const;

	/// @brief Make this object independent by calling deep_clone on all contained objects.
	/// @details All that this version does is to make the api description null.
	void make_independent();

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

	/// @brief Set the element type, by string.
	void
	set_element_type(
		std::string const & element_name
	);

	/// @brief Set the element type, by enum.
	void
	set_element_type(
		masala::core::chemistry::atoms::ElementTypeEnum const element_type
	);

	/// @brief Every class can name itself.
	/// @returns "ElementTypeAtomSelector".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::selectors::atom_selectors".
	std::string class_namespace() const override;

	/// @brief Get an object describing the API for this object.
	/// @note This is a weak pointer rather than a shared pointer since the
	/// original object is expected to hold on to its API definition (which includes
	/// funciton pointers to the functions of the instance).  Querying whether the
	/// weak pointer can be converted to a shared pointer serves on a check as to
	/// whether it is safe to use the function pointers.  Not ideal, but better than
	/// nothing.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief The type of atom that we will be selecting.
	masala::core::chemistry::atoms::ElementTypeEnum element_ = masala::core::chemistry::atoms::ElementTypeEnum::C;

	/// @brief The API descriptor.
	/// @details Lazily generated.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_description_;

}; // class ElementTypeAtomSelector

} // namespace atom_selectors
} // namespace selectors
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_selectors_atom_selectors_ElementTypeAtomSelector_hh