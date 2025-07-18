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

/// @file src/optimizers/annealing/ConstantAnnealingSchedule.cc
/// @brief Implementation of an annealing schedule that does not vary with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Class headers:
#include <optimizers/annealing/ConstantAnnealingSchedule.hh>

// Base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_ZeroInput.tmpl.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/setter_annotation/NoUISetterAnnotation.hh>
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
ConstantAnnealingSchedule::ConstantAnnealingSchedule(
    ConstantAnnealingSchedule const & src
) :
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule( src )
{
    *this = src;
}

/// @brief Assignment operator.
ConstantAnnealingSchedule &
ConstantAnnealingSchedule::operator=(
    ConstantAnnealingSchedule const & src
) {
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::operator=( src );
    std::lock( annealing_schedule_mutex(), src.annealing_schedule_mutex() );
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex(), std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.annealing_schedule_mutex(), std::adopt_lock );
    temperature_ = src.temperature_;
    return *this;
}

/// @brief Make a copy of this object.
masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
ConstantAnnealingSchedule::clone() const {
    return masala::make_shared< ConstantAnnealingSchedule >(*this);
}

/// @brief Make this object wholly independent.
/// @details Should be overridden for derived classes.
void
ConstantAnnealingSchedule::make_independent() {
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::make_independent();
}

/// @brief Make an independent copy of this object.
masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
ConstantAnnealingSchedule::deep_clone() const {
    ConstantAnnealingScheduleSP object_copy( std::static_pointer_cast<ConstantAnnealingSchedule>(clone()) );
    object_copy->make_independent();
    return object_copy;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the hierarchical categories for this plugin class.
/// @details The base class implementation returns { { "AnnealingSchedule" } }
std::vector< std::vector< std::string > >
ConstantAnnealingSchedule::get_categories() const {
    return masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::get_categories();
}

/// @brief Get the ahierarchical keywords for this plugin class.
/// @details The base class implementation returns { "annealing_schedule", "constant", "time_independent" }
std::vector< std::string >
ConstantAnnealingSchedule::get_keywords() const {
    std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::get_keywords() );
    outvec.push_back( "constant" );
    outvec.push_back( "time_independent" );
    return outvec;
}

/// @brief Get the class name ("ConstantAnnealingSchedule").
std::string
ConstantAnnealingSchedule::class_name() const {
    return "ConstantAnnealingSchedule";
}

/// @brief Get the class namespace ("standard_masala_plugins::optimizers::annealing").
std::string
ConstantAnnealingSchedule::class_namespace() const {
    return "standard_masala_plugins::optimizers::annealing";
}

