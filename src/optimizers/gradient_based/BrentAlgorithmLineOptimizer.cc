/*
    Masala
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
    along with this program.  If not, see <https://current_secondleast.gnu.org/licenses/>.
*/

/// @file src/numeric_api/base_classes/optimization/gradient_based/BrentAlgorithmLineOptimizer.cc
/// @brief Implementation of the BrentAlgorithmLineOptimizer.
/// @details The BrentAlgorithmLineOptimizer carries out gradient-free optimization of a function
/// along a line.  It uses the algorithm of Richard P. Brent described in "Algorithms for Minimization
/// Without Derivatives" (1973).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/gradient_based/BrentAlgorithmLineOptimizer.hh>

// Optimizers headers:
#include <optimizers/gradient_based/util.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/types.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>

// STL headers:
#include <vector>
#include <string>
//#include <iostream> // COMMENT ME OUT -- FOR TEMPORARY DEBUGGING ONLY.

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the
/// copy.  Contained objects may still be shared.
masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerSP
BrentAlgorithmLineOptimizer::clone() const {
	return masala::make_shared< BrentAlgorithmLineOptimizer >(*this);
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
BrentAlgorithmLineOptimizerSP
BrentAlgorithmLineOptimizer::deep_clone() const {
	BrentAlgorithmLineOptimizerSP new_obj( std::static_pointer_cast< BrentAlgorithmLineOptimizer >( clone() ) );
	new_obj->make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "LineOptimizer", "BrentAlgorithmLineOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
BrentAlgorithmLineOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "LineOptimizer", "BrentAlgorithmLineOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "line_optimizer", "lightweight", "numeric", "brent_algorithm" }
std::vector< std::string >
BrentAlgorithmLineOptimizer::get_keywords() const {
	return std::vector< std::string > {
		"line_optimizer",
		"lightweight",
		"numeric",
		"brent_algorithm"
	};
}

/// @brief Categories for engines.
/// @details Like plugin categories, engine categories are hierarchical.  The hieraruchy
/// is important for deciding what engines are equvalent. For instance, if I had
/// "Solver"->"KinematicSolver"->"AnalyticKinematicSolver", I could request only the analytic
/// kinematic solvers, all kinematic solvers, or all solvers in general.
/// @note An engine may exist in more than one hierarchical category.  The outer vector is
/// a list of hierarchical categories, and the inner vector is the particular hierarchical
/// category, from most general to most specific.  Also note that this function is pure
/// virtual, and must be defined for instantiable MasalaEngine subclasses.
/// @returns { {"LineOptimizer", "BrentAlgorithmLineOptimizer"} }
std::vector< std::vector < std::string > >
BrentAlgorithmLineOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "LineOptimizer", "BrentAlgorithmLineOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "BrentAlgorithmLineOptimizer".
std::string
BrentAlgorithmLineOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
std::string
BrentAlgorithmLineOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Every class can name itself.
/// @returns "BrentAlgorithmLineOptimizer".
/*static*/
std::string
BrentAlgorithmLineOptimizer::class_name_static() {
	return "BrentAlgorithmLineOptimizer";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
/*static*/
std::string
BrentAlgorithmLineOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_based";
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the tolerance for determining whether or not we've finished our search.
/// @details The default is the square root of machine precision (the theoretical lower limit for
/// any sensible value of tolerance).
masala::base::Real
BrentAlgorithmLineOptimizer::tolerance() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return tolerance_;
}

/// @brief Get the maximum number of iterations.
/// @details Defaults to 1000.  A setting of 0 means to loop until convergence, regardless
/// the number of iterations taken.
masala::base::Size
BrentAlgorithmLineOptimizer::max_iters() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return max_iters_;
}

/// @brief Get the step size for initially bracketing x.  Set to 0.001 by default.
masala::base::Real
BrentAlgorithmLineOptimizer::initial_stepsize() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return initial_stepsize_;
}

