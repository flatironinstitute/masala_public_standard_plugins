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

/// @file src/numeric_api/base_classes/optimization/gradient_based/LBFGSFunctionOptimizer.cc
/// @brief Implementation of the LBFGSFunctionOptimizer.
/// @details The LBFGSFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using the quasi-Newtonian limited-memory Broyden–Fletcher–Goldfarb–Shanno
/// algorithm.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/gradient_based/LBFGSFunctionOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/OptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.hh>

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

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the
/// copy.  Contained objects may still be shared.
masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizerSP
LBFGSFunctionOptimizer::clone() const {
	return masala::make_shared< LBFGSFunctionOptimizer >(*this);
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
LBFGSFunctionOptimizerSP
LBFGSFunctionOptimizer::deep_clone() const {
	LBFGSFunctionOptimizerSP new_obj( std::static_pointer_cast< LBFGSFunctionOptimizer >( clone() ) );
	new_obj->make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "RealValuedFunctionLocalOptimizer", "LBFGSFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
LBFGSFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "RealValuedFunctionLocalOptimizer", "LBFGSFunctionOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi-newtonian", "l-bfgs" }
std::vector< std::string >
LBFGSFunctionOptimizer::get_keywords() const {
	return std::vector< std::string > {
		"optimizer",
		"real_valued",
		"local_optimizer",
        "gradient_based",
		"numeric",
		"quasi-newtonian",
		"l-bfgs"
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
/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "LBFGSFunctionOptimizer"} }
std::vector< std::vector < std::string > >
LBFGSFunctionOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "Optimizer", "RealValuedFunctionLocalOptimizer", "LBFGSFunctionOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "LBFGSFunctionOptimizer".
std::string
LBFGSFunctionOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
std::string
LBFGSFunctionOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Every class can name itself.
/// @returns "LBFGSFunctionOptimizer".
/*static*/
std::string
LBFGSFunctionOptimizer::class_name_static() {
	return "LBFGSFunctionOptimizer";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
/*static*/
std::string
LBFGSFunctionOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_based";
}

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the maximum number of steps that we can take.
/// @details A setting of 0 means loop until convergence.
void
LBFGSFunctionOptimizer::set_max_iterations(
	masala::base::Size const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	max_iterations_ = setting;
}

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the maximum number of steps that we can take
/// @details A setting of 0 means loop until convergence.
masala::base::Size
LBFGSFunctionOptimizer::max_iterations() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return max_iterations_;
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
LBFGSFunctionOptimizer::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::getter;
	using masala::base::Size;
	using masala::base::Real;

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"A gradient-descent function optimizer that uses the limited-memory Broyden-Fletcher-Goldfarb-Shanno "
				"algorithm (L-BFGS), a quasi-Newtonian method that relies only on gradients to approximate the inverse "
				"Hessian matrix, to carry out gradient descent for a differentiable function in R^N.",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( LBFGSFunctionOptimizer, api_def );

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_max_iterations", "Set the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations_in", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &LBFGSFunctionOptimizer::set_max_iterations, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"max_iterations", "Get the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &LBFGSFunctionOptimizer::max_iterations, this )
			)
		);

		api_definition() = api_def;
	}

	return api_definition();
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a set of gradient-based loss function minimization problems, and produce a set of solutions.
/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
/// the problem with the same index.
std::vector< masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolutions_APICSP >
LBFGSFunctionOptimizer::run_real_valued_local_optimizer(
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblems_API const & //problems
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;

	std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP > outvec;
	//TODO TODO TODO;
	return outvec;
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
LBFGSFunctionOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer const & src
) {
	LBFGSFunctionOptimizer const * src_ptr_cast( dynamic_cast< LBFGSFunctionOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
LBFGSFunctionOptimizer::protected_make_independent() {
	// TODO
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer::protected_make_independent();
}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
