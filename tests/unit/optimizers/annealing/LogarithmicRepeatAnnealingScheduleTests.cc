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

/// @file tests/unit/optimizers/annealing/LogarithmicRepeatAnnealingScheduleTests.cc
/// @brief Unit tests for the LogarithmicRepeatAnnealingSchedule class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/annealing/LogarithmicRepeatAnnealingSchedule_API.hh>
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

std::string const test_name( "standard_masala_plugins::tests::unit::optimizers::annealing::LogarithmicRepeatAnnealingScheduleTests" );

TEST_CASE( "Instantiate the logarithmic repeat annealing schedule.", "[masala::numeric_api::auto_generated_api::optimization::annealing::LogarithmicRepeatAnnealingSchedule_API][optimization][plugin_manager]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::base::managers::tracer;
    using namespace masala::base::managers::plugin_module;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );
    MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );

    tracer->write_to_tracer( test_name, "Starting \"Instantiate the logarithmic repeat annealing schedule.\"" );

    REQUIRE_NOTHROW([&](){
        masala::base::managers::plugin_module::MasalaPluginAPISP plugin_object(
            plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{"AnnealingSchedule"}, "LogarithmicRepeatAnnealingSchedule", true )
        );
        CHECK( plugin_object != nullptr );
        tracer->write_to_tracer( test_name, "Created an object of type " + plugin_object->inner_class_name() + "." );
        LogarithmicRepeatAnnealingSchedule_APISP anneal_sched( std::dynamic_pointer_cast< LogarithmicRepeatAnnealingSchedule_API >( plugin_object ) );
        CHECK( anneal_sched != nullptr );
    }() );

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Test the samples of the logarithmic repeat annealing schedule with one repeat.", "[masala::numeric_api::auto_generated_api::optimization::annealing::LogarithmicRepeatAnnealingSchedule_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::base::managers::tracer;
    using namespace masala::base::managers::plugin_module;
    using masala::base::Real;
    using masala::base::Size;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );
    MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );

    tracer->write_to_tracer( test_name, "Starting \"Test the samples of the logarithmic repeat annealing schedule with one repeat.\"" );

	std::vector< Real > const expected_vals{ 120.0, 91.2369343143998, 69.3681515257509,
		52.7411457022189, 40.0995037174961, 30.4879648892769, 23.180237083189, 17.6241147345928,
		13.3997516532457, 10.1879355118038, 7.74596669241483, 5.88931878597818, 4.47769492694043,
		3.40442631607654, 2.58841183481742, 1.96798967126543, 1.49627786973884, 1.13763171431211,
		0.864950249938933, 0.657628409490851, 0.5
	};
	std::vector< Real > actual_vals(21, 0.0);
	std::vector< Real > actual_vals2(21, 0.0);

    REQUIRE_NOTHROW([&](){
		masala::base::managers::plugin_module::MasalaPluginAPISP plugin_object(
			plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{"AnnealingSchedule"}, "LogarithmicRepeatAnnealingSchedule", true )
		);
		CHECK( plugin_object != nullptr );
		tracer->write_to_tracer( test_name, "Created an object of type " + plugin_object->inner_class_name() + "." );
		LogarithmicRepeatAnnealingSchedule_APISP anneal_sched( std::dynamic_pointer_cast< LogarithmicRepeatAnnealingSchedule_API >( plugin_object ) );
		CHECK( anneal_sched != nullptr );

		anneal_sched->set_final_time_index(21);
		anneal_sched->set_temperature_initial(120.0);
		anneal_sched->set_temperature_final(0.5);
        anneal_sched->set_n_repeats(1);

		for( Size i(0); i<21; ++i ) {
			actual_vals[i] = anneal_sched->temperature();
		}

        tracer->write_to_tracer( test_name, "Expected1:\t[ " + masala::base::utility::container::container_to_string( expected_vals, ", " ) + " ]" );
        tracer->write_to_tracer( test_name, "Actual1:\t[ " + masala::base::utility::container::container_to_string( actual_vals, ", " ) + " ]" );

		CHECK( masala::base::utility::container::equal_within_threshold( expected_vals, actual_vals, 1.0e-6 ) );

		anneal_sched->reset_call_count();
		for( Size i(21); i>0; --i ) {
			actual_vals2[i-1] = anneal_sched->temperature(i-1);
		}


        tracer->write_to_tracer( test_name, "Expected2:\t[ " + masala::base::utility::container::container_to_string( expected_vals, ", " ) + " ]" );
        tracer->write_to_tracer( test_name, "Actual2:\t[ " + masala::base::utility::container::container_to_string( actual_vals2, ", " ) + " ]" );

		CHECK( masala::base::utility::container::equal_within_threshold( expected_vals, actual_vals2, 1.0e-6 ) );

		CHECK( std::abs( anneal_sched->temperature(6) - expected_vals[6] ) < 1.0e-6 );
		CHECK( std::abs( anneal_sched->temperature(8) - expected_vals[8] ) < 1.0e-6 );
		CHECK( std::abs( anneal_sched->temperature(512) - expected_vals[20] ) < 1.0e-6 );

    }() );

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Test the samples of the logarithmic repeat annealing schedule with three repeats.", "[masala::numeric_api::auto_generated_api::optimization::annealing::LogarithmicRepeatAnnealingSchedule_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::base::managers::tracer;
    using namespace masala::base::managers::plugin_module;
    using masala::base::Real;
    using masala::base::Size;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );
    MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );

    tracer->write_to_tracer( test_name, "Starting \"Test the samples of the logarithmic repeat annealing schedule with one repeat.\"" );

	std::vector< Real > const expected_vals{ 120.0, 91.2369343143998, 69.3681515257509,
		52.7411457022189, 40.0995037174961, 30.4879648892769, 23.180237083189, 17.6241147345928,
		13.3997516532457, 10.1879355118038, 7.74596669241483, 5.88931878597818, 4.47769492694043,
		3.40442631607654, 2.58841183481742, 1.96798967126543, 1.49627786973884, 1.13763171431211,
		0.864950249938933, 0.657628409490851, 0.5,
		120.0, 91.2369343143998, 69.3681515257509,
		52.7411457022189, 40.0995037174961, 30.4879648892769, 23.180237083189, 17.6241147345928,
		13.3997516532457, 10.1879355118038, 7.74596669241483, 5.88931878597818, 4.47769492694043,
		3.40442631607654, 2.58841183481742, 1.96798967126543, 1.49627786973884, 1.13763171431211,
		0.864950249938933, 0.657628409490851, 0.5,
		120.0, 91.2369343143998, 69.3681515257509,
		52.7411457022189, 40.0995037174961, 30.4879648892769, 23.180237083189, 17.6241147345928,
		13.3997516532457, 10.1879355118038, 7.74596669241483, 5.88931878597818, 4.47769492694043,
		3.40442631607654, 2.58841183481742, 1.96798967126543, 1.49627786973884, 1.13763171431211,
		0.864950249938933, 0.657628409490851, 0.5
	};
	std::vector< Real > actual_vals(63, 0.0);
	std::vector< Real > actual_vals2(63, 0.0);

    REQUIRE_NOTHROW([&](){
		masala::base::managers::plugin_module::MasalaPluginAPISP plugin_object(
			plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{"AnnealingSchedule"}, "LogarithmicRepeatAnnealingSchedule", true )
		);
		CHECK( plugin_object != nullptr );
		tracer->write_to_tracer( test_name, "Created an object of type " + plugin_object->inner_class_name() + "." );
		LogarithmicRepeatAnnealingSchedule_APISP anneal_sched( std::dynamic_pointer_cast< LogarithmicRepeatAnnealingSchedule_API >( plugin_object ) );
		CHECK( anneal_sched != nullptr );

		anneal_sched->set_final_time_index(63);
		anneal_sched->set_temperature_initial(120.0);
		anneal_sched->set_temperature_final(0.5);
        anneal_sched->set_n_repeats(3);

		for( Size i(0); i<63; ++i ) {
			actual_vals[i] = anneal_sched->temperature();
		}

        tracer->write_to_tracer( test_name, "Expected1:\t[ " + masala::base::utility::container::container_to_string( expected_vals, ", " ) + " ]" );
        tracer->write_to_tracer( test_name, "Actual1:\t[ " + masala::base::utility::container::container_to_string( actual_vals, ", " ) + " ]" );

		CHECK( masala::base::utility::container::equal_within_threshold( expected_vals, actual_vals, 1.0e-6 ) );

		anneal_sched->reset_call_count();
		for( Size i(63); i>0; --i ) {
			actual_vals2[i-1] = anneal_sched->temperature(i-1);
		}


        tracer->write_to_tracer( test_name, "Expected2:\t[ " + masala::base::utility::container::container_to_string( expected_vals, ", " ) + " ]" );
        tracer->write_to_tracer( test_name, "Actual2:\t[ " + masala::base::utility::container::container_to_string( actual_vals2, ", " ) + " ]" );

		CHECK( masala::base::utility::container::equal_within_threshold( expected_vals, actual_vals2, 1.0e-6 ) );

		CHECK( std::abs( anneal_sched->temperature(6) - expected_vals[6] ) < 1.0e-6 );
		CHECK( std::abs( anneal_sched->temperature(8) - expected_vals[8] ) < 1.0e-6 );
		CHECK( std::abs( anneal_sched->temperature(512) - expected_vals[62] ) < 1.0e-6 );

    }() );

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

} // namespace annealing
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
