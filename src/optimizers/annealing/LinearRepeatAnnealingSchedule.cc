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

/// @file src/optimizers/annealing/LinearRepeatAnnealingSchedule.cc
/// @brief Implementation of an annealing schedule that changes linearly with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Class headers:
#include <optimizers/annealing/LinearRepeatAnnealingSchedule.hh>

// Base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_ZeroInput.tmpl.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>

// STL headers

namespace standard_masala_plugins {
namespace optimizers {
namespace annealing {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
LinearRepeatAnnealingSchedule::LinearRepeatAnnealingSchedule(
    LinearRepeatAnnealingSchedule const & src
) :
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule( src )
{
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    temperature_initial_ = src.temperature_initial_;
    temperature_final_ = src.temperature_final_;
    call_count_final_ = src.call_count_final_;
}

/// @brief Assignment operator.
LinearRepeatAnnealingSchedule &
LinearRepeatAnnealingSchedule::operator=(
    LinearRepeatAnnealingSchedule const & src
) {
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::operator=( src );
    std::lock( annealing_schedule_mutex(), src.annealing_schedule_mutex() );
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex(), std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.annealing_schedule_mutex(), std::adopt_lock );
    temperature_initial_ = src.temperature_initial_;
    temperature_final_ = src.temperature_final_;
    call_count_final_ = src.call_count_final_;
    return *this;
}

/// @brief Make a copy of this object.
masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
LinearRepeatAnnealingSchedule::clone() const {
    return masala::make_shared< LinearRepeatAnnealingSchedule >(*this);
}

/// @brief Make this object wholly independent.
/// @details Should be overridden for derived classes.
void
LinearRepeatAnnealingSchedule::make_independent() {
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::make_independent();
}

/// @brief Make an independent copy of this object.
masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
LinearRepeatAnnealingSchedule::deep_clone() const {
    LinearRepeatAnnealingScheduleSP object_copy( std::static_pointer_cast<LinearRepeatAnnealingSchedule>(clone()) );
    object_copy->make_independent();
    return object_copy;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the hierarchical categories for this plugin class.
/// @details The base class implementation returns { { "AnnealingSchedule" } }
std::vector< std::vector< std::string > >
LinearRepeatAnnealingSchedule::get_categories() const {
    return masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::get_categories();
}

/// @brief Get the ahierarchical keywords for this plugin class.
/// @details The base class implementation returns { "annealing_schedule", "linear", "repeat", "time_dependent" }
std::vector< std::string >
LinearRepeatAnnealingSchedule::get_keywords() const {
	std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::get_keywords() );
	outvec.push_back( "linear" );
	outvec.push_back( "repeat" );
	outvec.push_back( "time_dependent" );
	return outvec;
}

/// @brief Get the class name ("LinearRepeatAnnealingSchedule").
std::string
LinearRepeatAnnealingSchedule::class_name() const {
    return "LinearRepeatAnnealingSchedule";
}

/// @brief Get the class namespace ("standard_masala_plugins::optimizers::annealing").
std::string
LinearRepeatAnnealingSchedule::class_namespace() const {
    return "standard_masala_plugins::optimizers::annealing";
}

/// @brief Get the API description.
masala::base::api::MasalaObjectAPIDefinitionCWP
LinearRepeatAnnealingSchedule::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using namespace masala::base::api::getter;
    using namespace masala::base::api::work_function;

    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );

    if( api_definition() == nullptr ) {
        MasalaObjectAPIDefinitionSP api_def(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this, "An annealing schedule that ramps linearly with time, then jumps back up sharply to ramp linearly with time again (a sawtooth pattern).", false, false
            )
        );

