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

/// @file tests/unit/optimizers/cost_function_network/cost_function/GraphIslandCountCostFunctionTests.cc
/// @brief Unit tests for the GraphIslandCountCostFunction class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers/cost_function_network/cost_function/GraphIslandCountCostFunction.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/SquareOfGraphIslandCountCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/SquareRootOfGraphIslandCountCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/LinearGraphIslandCountCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/LogOfGraphIslandCountCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>

// Masala base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_FourInput.tmpl.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/engine/MasalaDataRepresentation.hh>
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

std::string const tracer_name( "standard_masala_plugins::tests::unit::optimizers::cost_function_network::cost_function::GraphIslandCountCostFunctionTests" );

/// @brief Utility function: set up a graph.
void set_up_graph( masala::base::managers::engine::MasalaDataRepresentationAPI & costfxn ) {
	using masala::base::Size;
	using namespace standard_masala_plugins::optimizers::cost_function_network::cost_function;
	using namespace masala::base::api;
	using namespace masala::base::api::setter;

	masala::base::managers::engine::MasalaDataRepresentationSP dr_ptr( costfxn.get_inner_data_representation_object() );
	GraphIslandCountCostFunctionSP gicf_ptr( std::dynamic_pointer_cast< GraphIslandCountCostFunction >( dr_ptr ) );
	CHECK_OR_THROW( gicf_ptr != nullptr, tracer_name, "set_up_graph", "The " + costfxn.inner_class_name() + " object could not be interpreted as a GraphIslandCountCostFunction." );
	MasalaObjectAPIDefinitionCSP api_def( gicf_ptr->get_api_definition() );
	CHECK_OR_THROW( api_def != nullptr, tracer_name, "set_up_graph", "The " + gicf_ptr->class_name() + " object's API definition could not be retrieved." );

	// Get the needed setters:
	MasalaObjectAPISetterDefinition_OneInputCSP<bool> onebased_setter( api_def->get_oneinput_setter_function<bool>("set_one_based_absolute_node_indexing") );
	MasalaObjectAPISetterDefinition_OneInputCSP<Size> nodecount_setter( api_def->get_oneinput_setter_function<Size>("set_absolute_node_count") );
	MasalaObjectAPISetterDefinition_FourInputCSP<Size,Size,Size,Size> pair_interaction_setter( api_def->get_fourinput_setter_function<Size,Size,Size,Size>("declare_node_choice_pair_interaction") );
	CHECK_OR_THROW( onebased_setter != nullptr, tracer_name, "set_up_graph", "Could not get the \"set_one_based_absolute_node_indexing()\" function for the " + gicf_ptr->class_name() + " class." );
	CHECK_OR_THROW( api_def != nullptr, tracer_name, "set_up_graph", "Could not get the \"set_absolute_node_count()\" function for the " + gicf_ptr->class_name() + " class." );
	CHECK_OR_THROW( pair_interaction_setter != nullptr, tracer_name, "set_up_graph", "Could not get the \"declare_node_choice_pair_interaction()\" function for the " + gicf_ptr->class_name() + " class." );

	// Set up an 8-node graph with 3 choices per node:
	onebased_setter->function(true);
	nodecount_setter->function(8);

	pair_interaction_setter->function( 1, 2, 0, 0 );
	pair_interaction_setter->function( 2, 3, 0, 0 );

	pair_interaction_setter->function( 1, 3, 1, 1 );
	pair_interaction_setter->function( 1, 8, 1, 1 );
	pair_interaction_setter->function( 4, 5, 1, 1 );
	pair_interaction_setter->function( 4, 6, 1, 1 );

	pair_interaction_setter->function( 1, 4, 2, 2 );
	pair_interaction_setter->function( 2, 3, 2, 2 );
	pair_interaction_setter->function( 4, 6, 2, 2 );
	pair_interaction_setter->function( 5, 6, 2, 2 );
	pair_interaction_setter->function( 7, 8, 2, 2 );

	pair_interaction_setter->function( 3, 6, 0, 1 );
	pair_interaction_setter->function( 6, 7, 1, 2 );

	pair_interaction_setter->function( 1, 4, 0, 1 );
	pair_interaction_setter->function( 3, 5, 1, 2 );
	pair_interaction_setter->function( 5, 7, 2, 0 );
	pair_interaction_setter->function( 6, 8, 1, 0 );

	// Finalize the cost function:
	gicf_ptr->finalize( std::vector< Size >{ 1, 2, 3, 4, 5, 6, 7, 8 } );
}

TEST_CASE( "Instantiate a SquareOfGraphIslandCountCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::SquareOfGraphIslandCountCostFunction_API][instantiation]" ) {
	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::cost_function::SquareOfGraphIslandCountCostFunction_APISP costfxn(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::SquareOfGraphIslandCountCostFunction_API >()
		);
		costfxn->write_to_tracer( "Instantiated a SquareOfGraphIslandCountCostFunction." );
	}() );
}

TEST_CASE( "Instantiate a LinearGraphIslandCountCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::LinearGraphIslandCountCostFunction_API][instantiation]" ) {
	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::cost_function::LinearGraphIslandCountCostFunction_APISP costfxn(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::LinearGraphIslandCountCostFunction_API >()
		);
		costfxn->write_to_tracer( "Instantiated a LinearGraphIslandCountCostFunction." );
	}() );
}

TEST_CASE( "Instantiate a SquareRootOfGraphIslandCountCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::SquareRootOfGraphIslandCountCostFunction_API][instantiation]" ) {
	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::cost_function::SquareRootOfGraphIslandCountCostFunction_APISP costfxn(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::SquareRootOfGraphIslandCountCostFunction_API >()
		);
		costfxn->write_to_tracer( "Instantiated a SquareRootOfGraphIslandCountCostFunction." );
	}() );
}

TEST_CASE( "Instantiate a LogOfGraphIslandCountCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::LogOfGraphIslandCountCostFunction_API][instantiation]" ) {
	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::cost_function::LogOfGraphIslandCountCostFunction_APISP costfxn(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::LogOfGraphIslandCountCostFunction_API >()
		);
		costfxn->write_to_tracer( "Instantiated a LogOfGraphIslandCountCostFunction." );
	}() );
}

TEST_CASE( "Test the function of a SquareOfGraphIslandCountCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::SquareOfGraphIslandCountCostFunction_API]" ) {
	using masala::base::Size;
	using masala::base::Real;

	REQUIRE_NOTHROW([&](){
		optimizers_api::auto_generated_api::cost_function_network::cost_function::SquareOfGraphIslandCountCostFunction_APISP costfxn(
			masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::SquareOfGraphIslandCountCostFunction_API >()
		);
		costfxn->write_to_tracer( "Instantiated a SquareOfGraphIslandCountCostFunction." );

		set_up_graph( *costfxn );

		Real const all_zero_score( costfxn->compute_cost_function( std::vector< Size >{ 0, 0, 0, 0, 0, 0, 0, 0 } ) );
		Real const all_one_score( costfxn->compute_cost_function( std::vector< Size >{ 1, 1, 1, 1, 1, 1, 1, 1 } ) );
		Real const all_two_score( costfxn->compute_cost_function( std::vector< Size >{ 2, 2, 2, 2, 2, 2, 2, 2 } ) );
		Real const mixed_score_1( costfxn->compute_cost_function( std::vector< Size >{ 0, 0, 0, 1, 1, 1, 2, 2 } ) );
		Real const mixed_score_2( costfxn->compute_cost_function( std::vector< Size >{ 1, 0, 1, 1, 2, 1, 0, 0 } ) );

		costfxn->write_to_tracer( "{ 0, 0, 0, 0, 0, 0, 0, 0 } -> " + std::to_string( all_zero_score ) );
		costfxn->write_to_tracer( "{ 1, 1, 1, 1, 1, 1, 1, 1 } -> " + std::to_string( all_one_score ) );
		costfxn->write_to_tracer( "{ 2, 2, 2, 2, 2, 2, 2, 2 } -> " + std::to_string( all_two_score ) );
		costfxn->write_to_tracer( "{ 0, 0, 0, 1, 1, 1, 2, 2 } -> " + std::to_string( mixed_score_1 ) );
		costfxn->write_to_tracer( "{ 1, 0, 1, 1, 2, 1, 0, 0 } -> " + std::to_string( mixed_score_2 ) );

		CHECK( std::abs( all_zero_score + 4.0 ) < 1.0e-6 );
		CHECK( std::abs( all_one_score + 13.0 ) < 1.0e-6 );
		CHECK( std::abs( all_two_score + 11.0 ) < 1.0e-6 );
		CHECK( std::abs( mixed_score_1 + 49.0 ) < 1.0e-6 );
		CHECK( std::abs( mixed_score_2 + 18.0 ) < 1.0e-6 );
	}() );
}

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
