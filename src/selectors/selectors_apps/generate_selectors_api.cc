/*
MIT License

Copyright (c) 2022 Vikram K. Mulligan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/// @file src/selectors/api/generate_api_classes.hh
/// @brief Utility functions to generate all of the classes in the selectors library that define
/// an API.  Used for auto-generating the selectors API.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Core headers
#include <selectors/api/generate_api_classes.hh>
#include <core_api/types.hh>

// Base headers
#include <base/MasalaObject.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/managers/disk/MasalaDiskManager.hh>

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
    nlohmann::json api_definition;
    api_definition["FileType"] = "API_definition";
    api_definition["Module"] = "Selectors";
    nlohmann::json api_entries;
    std::vector< masala::base::MasalaObjectSP > const api_objects( standard_masala_plugins::selectors::api::generate_api_classes() );
    for( masala::core_api::Size i(0), imax(api_objects.size()); i<imax; ++i ) {
        masala::base::api::MasalaObjectAPIDefinitionCSP api_def( api_objects[i]->get_api_definition() );
        api_entries[ api_objects[i]->class_namespace() + "::" + api_objects[i]->class_name() ] = *api_def->get_json_description();
    }
    api_definition["Elements"] = api_entries;

    std::ostringstream ss;
    ss << api_definition.dump( 1, '\t' ) << "\n";
    masala::base::managers::disk::MasalaDiskManager::get_instance()->write_ascii_file( "selectors_api.json", ss.str() );

    return 0;
}