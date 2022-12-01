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

/// @file src/registration/StandardMasalaPluginsRegistrator.cc
/// @brief A static singleton that registers this plugin module on instantiation.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Project header:
#include <registration/StandardMasalaPluginsRegistrator.hh>

// Masala headers:
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>

// Standard Masala plugin headers -- selectors:
#include <selectors/atom_selectors/ElementTypeAtomSelectorCreator.hh>

namespace standard_masala_plugins {
namespace registration {

/// @brief Instantiate the static singleton and get a handle to it.
StandardMasalaPluginsRegistratorHandle
StandardMasalaPluginsRegistrator::get_instance() {
    static StandardMasalaPluginsRegistrator config_manager;
    return &config_manager;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////

/// @brief Private constructor: object can only be instantiated with getInstance().
/// @details Construction triggers registration of all instantiable classes in this module
/// that will be managed by the plugin manager.
StandardMasalaPluginsRegistrator::StandardMasalaPluginsRegistrator() {
    register_selectors();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Register all selectors in the standard Masala plugins module with the
/// Masala plugin manager.
void
StandardMasalaPluginsRegistrator::register_selectors() const {
    using namespace masala::base::managers::plugin_module;
    using namespace selectors::atom_selectors;

    MasalaPluginModuleManager::get_instance()->add_plugins(
        std::vector< MasalaPluginCreatorCSP >{
            masala::make_shared< ElementTypeAtomSelectorCreator >()
        }
    );
}

} // namespace registration
} // namespace standard_masala_plugins
