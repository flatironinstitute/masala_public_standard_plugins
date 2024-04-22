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

/// @file src/registration/register_sub_libraries.cc
/// @brief Function to register all of the other sub-libraries with the
/// Masala base plugin manager.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit headers:
#include <registration/register_sub_libraries.hh>

// Scoring headers:
#include <scoring_api/auto_generated_api/registration/register_scoring.hh>

// Selectors headers:
#include <selectors_api/auto_generated_api/registration/register_selectors.hh>

// Optimizers headers:
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// File interpreters headers:
#include <file_interpreters_api/auto_generated_api/registration/register_file_interpreters.hh>

namespace standard_masala_plugins {
namespace registration {

/// @brief Register sub-libraries with the Masala base plugin manager.
void
register_sub_libraries() {
    standard_masala_plugins::scoring_api::auto_generated_api::registration::register_scoring();
    standard_masala_plugins::selectors_api::auto_generated_api::registration::register_selectors();
    standard_masala_plugins::optimizers_api::auto_generated_api::registration::register_optimizers();
    standard_masala_plugins::file_interpreters_api::auto_generated_api::registration::register_file_interpreters();
}

/// @brief Unregister sub-libraries with the Masala base plugin manager.
void
unregister_sub_libraries() {
    standard_masala_plugins::scoring_api::auto_generated_api::registration::unregister_scoring();
    standard_masala_plugins::selectors_api::auto_generated_api::registration::unregister_selectors();
    standard_masala_plugins::optimizers_api::auto_generated_api::registration::unregister_optimizers();
    standard_masala_plugins::file_interpreters_api::auto_generated_api::registration::unregister_file_interpreters();
}

} // namespace registration
} // namespace standard_masala_plugins
