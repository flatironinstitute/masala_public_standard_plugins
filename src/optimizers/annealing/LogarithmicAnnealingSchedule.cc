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

/// @file src/optimizers/annealing/LogarithmicAnnealingSchedule.cc
/// @brief Implementation of an annealing schedule that changes linearly in logarithmic space with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Class headers:
#include <optimizers/annealing/LogarithmicAnnealingSchedule.hh>

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
LogarithmicAnnealingSchedule::LogarithmicAnnealingSchedule(
    LogarithmicAnnealingSchedule const & src
) :
    Parent()
{
    *this = src; // Locks mutex and calls protected_assign.
}

/// @brief Assignment operator.
LogarithmicAnnealingSchedule &
LogarithmicAnnealingSchedule::operator=(
    LogarithmicAnnealingSchedule const & src
) {
    std::lock( annealing_schedule_mutex(), src.annealing_schedule_mutex() );
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex(), std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.annealing_schedule_mutex(), std::adopt_lock );
    protected_assign(src);
    return *this;
}

/// @brief Make a copy of this object.
masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
LogarithmicAnnealingSchedule::clone() const {
    return masala::make_shared< LogarithmicAnnealingSchedule >(*this);
}

/// @brief Make this object wholly independent.
/// @details Should be overridden for derived classes.
void
LogarithmicAnnealingSchedule::make_independent() {
   Parent::make_independent();
}

/// @brief Make an independent copy of this object.
masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
LogarithmicAnnealingSchedule::deep_clone() const {
    LogarithmicAnnealingScheduleSP object_copy( std::static_pointer_cast<LogarithmicAnnealingSchedule>(clone()) );
    object_copy->make_independent();
    return object_copy;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the hierarchical categories for this plugin class.
/// @details The base class implementation returns { { "AnnealingSchedule" } }
std::vector< std::vector< std::string > >
LogarithmicAnnealingSchedule::get_categories() const {
    return masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::get_categories();
}

/// @brief Get the ahierarchical keywords for this plugin class.
/// @details The base class implementation returns { "annealing_schedule", "logarithmic", "time_dependent" }
std::vector< std::string >
LogarithmicAnnealingSchedule::get_keywords() const {
    std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::get_keywords() );
    outvec.push_back( "logarithmic" );
    outvec.push_back( "time_dependent" );
    return outvec;
}

/// @brief Get the class name ("LogarithmicAnnealingSchedule").
std::string
LogarithmicAnnealingSchedule::class_name() const {
    return "LogarithmicAnnealingSchedule";
}

/// @brief Get the class namespace ("standard_masala_plugins::optimizers::annealing").
std::string
LogarithmicAnnealingSchedule::class_namespace() const {
    return "standard_masala_plugins::optimizers::annealing";
}

