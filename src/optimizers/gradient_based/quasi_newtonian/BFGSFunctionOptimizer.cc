/*
    Standard Masala Plugins
    Copyright (C) 2025 Vikram K. Mulligan

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

/// @file src/numeric_api/base_classes/optimization/gradient_based/BFGSFunctionOptimizer.cc
/// @brief Implementation of the BFGSFunctionOptimizer.
/// @details The BFGSFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using the quasi-Newtonian Broyden–Fletcher–Goldfarb–Shanno
/// algorithm.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/gradient_based/quasi_newtonian/BFGSFunctionOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/OptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/PluginLineOptimizer.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/types.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/setter_annotation/OwnedSingleObjectSetterAnnotation.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/managers/engine/MasalaEngineAPI.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/threads/MasalaThreadedWorkExecutionSummary.hh>
#include <base/managers/threads/MasalaThreadedWorkRequest.hh>

// Optimizers headers:
#include <optimizers/gradient_based/BrentAlgorithmLineOptimizer.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {
namespace quasi_newtonian {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the
/// copy.  Contained objects may still be shared.
masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizerSP
BFGSFunctionOptimizer::clone() const {
	return masala::make_shared< BFGSFunctionOptimizer >(*this);
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
BFGSFunctionOptimizerSP
BFGSFunctionOptimizer::deep_clone() const {
	BFGSFunctionOptimizerSP new_obj( std::static_pointer_cast< BFGSFunctionOptimizer >( clone() ) );
	new_obj->make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer", "BFGSFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
BFGSFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer", "BFGSFunctionOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi-newtonian", "bfgs" }
std::vector< std::string >
BFGSFunctionOptimizer::get_keywords() const {
	return std::vector< std::string > {
		"optimizer",
		"real_valued",
		"local_optimizer",
        "gradient_based",
		"numeric",
		"quasi-newtonian",
		"bfgs"
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
/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer", "BFGSFunctionOptimizer"} }
std::vector< std::vector < std::string > >
BFGSFunctionOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer", "BFGSFunctionOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "BFGSFunctionOptimizer".
std::string
BFGSFunctionOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based::quasi_newtonian".
std::string
BFGSFunctionOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Every class can name itself.
/// @returns "BFGSFunctionOptimizer".
/*static*/
std::string
BFGSFunctionOptimizer::class_name_static() {
	return "BFGSFunctionOptimizer";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based::quasi_newtonian".
/*static*/
std::string
BFGSFunctionOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_based::quasi_newtonian";
}

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


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
BFGSFunctionOptimizer::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::setter::setter_annotation;
	using namespace masala::base::api::getter;
	using namespace masala::base::managers::engine;
	using masala::base::Size;
	using masala::base::Real;

	std::lock_guard< std::mutex > lock( mutex() );

	if( api_definition() == nullptr ) {
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"A gradient-descent function optimizer that uses the Broyden-Fletcher-Goldfarb-Shanno "
				"algorithm (BFGS), a quasi-Newtonian method that relies only on gradients to approximate the inverse "
				"Hessian matrix, to carry out gradient descent for a differentiable function in R^N.",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( BFGSFunctionOptimizer, api_def );

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_max_iterations", "Set the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations_in", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &BFGSFunctionOptimizer::set_max_iterations, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_threads_to_request", "Set the number of threads requested by this optimizer.  The actual number "
				"may be smaller if there is less work to do, or if there are fewer threads available.",
				"setting", "The number of threads to request.  Different starting points of the same problem or "
				"different starting points of different problems can be carried out simultaneously in threads.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::set_threads_to_request, this, std::placeholders::_1 )
			)
		);
		{
			MasalaObjectAPISetterDefinition_OneInputSP< MasalaEngineAPICSP > set_line_optimizer_setter(
				masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< MasalaEngineAPICSP > >(
					"set_line_optimizer", "Set a line optimizer to use for the line searches.  Used directly, "
					"not cloned.  If none is provided (or if this is set to nullptr), then a BrentAlgorithmLineOptimizer "
					"is used by default.  Throws if the MasalaEngine provided cannot be interpreted as a MasalaPluginLineOptimizer.",
					"line_optimizer_in", "The line optimizer to use when performing quasi-Newtonian gradient-descent minimization.",
					false, false, std::bind( &BFGSFunctionOptimizer::set_line_optimizer, this, std::placeholders::_1 )
				)
			);
			OwnedSingleObjectSetterAnnotationSP set_line_optimizer_setter_annotation( masala::make_shared< OwnedSingleObjectSetterAnnotation >() );
			set_line_optimizer_setter_annotation->set_plugin_manager_info(
				std::vector< std::string >{ "LineOptimizer" },
				std::vector< std::string >{ "line_optimizer" },
				true
			);
			set_line_optimizer_setter_annotation->set_engine_manager_info(
				std::vector< std::string >{ "LineOptimizer" },
				std::vector< std::string >{ "line_optimizer" },
				*set_line_optimizer_setter,
				true
			);
			set_line_optimizer_setter->add_setter_annotation( set_line_optimizer_setter_annotation );
			api_def->add_setter( set_line_optimizer_setter );
		}
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_tolerance", "Set the tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of tolerance).",
				"tolerance_in", "The tolerance to set.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::set_tolerance, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_gradient_tolerance", "Set the gradient tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of gradient tolerance).",
				"gradient_tolerance_in", "The gradient tolerance to set.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::set_gradient_tolerance, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< bool > >(
				"set_throw_if_iterations_exceeded", "Set whether we should throw if "
				"iteration maximum is exceeded (true), or just warn (false, the default).",
				"setting", "True if we want to throw if iteration maximum is exceeded, false otherwise.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::set_throw_if_iterations_exceeded, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_min_inv_hessian_determinant", "Set the minimum absolute value of the determinant of the approximate inverse "
				"Hessian matrix, below which we reset the Hessian to the identity matrix.  Quasi-Newtonian methods fail if the "
				"Hessian becomes singular.  The default value is 4 times machine precision, and rarely needs to be adjusted.",
				"min_inv_hessian_determinant", "The minimum value for the determinant of the approximation of the inverse Hessian, below "
				"which we reset the inverse Hessian to the identity matrix.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::set_min_inv_hessian_determinant, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"max_iterations", "Get the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &BFGSFunctionOptimizer::max_iterations, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"threads_to_request", "Get the number of threads requested by this optimizer.",
				"threads_to_request", "The number of threads requested by this optimizer.  The actual number "
				"may be smaller if there is less work to do, or if there are fewer threads available.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::threads_to_request, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerCSP > >(
				"line_optimizer", "Get the line optimizer to use for the line searches.  If this is nullptr, then a BrentAlgorithmLineOptimizer "
				"is used by default.",
				"line_optimizer", "The line optimizer to use for the line searches when performing quasi-Newtonian gradient descent minimization.",
				false, false, std::bind( &BFGSFunctionOptimizer::line_optimizer, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"tolerance", "Get the tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of tolerance).",
				"tolerance", "The tolerance for determining whether the search has converged.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::tolerance, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"gradient_tolerance", "Get the gradient tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of gradient tolerance).",
				"gradient_tolerance", "The tolerance for determining whether the search has converged.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::gradient_tolerance, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"throw_if_iterations_exceeded", "Get whether we throw if "
				"iteration maximum is exceeded (true), or just warn (false, the default).",
				"throw_if_iterations_exceeded", "True if we throw if iteration maximum is "
				"exceeded, false otherwise.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::throw_if_iterations_exceeded, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"min_inv_hessian_determinant", "Get the minimum absolute value of the determinant of the approximate inverse Hessian matrix, "
				"below which we reset the Hessian to the identity matrix.  Quasi-Newtonian methods fail if the Hessian becomes singular.  "
				"The default value is 4 times machine precision, and rarely needs to be adjusted.",
				"min_inv_hessian_determinant", "The minimum value for the determinant of the approximation of the inverse Hessian, below "
				"which we reset the inverse Hessian to the identity matrix.",
				false, false,
				std::bind( &BFGSFunctionOptimizer::min_inv_hessian_determinant, this )
			)
		);

		api_definition() = api_def;
	}

	return api_definition();
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
BFGSFunctionOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer const & src
) {
	BFGSFunctionOptimizer const * src_ptr_cast( dynamic_cast< BFGSFunctionOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );
	standard_masala_plugins::optimizers::gradient_based::QuasiNewtonianFunctionOptimizerBase::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
BFGSFunctionOptimizer::protected_make_independent() {
	// TODO
	standard_masala_plugins::optimizers::gradient_based::QuasiNewtonianFunctionOptimizerBase::protected_make_independent();
}

} // namespace quasi_newtonian
} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
