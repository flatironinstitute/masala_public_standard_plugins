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
	ElementTypeAtomSelectorSP clone() const;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	ElementTypeAtomSelectorSP deep_clone() const;

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