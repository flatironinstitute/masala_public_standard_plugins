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

/// @file src/optimizers/annealing/LogarithmicRepeatAnnealingSchedule.fwd.hh
/// @brief Forward declarations for an annealing schedule that changes linearly on a log scale with
/// time, then jumps back up to ramp down again (a sawtooth pattern).
/// @details Annealing schedules return temperature as a function of number of calls.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMasalaPlugins_src_optimizers_annealing_LogarithmicRepeatAnnealingSchedule_fwd_hh
#define StandardMasalaPlugins_src_optimizers_annealing_LogarithmicRepeatAnnealingSchedule_fwd_hh

#include <base/managers/memory/util.hh> // For MASALA_SHARED_POINTER

namespace standard_masala_plugins {
namespace optimizers {
namespace annealing {

	class LogarithmicRepeatAnnealingSchedule;

	/// @brief We will use the convention that an class name followed by SP
	/// represents a MASALA_SHARED_POINTER for objects of that class.
	using LogarithmicRepeatAnnealingScheduleSP = MASALA_SHARED_POINTER< LogarithmicRepeatAnnealingSchedule >;

	/// @brief We will use the convention that an class name followed by CSP
	/// represents a MASALA_SHARED_POINTER for const objects of that class.
	using LogarithmicRepeatAnnealingScheduleCSP = MASALA_SHARED_POINTER< LogarithmicRepeatAnnealingSchedule const >;

	/// @brief We will use the convention that an class name followed by WP
	/// represents a MASALA_WEAK_POINTER for objects of that class.
	using LogarithmicRepeatAnnealingScheduleWP = MASALA_WEAK_POINTER< LogarithmicRepeatAnnealingSchedule >;

	/// @brief We will use the convention that an class name followed by CWP
	/// represents a MASALA_WEAK_POINTER for const objects of that class.
	using LogarithmicRepeatAnnealingScheduleCWP = MASALA_WEAK_POINTER< LogarithmicRepeatAnnealingSchedule const >;

} // namespace annealing
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //StandardMasalaPlugins_src_optimizers_annealing_LogarithmicRepeatAnnealingSchedule_fwd_hh
