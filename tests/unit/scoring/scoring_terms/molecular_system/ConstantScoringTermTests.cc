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

/// @file tests/unit/scoring/scoring_terms/molecular_system/ConstantScoringTermTests.cc
/// @brief Unit tests for the ConstantScoringTerm class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <scoring_api/auto_generated_api/scoring_terms/molecular_system/ConstantScoringTerm_API.hh>
#include <registration_api/register_library.hh>

// Core headers:
#include <core_api/auto_generated_api/molecular_system/MolecularSystem_API.hh>
#include <core_api/auto_generated_api/chemistry/MolecularGeometry_API.hh>

// Base headers:
#include <base/types.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_FourInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace selectors {
namespace scoring {
namespace scoring_terms {
namespace molecular_system {

TEST_CASE( "Instantiate an ConstantScoringTerm", "[standard_masala_plugins::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_API][scoring][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        scoring_api::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_APISP const_scoreterm(
            masala::make_shared< scoring_api::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_API >()
        );
        const_scoreterm->write_to_tracer( "Instantiated an ConstantScoringTerm." );
    }() );
}

TEST_CASE( "Score two empty molecular systems", "[standard_masala_plugins::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_API][scoring][score]" ) {
    using namespace scoring_api::auto_generated_api::scoring_terms::molecular_system;
    using namespace masala::core_api::auto_generated_api::molecular_system;

    ConstantScoringTerm_APISP const_scoreterm;
    std::vector< masala::base::Real > scores;
    REQUIRE_NOTHROW([&](){
        const_scoreterm = masala::make_shared< ConstantScoringTerm_API >();
        const_scoreterm->set_constant_value( 0.25 );
        MolecularSystem_APISP molecular_system1( masala::make_shared< MolecularSystem_API >() );
        MolecularSystem_APISP molecular_system2( masala::make_shared< MolecularSystem_API >() );
        std::vector< MolecularSystem_APICSP > molsys_vec{ molecular_system1, molecular_system2 };
        const_scoreterm->write_to_tracer( "Preparing to score two empty molecular systems." );
        scores = const_scoreterm->score( molsys_vec, nullptr, nullptr, nullptr );
    }() );
    REQUIRE( scores.size() == 2 );
    CHECK( scores[0] == 0.25 );
    CHECK( scores[1] == 0.25 );
    const_scoreterm->write_to_tracer( "Score1: " + std::to_string(scores[0]) );
    const_scoreterm->write_to_tracer( "Score2: " + std::to_string(scores[1]) );
}

TEST_CASE( "Score two empty molecular systems, loading the score term from the plugin system", "[standard_masala_plugins::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_API][scoring][score]" ) {
    using namespace scoring_api::auto_generated_api::scoring_terms::molecular_system;
    using namespace masala::core_api::auto_generated_api::molecular_system;
	using namespace masala::base::api;
	using namespace masala::base::managers::plugin_module;

	standard_masala_plugins::registration_api::register_library();

	MasalaPluginAPISP scoreterm;
    std::vector< masala::base::Real > scores;
    REQUIRE_NOTHROW([&](){
		scoreterm = MasalaPluginModuleManager::get_instance()->create_plugin_object_instance_by_short_name( { "ScoringTerm" }, "ConstantScoringTerm", true );
		MasalaObjectAPIDefinitionCSP api_def( scoreterm->get_api_definition_for_inner_class() );
		REQUIRE( api_def != nullptr );
		
		setter::MasalaObjectAPISetterDefinition_OneInputCSP< masala::base::Real > setterfxn(
			api_def->get_oneinput_setter_function< masala::base::Real >( "set_constant_value")
		);
		setterfxn->function( 0.25 );

		work_function::MasalaObjectAPIWorkFunctionDefinition_FourInputCSP<
			std::vector< masala::base::Real >,
			std::vector< MolecularSystem_APICSP > const &,
			std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermAdditionalInput_APICSP > const * const,
			std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermCache_APISP > const * const,
			std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermAdditionalOutput_APICSP > * const
		> score_workfxn(
			api_def->get_fourinput_work_function<
				std::vector< masala::base::Real >,
				std::vector< MolecularSystem_APICSP > const &,
				std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermAdditionalInput_APICSP > const * const,
				std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermCache_APISP > const * const,
				std::vector< masala::core_api::auto_generated_api::scoring::ScoringTermAdditionalOutput_APICSP > * const
			>( std::string( "score" ) )
		);

        MolecularSystem_APISP molecular_system1( masala::make_shared< MolecularSystem_API >() );
        MolecularSystem_APISP molecular_system2( masala::make_shared< MolecularSystem_API >() );
        std::vector< MolecularSystem_APICSP > molsys_vec{ molecular_system1, molecular_system2 };

        scoreterm->write_to_tracer( "Preparing to score two empty molecular systems." );
        scores = (*score_workfxn->function_raw_ptr())( molsys_vec, nullptr, nullptr, nullptr );
    }() );
    REQUIRE( scores.size() == 2 );
    CHECK( scores[0] == 0.25 );
    CHECK( scores[1] == 0.25 );
    scoreterm->write_to_tracer( "Score1: " + std::to_string(scores[0]) );
    scoreterm->write_to_tracer( "Score2: " + std::to_string(scores[1]) );

	standard_masala_plugins::registration_api::unregister_library();
}

} // namespace molecular_system
} // namespace scoring_terms
} // namespace scoring
} // namespace selectors
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
