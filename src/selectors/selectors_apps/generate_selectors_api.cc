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

/// @file src/selectors/api/generate_api_classes.hh
/// @brief Utility functions to generate all of the classes in the selectors library that define
/// an API.  Used for auto-generating the selectors API.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Core headers
#include <core_api/types.hh>

// Registration header
#include <src/registration/StandardMasalaPluginsRegistrator.hh>

// Base headers
#include <base/MasalaObject.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/managers/disk/MasalaDiskManager.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/managers/plugin_module/MasalaPlugin.hh>

// External headers
#include <external/nlohmann_json/single_include/nlohmann/json.hpp>

// STL headers
#include <sstream>

// Program entry point:
int
main(
    // int argc,
    // char * argv[]
    int, char**
) {
    using namespace masala::base::managers::plugin_module;
    nlohmann::json api_definition;
    api_definition["FileType"] = "API_definition";
    api_definition["Module"] = "Selectors";
    nlohmann::json api_entries;
    MasalaPluginModuleManagerHandle pm( MasalaPluginModuleManager::get_instance() );
    std::vector< std::string > const api_object_names(
        pm->get_list_of_plugins_by_keywords( std::vector< std::string >{ "selector", "standard_masala_plugins" } )
    );
    for( masala::core_api::Size i(0), imax(api_object_names.size()); i<imax; ++i ) {
        MasalaPluginSP api_object( pm->create_plugin_object_instance( std::vector< std::string >{ "Selector" }, api_object_names[i] ) );
        masala::base::api::MasalaObjectAPIDefinitionCSP api_def( api_object->get_api_definition() );
        api_entries[ api_object->class_namespace() + "::" + api_object->class_name() ] = *api_def->get_json_description();
    }
    api_definition["Elements"] = api_entries;

    std::ostringstream ss;
    ss << api_definition.dump( 1, '\t' ) << "\n";
    masala::base::managers::disk::MasalaDiskManager::get_instance()->write_ascii_file( "selectors_api.json", ss.str() );

    return 0;
}