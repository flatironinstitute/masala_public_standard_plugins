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
#include <optimizers/gradient_based/BFGSFunctionOptimizer.hh>

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
/// @returns { { "Optimizer", "RealValuedFunctionLocalOptimizer", "BFGSFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
BFGSFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "RealValuedFunctionLocalOptimizer", "BFGSFunctionOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi-newtonian", "l-bfgs" }
std::vector< std::string >
BFGSFunctionOptimizer::get_keywords() const {
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
/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "BFGSFunctionOptimizer"} }
std::vector< std::vector < std::string > >
BFGSFunctionOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "Optimizer", "RealValuedFunctionLocalOptimizer", "BFGSFunctionOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "BFGSFunctionOptimizer".
std::string
BFGSFunctionOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_based".
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
/// @returns "standard_masala_plugins::optimizers::gradient_based".
/*static*/
std::string
BFGSFunctionOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_based";
}

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the maximum number of steps that we can take.
/// @details A setting of 0 means loop until convergence.
void
BFGSFunctionOptimizer::set_max_iterations(
	masala::base::Size const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	max_iterations_ = setting;
}

/// @brief Set a line optimizer to use for the line searches.
/// @details Used directly, not cloned.  If none is provided (or if this is set to
/// nullptr), then a BrentAlgorithmLineOptimizer is used by default.
void
BFGSFunctionOptimizer::set_line_optimizer(
	masala::base::managers::engine::MasalaEngineAPICSP line_optimizer_in
) {
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	if( line_optimizer_in == nullptr ) {
		line_optimizer_ = nullptr;
		write_to_tracer( "No line optimizer set.  The default BrentAlgorithmLineOptimizer will be used." );
	} else {
		PluginLineOptimizerCSP line_opt_cast( std::dynamic_pointer_cast< PluginLineOptimizer const >( line_optimizer_in->get_inner_engine_object_const() ) );
		CHECK_OR_THROW_FOR_CLASS( line_opt_cast != nullptr, "set_line_optimizer", "The provided objected was of type " + line_optimizer_in->inner_class_name() +
			", which is not a PluginLineOptimizer derived class!"
		);
		std::lock_guard< std::mutex > lock( mutex() );
		line_optimizer_ = line_opt_cast;
		write_to_tracer( "Set line optimizer to " + line_optimizer_->class_name() + "." );
	}
}

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the maximum number of steps that we can take
/// @details A setting of 0 means loop until convergence.
masala::base::Size
BFGSFunctionOptimizer::max_iterations() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return max_iterations_;
}

