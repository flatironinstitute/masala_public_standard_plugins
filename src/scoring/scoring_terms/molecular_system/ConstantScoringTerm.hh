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

/// @file src/scoring/scoring_terms/molecular_system/ConstantScoringTerm.hh
/// @brief Headers for a scoring term that returns a constant value.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_scoring_scoring_terms_molecular_system_ConstantScoringTerm_hh
#define Standard_Masala_Plugins_src_scoring_scoring_terms_molecular_system_ConstantScoringTerm_hh

// Parent class:
#include <core_api/base_classes/scoring/molecular_system/PluginWholeMolecularSystemScoringTerm.hh>

// Forward declarations:
#include <scoring/scoring_terms/molecular_system/ConstantScoringTerm.fwd.hh>

// Core API headers:
#include <core_api/auto_generated_api/molecular_system/MolecularSystem_API.fwd.hh>
#include <core_api/base_classes/scoring/molecular_system/PluginWholeMolecularSystemScoringTermAdditionalInput.fwd.hh>
#include <core_api/base_classes/scoring/molecular_system/PluginWholeMolecularSystemScoringTermAdditionalOutput.fwd.hh>
#include <core_api/base_classes/scoring/molecular_system/PluginWholeMolecularSystemScoringTermCache.fwd.hh>

// Base headers:
#include <base/types.hh>

namespace standard_masala_plugins {
namespace scoring {
namespace scoring_terms {
namespace molecular_system {


/// @brief A scoring term that returns a constant value.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class ConstantScoringTerm : public masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTerm {

	typedef masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTerm Parent;
	typedef masala::base::Real Real;
	typedef masala::base::Size Size;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION, DESTRUCTION, COPYING, AND CLONING
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	ConstantScoringTerm() = default;

	/// @brief Copy constructor.
	ConstantScoringTerm( ConstantScoringTerm const & ) = default;

	/// @brief Destructor.
	~ConstantScoringTerm() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the copy.
	ConstantScoringTermSP clone() const;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	ConstantScoringTermSP deep_clone() const;

	/// @brief Make this object independent by calling deep_clone on all contained objects.
	/// @details All that this version does is to make the api description null.
	void make_independent();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Every class can name itself.
	/// @returns "ConstantScoringTerm".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::scoring::scoring_terms::molecular_system".
	std::string class_namespace() const override;

	/// @brief Get a list of categories that this object could be sorted into.
    /// @returns { { "ScoringTerm", "WholeMolecularSystemScoringTerm" } }
    std::vector< std::vector< std::string > >
    get_categories() const override;

    /// @brief Get a list of keywords associated with this object.
    /// @returns { "scoring_term", "whole_molecular_system", "constant" }
    std::vector< std::string >
    get_keywords() const override;

	/// @brief Get an object describing the API for this object.
	/// @note This is a weak pointer rather than a shared pointer since the
	/// original object is expected to hold on to its API definition (which includes
	/// funciton pointers to the functions of the instance).  Querying whether the
	/// weak pointer can be converted to a shared pointer serves on a check as to
	/// whether it is safe to use the function pointers.  Not ideal, but better than
	/// nothing.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

protected:

	/// @brief Score a vector of structures (molecular systems), and produce a
	/// corresponding vector of scores.
	/// @param[in] molecular_systems A vector of at least one molecular system to score.
	/// @param[in] additional_inputs_ptr A pointer to a vector of (optional) additional
	/// inputs.  Can be nullptr.  If non-null, the vector must contain one entry for each
	/// molecular system.
	/// @param[in] caches_ptr A pointer to a vector of (optional) cache containers to permit
	/// data that persists from scoring attempt to scoring attempt to be stored.  Can be
	/// nullptr.  If non-null, the vector must contain one entry for each molecular system.
	/// @param[in] additional_inputs_ptr A pointer to a vector of (optional) additional
	/// outputs.  Can be nullptr.  If non-null, this vector will empty (length zero).  The derived
	/// class is responsible for ensuring that the vector is either kept at length zero
	/// or populated with one output per molecular system.
	std::vector< masala::base::Real >
	score_molecular_systems_derived(
		std::vector< masala::core_api::auto_generated_api::molecular_system::MolecularSystem_APICSP > const & molecular_systems,
		std::vector< masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTermAdditionalInputCSP > const * const additional_inputs_ptr,
		std::vector< masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTermCacheSP > const * const caches_ptr,
		std::vector< masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTermAdditionalOutputCSP > * const additional_outputs_ptr
	) const override;

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the constant value that this scoring term returns.
	void set_constant_value( Real const setting );

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the constant value that this scoring term returns.
	Real get_constant_value() const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief The value that this scoring term always returns.
	Real constant_value_ = 0.0;

}; // class ConstantScoringTerm

} // namespace molecular_system
} // namespace scoring_terms
} // namespace scoring
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_scoring_scoring_terms_molecular_system_ConstantScoringTerm_hh