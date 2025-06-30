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

/// @file src/registration_api/register_library.cc
/// @brief Function to register a library with the Masala base plugin manager.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#include <registration_api/register_library.hh>
#include <registration/register_sub_libraries.hh>

#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/version/MasalaModuleVersionInfo.hh>
#include <base/managers/version/MasalaVersionManager.hh>

namespace standard_masala_plugins {
namespace registration_api {

/// @brief Register this library with the Masala base plugin manager.
extern "C"
void
register_library() {
    using namespace masala::base::managers::version;
    using masala::base::Size;
    masala::base::managers::tracer::MasalaTracerManager::get_instance()->write_to_tracer(
        "standard_masala_plugins::registration_api::register_library", "Registering standard Masala plugins."
    );

    MasalaModuleVersionInfoSP module_version_info(
        masala::make_shared< MasalaModuleVersionInfo >(
            "Standard Masala Plugins",
            std::pair< Size, Size >( STANDARD_MASALA_PLUGINS_MAJOR_VERSION, STANDARD_MASALA_PLUGINS_MINOR_VERSION )
        )
    );
    module_version_info->add_requirement_with_minimum_version(
        "Masala",
        true,
        std::pair< Size, Size >( 0, 11 ), // Min version
        "",
        "Version 0.11 changed base class names for optimizers, for consistency.  "
        "Version 0.10 added support for deprecation annotations, and updated the build system for centralized versioning.  "
        "Version 0.9 added support for no-UI function annotations.  "
        "Version 0.8 cleaned up a few violations of Masala conventions related to scratch spaces.  "
		"Prior to version 0.7, cost function problems and cost functions could not accept scratch space pointers.  "
		"Prior to version 0.6, annotations for API functions indicating preferred data representations were not available.  "
		"Prior to version 0.5, annotations for API functions were not available, and certain problems existed "
		"with including Masala objects in API definitions.  "
		"Prior to version 0.4, certain functions in the CostFunctionNetworkOptimizationProblem class (like "
		"make_independent()) were unnecessarily made virtual and overridable.  "
		"Prior to version 0.3, certain accessor functions for setter descriptions in API definitions "
		"returned the wrong type.  "
        "Prior to version 0.2, the Standard Masala Library's SquareOfChoicePenaltySumCostFunction "
        "and FunctionOfIntegerPenaltySumCostFunction were in the core Masala library."
    );
    MasalaVersionManager::get_instance()->add_library_information( module_version_info );
    standard_masala_plugins::registration::register_sub_libraries();
}

/// @brief Unregister this library with the Masala base plugin manager.
extern "C"
void
unregister_library() {
    using namespace masala::base::managers::version;
    masala::base::managers::tracer::MasalaTracerManager::get_instance()->write_to_tracer(
        "standard_masala_plugins::registration_api::register_library", "Unregistering standard Masala plugins."
    );
    MasalaVersionManager::get_instance()->remove_library_information( "Standard Masala Plugins" );
    standard_masala_plugins::registration::unregister_sub_libraries();
}

} // namespace registration_api
} // namespace standard_masala_plugins
