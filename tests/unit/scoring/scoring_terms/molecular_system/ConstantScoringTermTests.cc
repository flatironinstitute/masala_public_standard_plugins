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

/// @file tests/unit/scoring/scoring_terms/molecular_system/ConstantScoringTermTests.cc
/// @brief Unit tests for the ConstantScoringTerm class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <scoring_api/auto_generated_api/scoring_terms/molecular_system/ConstantScoringTerm_API.hh>

// Core headers:
#include <core_api/auto_generated_api/molecular_system/MolecularSystem_API.hh>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace selectors {
namespace scoring {
namespace scoring_terms {
namespace molecular_system {

TEST_CASE( "Instantiate an ConstantScoringTerm", "[standard_masala_plugins::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_API][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        scoring_api::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_APISP const_scoreterm(
            masala::make_shared< scoring_api::auto_generated_api::scoring_terms::molecular_system::ConstantScoringTerm_API >()
        );
        const_scoreterm->write_to_tracer( "Instantiated an ConstantScoringTerm." );
    }() );
}

// TEST_CASE( "Select oxygen atoms in an empty molecular system", "[standard_masala_plugins::auto_generated_api::atom_selectors::ElementTypeAtomSelector_API][selection][apply]" ) {
//     using namespace selectors_api::auto_generated_api::atom_selectors;
//     using namespace masala::core_api::auto_generated_api::molecular_system;
//     using namespace masala::core_api::auto_generated_api::selection::atom_selection;

//     AtomSelection_APICSP selection;
//     REQUIRE_NOTHROW([&](){
//         ElementTypeAtomSelector_APISP elemsel( masala::make_shared< ElementTypeAtomSelector_API >() );
//         MolecularSystem_APISP molecular_system( masala::make_shared< MolecularSystem_API >() );
//         selection = elemsel->generate_atom_selection( *molecular_system );
//     }() );
//     REQUIRE( selection->num_selected_atoms() == 0 );
// }

} // namespace molecular_system
} // namespace scoring_terms
} // namespace scoring
} // namespace selectors
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
