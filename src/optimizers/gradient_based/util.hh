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

// External headers:
#include <external/eigen/Eigen/Core>

// STL headers:
#include <functional>
#include <algorithm>
#include <cmath>
//#include <iostream> // COMMENT ME OUT -- FOR DEBUGGING ONLY.

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

	/// @brief Given a function f(x) (fxn), a starting point (x0), the gradient
	/// at this point (grad_of_fxn_at_x0), a search direction (search_dir), and Armijo
	/// parameters c (function decrease factor) and tau (stepsize decrease factor), find
	/// the largest step that we can that satisfies the Armijo condition.  This does
	/// NOT find the minimum in the search direction, but only a point with "sufficient"
	/// decrease in the function value.
	/// @details The Armijo condition is f(x0+alpha*dir) <= f(x0) + alpha*c*m, where
	/// alpha is a step size multiplier, dir is a search direction, c is a constant
	/// (1/2 is viable), and m is the slope of f in the direction dir at x0.
	/// @returns The value of alpha, the largest value of the multiplier found that
	/// satisfies the Armijo condition.  Also, updates x to be x0 + alpha * dir.
	inline
	masala::base::Real
	armijo_inexact_linesearch(
		std::function< masala::base::Real( Eigen::VectorXd const & ) > const & fxn,
		Eigen::VectorXd const & x0,
		masala::base::Real const& fxn_at_x0,
		Eigen::VectorXd const & grad_of_fxn_at_x0,
		Eigen::VectorXd const & search_dir,
		Eigen::VectorXd & x,
		masala::base::Real & fxn_at_x,
		masala::base::Real const c = 0.5,
		masala::base::Real const tau = 0.5
	) {
		using masala::base::Size;
		using masala::base::Real;
		Size const ndims( x0.size() );
		CHECK_OR_THROW( grad_of_fxn_at_x0.size() == ndims, "standard_masala_plugins::optimizers::gradient_based",
			"armijo_inexact_linesearch", "Expected dimensions of point and gradient to match, but got x0.size(): "
			+ std::to_string( x0.size() ) + ", gradient.size(): " + std::to_string( grad_of_fxn_at_x0.size( )) + "."
		);
		CHECK_OR_THROW( search_dir.size() == ndims, "standard_masala_plugins::optimizers::gradient_based",
			"armijo_inexact_linesearch", "Expected dimensions of point and search direction to match, but got x0.size(): "
			+ std::to_string( x0.size() ) + ", search_dir.size(): " + std::to_string( search_dir.size( )) + "."
		);
		CHECK_OR_THROW( 0.0 < tau && tau < 1.0, "standard_masala_plugins::optimizers::gradient_based",
			"armijo_inexact_linesearch", "Tau was set to " + std::to_string(tau) + ", but it must be "
			"between 0.0 and 1.0, exclusive."
		);

		x.resize( ndims );

		Real const m( grad_of_fxn_at_x0.dot( search_dir ) );
		Real const t = -c*m;
		Real alpha(1.0);

		x = x0 + alpha * search_dir;
		fxn_at_x = fxn( x );

		while( fxn_at_x0 - fxn_at_x < alpha * t ) {
			alpha *= tau;
			x = x0 + alpha * search_dir;
			fxn_at_x = fxn( x );
		}
	}


	/// @brief Given a function f(x) of one variable x, and two starting points (left and centre),
	/// bracket the (hopefully nearest) local minimum.
	/// @details After this function completes, the values of left, centre, and right are updated
	/// so that left and right bracket the local minimum, and centre is somewhere in between.  This
	/// function also evaluates the function f(x) at left, centre, and right, and stores the
	/// function's value in fxn_left, fxn_centre, and fxn_right, respectively.
	/// @param[inout] left On input, an initial guess for the lower extreme for the possible value
	/// of x that minimizes f(x).  On output, a best estimate of the lower extreme of the possible
	/// value of x that minimizes f(x).
	/// @param[inout] centre On input, a point a small distance from left that establishes initial
	/// step size for the braketing search.  On output, a point somewhere between left and right
	/// where f(centre) < f(left) and f(centre) < f(right).
	/// @param[out] right On output, a best estimate of the upper extreme of the possible value of
	/// x that minimizes f(x).
	/// @param[out] fxn_left The function f(x) evaluated at left.  (That is, f(left)).
	/// @param[out] fxn_centre The function f(x) evaluated at centre.  (That is, f(centre)).
	/// @param[out] fxn_right The function f(x) evaluated at right.  (That is, f(right)).
	/// @param[in] fxn The function f(x).  This should be a std::fxn with one real input and one
	/// real output.
	/// @param[in] min_denom The smallest value of the denominator when carrying out parabolic
	/// extrapolation.  Used to avoid divide-by-zero errors; in most cases, the default value of
	/// 1.0e-20 is reasonable.
	/// @param[in] max_parabolic_mag_factor The largest amount by which the parabolic extrapolation
	/// may increase the step size.  Must be greater than 1.0.  Usually, the default of 2.0 is
	/// reasonable.
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
		masala::base::Real const max_parabolic_mag_factor = 2.0
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
			parabolic_min_limit,
			fxn_parabolic_min;

		//masala::base::Size counter(0); // COMMENT ME OUT -- FOR DEBUGGING ONLY.

		while( fxn_centre > fxn_right ) {
			// ++counter; // COMMENT ME OUT -- FOR DEBUGGING ONLY.
			// std::cout << "Round " << counter
			// 	<< ":\tleft=" << left
			// 	<< "\tcen=" << centre 
			// 	<< "\tright=" << right
			// 	<< "\tparab_min=" << parabolic_min
			// 	<< "\tf(left)=" << fxn_left
			// 	<< "\tf(cen)=" << fxn_centre 
			// 	<< "\tf(right)=" << fxn_right
			// 	<< "\tf(parab_min)=" << fxn_parabolic_min
			// 	<< std::endl; // COMMENT ME OUT -- FOR DEBUGGING ONLY.

			// Update differences:
			centre_left = centre - left;
			fxn_centre_fxn_right = fxn_centre - fxn_right;
			centre_right = centre - right;
			fxn_centre_fxn_left = fxn_centre - fxn_left;

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
			if(
				//(centre-parabolic_min)*(parabolic_min-right) > 0.0
				( (!swapped) && (centre < parabolic_min && parabolic_min < right) ) ||
				( swapped && (centre > parabolic_min && parabolic_min > right) )
			) {
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

					if( left > right ) {
						std::swap( left, right );
						std::swap( fxn_left, fxn_right );
					}
					return;
				} else if ( fxn_parabolic_min > fxn_centre ) {
					// The minimum is between left and parabolic min, with centre less than either.  So set:
					// - new left = left (no change)
					// - new centre = centre (no change)
					// - new right = parabolic min
					right = parabolic_min;
					fxn_right = fxn_parabolic_min;

					if( left > right ) {
						std::swap( left, right );
						std::swap( fxn_left, fxn_right );
					}
					return;
				} else {
					// The parabolic fit did not work.  Use a golden ratio search instead.
					parabolic_min = right - MASALA_GOLDEN_RATIO * centre_right;
					fxn_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.
				}
			} else {
				parabolic_min_limit = centre - max_parabolic_mag_factor * centre_right;
				if(
					//(right-parabolic_min)*(parabolic_min-parabolic_min_limit) > 0.0
					( (!swapped) && (parabolic_min > right && parabolic_min < parabolic_min_limit ) ) ||
					( swapped && (parabolic_min < right && parabolic_min > parabolic_min_limit ) )
				) {
					// If we're past the rightmost point, but within the magnification limit:
					fxn_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.
					if( fxn_parabolic_min < fxn_right ) {
						// If the function is decreasing, keep shifting to the right by golden
						// section search.
						centre = right;
						fxn_centre = fxn_right;
						right = parabolic_min;
						fxn_centre_fxn_right = fxn_parabolic_min;
						parabolic_min += MASALA_GOLDEN_RATIO * ( parabolic_min - right );
						fxn_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.
					}
				} else if(
					( (!swapped) && (parabolic_min > parabolic_min_limit ) ) ||
					( swapped && (parabolic_min < parabolic_min_limit ) )
				) {
					// We're beyond the allowed maximium step size.
					parabolic_min = parabolic_min_limit;
					fxn_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.
				} else {
					// Fall back on a golden search.
					parabolic_min = right - MASALA_GOLDEN_RATIO * centre_right;
					fxn_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.
				}
			}
			// If we reach here, shift points appropriately.
			left = centre;
			fxn_left = fxn_centre;
			centre = right;
			fxn_centre = fxn_right;
			right = parabolic_min;
			fxn_right = fxn_parabolic_min;
		}

		if( left > right ) {
			std::swap( left, right );
			std::swap( fxn_left, fxn_right );
		}
	}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_util_hh