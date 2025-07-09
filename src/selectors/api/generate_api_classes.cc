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

/// @file src/selectors/api/generate_api_classes.cc
/// @brief Utility functions to generate all of the classes in the selectors library that define
/// an API.  Used for auto-generating the selectors API.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Project header
#include <selectors/api/generate_api_classes.hh>

// Core headers
#include <selectors/atom_selectors/ElementTypeAtomSelector.hh>

namespace standard_masala_plugins {
namespace selectors {
namespace api {

    /// @brief Generate an instance of each of the classes in core that have a defined API, and
    /// return a vector of owning pointers to these instances.
    /// @details Used for auto-generating the core API description (as a JSON file).
    std::vector< masala::base::MasalaObjectSP >
    generate_api_classes() {
        std::vector< masala::base::MasalaObjectSP > outvec;

        // Add to this vector whenever a class is added with a defined API:
        outvec.emplace_back( masala::make_shared< standard_masala_plugins::selectors::atom_selectors::ElementTypeAtomSelector >() );
        // ADD MORE ENTRIES HERE

        return outvec;
    }

} // namespace api
} // namespace selectors
} // namespace standard_masala_plugins {

