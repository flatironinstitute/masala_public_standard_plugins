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
#include <numeric_api/base_classes/optimization/annealing/PluginAnnealingSchedule.hh>

// STL headers
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace annealing {

/// @brief An annealing schedule that changes linearly  in logarithmic space with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class LogarithmicAnnealingSchedule : public masala::numeric_api::base_classes::optimization::annealing::PluginAnnealingSchedule {

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

	/// @brief Reset this object.
	void reset();

	/// @brief Set the initial temperature.
	/// @details In kcal/mol.  Must be non-negative.
	void
	set_temperature_initial(
		masala::base::Real const temperature_in
	);

	/// @brief Set the final temperature.
	/// @details In kcal/mol.  Must be non-negative.
	void
	set_temperature_final(
		masala::base::Real const temperature_in
	);

	/// @brief Set the index of the expected final timepoint.
	void
	set_final_time_index(
		masala::base::Size const final_time_index_in
	) override;

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the call count.
	masala::base::Size get_call_count() const;

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Reset this object without locking mutex.  Should be called from a mutex-locked
	/// context.  Derived classes should override this function and call the base class version.
	virtual void protected_reset();

	/// @brief Copy object src to this object without locking mutex.  Should be called from a mutex-locked
	/// context.  Derived classes should override this function and call the base class version.
	virtual void protected_assign( LogarithmicAnnealingSchedule const & src );

	/// @brief Access the initial temperature from a mutex-locked context.
	inline masala::base::Real protected_temperature_initial() const { return temperature_initial_; }

	/// @brief Access the final temperature from a mutex-locked context.
	inline masala::base::Real protected_temperature_final() const { return temperature_final_; }

	/// @brief Access the expected call count from a mutex-locked context.
	inline masala::base::Size protected_call_count_final() const { return call_count_final_; }

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The initial temperature.
	/// @details In units of kcal/mol.  Defaults to 3.0.
	masala::base::Real temperature_initial_ = 3.0;

	/// @brief The final temperature.
	/// @details In units of kcal/mol.  Defaults to 0.4.
	masala::base::Real temperature_final_ = 0.4;

	/// @brief The number of calls expected.
	/// @details Defaults to 100,000, arbitrarily.
	masala::base::Size call_count_final_ = 100000;

}; // class LogarithmicAnnealingSchedule

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //StandardMasalaPlugins_src_optimizers_annealing_LogarithmicAnnealingSchedule_hh