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

/// @file tests/unit/selectors/ElementTypeAtomSelectorTests.cc
/// @brief Unit tests for the ElementTypeAtomSelector class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <selectors_api/auto_generated_api/atom_selectors/ElementTypeAtomSelector_API.hh>

// Core headers:
#include <core_api/auto_generated_api/pose/Pose_API.hh>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace selectors {
namespace atom_selectors {

TEST_CASE( "Instantiate an ElementTypeAtomSelector", "[standard_masala_plugins::auto_generated_api::atom_selectors::ElementTypeAtomSelector_API][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        selectors_api::auto_generated_api::atom_selectors::ElementTypeAtomSelector_APISP elemsel(
            std::make_shared< selectors_api::auto_generated_api::atom_selectors::ElementTypeAtomSelector_API >()
        );
        elemsel->write_to_tracer( "Instantiated an ElementTypeAtomSelector." );
    }() );
}

} // namespace atom_selectors
} // namespace selectors
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