/// @brief Get the API description.
masala::base::api::MasalaObjectAPIDefinitionCWP
LogarithmicAnnealingSchedule::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using namespace masala::base::api::getter;
    using namespace masala::base::api::work_function;

    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );

    if( api_definition() == nullptr ) {
        MasalaObjectAPIDefinitionSP api_def(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this, "An annealing schedule that ramps linearly in logarithmic space with time.  If ramping from a high to a low temperature, this means that more time is spent at the low temperatures.", false, false
            )
        );

        // Constructors
        api_def->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< LogarithmicAnnealingSchedule > >( 
                "LogarithmicAnnealingSchedule", "Construct a LogarithmicAnnealingSchedule object."
            )
        );
        api_def->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< LogarithmicAnnealingSchedule, LogarithmicAnnealingSchedule const & > >( 
                "LogarithmicAnnealingSchedule", "Copy another LogarithmicAnnealingSchedule object.",
                "src", "The object to copy, unaltered by this operation."
            )
        );

        // Setters
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
                "reset", "Reset this object's call count and all settings.",
                false, false, std::bind( &LogarithmicAnnealingSchedule::reset, this )
            )
        );
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
                "reset_call_count", "Reset this object's call count.",
                false, true, std::bind( &LogarithmicAnnealingSchedule::reset_call_count, this )
            )
        );
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Size > >(
                "set_final_time_index", "Set the final time index in the annealing schedule.",
                "final_time_index", "The index of the final timepoint in the annealing schedule.  Note that this is actually "
                "the final zero-based index plus one, or equivalently the number of timepoints.",
                false, true, std::bind( &LogarithmicAnnealingSchedule::set_final_time_index, this, std::placeholders::_1 )
            )
        );
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Real > >(
                "set_temperature_initial", "Set the initial temperature, in kcal/mol.  Default is 100.0.",
                "temperature_in", "The temperature to set, in kcal/mol.  Must be non-negative.",
                false, true, std::bind( &LogarithmicAnnealingSchedule::set_temperature_initial, this, std::placeholders::_1 )
            )
        );
		api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Real > >(
                "set_temperature_final", "Set the final temperature, in kcal/mol.  Default is 0.3.",
                "temperature_in", "The temperature to set, in kcal/mol.  Must be non-negative.",
                false, true, std::bind( &LogarithmicAnnealingSchedule::set_temperature_final, this, std::placeholders::_1 )
            )
        );

        // Getters
        api_def->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::base::Size > >(
                "get_call_count", "Get the current call count.",
                "call_count", "The number of times the temperature() function has been called.  Equivalently, the zero-based index "
                "of the last call.",
                false, false, std::bind( &LogarithmicAnnealingSchedule::get_call_count, this )
            )
        );

        // Work functions
        api_def->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ZeroInput< masala::base::Real > >(
                "temperature", "Get the temperature at the current timepoint, and increment the timepoint counter.",
                true, false, false, true, "temperature",
                "The temperature at the current timepoint (which varies linearly in logarithmic space with timepoint).",
                std::bind( static_cast<masala::base::Real(LogarithmicAnnealingSchedule::*)() const>( &LogarithmicAnnealingSchedule::temperature ), this )
            )
        );
        api_def->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< masala::base::Real, masala::base::Size > >(
                "temperature", "Get the temperature at the given timepoint.  This does not increment the "
                "timepoint counter.  Note that the timepoints are zero-indexed (i.e. the first timepoint is time 0).",
                true, false, false, true,
                "time_index", "The timepoint at which we are getting temperature.",
                "temperature",
                "The temperature at the current timepoint (which varies linearly in logarithmic space with timepoint).",
                std::bind( static_cast<masala::base::Real(LogarithmicAnnealingSchedule::*)( masala::base::Size const ) const>( &LogarithmicAnnealingSchedule::temperature ), this, std::placeholders::_1 )
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
LogarithmicAnnealingSchedule::temperature() const {
	using masala::base::Size;
	using masala::base::Real;
	std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
	Parent::increment_call_count();
	Size const callcount( call_count() );
	if( callcount >= protected_call_count_final() ) { return protected_temperature_final(); }
	Real const f( static_cast< Real >( callcount - 1 ) / static_cast< Real >( protected_call_count_final() - 1 ) );
	return std::exp( f * log_final_temperature_ + (1.0 - f) * log_initial_temperature_ );
}

/// @brief Return temperature for the Nth timepoint.
masala::base::Real
LogarithmicAnnealingSchedule::temperature(
    masala::base::Size const time_index
) const {
	std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
	if( time_index >= protected_call_count_final() ) {
		return protected_temperature_final();
	}
	masala::base::Real const f( static_cast< masala::base::Real >( time_index ) / static_cast< masala::base::Real >( protected_call_count_final() - 1 ) );
	return std::exp( f * log_final_temperature_ + (1.0 - f) * log_initial_temperature_ );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the initial temperature.
/// @details In kcal/mol.  Must be positive.
void
LogarithmicAnnealingSchedule::set_temperature_initial(
    masala::base::Real const temperature_in
) {
	CHECK_OR_THROW_FOR_CLASS( temperature_in > 0.0, "set_temperature_initial", "The initial temperature must be greater than zero, but got " + std::to_string( temperature_in ) + " kcal/mol." );
	std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
	nonconst_protected_temperature_initial() = temperature_in;
	log_initial_temperature_ = std::log( temperature_in );
}

/// @brief Set the final temperature.
/// @details In kcal/mol.  Must be positive.
void
LogarithmicAnnealingSchedule::set_temperature_final(
    masala::base::Real const temperature_in
) {
	CHECK_OR_THROW_FOR_CLASS( temperature_in > 0.0, "set_temperature_final", "The final temperature must be greater than zero, but got " + std::to_string( temperature_in ) + " kcal/mol." );
	std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
	nonconst_protected_temperature_final() = temperature_in;
	log_final_temperature_ = std::log( temperature_in );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Reset this object without locking mutex.  Should be called from a mutex-locked
/// context.  Derived classes should override this function and call the base class version.
/*virtual*/
void
LogarithmicAnnealingSchedule::protected_reset() {
	log_initial_temperature_ = std::log( 100.0 );
	log_final_temperature_ = std::log( 0.3 );
	Parent::protected_reset();
}

/// @brief Copy object src to this object without locking mutex.  Should be called from a mutex-locked
/// context.  Derived classes should override this function and call the base class version.
/*virtual*/
void
LogarithmicAnnealingSchedule::protected_assign(
    LinearAnnealingSchedule const & src
) {
	LogarithmicAnnealingSchedule const * src_ptr_cast( dynamic_cast< LogarithmicAnnealingSchedule const * >( & src) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "A " + src.class_name() +
		" object was passed to this function.  This could not be cast to a LogarithmicAnnealingSchedule object."
	);
	log_initial_temperature_ = src_ptr_cast->log_initial_temperature_;
	log_final_temperature_ = src_ptr_cast->log_final_temperature_;
	Parent::protected_assign( src );
}

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins
