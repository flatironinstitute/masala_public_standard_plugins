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

} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
