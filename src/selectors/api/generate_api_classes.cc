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
        outvec.emplace_back( std::make_shared< standard_masala_plugins::selectors::atom_selectors::ElementTypeAtomSelector >() );
        // ADD MORE ENTRIES HERE

        return outvec;
    }

} // namespace api
} // namespace selectors
} // namespace standard_masala_plugins {