        // Constructors
        api_def->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< LinearRepeatAnnealingSchedule > >( 
                "LinearRepeatAnnealingSchedule", "Construct a LinearRepeatAnnealingSchedule object, with temperature initialized to 0.62 kcal/mol."
            )
        );
        api_def->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< LinearRepeatAnnealingSchedule, LinearRepeatAnnealingSchedule const & > >( 
                "LinearRepeatAnnealingSchedule", "Copy another LinearRepeatAnnealingSchedule object.",
                "src", "The object to copy, unaltered by this operation."
            )
        );

        // Setters
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
                "reset", "Reset this object's call count, as well as setting temperature back to 0.62.",
                false, false, std::bind( &LinearRepeatAnnealingSchedule::reset, this )
            )
        );
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
                "reset_call_count", "Reset this object's call count.",
                false, true, std::bind( &LinearRepeatAnnealingSchedule::reset_call_count, this )
            )
        );
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Size > >(
                "set_final_time_index", "Set the final time index in the annealing schedule.",
                "final_time_index", "The index of the final timepoint in the annealing schedule.",
                false, true, std::bind( &LinearRepeatAnnealingSchedule::set_final_time_index, this, std::placeholders::_1 )
            )
        );
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Real > >(
                "set_temperature_initial", "Set the initial temperature, in kcal/mol.  Default is 3.0.",
                "temperature_in", "The temperature to set, in kcal/mol.  Must be non-negative.",
                false, false, std::bind( &LinearRepeatAnnealingSchedule::set_temperature_initial, this, std::placeholders::_1 )
            )
        );        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Real > >(
                "set_temperature_final", "Set the final temperature, in kcal/mol.  Default is 0.4.",
                "temperature_in", "The temperature to set, in kcal/mol.  Must be non-negative.",
                false, false, std::bind( &LinearRepeatAnnealingSchedule::set_temperature_final, this, std::placeholders::_1 )
            )
        );

        // Getters
        api_def->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::base::Size > >(
                "get_call_count", "Get the current call count.",
                "call_count", "The number of times the temperature() function has been called.",
                false, false, std::bind( &LinearRepeatAnnealingSchedule::get_call_count, this )
            )
        );

        // Work functions
        api_def->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ZeroInput< masala::base::Real > >(
                "temperature", "Get the temperature at the current timepoint, and increment the timepoint counter.",
                true, false, false, true, "temperature",
                "The temperature at the current timepoint (which varies linearly with timepoint).",
                std::bind( static_cast<masala::base::Real(LinearRepeatAnnealingSchedule::*)() const>( &LinearRepeatAnnealingSchedule::temperature ), this )
            )
        );
        api_def->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< masala::base::Real, masala::base::Size > >(
                "temperature", "Get the temperature at the given timepoint.  This does not increment the "
                "timepoint counter.",
                true, false, false, true,
                "time_index", "The timepoint at which we are getting temperature.",
                "temperature",
                "The temperature at the current timepoint (which varies linearly with timepoint).",
                std::bind( static_cast<masala::base::Real(LinearRepeatAnnealingSchedule::*)( masala::base::Size const ) const>( &LinearRepeatAnnealingSchedule::temperature ), this, std::placeholders::_1 )
            )
        );

        api_definition() = api_def;
    }
    return api_definition();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Return temperature.
masala::base::Real
LinearRepeatAnnealingSchedule::temperature() const {
	TODO TODO TODO;
    // using masala::base::Size;
    // using masala::base::Real;
    // std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    // masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::increment_call_count();
    // Size const callcount( call_count() );
    // Real const f( static_cast< Real >( callcount - 1 ) / static_cast< Real >( call_count_final_ - 1 ) );
    // return f * temperature_final_ + (1.0 - f) * temperature_initial_;
}

/// @brief Return temperature for the Nth timepoint.
masala::base::Real
LinearRepeatAnnealingSchedule::temperature(
    masala::base::Size const time_index
) const {
	TODO TODO TODO;
    // std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    // if( time_index > call_count_final_ ) {
    //     return temperature_final_;
    // }
    // masala::base::Real const f( static_cast< masala::base::Real >( time_index - 1 ) / static_cast< masala::base::Real >( call_count_final_ - 1 ) );
    // return f * temperature_final_ + (1.0 - f) * temperature_initial_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Reset this object without locking mutex.  Should be called from a mutex-locked
/// context.  Derived classes should override this function and call the base class version.
void
LinearRepeatAnnealingSchedule::protected_reset() /*override*/ {
    LinearAnnealingSchedule::protected_reset();
}

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins
