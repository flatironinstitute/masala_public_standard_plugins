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

/// @file src/optimizers/annealing/LogarithmicAnnealingSchedule.hh
/// @brief Headers for an annealing schedule that changes linearly  in logarithmic space with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMasalaPlugins_src_optimizers_annealing_LogarithmicAnnealingSchedule_hh
#define StandardMasalaPlugins_src_optimizers_annealing_LogarithmicAnnealingSchedule_hh

// Forward declarations:
#include <optimizers/annealing/LogarithmicAnnealingSchedule.fwd.hh>

// Parent class:
#include <optimizers/annealing/LinearAnnealingSchedule.hh>

// STL headers
#include <mutex>
#include <cmath>

namespace standard_masala_plugins {
namespace optimizers {
namespace annealing {

/// @brief An annealing schedule that changes linearly  in logarithmic space with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class LogarithmicAnnealingSchedule : public standard_masala_plugins::optimizers::annealing::LinearAnnealingSchedule {

	typedef standard_masala_plugins::optimizers::annealing::LinearAnnealingSchedule Parent;
	typedef standard_masala_plugins::optimizers::annealing::LinearAnnealingScheduleSP ParentSP;
	typedef standard_masala_plugins::optimizers::annealing::LinearAnnealingScheduleCSP ParentCSP;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	LogarithmicAnnealingSchedule() = default;

	/// @brief Copy constructor.
	LogarithmicAnnealingSchedule( LogarithmicAnnealingSchedule const & );

	/// @brief Assignment operator.
	LogarithmicAnnealingSchedule & operator=( LogarithmicAnnealingSchedule const & );

	/// @brief Virtual destructor.
	virtual ~LogarithmicAnnealingSchedule() = default;

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
	/// @details The base class implementation returns { "annealing_schedule", "logarithmic", "time_dependent" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the class name ("LogarithmicAnnealingSchedule").
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

	/// @brief Set the initial temperature.
	/// @details In kcal/mol.  Must be positive.
	void
	set_temperature_initial(
		masala::base::Real const temperature_in
	) override;

	/// @brief Set the final temperature.
	/// @details In kcal/mol.  Must be positive.
	void
	set_temperature_final(
		masala::base::Real const temperature_in
	) override;

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////


protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Reset this object without locking mutex.  Should be called from a mutex-locked
	/// context.  Derived classes should override this function and call the base class version.
	void protected_reset() override;

	/// @brief Copy object src to this object without locking mutex.  Should be called from a mutex-locked
	/// context.  Derived classes should override this function and call the base class version.
	void protected_assign( LinearAnnealingSchedule const & src ) override;

	/// @brief Access the log of the initial temperature.
	inline masala::base::Real protected_log_initial_temperature() const { return log_initial_temperature_; }

	/// @brief Access the log of the final temperature.
	inline masala::base::Real protected_log_final_temperature() const { return log_final_temperature_; }

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The natural logarithm of the initial temperature.  Defaults to ln(100.0).
	masala::base::Real log_initial_temperature_ = ( std::log( 100.0 ) );

	/// @brief The natural logarithm of the final temperature.  Defaults to ln(0.3).
	masala::base::Real log_final_temperature_ = ( std::log( 0.3 ) );

}; // class LogarithmicAnnealingSchedule

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //StandardMasalaPlugins_src_optimizers_annealing_LogarithmicAnnealingSchedule_hh