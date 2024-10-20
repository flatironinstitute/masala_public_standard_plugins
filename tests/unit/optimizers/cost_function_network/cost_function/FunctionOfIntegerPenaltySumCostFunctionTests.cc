/*
    Standard Masala Plugins
    Copyright (C) 2024 Vikram K. Mulligan

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

/// @file tests/unit/optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunctionTests.cc
/// @brief Unit tests for the FunctionOfIntegerPenaltySumCostFunction class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>

// Masala base headers:
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <sstream>
#include <iomanip>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

TEST_CASE( "Instantiate an FunctionOfIntegerPenaltySumCostFunction.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::FunctionOfIntegerPenaltySumCostFunction_API][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        optimizers_api::auto_generated_api::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_APISP mcopt(
            masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_API >()
        );
        mcopt->write_to_tracer( "Instantiated a FunctionOfIntegerPenaltySumCostFunction." );
    }() );
}

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
