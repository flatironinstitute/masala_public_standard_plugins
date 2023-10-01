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

/// @file tests/unit/file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreterTests.cc
/// @brief Unit tests for the BinaryCostFunctionNetworkProblemRosettaFileInterpreter class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <file_interpreters_api/auto_generated_api/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API.hh>

// Registration headers:
#include <registration_api/register_library.hh>

// Masala numeric headers:

// Masala base headers:
#include <base/managers/file_interpreter/MasalaFileInterpreterManager.hh>
#include <base/managers/file_interpreter/MasalaFileInterpreterCreator.hh>

// STL headers:

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace file_interpreters {
namespace cost_function_network {

TEST_CASE( "Instantiate a BinaryCostFunctionNetworkProblemRosettaFileInterpreter.", "[standard_masala_plugins::file_interpreters_api::auto_generated_api::cost_function_network::BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API][instantiation]" ) {
	using namespace file_interpreters_api::auto_generated_api::cost_function_network;
    REQUIRE_NOTHROW([&](){
        BinaryCostFunctionNetworkProblemRosettaFileInterpreter_APISP fileinterp(
            masala::make_shared< BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API >()
        );
        fileinterp->write_to_tracer( "Instantiated a BinaryCostFunctionNetworkProblemRosettaFileInterpreter." );
    }() );
}

TEST_CASE( "Instantiate a BinaryCostFunctionNetworkProblemRosettaFileInterpreter from the MasalaFileInterpreterManager.", "[standard_masala_plugins::file_interpreters_api::auto_generated_api::cost_function_network::BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API][instantiation]" ) {
	using namespace standard_masala_plugins::file_interpreters_api::auto_generated_api::cost_function_network;
	using namespace masala::base::managers::file_interpreter;

    REQUIRE_NOTHROW([&](){
		registration_api::register_library();

		std::vector< MasalaFileInterpreterCreatorCSP > const creators(
			MasalaFileInterpreterManager::get_instance()->get_file_interpreter_by_name(
				"BinaryCostFunctionNetworkProblemRosettaFileInterpreter"
			)
		);
		CHECK( creators.size() == 1 );
        BinaryCostFunctionNetworkProblemRosettaFileInterpreter_APISP fileinterp(
            std::dynamic_pointer_cast< BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API >(
				creators[0]->create_file_interpreter()
			)
        );
		CHECK( fileinterp != nullptr );
        fileinterp->write_to_tracer( "Instantiated a BinaryCostFunctionNetworkProblemRosettaFileInterpreter from the MasalaFileInterpreterManager." );

		registration_api::unregister_library();
    }() );
}

} // namespace cost_function_network
} // namespace file_interpreters
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
