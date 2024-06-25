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
#include <numeric_api/auto_generated_api/optimization/gradient_based/GradientBasedFunctionOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/gradient_based/GradientBasedFunctionOptimizationSolutions_API.hh>

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
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "GradientBased", "LBFGSFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
LBFGSFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "GradientBased", "LBFGSFunctionOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "gradient_based", "numeric", "quasi-newtonian", "l-bfgs" }
std::vector< std::string >
LBFGSFunctionOptimizer::get_keywords() const {
	return std::vector< std::string > {
		"optimizer",
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
/// @returns { {"Optimizer", "GradientBased", "LBFGSFunctionOptimizer"} }
std::vector< std::vector < std::string > >
LBFGSFunctionOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "Optimizer", "GradientBased", "LBFGSFunctionOptimizer" } };
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

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"A gradient-descent function optimizer that uses the limited-memory Broyden–Fletcher–Goldfarb–Shanno "
				"algorithm (L-BFGS), a quasi-Newtonian method that relies only on gradients to approximate the inverse "
				"Hessian matrix, to carry out gradient descent for a differentiable function in R^N.",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( LBFGSFunctionOptimizer, api_def );

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
std::vector< masala::numeric_api::auto_generated_api::optimization::gradient_based::GradientBasedFunctionOptimizationSolutions_APICSP >
LBFGSFunctionOptimizer::run_gradient_based_optimizer(
	masala::numeric_api::auto_generated_api::optimization::gradient_based::GradientBasedFunctionOptimizationProblems_API const & problems
) const {
	TODO TODO TODO;
}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