/// @brief Get the API description.
masala::base::api::MasalaObjectAPIDefinitionCWP
ConstantAnnealingSchedule::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using namespace masala::base::api::getter;
    using namespace masala::base::api::work_function;

    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );

    if( api_definition() == nullptr ) {
        MasalaObjectAPIDefinitionSP api_def(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this, "An annealing schedule that does not vary with time.", false, false
            )
        );

        // Constructors
        api_def->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< ConstantAnnealingSchedule > >( 
                "ConstantAnnealingSchedule", "Construct a ConstantAnnealingSchedule object, with temperature initialized to 0.62 kcal/mol."
            )
        );
        api_def->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< ConstantAnnealingSchedule, ConstantAnnealingSchedule const & > >( 
                "ConstantAnnealingSchedule", "Copy another ConstantAnnealingSchedule object.",
                "src", "The object to copy, unaltered by this operation."
            )
        );

        // Setters
        {
			MasalaObjectAPISetterDefinition_ZeroInputSP reset_fxn(
				masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
					"reset", "Reset this object's call count, as well as setting temperature back to 0.62.",
					false, false, std::bind( &ConstantAnnealingSchedule::reset, this )
				)
			);
			reset_fxn->add_setter_annotation( masala::make_shared< setter_annotation::NoUISetterAnnotation >() );
			api_def->add_setter( reset_fxn );
        }
		{
			MasalaObjectAPISetterDefinition_ZeroInputSP reset_call_fxn(
				masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
					"reset_call_count", "Reset this object's call count.",
					false, true, std::bind( &ConstantAnnealingSchedule::reset_call_count, this )
				)
			);
			reset_call_fxn->add_setter_annotation( masala::make_shared< setter_annotation::NoUISetterAnnotation >() );
			api_def->add_setter( reset_call_fxn );
		}
		{
			MasalaObjectAPISetterDefinition_OneInputSP< masala::base::Size > set_final_fxn(
				masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Size > >(
					"set_final_time_index", "Set the final time index in the annealing schedule.  (Does nothing for a constant annealing schedule.)",
					"final_time_index", "The index of the final timepoint in the annealing schedule.",
					false, true, std::bind( &ConstantAnnealingSchedule::set_final_time_index, this, std::placeholders::_1 )
				)
			);
			set_final_fxn->add_setter_annotation( masala::make_shared< setter_annotation::NoUISetterAnnotation >() );
			api_def->add_setter( set_final_fxn );
		}
        api_def->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::Real > >(
                "set_temperature", "Set the temperature, in kcal/mol.  Default is 0.62.",
                "temperature_in", "The temperature to set, in kcal/mol.",
                false, false, std::bind( &ConstantAnnealingSchedule::set_temperature, this, std::placeholders::_1 )
            )
        );

        // Getters
        api_def->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::base::Size > >(
                "get_call_count", "Get the current call count.",
                "call_count", "The number of times the temperature() function has been called.",
                false, false, std::bind( &ConstantAnnealingSchedule::get_call_count, this )
            )
        );

        // Work functions
        api_def->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ZeroInput< masala::base::Real > >(
                "temperature", "Get the temperature at the current timepoint, and increment the timepoint counter.  In this "
                "case, the same value is returned every time.", true, false, false, true, "temperature",
                "The temperature at the current timepoint (the constant temperature value).",
                std::bind( static_cast<masala::base::Real(ConstantAnnealingSchedule::*)() const>( &ConstantAnnealingSchedule::temperature ), this )
            )
        );
        api_def->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< masala::base::Real, masala::base::Size > >(
                "temperature", "Get the temperature at the given timepoint.  In this "
                "case, the same value is returned every time.  This does not increment the "
                "timepoint counter.",
                true, false, false, true,
                "time_index", "The timepoint at which we are getting temperature.",
                "temperature",
                "The temperature at the current timepoint (the constant temperature value).",
                std::bind( static_cast<masala::base::Real(ConstantAnnealingSchedule::*)( masala::base::Size const ) const>( &ConstantAnnealingSchedule::temperature ), this, std::placeholders::_1 )
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
ConstantAnnealingSchedule::temperature() const {
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::increment_call_count();
    return temperature_;
}

/// @brief Return temperature for the Nth timepoint.
masala::base::Real
ConstantAnnealingSchedule::temperature(
    masala::base::Size const
) const {
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    return temperature_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Reset this object.
void
ConstantAnnealingSchedule::reset() {
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    temperature_ = 0.62;
    masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule::reset_call_count();
}

/// @brief Set the temperature.
/// @details In kcal/mol.  Must be non-negative.
void
ConstantAnnealingSchedule::set_temperature(
    masala::base::Real const temperature_in
) {
    CHECK_OR_THROW_FOR_CLASS( temperature_in >= 0.0, "set_temperature", "The temperature must be greater than or equal to zero, but got " + std::to_string( temperature_in ) + " kcal/mol." );
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    temperature_ = temperature_in;
}

/// @brief Set the index of the expected final timepoint.
/// @details For the constant annealing schedule, this does nothing.
void
ConstantAnnealingSchedule::set_final_time_index(
    masala::base::Size const
) {
    //GNDN
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the call count.
masala::base::Size
ConstantAnnealingSchedule::get_call_count() const {
    std::lock_guard< std::mutex > lock( annealing_schedule_mutex() );
    return call_count();
}

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins
