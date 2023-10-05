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
/// @brief Unit tests for the BinaryCostFunctionNetworkProblemRosettaFileInterpreter class.  This class reads the binary
/// cost function network optimization problem file format written by Rosetta (a.k.a. the packing problem format) and
/// constructs a user-defined type of CostFunctionNetworkOptimizationProblem.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <file_interpreters_api/auto_generated_api/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API.hh>

// Registration headers:
#include <registration_api/register_library.hh>

// Masala numeric_api headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>

// Masala base headers:
#include <base/managers/file_interpreter/MasalaFileInterpreterManager.hh>
#include <base/managers/file_interpreter/MasalaFileInterpreterCreator.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/environment/MasalaEnvironmentManager.hh>

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

	registration_api::register_library();

    REQUIRE_NOTHROW([&](){
		std::vector< MasalaFileInterpreterCreatorCSP > const creators(
			MasalaFileInterpreterManager::get_instance()->get_file_interpreters_by_short_name(
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
    }() );

	registration_api::unregister_library();
}

TEST_CASE( "Read a large cost function network optimization problem.", "[standard_masala_plugins::file_interpreters_api::auto_generated_api::cost_function_network::BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API][file read]" ) {
	using namespace standard_masala_plugins::file_interpreters_api::auto_generated_api::cost_function_network;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	using namespace masala::base::managers::file_interpreter;
	using namespace masala::base::managers::environment;
	using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
	using namespace masala::base::managers::tracer;

	registration_api::register_library();

    REQUIRE_NOTHROW([&](){
		std::string library_path;
		bool const env_var_success( MasalaEnvironmentManager::get_instance()->get_environment_variable( "MASALA_STANDARD_PLUGINS", library_path ) );
		if( !env_var_success ) {
			MasalaTracerManager::get_instance()->write_to_tracer(
				"tests::unit::file_interpreters::cost_function_network::BinaryCostFunctionNetworkProblemRosettaFileInterpreterTests",
				"Could not load the value of the MASALA_STANDARD_PLUGINS environment variable.  Note that this test suite assumes that "
				"this environment variable has been set to point to the installation directory of the Masala standard plugins library."
			);
		}
		CHECK(env_var_success);

		MasalaFileInterpreterCreatorCSP creator(
			MasalaFileInterpreterManager::get_instance()->get_file_interpreter_by_full_name(
				"standard_masala_plugins::file_interpreters::cost_function_network::BinaryCostFunctionNetworkProblemRosettaFileInterpreter"
			)
		);
		CHECK( creator != nullptr );
        BinaryCostFunctionNetworkProblemRosettaFileInterpreter_APISP fileinterp(
            std::dynamic_pointer_cast< BinaryCostFunctionNetworkProblemRosettaFileInterpreter_API >(
				creator->create_file_interpreter()
			)
        );
		CHECK( fileinterp != nullptr );
        fileinterp->write_to_tracer( "Instantiated a BinaryCostFunctionNetworkProblemRosettaFileInterpreter from the MasalaFileInterpreterManager." );

		// Configure the reader:
		fileinterp->set_cfn_problem_type_to_generate( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" );

		// Read a file:
		CostFunctionNetworkOptimizationProblems_APISP problems(
			fileinterp->cfn_problems_from_ascii_file( library_path + "/database/unit_test_data/file_interpreters/cost_function_network/1qys_16_bit_problem_binary.pdb" )
		);
		CHECK( problems->n_problems() == 1 );
		PairwisePrecomputedCostFunctionNetworkOptimizationProblem_APICSP problem( std::dynamic_pointer_cast< PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API const >( problems->problem(0) ) );
		CHECK( problem != nullptr );
		
		// Check that we have the correct number of variable nodes, and the correct number of
		// choices at each node:
		CHECK( problem->n_choices_at_variable_nodes().size() == 4 );
		CHECK( problem->n_choices_at_variable_nodes()[0].second == 9 );
		CHECK( problem->n_choices_at_variable_nodes()[1].second == 16 );
		CHECK( problem->n_choices_at_variable_nodes()[2].second == 12 );
		CHECK( problem->n_choices_at_variable_nodes()[3].second == 11 );
    }() );

	// Clean up at the end.
	registration_api::unregister_library();
}

} // namespace cost_function_network
} // namespace file_interpreters
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
