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

/// @file src/numeric_api/base_classes/optimization/gradient_based/GradientDescentFunctionOptimizer.cc
/// @brief Implementation of the GradientDescentFunctionOptimizer.
/// @details The GradientDescentFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using an iterative approach of computing gradients and carrying out
/// line searches with a line search algorithm.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/gradient_based/GradientDescentFunctionOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/OptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/LineOptimizer.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/types.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/setter_annotation/OwnedSingleObjectSetterAnnotation.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
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

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the
/// copy.  Contained objects may still be shared.
masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizerSP
GradientDescentFunctionOptimizer::clone() const {
	return masala::make_shared< GradientDescentFunctionOptimizer >(*this);
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
GradientDescentFunctionOptimizerSP
GradientDescentFunctionOptimizer::deep_clone() const {
	GradientDescentFunctionOptimizerSP new_obj( std::static_pointer_cast< GradientDescentFunctionOptimizer >( clone() ) );
	new_obj->make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "RealValuedFunctionLocalOptimizer", "GradientDescentFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
GradientDescentFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "RealValuedFunctionLocalOptimizer", "GradientDescentFunctionOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi-newtonian", "l-bfgs" }
std::vector< std::string >
GradientDescentFunctionOptimizer::get_keywords() const {
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
/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "GradientDescentFunctionOptimizer"} }
std::vector< std::vector < std::string > >
GradientDescentFunctionOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "Optimizer", "RealValuedFunctionLocalOptimizer", "GradientDescentFunctionOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "GradientDescentFunctionOptimizer".
std::string
GradientDescentFunctionOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
std::string
GradientDescentFunctionOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Every class can name itself.
/// @returns "GradientDescentFunctionOptimizer".
/*static*/
std::string
GradientDescentFunctionOptimizer::class_name_static() {
	return "GradientDescentFunctionOptimizer";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
/*static*/
std::string
GradientDescentFunctionOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_based";
}

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the maximum number of steps that we can take.
/// @details A setting of 0 means loop until convergence.
void
GradientDescentFunctionOptimizer::set_max_iterations(
	masala::base::Size const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	max_iterations_ = setting;
}

/// @brief Set a line optimizer to use for the line searches.
/// @details Used directly, not cloned.  If none is provided (or if this is set to
/// nullptr), then a BrentAlgorithmLineOptimizer is used by default.
void
GradientDescentFunctionOptimizer::set_line_optimizer(
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP const & line_optimizer_in
) {
	std::lock_guard< std::mutex > lock( mutex() );
	line_optimizer_ = line_optimizer_in;
}

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the maximum number of steps that we can take
/// @details A setting of 0 means loop until convergence.
masala::base::Size
GradientDescentFunctionOptimizer::max_iterations() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return max_iterations_;
}

