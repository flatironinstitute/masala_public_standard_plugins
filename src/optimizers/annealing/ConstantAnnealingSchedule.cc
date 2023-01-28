/*
    Masala
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
ConstantAnnealingSchedule::ConstantAnnealingSchedule(
    ConstantAnnealingSchedule const & src
) :
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule( src )
{
    std::lock_guard< std::mutex > lock( src.constant_annealing_schedule_mutex_ );
    temperature_ = src.temperature_;
}

/// @brief Assignment operator.
ConstantAnnealingSchedule &
ConstantAnnealingSchedule::operator=(
    ConstantAnnealingSchedule const & src
) {
    std::lock( constant_annealing_schedule_mutex_, src.constant_annealing_schedule_mutex_ );
    std::lock_guard< std::mutex > lock( constant_annealing_schedule_mutex_, std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.constant_annealing_schedule_mutex_, std::adopt_lock );
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule::operator=( src );
    temperature_ = src.temperature_;
    return *this;
}

/// @brief Make a copy of this object.
masala::numeric_api::base_classes::optimization::annealing::AnnealingScheduleSP
ConstantAnnealingSchedule::clone() const {
    return masala::make_shared< ConstantAnnealingSchedule >(*this);
}

/// @brief Make this object wholly independent.
/// @details Should be overridden for derived classes.
void
ConstantAnnealingSchedule::make_independent() {
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule::make_independent();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the hierarchical categories for this plugin class.
/// @details The base class implementation returns { { "AnnealingSchedule" } }
std::vector< std::vector< std::string > >
ConstantAnnealingSchedule::get_categories() const {
    return masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule::get_categories();
}

/// @brief Get the ahierarchical keywords for this plugin class.
/// @details The base class implementation returns { "annealing_schedule", "constant", "time_independent" }
std::vector< std::string >
ConstantAnnealingSchedule::get_keywords() const {
    std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule::get_keywords() );
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

    std::lock_guard< std::mutex > lock( constant_annealing_schedule_mutex_ );

    if( api_definition_ == nullptr ) {
        MasalaObjectAPIDefinitionSP api_definition(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this, "An annealing schedule that does not vary with time.", false
            )
        );

        // Constructors
        api_definition->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_ZeroInput< ConstantAnnealingSchedule > >( 
                "ConstantAnnealingSchedule", "Construct a ConstantAnnealingSchedule object, with temperature initialized to 0.62 kcal/mol."
            )
        );
        api_definition->add_constructor(
            masala::make_shared< MasalaObjectAPIConstructorDefinition_OneInput< ConstantAnnealingSchedule, ConstantAnnealingSchedule const & > >( 
                "ConstantAnnealingSchedule", "Copy another ConstantAnnealingSchedule object.",
                "src", "The object to copy, unaltered by this operation."
            )
        );

        // Setters
        api_definition->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_ZeroInput >(
                "reset", "Reset this object's call count, as well as setting temperature back to 0.62.",
                false, false, std::bind( &ConstantAnnealingSchedule::reset, this )
            )
        );
        api_definition->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::numeric_api::Real > >(
                "set_temperature", "Set the temperature, in kcal/mol.  Default is 0.62.",
                "temperature_in", "The temperature to set, in kcal/mol.",
                false, false, std::bind( &ConstantAnnealingSchedule::set_temperature, this, std::placeholders::_1 )
            )
        );

        // Getters
        api_definition->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::numeric_api::Size > >(
                "get_call_count", "Get the current call count.",
                "call_count", "The number of times the temperature() function has been called.",
                false, false, std::bind( &ConstantAnnealingSchedule::get_call_count, this )
            )
        );
        api_definition->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::numeric_api::Real > >(
                "get_temperature", "Get the temperature, in kcal/mol.  Default is 0.62.",
                "temperature", "The temperature, in kcal/mol.",
                false, false, std::bind( &ConstantAnnealingSchedule::get_temperature, this )
            )
        );

        // Work functions
        api_definition->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ZeroInput< masala::numeric_api::Real > >(
                "temperature", "Get the temperature at the current timepoint, and increment the timepoint counter.  In this "
                "case, the same value is returned every time.", true, false, true, false, "temperature",
                "The temperature at the current timepoint (the constant temperature value).",
                std::bind( static_cast<masala::numeric_api::Real(ConstantAnnealingSchedule::*)() const>( &ConstantAnnealingSchedule::temperature ), this )
            )
        );
        api_definition->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< masala::numeric_api::Real, masala::numeric_api::Size > >(
                "temperature", "Get the temperature at the given timepoint.  In this "
                "case, the same value is returned every time.  This does not increment the "
                "timepoint counter.",
                true, false, true, false,
                "time_index", "The timepoint at which we are getting temperature.",
                "temperature",
                "The temperature at the current timepoint (the constant temperature value).",
                std::bind( static_cast<masala::numeric_api::Real(ConstantAnnealingSchedule::*)( masala::numeric_api::Size const ) const>( &ConstantAnnealingSchedule::temperature ), this, std::placeholders::_1 )
            )
        );

        api_definition_ = api_definition;
    }
    return api_definition_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Return temperature.
masala::numeric_api::Real
ConstantAnnealingSchedule::temperature() const {
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule::increment_call_count();
    return temperature_;
}

/// @brief Return temperature for the Nth timepoint.
masala::numeric_api::Real
ConstantAnnealingSchedule::temperature(
    masala::numeric_api::Size const
) const {
    return temperature_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Reset this object.
void
ConstantAnnealingSchedule::reset() {
    std::lock_guard< std::mutex > lock( constant_annealing_schedule_mutex_ );
    temperature_ = 0.62;
    masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule::reset_call_count();
}

/// @brief Set the temperature.
/// @details In kcal/mol.  Must be positive.
void
ConstantAnnealingSchedule::set_temperature(
    masala::numeric_api::Real const temperature_in
) {
    CHECK_OR_THROW_FOR_CLASS( temperature_in > 0.0, "set_temperature", "The temperature must be greater than zero, but got " + std::to_string( temperature_in ) + " kcal/mol." );
    std::lock_guard< std::mutex > lock( constant_annealing_schedule_mutex_ );
    temperature_ = temperature_in;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the call count.
masala::numeric_api::Size
ConstantAnnealingSchedule::get_call_count() const {
    return call_count();
}

/// @brief Get the temperature.
masala::numeric_api::Real
ConstantAnnealingSchedule::get_temperature() const {
    std::lock_guard< std::mutex > lock( constant_annealing_schedule_mutex_ );
    return temperature_;
}

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins
