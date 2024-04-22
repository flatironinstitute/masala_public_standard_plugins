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
#include <core_api/auto_generated_api/scoring/ScoringTermAdditionalInput_API.hh>
#include <core_api/auto_generated_api/scoring/ScoringTermAdditionalOutput_API.hh>
#include <core_api/auto_generated_api/scoring/ScoringTermCache_API.hh>

// Base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_FourInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>

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
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_description(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"A scoring term that returns a constant value.",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( ConstantScoringTerm, api_description );

		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"get_constant_value",
				"Get the constant value that this scoring term returns.",
				"constant_value",
				"The constant value that this scoring term always returns.",
				false, false, std::bind( &ConstantScoringTerm::get_constant_value, this )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_constant_value",
				"Set the constant value that this scoring term returns.",
				"constant_value_in",
				"The constant value that this scoring term always returns.",
				false, false, std::bind( &ConstantScoringTerm::set_constant_value, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_FourInput<
				std::vector< masala::base::Real >,
				std::vector< masala::core_api::auto_generated_api::molecular_system::MolecularSystem_APICSP > const &,
				std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermAdditionalInput_APICSP > const * const,
				std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermCache_APISP > const * const,
				std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermAdditionalOutput_APICSP > * const
				>
			>(
				"score",
				"Given a vector of molecular systems, return a vector of one score per molecular system.  "
				"In the case of the ConstantScoringTerm, the scores returned are a user-set constant value.",
				true, false, false, false,
				"molecular_systems", "The vector of molecular systems to score.  At least one molecular system must be provided.",
				"additional_inputs_ptr", "A pointer to a vector of (optional) additional inputs.  Can be nullptr.  If non-null, the vector must contain one entry for each molecular system.",
				"caches_ptr", "A pointer to a vector of (optional) cache containers to permit data that persists from scoring attempt to scoring attempt to be stored.  Can be nullptr.  If non-null, the vector must contain one entry for each molecular system.",
				"additional_outputs_ptr", "A pointer to a vector of (optional) additional outputs.  Can be nullptr.  If non-null, this vector will be cleared and populated with one output per molecular system.",
				"scores", "A vector of scores, one per input molecular system.",
				std::bind( &ConstantScoringTerm::score, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 )
			)
		);

		api_definition() = api_description;
	}

	return api_definition();
}

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
ConstantScoringTerm::score_molecular_systems_derived(
	std::vector< masala::core_api::auto_generated_api::molecular_system::MolecularSystem_APICSP > const & molecular_systems,
	std::vector< masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTermAdditionalInputCSP > const * const,
	std::vector< masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTermCacheSP > const * const,
	std::vector< masala::core_api::base_classes::scoring::molecular_system::PluginWholeMolecularSystemScoringTermAdditionalOutputCSP > * const
) const {
	std::lock_guard< std::mutex > lock( mutex() );
	return std::vector< Real >( molecular_systems.size(), constant_value_ );
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the constant value that this scoring term returns.
void
ConstantScoringTerm::set_constant_value(
	masala::base::Real const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	constant_value_ = setting;
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the constant value that this scoring term returns.
masala::base::Real
ConstantScoringTerm::get_constant_value() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return constant_value_;
}

} // namespace molecular_system
} // namespace scoring_terms
} // namespace scoring
} // namespace standard_masala_plugins