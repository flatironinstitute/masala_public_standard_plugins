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

/// @file tests/unit/registration/version_manager/RegistrationVersionCheckTests.cc
/// @brief Unit tests for registering the standard Masala plugins library, and for ensuring that
/// its version requirements are satisfied.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <base/managers/version/MasalaVersionManager.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/managers/plugin_module/MasalaPluginLibraryManager.hh>
#include <src/registration_api/register_library.hh>
#include <src/optimizers_api/auto_generated_api/registration/register_optimizers.hh>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace registration {
namespace version_manager {

TEST_CASE( "Register and check version compatibility", "[standard_masala_plugins::registration_api::register_library][registration][MasalaPluginLibraryManager][MasalaVersionManager]" ) {
    using namespace masala::base::managers::version;
    masala::base::Size n_before, n_registered, n_after;
    REQUIRE_NOTHROW([&](){
        MasalaVersionManagerHandle vm( MasalaVersionManager::get_instance() );
        n_before = vm->n_modules_registered();
        standard_masala_plugins::registration_api::register_library();
        n_registered = vm->n_modules_registered();
        standard_masala_plugins::registration_api::unregister_library();
        n_after = vm->n_modules_registered();
    }() );

    CHECK( n_before == 1 );
    CHECK( n_registered == 2 );
    CHECK( n_after == 1 );
}

TEST_CASE( "Register and check that plugins were registered", "[standard_masala_plugins::registration_api::register_library][registration][MasalaPluginLibraryManager][MasalaPluginModuleManager]" ) {
    using namespace masala::base::managers::plugin_module;
    masala::base::Size n_plugins_before, n_plugins_registered, n_plugins_after;
    REQUIRE_NOTHROW([&](){
        MasalaPluginModuleManagerHandle pm( MasalaPluginModuleManager::get_instance() );
        n_plugins_before = pm->get_all_plugin_list().size();
        standard_masala_plugins::registration_api::register_library();
        n_plugins_registered = pm->get_all_plugin_list().size();
        standard_masala_plugins::registration_api::unregister_library();
        n_plugins_after = pm->get_all_plugin_list().size();
    }() );

    CHECK( n_plugins_registered > n_plugins_before );
    CHECK( n_plugins_registered - n_plugins_before >= 4 ); // This library defines at least 4 plugins.
    CHECK( n_plugins_after == n_plugins_before );
}



} // namespace atom_selectors
} // namespace selectors
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