/// @brief Get the line optimizer used for the line searches.
/// @details Could be nullptr, in which case a BrentAlgorithmLineOptimizer
/// is used by default.
masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP
GradientDescentFunctionOptimizer::line_optimizer() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return line_optimizer_;
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
GradientDescentFunctionOptimizer::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::setter::setter_annotation;
	using namespace masala::base::api::getter;
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
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
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( GradientDescentFunctionOptimizer, api_def );

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_max_iterations", "Set the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations_in", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &GradientDescentFunctionOptimizer::set_max_iterations, this, std::placeholders::_1 )
			)
		);
		{
			MasalaObjectAPISetterDefinition_OneInputSP< LineOptimizerCSP const & > set_line_optimizer_setter(
				masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< LineOptimizerCSP const & > >(
					"set_line_optimizer", "Set a line optimizer to use for the line searches.  Used directly, "
					"not cloned.  If none is provided (or if this is set to nullptr), then a BrentAlgorithmLineOptimizer "
					"is used by default.",
					"line_optimizer_in", "The line optimizer to use when performing gradient-descent minimization.",
					false, false, std::bind( &GradientDescentFunctionOptimizer::set_line_optimizer, this, std::placeholders::_1 )
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
				true
			);
			set_line_optimizer_setter->add_setter_annotation( set_line_optimizer_setter_annotation );
			api_def->add_setter( set_line_optimizer_setter );
		}

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"max_iterations", "Get the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &GradientDescentFunctionOptimizer::max_iterations, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< LineOptimizerCSP > >(
				"line_optimizer", "Get the line optimizer to use for the line searches.  If this is nullptr, then a BrentAlgorithmLineOptimizer "
				"is used by default.",
				"line_optimizer", "The line optimizer to use for the line searches.",
				false, false, std::bind( &GradientDescentFunctionOptimizer::line_optimizer, this )
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
GradientDescentFunctionOptimizer::run_real_valued_local_optimizer(
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblems_API const & problems
) const {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	using namespace masala::base::managers::threads;

	std::lock_guard< std::mutex > lock( mutex() );

	std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP > outvec;
	LineOptimizerCSP line_optimizer(
		line_optimizer == nullptr ?
		generate_brent_optimizer() :
		line_optimizer_
	);

	MasalaThreadedWorkRequest work_vector( protected_threads_to_request() );
	Size const nproblems( problems.n_problems() );
	work_vector.reserve( nproblems );
	outvec.resize( nproblems );
	outvec.shrink_to_fit();
	for( Size iproblem(0); iproblem < nproblems; ++iproblem ) {
		RealValuedFunctionLocalOptimizationProblem_APICSP curproblem(
			std::dynamic_pointer_cast< RealValuedFunctionLocalOptimizationProblem_API const >( problems.problem(iproblem) )
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem != nullptr, "run_real_valued_local_optimizer",
			"Could not interpret problem " + std::to_string( iproblem + 1 )
			+ " as a RealValuedFunctionLocalOptimizationProblem.  Problem type was "
			+ problems.problem(iproblem)->inner_class_name() + "."
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem->has_objective_function(), "run_real_valued_local_optimizer",
			"The " + class_name() + " requires that every problem have an objective function defined.  No "
			"real-valued objective function was found for problem " + std::to_string(iproblem + 1) + "."
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem->has_objective_function_gradient(), "run_real_valued_local_optimizer",
			"The " + class_name() + " requires that every problem have an objective function gradient defined.  No "
			"gradient function was found for problem " + std::to_string(iproblem + 1) + "."
		);

		Size const n_starting_points( curproblem->starting_points().size() );

		for( Size i_starting_point(0); i_starting_point < n_starting_points; ++i_starting_point ) {

			TODO SPLIT MULTIPLE STARTING POINTS OVER DIFFERENT WORK VECTOR JOBS;
			TODO INITIALIZE SOLUTIONS CONTAINER TO HAVE ONE SOLUTION PER STARTING POINT;

			LineOptimizerSP line_optimizer_copy( line_optimizer->clone() );
			line_optimizer_copy->make_independent();
			work_vector.add_job(
				std::bind(
					&GradientDescentFunctionOptimizer::run_real_valued_local_optimizer_on_one_problem,
					this,
					curproblem,
					i_starting_point,
					line_optimizer_copy,
					std::ref( outvec[iproblem] )
				)
			);
		}
	}

	MasalaThreadedWorkExecutionSummary const work_summary(
		MasalaThreadManager::get_instance()->do_work_in_threads( work_vector )
	);
	work_summary.write_summary_to_tracer();

	return outvec;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run a single local optimization problem in a thread.  This function runs in parallel
/// in threads.  This function is called from a mutex-locked context.
/// @param[in] problem The problem to solve.
/// @param[in] starting_point_index The index of the starting point for the problem.
/// @param[in] line_optimizer The line optimizer to use when solving this problem.
/// @param[out] solutions The solutions container pointer.  This will be updated to point to a new
/// solutions container object, containing a single solution.
void
GradientDescentFunctionOptimizer::run_real_valued_local_optimizer_on_one_problem(
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblem_APICSP problem,
	masala::base::Size const starting_point_index,
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP line_optimizer,
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolutions_APICSP & solutions
) const {
	using masala::base::Size;
	using masala::base::Real;

	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblem_API const & prob( *problem );

	std::function< Real( std::vector< Real > const & ) > const & fxn( prob.objective_function() );
	std::function< Real( std::vector< Real > const &, std::vector< Real > & ) > const & fxn_grad( prob.objective_function_gradient() );

	TODO CONVERT BELOW TO EIGEN VECTORS;

	std::vector< Real > const & x0( prob.starting_points()[starting_point_index] );
	Eigen::Vector< Real, Eigen::Dynamic > x;
	x.resize( x0.size() );
	for( Size i(0); i<x0.size(); ++i ) { x[i] = x0[i]; }
	Eigen::Vector< Real, Eigen::Dynamic > grad_at_x;
	grad_at_x.resize( x0.size() );
	Real f_at_x;

	Size iter_counter(0);
	while( max_iterations_ == 0 ? true : iter_counter < max_iterations_ ) {
		++iter_counter;
		f_at_x = fxn_grad( x, grad_at_x ); // Evaluate the function and its gradient.
		line_optimizer->run_line_optimizer(
			fxn, x, fxn_at_x, grad_at_x, grad_at_x, new_x, new_f_at_x
		)
		TODO CONVERGENCE CONDITION;
		TODO TODO TODO;
	}
}

/// @brief Generate the Brent optimizer used by default if another line optimizer is not provided.
masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP
GradientDescentFunctionOptimizer::generate_brent_optimizer() const {
	return masala::make_shared< BrentAlgorithmLineOptimizer >();
}


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
GradientDescentFunctionOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer const & src
) {
	GradientDescentFunctionOptimizer const * src_ptr_cast( dynamic_cast< GradientDescentFunctionOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );

	max_iterations_ = src_ptr_cast->max_iterations_;
	line_optimizer_ = src_ptr_cast->line_optimizer_;

	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
GradientDescentFunctionOptimizer::protected_make_independent() {
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	if( line_optimizer_ != nullptr ) {
		LineOptimizerSP line_optimizer_copy( line_optimizer_->clone() );
		line_optimizer_copy->make_independent();
		line_optimizer_ = line_optimizer_copy;
	}
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer::protected_make_independent();
}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
