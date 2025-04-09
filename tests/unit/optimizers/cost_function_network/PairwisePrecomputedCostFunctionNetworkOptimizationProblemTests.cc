/*
	Standard Masala Plugins
	Copyright (C) 2025 Vikram K. Mulligan

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

/// @file tests/unit/optimizers/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblemTests.cc
/// @brief Unit tests for the PairwisePrecomputedCostFunctionNetworkOptimizationProblem class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API.hh>
#include <optimizers/cost_function_network/PairwisePrecomputedCFNProblemScratchSpace.hh>
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>

// Masala base headers:
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>

// STL headers:
#include <sstream>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace cost_function_network {

std::string const test_namespace( "standard_masala_plugins::tests::unit::optimizers::cost_function_network" );
std::string const test_name( "PairwisePrecomputedCostFunctionNetworkOptimizationProblemTests" );

TEST_CASE( "Instantiate a PairwisePrecomputedCostFunctionNetworkOptimizationProblem.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API][instantiation]" ) {
	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem_APISP problem(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API >()
		);
		problem->write_to_tracer( "Instantiated a PairwisePrecomputedCostFunctionNetworkOptimizationProblem." );
	}() );
}

TEST_CASE( "Instantiate a PairwisePrecomputedCostFunctionNetworkOptimizationProblem from the plugin manager.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API][instantiation][plugin_manager]" ) {
	using namespace masala::base::managers::plugin_module;
	using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
	using namespace standard_masala_plugins::optimizers_api::auto_generated_api::registration;
	REQUIRE_NOTHROW([&](){
		register_optimizers();
		MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );
		masala::base::managers::plugin_module::MasalaPluginAPISP problem_uncast(
			plugman->create_plugin_object_instance_by_short_name(
				std::vector< std::string >{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem" },
				"PairwisePrecomputedCostFunctionNetworkOptimizationProblem", true
			)
		);
		PairwisePrecomputedCostFunctionNetworkOptimizationProblem_APISP problem( std::dynamic_pointer_cast< PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API >( problem_uncast ) );
		CHECK( problem != nullptr );

		problem->write_to_tracer( "Instantiated a PairwisePrecomputedCostFunctionNetworkOptimizationProblem." );
		unregister_optimizers();
	}() );
}

TEST_CASE( "Test PairwisePrecomputedCostFunctionNetworkOptimizationProblem::compute_score_change().", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API][scoring]" ) {
	using namespace masala::base::managers::plugin_module;
	using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
	using namespace standard_masala_plugins::optimizers_api::auto_generated_api::registration;
	using namespace standard_masala_plugins::optimizers::cost_function_network;
	using masala::base::Size;
	
	REQUIRE_NOTHROW([&](){
		register_optimizers();
		MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );
		masala::base::managers::plugin_module::MasalaPluginAPISP problem_uncast(
			plugman->create_plugin_object_instance_by_short_name(
				std::vector< std::string >{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem" },
				"PairwisePrecomputedCostFunctionNetworkOptimizationProblem", true
			)
		);
		PairwisePrecomputedCostFunctionNetworkOptimizationProblem_APISP problem( std::dynamic_pointer_cast< PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API >( problem_uncast ) );
		CHECK( problem != nullptr );

		problem->write_to_tracer( "Instantiated a PairwisePrecomputedCostFunctionNetworkOptimizationProblem." );

		// Configure the problem:
		problem->set_minimum_number_of_choices_at_node(0, 4);
		problem->set_minimum_number_of_choices_at_node(1, 4);
		problem->set_minimum_number_of_choices_at_node(2, 4);
		problem->set_minimum_number_of_choices_at_node(3, 4);

		problem->set_onebody_penalty( 0, 0, 3.0 );
		problem->set_onebody_penalty( 0, 1, -2.0 );
		problem->set_onebody_penalty( 0, 2, 1.0 );
		problem->set_onebody_penalty( 1, 0, 4.0 );
		problem->set_onebody_penalty( 1, 1, -5.0 );
		problem->set_onebody_penalty( 1, 2, -3.0 );
		problem->set_onebody_penalty( 2, 0, -1.0 );
		problem->set_onebody_penalty( 2, 1, -1.0 );
		problem->set_onebody_penalty( 2, 2, 2.0 );
		problem->set_onebody_penalty( 3, 0, 4.0 );
		problem->set_onebody_penalty( 3, 1, 0.5 );
		problem->set_onebody_penalty( 3, 2, 4.0 );

		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 0 ), 1.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 1 ), 4.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 2 ), 2.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 0 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 1 ), -2.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 2 ), 7.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 0 ), 3.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 1 ), 6.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 2 ), -6.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 3, 0 ), -6.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 3, 1 ), 4.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 3, 2 ), 2.5 );

		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 0 ), 4.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 1 ), -2.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 2 ), 3.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 0 ), -3.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 1 ), 2.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 2 ), -4.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 0 ), 7.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 1 ), 7.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 2 ), -8.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 3, 0 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 3, 1 ), 5.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 3, 2 ), 2.5 );

		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 0, 0 ), 0.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 0, 1 ), -0.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 0, 2 ), 5.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 1, 0 ), -3.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 1, 1 ), 0.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 1, 2 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 2, 0 ), -7.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 2, 1 ), 2.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 2, 2 ), -8.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 3, 0 ), 4.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 3, 1 ), -5.5 );
		problem->set_twobody_penalty( std::make_pair( 0, 3 ), std::make_pair( 3, 2 ), -2.5 );

		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 0 ), 5.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 1 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 2 ), -5.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 0 ), 1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 1 ), 1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 2 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 0 ), 3.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 1 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 2 ), 2.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 3, 0 ), -4.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 3, 1 ), -3.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 3, 2 ), 4.5 );

		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 0, 0 ), -0.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 0, 1 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 0, 2 ), -5.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 1, 0 ), -2.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 1, 1 ), 2.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 1, 2 ), -1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 2, 0 ), 3.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 2, 1 ), 1.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 2, 2 ), -0.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 3, 0 ), -4.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 3, 1 ), 4.5 );
		problem->set_twobody_penalty( std::make_pair( 1, 3 ), std::make_pair( 3, 2 ), -1.5 );

		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 0, 0 ), -3.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 0, 1 ), -3.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 0, 2 ), 2.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 1, 0 ), -3.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 1, 1 ), 6.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 1, 2 ), -0.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 2, 0 ), 8.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 2, 1 ), 4.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 2, 2 ), -2.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 3, 0 ), -9.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 3, 1 ), -5.5 );
		problem->set_twobody_penalty( std::make_pair( 2, 3 ), std::make_pair( 3, 2 ), -3.5 );

		problem->finalize();

		CHECK( problem->finalized() );

		PairwisePrecomputedCFNProblemScratchSpaceSP scratch( std::dynamic_pointer_cast< PairwisePrecomputedCFNProblemScratchSpace >( problem->generate_cfn_problem_scratch_space() ) );
		CHECK( scratch != nullptr );
		CHECK( std::abs( problem->compute_absolute_score( std::vector< Size >{ 0, 0, 0, 0 }, scratch.get() ) - 18.0 ) < 1.0e-6 );

		unregister_optimizers();
	}() );
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
