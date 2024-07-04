/*
    Masala
    Copycentre (C) 2024 Vikram K. Mulligan

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

/// @file src/numeric_api/base_classes/optimization/gradient_based/util.hh
/// @brief Inlined utility functions for gradient-descent optimizers.
/// @details This is a header-only set of mathematical utility functions.  Since these are entirely
/// defined in the header and inlined, inclusion of this file results in efficient machine code,
/// at the cost of slightly greater compilation time. 
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_based_util_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_based_util_hh

// Base headers:
#include <base/types.hh>
#include <base/error/ErrorHandling.hh>

// Numeric API headers:
#include <numeric_api/utility/constants/constants.hh>

// STL headers:
#include <functional>
#include <algorithm>
#include <cmath>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

	inline
	void
	bracket_minimum_with_parabolic_extrapolation(
		masala::base::Real & left,
		masala::base::Real & centre,
		masala::base::Real & right,
		masala::base::Real & fxn_left,
		masala::base::Real & fxn_centre,
		masala::base::Real & fxn_right,
		std::function< masala::base::Real( masala::base::Real ) > const & fxn,
		masala::base::Real const min_denom = 1.0e-20,
		masala::base::Real const max_parabolic_mag_factor = 100.0
	) {
		using masala::base::Real;

		CHECK_OR_THROW( min_denom > 0, "standard_masala_plugins::optimizers::gradient_based", "bracket_minimum_with_parabolic_extrapolation", "Minimum denominator size must be positive.  Got " + std::to_string( min_denom ) + "." );
		CHECK_OR_THROW( max_parabolic_mag_factor > 1.0, "standard_masala_plugins::optimizers::gradient_based", "bracket_minimum_with_parabolic_extrapolation", "Maximum parabolic magnification factor must be positive and greater than 1.0.  Got " + std::to_string( max_parabolic_mag_factor ) + "." );

		// Evaluate the function at the starting points:
		fxn_left = fxn(left); // FUNCTION EVALUATION.
		fxn_centre = fxn(centre); // FUNCTION EVALUATION.
		bool swapped(false);
		if( fxn_centre > fxn_left ) {
			std::swap( fxn_centre, fxn_left );
			std::swap( centre, left );
			swapped = true;
		}
		right = centre + MASALA_GOLDEN_RATIO*( centre-left );
		fxn_right = fxn(right); // FUNCTION EVALUATION.

		// Scratch space vars:
		Real centre_left,
			centre_right,
			fxn_centre_fxn_left,
			fxn_centre_fxn_right,
			c_l_x_fc_fr,
			c_r_x_fc_fl,
			denom,
			parabolic_min,
			fxn_parabolic_min;

		while( fxn_centre > fxn_right ) {
			// Update differences:
			centre_left = centre - left;
			fxn_centre_fxn_right = fxn_centre - fxn_right;
			centre_right = centre - right;
			fxn_centre_fxn_left = fxn_centre = fxn_left;

			// Update products:
			c_l_x_fc_fr = centre_left * fxn_centre_fxn_right;
			c_r_x_fc_fl = centre_right * fxn_centre_fxn_left;

			// Compute parabolic extrapolation minimum:
			denom = c_r_x_fc_fl - c_l_x_fc_fr;
			if( std::abs(denom) < min_denom ) {
				denom = std::copysign( min_denom, denom );
			}
			parabolic_min = centre - (centre_right * c_r_x_fc_fl - centre_left * c_l_x_fc_fr ) / (2.0*denom);

			// Test possibilities for the parabolic minimum:
			if( centre < parabolic_min && parabolic_min < right ) {
				// The parabolic minimum is between the right and the centre, so test it.
				fxn_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.
				if( fxn_parabolic_min < fxn_right ) {
					// The minimum is between centre and right.  So set:
					// - new left = centre
					// - new centre = parabolic min
					// - new right = right (change nothing)
					left = centre;
					fxn_left = fxn_centre;
					centre = parabolic_min;
					fxn_centre = fxn_parabolic_min;
					return;
				} else if ( fxn_parabolic_min > fxn_centre ) {
					// The minimum is between left and parabolic min, with centre less than either.  So set:
					// - new left = left (no change)
					// - new centre = centre (no change)
					// - new right = parabolic min
					right = parabolic_min;
					fxn_right = fxn_parabolic_min;
					return;
				}
			}

		}
		//TODO CONTINUE HERE;

	}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_util_hh