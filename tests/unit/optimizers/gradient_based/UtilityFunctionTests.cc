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

/// @file tests/unit/optimizers/gradient_based/UtilityFunctionTests.cc
/// @brief Unit tests for the utility functions used for line searches, gradient-descent
/// minimization, and other local optimization.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers/gradient_based/util.hh>

// Masala base headers:
#include <base/types.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>

// STL headers
#include <cmath>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace gradient_based {

masala::base::Real
test_gaussian(
	masala::base::Real const x,
	masala::base::Real const coeff,
	masala::base::Real const offset,
	masala::base::Real const stddev
) {
	return coeff * std::exp( -1.0 * std::pow( ( x - offset ) / stddev , 2.0 ) );
}

/// @brief See util_test_fxn_1.png to see this plotted.
masala::base::Real
test_function_1(
	masala::base::Real const x
) {
	// For plotting using Desmos:
	// -1.0\cdot\exp\left(-\left(\frac{\left(x-3\right)}{0.5}\right)^{2}\right)-2.0\cdot\exp\left(-\left(\frac{\left(x-2\right)}{0.25}\right)^{2}\right)+0.5\cdot\exp\left(-\left(\frac{\left(x-3.25\right)}{0.1}\right)^{2}\right)
	return test_gaussian( x, -1.0, 3.0, 0.5 ) + test_gaussian( x, -2.0, 2.0, 0.25 ) + test_gaussian( x, 0.5, 3.25, 0.1 );
}

TEST_CASE( "Find the bounds of a local minimization problem using parabolic extrapolation.", "[standard_masala_plugins::optimizers::gradient_based::bracket_minimum_with_parabolic_extrapolation][local_minimization][bounds]" ) {
    //REQUIRE_NOTHROW([&](){
        
    //}() );
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
