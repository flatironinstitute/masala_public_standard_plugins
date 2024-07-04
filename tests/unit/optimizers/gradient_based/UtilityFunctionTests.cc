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
#include <functional>

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
/// @details Has minima at (2.002, -2.018), (2.995, -0.999), and (3.397,-0.475).
/// Has maxima at (2.440, -0.376) and (3.266,-0.266).  Can be multiplied by -1 by
/// setting invert true.
masala::base::Real
test_function_1(
	masala::base::Real const x,
	bool invert
) {
	// For plotting using Desmos:
	// -1.0\cdot\exp\left(-\left(\frac{\left(x-3\right)}{0.5}\right)^{2}\right)-2.0\cdot\exp\left(-\left(\frac{\left(x-2\right)}{0.25}\right)^{2}\right)+0.5\cdot\exp\left(-\left(\frac{\left(x-3.25\right)}{0.1}\right)^{2}\right)
	return ( invert ? -1.0 : 1.0 ) * ( test_gaussian( x, -1.0, 3.0, 0.5 ) + test_gaussian( x, -2.0, 2.0, 0.25 ) + test_gaussian( x, 0.5, 3.25, 0.1 ) );
}

TEST_CASE( "Find the bounds of a local minimization problem using parabolic extrapolation.", "[standard_masala_plugins::optimizers::gradient_based::bracket_minimum_with_parabolic_extrapolation][local_minimization][bounds]" ) {
	using masala::base::Real;
	using namespace standard_masala_plugins::optimizers::gradient_based;
	using masala::base::managers::tracer::MasalaTracerManager;
	using masala::base::managers::tracer::MasalaTracerManagerHandle;

	MasalaTracerManagerHandle tm( MasalaTracerManager::get_instance() );

	std::function< Real( Real ) > const fxn1( std::bind( &test_function_1, std::placeholders::_1, false ) );
	std::vector< std::pair< Real, Real > > initial_lefts_and_centres {
		std::make_pair( 1.0, 1.1 ),
		std::make_pair( 2.4, 2.41 ),
		std::make_pair( 1.0, 0.9 ),
		std::make_pair( 2.45, 2.46 ),
		std::make_pair( 5.0, 5.1 )
	};

	masala::base::Size counter(0);
	for( auto const entry : initial_lefts_and_centres ) {
		++counter;
		Real left(1.0), centre( 1.1 ), right(0), fxn_left(0), fxn_centre(0), fxn_right(0);
		REQUIRE_NOTHROW([&](){
			bracket_minimum_with_parabolic_extrapolation( left, centre, right, fxn_left, fxn_centre, fxn_right, fxn1 );
		}() );

		tm->write_to_tracer( "standard_masala_plugins::tests::unit::optimizers::gradient_based::UtilityFunctionTests", "Attempt " + std::to_string(counter) + ":\tinitial_left = " + std::to_string(entry.first) + "\tinitial_centre = " + std::to_string(entry.second)  );
		tm->write_to_tracer( "standard_masala_plugins::tests::unit::optimizers::gradient_based::UtilityFunctionTests", "left = " + std::to_string(left) + "\tfxn_left = " + std::to_string( fxn_left ) );
		tm->write_to_tracer( "standard_masala_plugins::tests::unit::optimizers::gradient_based::UtilityFunctionTests", "centre = " + std::to_string(centre) + "\tfxn_centre = " + std::to_string( fxn_centre ) );
		tm->write_to_tracer( "standard_masala_plugins::tests::unit::optimizers::gradient_based::UtilityFunctionTests", "right = " + std::to_string(right) + "\tfxn_right = " + std::to_string( fxn_right ) );

		if( counter <= 3 ) {
			CHECK( left <= 2.002 );
			CHECK( right >= 2.002 );
			CHECK( right < 2.440 );
		} else if( counter == 4 ) {
			CHECK( left > 2.440 );
			CHECK( left <= 2.995 );
			CHECK( right >= 2.995 );
			CHECK( right < 3.266 );
		} else {
			CHECK( left > 3.266 );
			CHECK( left <= 3.397 );
			CHECK( right >= 3.397 );
		}
		CHECK( right > centre );
		CHECK( right > left );
		CHECK( left < centre );
		CHECK( fxn_centre < fxn_left );
		CHECK( fxn_centre < fxn_right );
	}
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
