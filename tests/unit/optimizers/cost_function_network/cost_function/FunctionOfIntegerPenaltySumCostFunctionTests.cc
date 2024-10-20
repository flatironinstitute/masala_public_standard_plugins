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

/// @file tests/unit/optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunctionTests.cc
/// @brief Unit tests for the FunctionOfIntegerPenaltySumCostFunction class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>

// Masala base headers:
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <sstream>
#include <iomanip>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

std::string const tracer_name( "standard_masala_plugins::tests::unit::optimizers::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunctionTests" );

TEST_CASE( "Instantiate an FunctionOfIntegerPenaltySumCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::FunctionOfIntegerPenaltySumCostFunction_API][instantiation]" ) {
	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_APISP costfxn(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_API >()
		);
		costfxn->write_to_tracer( "Instantiated a FunctionOfIntegerPenaltySumCostFunction." );
	}() );
}

TEST_CASE( "Set up a FunctionOfIntegerPenaltySumCostFunction with quadratic penalties.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::FunctionOfIntegerPenaltySumCostFunction_API][instantiation][setup][quadratic]" ) {
	using namespace masala::base::managers::plugin_module;
	using namespace masala::base::managers::tracer;
	using masala::base::Real;
	using masala::base::Size;
	using namespace masala::base::utility::container;

	standard_masala_plugins::optimizers_api::auto_generated_api::registration::register_optimizers();
	masala::numeric_api::auto_generated_api::registration::register_numeric();

	MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );
	MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );

	tracer->write_to_tracer( tracer_name, "Starting \"Set up a FunctionOfIntegerPenaltySumCostFunction with quadratic penalties.\"" );

	REQUIRE_NOTHROW([&](){
		MasalaPluginAPISP plugin( plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{ "CostFunction" }, "FunctionOfIntegerPenaltySumCostFunction", true ) );
		CHECK( plugin != nullptr );
		optimizers_api::auto_generated_api::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_APISP costfxn(
			std::dynamic_pointer_cast< optimizers_api::auto_generated_api::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_API >(plugin)
		);
		CHECK( costfxn != nullptr );
		tracer->write_to_tracer( tracer_name, "Instantiated a FunctionOfIntegerPenaltySumCostFunction." );

		costfxn->set_penalty_range_start( -2 );
		costfxn->set_penalty_function( std::vector< Real >{ 4.0, 1.0, 0.0, 1.0 } );
		costfxn->set_penalty_function_behaviour_high_by_string( "quadratic" );
		costfxn->set_penalty_function_behaviour_low_by_string( "quadratic" );
		costfxn->set_penalties_for_all_choices_at_node( 0, std::vector< signed long >{ -1, 0, 1 } );
		costfxn->set_penalties_for_all_choices_at_node( 1, std::vector< signed long >{ -1, 0, 1 } );
		costfxn->set_penalties_for_all_choices_at_node( 2, std::vector< signed long >{ -1, 0, 1 } );
		costfxn->set_penalties_for_all_choices_at_node( 3, std::vector< signed long >{ -1, 0, 1 } );
		costfxn->set_penalties_for_all_choices_at_node( 4, std::vector< signed long >{ -1, 0, 1 } );
		costfxn->set_weight( 2.0 );

		costfxn->finalize( std::vector< masala::base::Size >{ 0, 1, 2, 3, 4 } );

		std::vector< Real > cost_fxns( 11 );
		std::vector< Real > const expected{ 50.0, 32.0, 18.0, 8.0, 2.0, 0, 2.0, 8.0, 18.0, 32.0, 50.0 };
		cost_fxns[ 0] = costfxn->compute_cost_function( std::vector< Size >{  0,  0,  0,  0,  0 } ); // 25
		cost_fxns[ 1] = costfxn->compute_cost_function( std::vector< Size >{  0,  0,  0,  0,  1 } ); // 16
		cost_fxns[ 2] = costfxn->compute_cost_function( std::vector< Size >{  0,  0,  0,  1,  1 } ); // 9
		cost_fxns[ 3] = costfxn->compute_cost_function( std::vector< Size >{  0,  0,  1,  1,  1 } ); // 4
		cost_fxns[ 4] = costfxn->compute_cost_function( std::vector< Size >{  0,  1,  1,  1,  1 } ); // 1
		cost_fxns[ 5] = costfxn->compute_cost_function( std::vector< Size >{  1,  1,  1,  1,  1 } ); // 0
		cost_fxns[ 6] = costfxn->compute_cost_function( std::vector< Size >{  1,  1,  1,  1,  2 } ); // 1
		cost_fxns[ 7] = costfxn->compute_cost_function( std::vector< Size >{  1,  1,  1,  2,  2 } ); // 4
		cost_fxns[ 8] = costfxn->compute_cost_function( std::vector< Size >{  1,  1,  2,  2,  2 } ); // 9
		cost_fxns[ 9] = costfxn->compute_cost_function( std::vector< Size >{  1,  2,  2,  2,  2 } ); // 16
		cost_fxns[10] = costfxn->compute_cost_function( std::vector< Size >{  2,  2,  2,  2,  2 } ); // 25

		tracer->write_to_tracer( tracer_name, "Expected:\t[" + container_to_string( expected, "," ) + "]" );
		tracer->write_to_tracer( tracer_name, "Actual:\t[" + container_to_string( cost_fxns, "," ) + "]" );

		CHECK( equal_within_threshold( expected, cost_fxns, 1.0e-6 ) );

	}() );

	masala::numeric_api::auto_generated_api::registration::unregister_numeric();
	standard_masala_plugins::optimizers_api::auto_generated_api::registration::unregister_optimizers();
}

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
