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

/// @file src/optimizers/annealing/ConstantAnnealingSchedule.hh
/// @brief Headers for an annealing schedule that does not vary with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMasalaPlugins_src_optimizers_annealing_ConstantAnnealingSchedule_hh
#define StandardMasalaPlugins_src_optimizers_annealing_ConstantAnnealingSchedule_hh

// Forward declarations:
#include <optimizers/annealing/ConstantAnnealingSchedule.fwd.hh>

// Parent class:
#include <numeric_api/base_classes/optimization/annealing/AnnealingSchedule.hh>

// STL headers
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace annealing {

/// @brief An annealing schedule that does not vary with time.
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class ConstantAnnealingSchedule : public masala::numeric_api::base_classes::optimization::annealing::AnnealingSchedule {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	ConstantAnnealingSchedule() = default;

	/// @brief Copy constructor.
	ConstantAnnealingSchedule( ConstantAnnealingSchedule const & );

	/// @brief Assignment operator.
	ConstantAnnealingSchedule & operator=( ConstantAnnealingSchedule const & );

	/// @brief Virtual destructor.
	virtual ~ConstantAnnealingSchedule() = default;

	/// @brief Make a copy of this object.
	masala::numeric_api::base_classes::optimization::annealing::AnnealingScheduleSP
	clone() const override;

	/// @brief Make this object wholly independent.
	/// @details Should be overridden for derived classes.
	void make_independent() override;

	/// @brief Make an independent copy of this object.
	ConstantAnnealingScheduleSP deep_clone() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the hierarchical categories for this plugin class.
	/// @details The base class implementation returns { { "AnnealingSchedule" } }
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the ahierarchical keywords for this plugin class.
	/// @details The base class implementation returns { "annealing_schedule", "constant", "time_independent" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the class name ("ConstantAnnealingSchedule").
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
	masala::numeric_api::Real
	temperature() const override;

	/// @brief Return temperature for the Nth timepoint.
	masala::numeric_api::Real
	temperature(
		masala::numeric_api::Size const time_index
	) const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Reset this object.
	void reset();

	/// @brief Set the temperature.
	/// @details In kcal/mol.  Must be positive.
	void set_temperature( masala::numeric_api::Real const temperature_in );

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the call count.
	masala::numeric_api::Size get_call_count() const;

	/// @brief Get the temperature.
	masala::numeric_api::Real get_temperature() const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief Mutex for locking the derived class data.
	mutable std::mutex constant_annealing_schedule_mutex_;

	/// @brief The temperature.
	/// @details In units of kcal/mol.  Defaults to 0.62.
	masala::numeric_api::Real temperature_ = 0.62;

	/// @brief The API definition (not copied).
	masala::base::api::MasalaObjectAPIDefinitionCSP api_definition_;

}; // class ConstantAnnealingSchedule

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //StandardMasalaPlugins_src_optimizers_annealing_ConstantAnnealingSchedule_hh