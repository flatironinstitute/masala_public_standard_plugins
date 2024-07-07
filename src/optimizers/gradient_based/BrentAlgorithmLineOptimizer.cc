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
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

// STL headers:
#include <vector>
#include <string>

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
	CHECK_OR_THROW_FOR_CLASS( setting >= 0.99 * std::sqrt( std::numeric_limits< masala::base::Real >::denorm_min() ),
		"set_tolerance", "The tolerance must be greater than or equal to the square root of machine precision ("
		+ std::to_string( std::sqrt( std::numeric_limits< masala::base::Real >::denorm_min() ) )
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

/// @brief GSet the step size for initially bracketing x.  Set to 0.001 by default.
void
BrentAlgorithmLineOptimizer::set_initial_stepsize(
	masala::base::Real const setting
) {
	CHECK_OR_THROW_FOR_CLASS(
		initial_stepsize_ >= std::numeric_limits< masala::base::Real >::denorm_min(),
		"set_initial_stepsize", "The intial step size must be greater than or equal to machine "
		"precision (" + std::to_string(std::numeric_limits< masala::base::Real >::denorm_min())
		+ ").  Got " + std::to_string(setting) + "."
	);
	std::lock_guard< std::mutex > lock( mutex() );
	initial_stepsize_ = setting;
}

/// @brief Set whether we should we throw if iterations are exceeded (true), or just warn
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

	// note: a -> left, b -> right.  xmin: use x directly; fmin: use fxn_at_x directly.

	// Defined in util.hh -- find bounds for the minimum:
	Real left(x-initial_stepsize_), right, fxn_at_left, fxn_at_right;
	bracket_minimum_with_parabolic_extrapolation(
		left, x, right, fxn_at_left, fxn_at_x, fxn_at_right, fxn
	);
	CHECK_OR_THROW_FOR_CLASS( left < right, "run_line_optimizer", "Expected left to be less than right; got " + std::to_string(left) + " and " + std::to_string(right) + " for left and centre, respectively." );

	Real ddd(0.0), etemp, fxn_at_parabolic_min, fv, fw, small_epsilon( std::numeric_limits< Real >::epsilon() * 1.0e-3 ),
		ppp, qqq, rrr, tol1, tol2, parabolic_min, vvv, www, xxxm, eee(0.0), x_minus_www, x_minus_vvv;

	// Start with the best estimate for a lowish value between the
	// extrema from the initial bracketing:
	www = vvv = x;
	fw = fv = fxn_at_x; // Avoid an unnecessary repeated function evaluation here.

	// Lock mutex.
	std::lock_guard< std::mutex > lock( mutex() );

	Size iter_counter(0);
	while( max_iters_ > 0 ? iter_counter < max_iters_ : true ) {
		++iter_counter;
		xxxm = 0.5*(left+right);
		tol1 = tolerance_ * std::abs(x) + small_epsilon;
		tol2 = 2.0*(tol1);
		if( std::abs(x - xxxm) <= (tol2 - 0.5 * (right-left) ) ) {
			return;
		}
		if( std::abs(eee) > tol1 ) {
			x_minus_www = x - www;
			x_minus_vvv = x - vvv;
			rrr = x_minus_www * (fxn_at_x - fv );
			qqq = x_minus_vvv * (fxn_at_x - fw );
			ppp = x_minus_vvv * qqq - x_minus_www * rrr;
			qqq = 2.0 * (qqq - rrr);
			if( qqq > 0.0 ) {
				ppp *= -1;
			} else {
				qqq = std::abs(qqq);
			}
			etemp = eee;
			eee = ddd;
			if( std::abs(ppp) >= std::abs(qqq*etemp/2.0) ||
				ppp <= qqq*(left-x) ||
				ppp >= qqq*(right-x)
			) {
				eee = (x >= xxxm ? left-x : right-x);
				ddd = MASALA_ONE_MINUS_INV_GOLDEN_RATIO * eee;
			} else {
				ddd = ppp/qqq;
				parabolic_min = x+ddd;
				if( parabolic_min-left < tol2 ||
					right-parabolic_min < tol2
				) {
					ddd = std::copysign( tol1, xxxm-x );
				}
			}
		} else {
			eee = (x >= xxxm ? left-x : right-x );
			ddd = MASALA_ONE_MINUS_INV_GOLDEN_RATIO * eee;
		}
		parabolic_min = (std::abs(ddd) >= tol1 ? x + ddd : x + std::copysign(tol1, ddd) );
		fxn_at_parabolic_min = fxn(parabolic_min); // FUNCTION EVALUATION.

		if( fxn_at_parabolic_min <= fxn_at_x ) {
			if( parabolic_min >= x ) {
				left = x;
			} else {
				right = x;
			}
			vvv = www;
			fv = fw;
			www = x;
			fw = fxn_at_x;
			x = parabolic_min;
			fxn_at_x = fxn_at_parabolic_min;
		} else {
			if( parabolic_min < x ) {
				left = parabolic_min;
			} else {
				right = parabolic_min;
			}
			if( fxn_at_parabolic_min <= fw || www == x ) {
				vvv = www;
				www = parabolic_min;
				fv = fw;
				fw = fxn_at_parabolic_min;
			} else if( fxn_at_parabolic_min <= fv || vvv == x || vvv == www ) {
				vvv = parabolic_min;
				fv = fxn_at_parabolic_min;
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
