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
    along with this program.  If not, see <https://current_secondleast.gnu.org/licenses/>.
*/

/// @file src/numeric_api/base_classes/optimization/gradient_based/ArmijoInexactLineOptimizer.cc
/// @brief Implementation of the ArmijoInexactLineOptimizer.
/// @details The ArmijoInexactLineOptimizer carries out gradient-free optimization of a function
/// along a line.  It uses the algorithm of Richard P. Brent described in "Algorithms for Minimization
/// Without Derivatives" (1973).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/gradient_based/ArmijoInexactLineOptimizer.hh>

// Optimizers headers:
#include <optimizers/gradient_based/util.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/types.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ThreeInput.tmpl.hh>

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
ArmijoInexactLineOptimizer::clone() const {
	return masala::make_shared< ArmijoInexactLineOptimizer >(*this);
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
ArmijoInexactLineOptimizerSP
ArmijoInexactLineOptimizer::deep_clone() const {
	ArmijoInexactLineOptimizerSP new_obj( std::static_pointer_cast< ArmijoInexactLineOptimizer >( clone() ) );
	new_obj->make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "LineOptimizer", "ArmijoInexactLineOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
ArmijoInexactLineOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "LineOptimizer", "ArmijoInexactLineOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "line_optimizer", "lightweight", "numeric",  "armijo", "inexact" }
std::vector< std::string >
ArmijoInexactLineOptimizer::get_keywords() const {
	return std::vector< std::string > {
		"line_optimizer",
		"lightweight",
		"numeric",
		"armijo",
		"inexact"
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
/// @returns { {"LineOptimizer", "ArmijoInexactLineOptimizer"} }
std::vector< std::vector < std::string > >
ArmijoInexactLineOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "LineOptimizer", "ArmijoInexactLineOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "ArmijoInexactLineOptimizer".
std::string
ArmijoInexactLineOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
std::string
ArmijoInexactLineOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Every class can name itself.
/// @returns "ArmijoInexactLineOptimizer".
/*static*/
std::string
ArmijoInexactLineOptimizer::class_name_static() {
	return "ArmijoInexactLineOptimizer";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
/*static*/
std::string
ArmijoInexactLineOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_based";
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////


/// @brief Get the intial step size, as a multiple of the gradient.  Defaults to 1.0
masala::base::Real
ArmijoInexactLineOptimizer::initial_stepsize() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return initial_stepsize_;
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the intial step size, as a multiple of the gradient.  Defaults to 1.0
void
ArmijoInexactLineOptimizer::set_initial_stepsize(
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
ArmijoInexactLineOptimizer::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;
	using masala::base::Real;
	using masala::base::Size;

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"The ArmijoInexactLineOptimizer carries out gradient-free optimization of a function "
				"along a line.  It uses the algorithm of Richard P. Brent described in ''Algorithms for Minimization "
				"Without Derivatives'' (1973).",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( ArmijoInexactLineOptimizer, api_def );

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"initial_stepsize", "Get the step size for initially bracketing x.  "
				"Set to 0.001 by default.",
				"initial_stepsize", "The initial step size.",
				false, false,
				std::bind( &ArmijoInexactLineOptimizer::initial_stepsize, this )
			)
		);

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_initial_stepsize", "Set the step size for initially bracketing x.  "
				"Set to 0.001 by default.",
				"initial_stepsize_in", "The initial step size to set.",
				false, false,
				std::bind( &ArmijoInexactLineOptimizer::set_initial_stepsize, this, std::placeholders::_1 )
			)
		);

		// Work functions:
		// api_def->add_work_function(
		// 	masala::make_shared<
		// 		MasalaObjectAPIWorkFunctionDefinition_ThreeInput<
		// 			void,
		// 			std::function< Real( Real ) > const &,
		// 			Real &,
		// 			Real &
		// 		>
		// 	> (
		// 		"run_line_optimizer", "Run the line optimizer on a single line optimization problem, and produce a single solution.  "
		// 		"The solution is a pair of (x, f(x)) where x reduces (but doesn't necessarily minimize) f.  Note that this function locks "
		// 		"the object mutex, so this object is intended to be used to minimize a single function at a time (unlike other optimizers "
		// 		"that take a vector of minimization problems to carry out in parallel).",
		// 		true, false, true, false,
		// 		"fxn", "The function, f(x), to minimize.  This should be a std::function object that takes a Real and returns a Real.",
		// 		"x", "The value of x that minimizes -- or, rather, adequately reduces -- f(x).  Set by this function.  (The input value is used as the starting point to find a local minimum.)",
		// 		"fxn_at_x", "The value of the function f(x) at the value of x that adequately reduces f(x).  Set by this function.",
		// 		"void", "This function produces no return value.  Instead, x and fxn_at_x are set by this function.",
		// 		std::bind( &ArmijoInexactLineOptimizer::run_line_optimizer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
		// 	)
		// );

		api_definition() = api_def;
	}

	return api_definition();
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the line optimizer on a single line optimization problem, and produce a single solution.
/// @details Must be implemented by derived classes.  The solution is a pair of (x, f(x)) where x minimizes f.  Note
/// that this function locks the object mutex, so this object is intended to be used to minimize a single function
/// at a time (unlike other optimizers that take a vector of minimization problems to carry out in parallel).
/// @param[in] fxn The function to (approximately) minimize.
/// @param[in] x0 The starting point for the search.
/// @param[in] fxn_at_x0 The value of the function at the starting point for the search.
/// @param[in] grad_of_fxn_at_x0 The gradient of the function at the starting point for the search.
/// @param[in] search_dir The search direction, which may or may not match the negative gradient of the starting point.
/// @param[out] x The output value of x that adequately reduces f(x).
/// @param[out] fxn_at_x The output value of f(x) where x adequately reduces f(x).
void
ArmijoInexactLineOptimizer::run_line_optimizer(
	std::function< masala::base::Real( Eigen::VectorXd const & ) > const & ,//fxn,
	Eigen::VectorXd const & ,//x0,
	masala::base::Real const & ,//fxn_at_x0,
	Eigen::VectorXd const & ,//grad_of_fxn_at_x0,
	Eigen::VectorXd const & ,//search_dir,
	Eigen::VectorXd & ,//x,
	masala::base::Real & //fxn_at_x
) const {
	using masala::base::Size;
	using masala::base::Real;

	// Lock mutex.
	std::lock_guard< std::mutex > lock( mutex() );

	//TODO TODO TODO;

}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
ArmijoInexactLineOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer const & src
) {
	ArmijoInexactLineOptimizer const * src_ptr_cast( dynamic_cast< ArmijoInexactLineOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );
	initial_stepsize_ = src_ptr_cast->initial_stepsize_;
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
ArmijoInexactLineOptimizer::protected_make_independent() {
	// Nothing to be done at the level of this derived class to make it independent.
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer::protected_make_independent();
}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
