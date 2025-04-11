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

/// @file src/optimizers/annealing/LogarithmicRepeatAnnealingSchedule.hh
/// @brief Headers for an annealing schedule that changes linearly with time, then jumps back up to ramp down again (a sawtooth pattern).
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMasalaPlugins_src_optimizers_annealing_LogarithmicRepeatAnnealingSchedule_hh
#define StandardMasalaPlugins_src_optimizers_annealing_LogarithmicRepeatAnnealingSchedule_hh

// Forward declarations:
#include <optimizers/annealing/LogarithmicRepeatAnnealingSchedule.fwd.hh>

// Parent class:
#include <optimizers/annealing/LinearAnnealingSchedule.hh>

// STL headers
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace annealing {

/// @brief An annealing schedule that changes linearly with time, then jumps back up to ramp down again (a sawtooth pattern).
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class LogarithmicRepeatAnnealingSchedule : public standard_masala_plugins::optimizers::annealing::LinearAnnealingSchedule {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	LogarithmicRepeatAnnealingSchedule() = default;

	/// @brief Copy constructor.
	LogarithmicRepeatAnnealingSchedule( LogarithmicRepeatAnnealingSchedule const & );

	/// @brief Assignment operator.
	LogarithmicRepeatAnnealingSchedule & operator=( LogarithmicRepeatAnnealingSchedule const & );

	/// @brief Virtual destructor.
	virtual ~LogarithmicRepeatAnnealingSchedule() = default;

	/// @brief Make a copy of this object.
	masala::numeric::optimization::annealing::AnnealingScheduleBaseSP
	clone() const override;

	/// @brief Make this object wholly independent.
	/// @details Should be overridden for derived classes.
	void make_independent() override;

	/// @brief Make an independent copy of this object.
	masala::numeric::optimization::annealing::AnnealingScheduleBaseSP deep_clone() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the hierarchical categories for this plugin class.
	/// @details The base class implementation returns { { "AnnealingSchedule" } }
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the ahierarchical keywords for this plugin class.
	/// @details The base class implementation returns { "annealing_schedule", "linear", "repeat", "time_dependent" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the class name ("LogarithmicRepeatAnnealingSchedule").
	std::string class_name() const override;

	/// @brief Get the class namespace ("standard_masala_plugins::optimizers::annealing").
	std::string class_namespace() const override;

	/// @brief Get the API description.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Return temperature.
	masala::base::Real
	temperature() const override;

	/// @brief Return temperature for the Nth timepoint.
	masala::base::Real
	temperature(
		masala::base::Size const time_index
	) const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the number of times that we'll ramp the temperature.
	/// @details Must be positive.
	void set_n_repeats( masala::base::Size const setting );

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the number of times that we'll ramp the temperature.
	/// @details Must be positive.
	masala::base::Size n_repeats() const;

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Reset this object without locking mutex.  Should be called from a mutex-locked
	/// context.  Derived classes should override this function and call the base class version.
	void protected_reset() override;

	/// @brief Copy object src to this object without locking mutex.  Should be called from a mutex-locked
	/// context.  Derived classes should override this function and call the base class version.
	void protected_assign( LinearAnnealingSchedule const & src ) override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The number of times to ramp.  Defaults to 3.
	masala::base::Size n_repeats_ = 3;

}; // class LogarithmicRepeatAnnealingSchedule

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //StandardMasalaPlugins_src_optimizers_annealing_LogarithmicRepeatAnnealingSchedule_hh