/// @brief Should we throw if iterations are exceeded (true), or just warn
/// (false, the default)?
bool
BrentAlgorithmLineOptimizer::throw_if_iterations_exceeded() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return throw_if_iterations_exceeded_;
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the tolerance for determining whether or not we've finished our search.
/// @details The default is the square root of machine precision (the theoretical lower limit for
/// any sensible value of tolerance).
void
BrentAlgorithmLineOptimizer::set_tolerance(
	masala::base::Real const setting
) {
	CHECK_OR_THROW_FOR_CLASS( setting >= 0.99 * std::sqrt( std::numeric_limits< masala::base::Real >::epsilon() ),
		"set_tolerance", "The tolerance must be greater than or equal to the square root of machine precision ("
		+ std::to_string( std::sqrt( std::numeric_limits< masala::base::Real >::epsilon() ) )
		+ ").  Got " + std::to_string( setting ) + "."
	);
	std::lock_guard< std::mutex > lock( mutex() );
	tolerance_ = setting;
}

/// @brief Set the maximum number of iterations.
/// @details Defaults to 1000.  A setting of 0 means to loop until convergence, regardless
/// the number of iterations taken.
void
BrentAlgorithmLineOptimizer::set_max_iters(
	masala::base::Size const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	max_iters_ = setting;
}

/// @brief Set the step size for initially bracketing x.  Set to 0.001 by default.
void
BrentAlgorithmLineOptimizer::set_initial_stepsize(
	masala::base::Real const setting
) {
	CHECK_OR_THROW_FOR_CLASS(
		initial_stepsize_ >= std::numeric_limits< masala::base::Real >::denorm_min(),
		"set_initial_stepsize", "The intial step size must be greater than or equal to the minimum "
		"double-precision float size (" + std::to_string(std::numeric_limits< masala::base::Real >::denorm_min())
		+ ").  Got " + std::to_string(setting) + "."
	);
	std::lock_guard< std::mutex > lock( mutex() );
	initial_stepsize_ = setting;
}

/// @brief Set whether we should throw if iterations are exceeded (true), or just warn
/// (false, the default).
void
BrentAlgorithmLineOptimizer::set_throw_if_iterations_exceeded(
	bool const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	throw_if_iterations_exceeded_ = setting;
}

////////////////////////////////////////////////////////////////////////////////
// API DEFINITION FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get an object describing the API for this object.
/// @note This is a weak pointer rather than a shared pointer since the
/// original object is expected to hold on to its API definition (which includes
/// funciton pointers to the functions of the instance).  Querying whether the
/// weak pointer can be converted to a shared pointer serves on a check as to
/// whether it is safe to use the function pointers.  Not ideal, but better than
/// nothing.
masala::base::api::MasalaObjectAPIDefinitionCWP
BrentAlgorithmLineOptimizer::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::getter;
	using masala::base::Real;
	using masala::base::Size;

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"The BrentAlgorithmLineOptimizer carries out gradient-free optimization of a function "
				"along a line.  It uses the algorithm of Richard P. Brent described in ''Algorithms for Minimization "
				"Without Derivatives'' (1973).",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( BrentAlgorithmLineOptimizer, api_def );

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"tolerance", "Get the tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of tolerance).",
				"tolerance", "The tolerance for determining whether the search has converged.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::tolerance, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"max_iters", "Get the maximum number of iterations.  Defaults to 1000.  "
				"A setting of 0 means to loop until convergence, regardless the number of "
				"iterations taken.",
				"max_iters", "The maximum iterations, or 0 if we loop until convergence.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::max_iters, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"initial_stepsize", "Get the step size for initially bracketing x.  "
				"Set to 0.001 by default.",
				"initial_stepsize", "The initial step size.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::initial_stepsize, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"throw_if_iterations_exceeded", "Get whether we throw if "
				"iteration maximum is exceeded (true), or just warn (false, the default).",
				"throw_if_iterations_exceeded", "True if we throw if iteration maximum is "
				"exceeded, false otherwise.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::throw_if_iterations_exceeded, this )
			)
		);

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_tolerance", "Set the tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of tolerance).",
				"tolerance_in", "The tolerance to set.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::set_tolerance, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_max_iters", "Set the maximum number of iterations.  Defaults to 1000.  "
				"A setting of 0 means to loop until convergence, regardless the number of "
				"iterations taken.",
				"max_iters_in", "The maximum iterations to set.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::set_max_iters, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_initial_stepsize", "Set the step size for initially bracketing x.  "
				"Set to 0.001 by default.",
				"initial_stepsize_in", "The initial step size to set.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::set_initial_stepsize, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< bool > >(
				"set_throw_if_iterations_exceeded", "Set whether we should throw if "
				"iteration maximum is exceeded (true), or just warn (false, the default).",
				"setting", "True if we want to throw if iteration maximum is exceeded, false otherwise.",
				false, false,
				std::bind( &BrentAlgorithmLineOptimizer::set_throw_if_iterations_exceeded, this, std::placeholders::_1 )
			)
		);

		api_definition() = api_def;
	}

	return api_definition();
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the line optimizer on a single line optimization problem, and produce a single solution.
/// @details Must be implemented by derived classes.  The solution is a pair of (x, f(x)) where x minimizes f.
/// @param[in] fxn The function to minimize.
/// @param[out] x The value of x that (locally) minimizes f(x).
/// @param[out] fxn_at_x The value of f(x) where x (locally) minimizes f(x).
void
BrentAlgorithmLineOptimizer::run_line_optimizer(
	std::function< masala::base::Real( masala::base::Real ) > const & fxn,
	masala::base::Real & x,
	masala::base::Real & fxn_at_x
) const {
	using masala::base::Size;
	using masala::base::Real;

	// Lock mutex.
	std::lock_guard< std::mutex > lock( mutex() );

	// Defined in util.hh -- find bounds for the minimum:
	Real left(x-initial_stepsize_), right, fxn_at_left, fxn_at_right;
	bracket_minimum_with_parabolic_extrapolation(
		left, x, right, fxn_at_left, fxn_at_x, fxn_at_right, fxn
	);
	CHECK_OR_THROW_FOR_CLASS( left < right, "run_line_optimizer", "Expected left to be less than right; got " + std::to_string(left) + " and " + std::to_string(right) + " for left and centre, respectively." );

	Real step_offset(0.0), etemp, fxn_at_parabolic_min, previous_secondleast,
		current_secondleast, fxn_at_previous_secondleast, fxn_at_current_secondleast,
		quotient_numerator, quotient_denominator, rrr, absolute_tolerance,
		twice_absolute_tolerance, parabolic_min, left_right_midpoint,
		x_dist_to_furthest_edge(0.0), x_minus_current_secondleast, x_minus_previous_secondleast;
	Real const small_epsilon( std::numeric_limits< Real >::epsilon() * 1.0e-3 );

	// Start with the best estimate for a lowish value between the
	// extrema from the initial bracketing:
	current_secondleast = previous_secondleast = x;
	fxn_at_current_secondleast = fxn_at_previous_secondleast = fxn_at_x; // Avoid an unnecessary repeated function evaluation here.

	// std::cout << std::setprecision(40);  // COMMENT ME OUT -- FOR TEMPORARY DEBUGGING ONLY.
	// std::cout << "Tol=" << tolerance_ << std::endl;

	Size iter_counter(0);
	while( max_iters_ > 0 ? iter_counter < max_iters_ : true ) {
		++iter_counter;
		//std::cout << iter_counter << ": x=" << x << " f(x)=" << fxn_at_x << " right=" << right << " left=" << left << std::endl;  // COMMENT ME OUT -- FOR TEMPORARY DEBUGGING ONLY.
		left_right_midpoint = (left+right)/2.0;
		absolute_tolerance = tolerance_ * std::abs(x) + small_epsilon;
		twice_absolute_tolerance = 2.0*(absolute_tolerance);
		if( std::abs(x - left_right_midpoint) <= (twice_absolute_tolerance - 0.5 * (right-left) ) ) {
			return;
		}
		if( std::abs(x_dist_to_furthest_edge) > absolute_tolerance ) {
			x_minus_current_secondleast = x - current_secondleast;
			x_minus_previous_secondleast = x - previous_secondleast;
			rrr = x_minus_current_secondleast * (fxn_at_x - fxn_at_previous_secondleast );
			quotient_denominator = x_minus_previous_secondleast * (fxn_at_x - fxn_at_current_secondleast );
			quotient_numerator = x_minus_previous_secondleast * quotient_denominator - x_minus_current_secondleast * rrr;
			quotient_denominator = 2.0 * (quotient_denominator - rrr);
			if( quotient_denominator > 0.0 ) {
				quotient_numerator *= -1;
			} else {
				quotient_denominator *= -1;
			}
			etemp = x_dist_to_furthest_edge;
			x_dist_to_furthest_edge = step_offset;
			if( std::abs(quotient_numerator) >= std::abs(quotient_denominator*etemp/2.0) ||
				quotient_numerator <= quotient_denominator*(left-x) ||
				quotient_numerator >= quotient_denominator*(right-x)
			) {
				x_dist_to_furthest_edge = (x >= left_right_midpoint ? left-x : right-x);
				step_offset = MASALA_ONE_MINUS_INV_GOLDEN_RATIO * x_dist_to_furthest_edge;
			} else {
				step_offset = quotient_numerator/quotient_denominator;
				parabolic_min = x+step_offset;
				if( parabolic_min-left < twice_absolute_tolerance ||
					right-parabolic_min < twice_absolute_tolerance
				) {
					step_offset = std::copysign( absolute_tolerance, left_right_midpoint-x );
				}
			}
		} else {
			x_dist_to_furthest_edge = (x >= left_right_midpoint ? left-x : right-x );
			step_offset = MASALA_ONE_MINUS_INV_GOLDEN_RATIO * x_dist_to_furthest_edge;
		}
		parabolic_min = (std::abs(step_offset) >= absolute_tolerance ? x + step_offset : x + std::copysign(absolute_tolerance, step_offset) );
		fxn_at_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.

		if( fxn_at_parabolic_min <= fxn_at_x ) {
			if( parabolic_min >= x ) {
				left = x;
			} else {
				right = x;
			}
			previous_secondleast = current_secondleast;
			fxn_at_previous_secondleast = fxn_at_current_secondleast;
			current_secondleast = x;
			fxn_at_current_secondleast = fxn_at_x;
			x = parabolic_min;
			fxn_at_x = fxn_at_parabolic_min;
		} else {
			if( parabolic_min < x ) {
				left = parabolic_min;
			} else {
				right = parabolic_min;
			}
			if( fxn_at_parabolic_min <= fxn_at_current_secondleast || current_secondleast == x ) {
				previous_secondleast = current_secondleast;
				current_secondleast = parabolic_min;
				fxn_at_previous_secondleast = fxn_at_current_secondleast;
				fxn_at_current_secondleast = fxn_at_parabolic_min;
			} else if( fxn_at_parabolic_min <= fxn_at_previous_secondleast || previous_secondleast == x || previous_secondleast == current_secondleast ) {
				previous_secondleast = parabolic_min;
				fxn_at_previous_secondleast = fxn_at_parabolic_min;
			}
		}
	}

	if( throw_if_iterations_exceeded_ ) {
		MASALA_THROW( class_namespace_and_name(), "run_line_optimizer", "Iterations exceeded and function not converged!" );
 	} else {
		write_to_tracer( "Warning: After " + std::to_string( iter_counter ) + " iterations, the function has not converged!" );
	}

}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
BrentAlgorithmLineOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer const & src
) {
	BrentAlgorithmLineOptimizer const * src_ptr_cast( dynamic_cast< BrentAlgorithmLineOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
BrentAlgorithmLineOptimizer::protected_make_independent() {
	// TODO
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer::protected_make_independent();
}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