/// @brief Get the line optimizer used for the line searches.
/// @details Could be nullptr, in which case a BrentAlgorithmLineOptimizer
/// is used by default.
masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerCSP
BFGSFunctionOptimizer::line_optimizer() const {
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
BFGSFunctionOptimizer::run_real_valued_local_optimizer(
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblems_API const & problems
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;
	using namespace masala::base::managers::threads;
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	using masala::base::Size;

	std::lock_guard< std::mutex > lock( mutex() );

	PluginLineOptimizerCSP line_optimizer(
		line_optimizer_ == nullptr ?
		masala::make_shared< BrentAlgorithmLineOptimizer >() :
		line_optimizer_
	);

	std::vector< RealValuedFunctionLocalOptimizationSolutions_APISP > outvec_nonconst( problems.n_problems() );

	MasalaThreadedWorkRequest work_vector;
	Size jobcounter(0);
	for( Size i(0); i<problems.n_problems(); ++i ) {
		outvec_nonconst[i] = masala::make_shared< RealValuedFunctionLocalOptimizationSolutions_API >();

		RealValuedFunctionLocalOptimizationProblem_APICSP curproblem(
			std::dynamic_pointer_cast< RealValuedFunctionLocalOptimizationProblem_API const >( problems.problem(i) )
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem != nullptr, "run_real_valued_local_optimizer",
			"Could not interpret problem " + std::to_string(i) + " (of type " + problems.problem(i)->inner_class_name() +
			") as a RealValuedFunctionLocalOptimizationProblem."
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem->has_objective_function(), "run_real_valued_local_optimizer",
			"Problem " + std::to_string(i) + " (of type " + curproblem->inner_class_name() +
			") does not implement an objective function."
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem->has_objective_function_gradient(), "run_real_valued_local_optimizer",
			"Problem " + std::to_string(i) + " (of type " + curproblem->inner_class_name() +
			") does not implement an objective function gradient."
		);
		CHECK_OR_THROW_FOR_CLASS( curproblem->has_at_least_one_starting_point(), "run_real_valued_local_optimizer",
			"Problem " + std::to_string(i) + " (of type " + curproblem->inner_class_name() +
			") does not have at least one starting point."
		);

		Size const nstarts( curproblem->starting_points().size() );
		for( Size j(0); j<nstarts; ++j ) {
			PluginLineOptimizerCSP line_optimizer_clone( std::dynamic_pointer_cast< PluginLineOptimizer const >( line_optimizer->deep_clone() ) );
			CHECK_OR_THROW_FOR_CLASS( line_optimizer_clone != nullptr, "run_real_valued_local_optimizer",
					"Unable to properly clone the " + line_optimizer->class_name() + " class instance.  This is a "
					"program error that ought not to happen.  Please consult a developer."
			);

			work_vector.add_job(
				std::bind(
					&BFGSFunctionOptimizer::run_one_job_in_threads,
					this,
					jobcounter,
					i, j,
					std::cref(curproblem),
					line_optimizer_clone,
					std::ref(outvec_nonconst[i])
				)
			);
			++jobcounter;
		}
	}

	// Actually run the work in threads:
	work_vector.set_n_threads_to_request( threads_to_request() );
	MasalaThreadedWorkExecutionSummary const execution_summary(
		MasalaThreadManager::get_instance()->do_work_in_threads( work_vector )
	);

	// Nonconst to const:
	std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP > outvec( problems.n_problems() );
	for( Size i(0); i<problems.n_problems(); ++i ) {
		outvec[i] = outvec_nonconst[i];
	}
	return outvec;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a single gradient-based loss function minimization problem, and produce a single solution.
/// @details This function executes in threads.  Expected to be called from a mutex-locked context.
void
BFGSFunctionOptimizer::run_one_job_in_threads(
	masala::base::Size const job_index,
	masala::base::Size const problem_index,
	masala::base::Size const start_index,
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblem_APICSP const & problem,
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerCSP line_optimizer, // Deliberately passed by shared pointer copy.
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolutions_APISP & solutions
) const {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;

	// Redundant checks:
	CHECK_OR_THROW_FOR_CLASS( problem->has_objective_function(), "run_one_job_in_threads",
		"Problem " + std::to_string(problem_index) + " (of type " + problem->inner_class_name() +
		") does not implement an objective function."
	);
	CHECK_OR_THROW_FOR_CLASS( problem->has_objective_function_gradient(), "run_one_job_in_threads",
		"Problem " + std::to_string(problem_index) + " (of type " + problem->inner_class_name() +
		") does not implement an objective function gradient."
	);
	CHECK_OR_THROW_FOR_CLASS( problem->has_at_least_one_starting_point(), "run_one_job_in_threads",
		"Problem " + std::to_string(problem_index) + " (of type " + problem->inner_class_name() +
		") does not have at least one starting point."
	);
	CHECK_OR_THROW_FOR_CLASS( line_optimizer != nullptr, "run_one_job_in_threads",
		"Got a null pointer for the line optimizer.  This is a program error that ought not to happen.  "
		"Please consult a developer."
	);

	bool converged(false);

	Size iter(0);
	Eigen::Vector< Real, Eigen::Dynamic > p( problem->starting_points()[start_index] );
	Eigen::Vector< Real, Eigen::Dynamic > pnew;
	pnew.resize( p.size() );
	pnew = p;
	std::function< Real( Eigen::Vector< Real, Eigen::Dynamic > const & ) > compute_fxn( problem->objective_function() );
	std::function< Real( Eigen::Vector< Real, Eigen::Dynamic > const &, Eigen::Vector< Real, Eigen::Dynamic > & ) > compute_fxn_grad( problem->objective_function_gradient() );

	Real curscore( compute_fxn(p) );
	Real newscore( curscore );
	Eigen::Vector< Real, Eigen::Dynamic > curgrad, newgrad, curdirection;
	curgrad.resize( p.size() );
	newgrad.resize( p.size() );
	curdirection.resize( p.size() );
	compute_fxn_grad( p, curgrad );
	curdirection = curgrad;

	Eigen::Matrix< Real, Eigen::Dynamic, Eigen::Dynamic > inv_hessian;
	inv_hessian.setIdentity( p.size(), p.size() );

	while( max_iterations_ == 0 || iter < max_iterations_ ) {

		// Run the line optimizer along the current direction (the gradient modified by the approximation of the inverse Hessian):
		line_optimizer->run_line_optimizer( compute_fxn, p, curscore, curgrad, -curdirection, pnew, newscore );

		// Test for convergence:
		if( search_converged( p, pnew ) ) {
			converged = true;
			break;
		}

		// Compute the new gradient:
		compute_fxn_grad( pnew, newgrad );

		// Test for convergence:
		if( gradient_converged( newgrad ) ) {
			converged = true;
			break;
		}

		// Update the inverse Hessian approximation:
		update_inverse_hessian( p, pnew, curgrad, newgrad, inv_hessian );

		// Update the search direction:
		curdirection = inv_hessian * newgrad;

		// Update the current position and gradient:
		p = pnew;
		curgrad = newgrad;
		curscore = newscore;

		// Increment the iteration:
		++iter;
	}

	if( !converged ) {
		write_to_tracer( "Warning!  The maximum iterations (" +  std::to_string(max_iterations_) + ") for job "
			+ std::to_string(job_index) + " (problem " + std::to_string(problem_index) + ", starting point " + std::to_string(start_index)
			+ ")" + " were exhausted, but the function did not converge!"
		);
	} else {
		write_to_tracer( "For problem " + std::to_string(problem_index) + ", starting point " + std::to_string(start_index) + ", the "
			+ class_name() + "'s search for a local minimum converged in " + std::to_string( iter+1 ) + " iterations.  New function value: " +
			std::to_string( newscore ) + "."
		);
	}

	RealValuedFunctionLocalOptimizationSolution_APISP solution_out( masala::make_shared< RealValuedFunctionLocalOptimizationSolution_API >() );
	solution_out->set_converged(converged);
	solution_out->set_iterations( iter + 1 );
	solution_out->set_problem( problem );
	solution_out->set_starting_point_and_index( problem->starting_points()[start_index], start_index );
	solution_out->set_n_times_solution_was_produced(1);
	solution_out->set_solution_point( pnew );
	solution_out->set_solution_score( newscore );
	solution_out->set_solution_score_data_representation_approximation( newscore );
	solution_out->set_solution_score_solver_approximation( newscore );

	solutions->add_optimization_solution( solution_out );
}

/// @brief Update the approximation of the inverse of the Hessian matrix.
/// @details The update rule differs between the DFP, BFGS, and L-BFGS algorithms.
/// @note Expected to be called from a mutex-locked context.
void
BFGSFunctionOptimizer::update_inverse_hessian(
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ,//p_old,
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ,//p_new,
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ,//grad_old,
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ,//grad_new,
	Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > & //inv_hessian
) const {
	TODO TODO TODO;
}

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
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
BFGSFunctionOptimizer::protected_make_independent() {
	// TODO
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer::protected_make_independent();
}

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins
