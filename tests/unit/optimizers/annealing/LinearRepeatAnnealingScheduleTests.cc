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

/// @file tests/unit/optimizers/annealing/LinearRepeatAnnealingScheduleTests.cc
/// @brief Unit tests for the LinearRepeatAnnealingSchedule class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/annealing/LinearRepeatAnnealingSchedule_API.hh>
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>

// Masala base headers:
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <sstream>
#include <iomanip>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace annealing {

std::string const test_name( "standard_masala_plugins::tests::unit::optimizers::annealing::LinearRepeatAnnealingScheduleTests" );

TEST_CASE( "Instantiate the linear repeat annealing schedule.", "[masala::numeric_api::auto_generated_api::optimization::annealing::LinearRepeatAnnealingSchedule_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::base::managers::tracer;
    using namespace masala::base::managers::plugin_module;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );
    MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );

    tracer->write_to_tracer( test_name, "Starting \"Instantiate the linear repeat annealing schedule.\"" );

    REQUIRE_NOTHROW([&](){
        masala::base::managers::plugin_module::MasalaPluginAPISP plugin_object(
            plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{"AnnealingSchedule"}, "LinearRepeatAnnealingSchedule", true )
        );
        CHECK( plugin_object != nullptr );
        tracer->write_to_tracer( test_name, "Created an object of type " + plugin_object->inner_class_name() + "." );
        LinearRepeatAnnealingSchedule_APISP anneal_sched( std::dynamic_pointer_cast< LinearRepeatAnnealingSchedule_API >( plugin_object ) );
        CHECK( anneal_sched != nullptr );
    }() );

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Test the samples of the linear repeat annealing schedule.", "[masala::numeric_api::auto_generated_api::optimization::annealing::LinearRepeatAnnealingSchedule_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::base::managers::tracer;
    using namespace masala::base::managers::plugin_module;
    using masala::base::Real;
    using masala::base::Size;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );
    MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );

    tracer->write_to_tracer( test_name, "Starting \"Test the samples of the linear repeat annealing schedule.\"" );

    std::vector< Real > const expected_vals{ 50.0, 45.0, 40.0, 35.0, 30.0, 25.0, 20.0 };
    std::vector< Real > actual_vals(7, 0.0);
    std::vector< Real > actual_vals2(7, 0.0);

    REQUIRE_NOTHROW([&](){
		masala::base::managers::plugin_module::MasalaPluginAPISP plugin_object(
			plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{"AnnealingSchedule"}, "LinearRepeatAnnealingSchedule", true )
		);
		CHECK( plugin_object != nullptr );
		tracer->write_to_tracer( test_name, "Created an object of type " + plugin_object->inner_class_name() + "." );
		LinearRepeatAnnealingSchedule_APISP anneal_sched( std::dynamic_pointer_cast< LinearRepeatAnnealingSchedule_API >( plugin_object ) );
		CHECK( anneal_sched != nullptr );

		anneal_sched->set_final_time_index(7);
		anneal_sched->set_temperature_initial(50.0);
		anneal_sched->set_temperature_final(20.0);
        anneal_sched->set_n_repeats(1);

		for( Size i(0); i<=6; ++i ) {
			actual_vals[i] = anneal_sched->temperature();
		}

        tracer->write_to_tracer( test_name, "Expected1:\t[ " + masala::base::utility::container::container_to_string( expected_vals, ", " ) + " ]" );
        tracer->write_to_tracer( test_name, "Actual1:\t[ " + masala::base::utility::container::container_to_string( actual_vals, ", " ) + " ]" );

		CHECK( masala::base::utility::container::equal_within_threshold( expected_vals, actual_vals, 1.0e-6 ) );

		anneal_sched->reset_call_count();
		for( Size i(7); i>0; --i ) {
			actual_vals2[i-1] = anneal_sched->temperature(i-1);
		}


        tracer->write_to_tracer( test_name, "Expected2:\t[ " + masala::base::utility::container::container_to_string( expected_vals, ", " ) + " ]" );
        tracer->write_to_tracer( test_name, "Actual2:\t[ " + masala::base::utility::container::container_to_string( actual_vals2, ", " ) + " ]" );

		CHECK( masala::base::utility::container::equal_within_threshold( expected_vals, actual_vals2, 1.0e-6 ) );

		CHECK( anneal_sched->temperature(7) == expected_vals[6] );
		CHECK( anneal_sched->temperature(8) == expected_vals[6] );
		CHECK( anneal_sched->temperature(512) == expected_vals[6] );

    }() );

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

} // namespace annealing
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
