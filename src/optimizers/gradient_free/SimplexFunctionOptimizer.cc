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

/// @file src/numeric_api/base_classes/optimization/gradient_free/SimplexFunctionOptimizer.cc
/// @brief Implementation of the SimplexFunctionOptimizer.
/// @details The SimplexFunctionOptimizer carries out local minimization of an arbitrary function
/// without using gradient information.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/gradient_free/SimplexFunctionOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/OptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolution_API.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/PluginLineOptimizer.hh>

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
#include <base/utility/container/container_util.tmpl.hh> // COMMENT ME OUT.  FOR DEGBUGGING ONLY.

// Optimizers headers:

// STL headers:
#include <vector>
#include <string>
// #include <iostream> // COMMENT ME OUT.  FOR DEGBUGGING ONLY.

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_free {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the
/// copy.  Contained objects may still be shared.
masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizerSP
SimplexFunctionOptimizer::clone() const {
	return masala::make_shared< SimplexFunctionOptimizer >(*this);
}

/// @brief Deep clone operation: copy this object and return a shared pointer to the
/// copy, making sure that all contained objects are also copied.
SimplexFunctionOptimizerSP
SimplexFunctionOptimizer::deep_clone() const {
	SimplexFunctionOptimizerSP new_obj( std::static_pointer_cast< SimplexFunctionOptimizer >( clone() ) );
	new_obj->make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "RealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
SimplexFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "RealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_free", "numeric" }
std::vector< std::string >
SimplexFunctionOptimizer::get_keywords() const {
	return std::vector< std::string > {
		"optimizer",
		"real_valued",
		"local_optimizer",
        "gradient_free",
		"numeric"
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
/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer"} }
std::vector< std::vector < std::string > >
SimplexFunctionOptimizer::get_engine_categories() const {
    return std::vector< std::vector < std::string > >{ { "Optimizer", "RealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer" } };
}

/// @brief Every class can name itself.
/// @returns "SimplexFunctionOptimizer".
std::string
SimplexFunctionOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_free".
std::string
SimplexFunctionOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Every class can name itself.
/// @returns "SimplexFunctionOptimizer".
/*static*/
std::string
SimplexFunctionOptimizer::class_name_static() {
	return "SimplexFunctionOptimizer";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::gradient_free".
/*static*/
std::string
SimplexFunctionOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::gradient_free";
}

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the maximum number of steps that we can take.
/// @details A setting of 0 means loop until convergence.
void
SimplexFunctionOptimizer::set_max_iterations(
	masala::base::Size const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	max_iterations_ = setting;
}

/// @brief Set the initial size of the simplex.  The initial simplex points will be the starting
/// point plus a small step in each of the cardinal directions.
void
SimplexFunctionOptimizer::set_initial_simplex_size(
	masala::base::Real const size_in
) {
	std::lock_guard< std::mutex > lock( mutex() );
	CHECK_OR_THROW_FOR_CLASS( size_in >= std::numeric_limits< masala::base::Real >::epsilon(),
		"set_initial_simplex_size", "The initial simplex size must be greater than "
		+ std::to_string( std::numeric_limits< masala::base::Real >::epsilon() ) + "."
	);
	initial_simplex_size_ = size_in;
}

/// @brief Set the tolerance for determining whether or not we've finished our search.
/// @details The default is the square root of machine precision (the theoretical lower limit for
/// any sensible value of tolerance).
void
SimplexFunctionOptimizer::set_tolerance(
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

/// @brief Set whether we should throw if iterations are exceeded (true), or just warn
/// (false, the default).
void
SimplexFunctionOptimizer::set_throw_if_iterations_exceeded(
	bool const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	throw_if_iterations_exceeded_ = setting;
}

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the maximum number of steps that we can take
/// @details A setting of 0 means loop until convergence.
masala::base::Size
SimplexFunctionOptimizer::max_iterations() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return max_iterations_;
}

/// @brief Get the initial size of the simplex.  The initial simplex points will be the starting
/// point plus a small step in each of the cardinal directions.
masala::base::Real
SimplexFunctionOptimizer::initial_simplex_size() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return initial_simplex_size_;
}

/// @brief Get the tolerance for determining whether or not we've finished our search.
/// @details The default is the square root of machine precision (the theoretical lower limit for
/// any sensible value of tolerance).
masala::base::Real
SimplexFunctionOptimizer::tolerance() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return tolerance_;
}

/// @brief Should we throw if iterations are exceeded (true), or just warn
/// (false, the default)?
bool
SimplexFunctionOptimizer::throw_if_iterations_exceeded() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return throw_if_iterations_exceeded_;
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
SimplexFunctionOptimizer::get_api_definition() {
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
				"The SimplexFunctionOptimizer carries out local minimization of an arbitrary function "
				"without using gradient information.  This is relatively inefficient compared to quasi-Newtonian "
				"methods like DFP, BFGS, or L-BFGS.",
				false, false
			)
		);
		
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( SimplexFunctionOptimizer, api_def );

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_max_iterations", "Set the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations_in", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &SimplexFunctionOptimizer::set_max_iterations, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_initial_simplex_size", "Set the initial size of the simplex.  The initial simplex points will be the starting "
				"point plus a small step in each of the cardinal directions.",
				"size_in", "The small offset to add to each of the coordinates of the initial point to get the initial simplex.",
				false, false, std::bind( &SimplexFunctionOptimizer::set_initial_simplex_size, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_tolerance", "Set the tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of tolerance).",
				"tolerance_in", "The tolerance to set.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::set_tolerance, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< bool > >(
				"set_throw_if_iterations_exceeded", "Set whether we should throw if "
				"iteration maximum is exceeded (true), or just warn (false, the default).",
				"setting", "True if we want to throw if iteration maximum is exceeded, false otherwise.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::set_throw_if_iterations_exceeded, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"max_iterations", "Get the maximum number of steps that we can take.  A setting of 0 means loop until convergence.",
				"max_iterations", "The maximum number of iterations for the quasi-Newton gradient descent search for a local minimum.",
				false, false, std::bind( &SimplexFunctionOptimizer::max_iterations, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"initial_simplex_size", "Get the initial size of the simplex.  The initial simplex points will be the starting "
				"point plus a small step in each of the cardinal directions.",
				"initial_simplex_size", "The small offset that is added to each of the coordinates of the initial point to get the initial simplex.",
				false, false, std::bind( &SimplexFunctionOptimizer::initial_simplex_size, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"tolerance", "Get the tolerance for determining whether or not we've "
				"finished our search.  The default is the square root of machine precision "
				"(the theoretical lower limit for any sensible value of tolerance).",
				"tolerance", "The tolerance for determining whether the search has converged.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::tolerance, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"throw_if_iterations_exceeded", "Get whether we throw if "
				"iteration maximum is exceeded (true), or just warn (false, the default).",
				"throw_if_iterations_exceeded", "True if we throw if iteration maximum is "
				"exceeded, false otherwise.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::throw_if_iterations_exceeded, this )
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
SimplexFunctionOptimizer::run_real_valued_local_optimizer(
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblems_API const & problems
) const {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::numeric_api::auto_generated_api::optimization;
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	using namespace masala::base::managers::threads;

	std::lock_guard< std::mutex > lock( mutex() );

	MasalaThreadedWorkRequest workvec( threads_to_request() );
	std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP > solutions;
	solutions.resize( problems.n_problems() );

	for( Size i(0), imax(problems.n_problems()); i<imax; ++i ) {
		RealValuedFunctionLocalOptimizationProblem_APICSP problem( std::dynamic_pointer_cast< RealValuedFunctionLocalOptimizationProblem_API const >( problems.problem(i) ) );
		CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "run_real_valued_local_optimizer",
			"Problem " + std::to_string(i) + " is of type " + problems.problem(i)->inner_class_name() +
			", which could not be interpreted as a RealValuedFunctionLocalOptimizationProblem."
		);
		CHECK_OR_THROW_FOR_CLASS( problem->has_objective_function(), "run_real_valued_local_optimizer",
			"No objective function was defined for problem " + std::to_string( i ) + "."
		);
		CHECK_OR_THROW_FOR_CLASS( problem->has_at_least_one_starting_point(), "run_real_valued_local_optimizer",
			"No starting point was defined for problem " + std::to_string( i ) + "."
		);
		for( Size j(0), jmax( problem->starting_points().size() ); j<=jmax; ++j ) {
			workvec.add_job(
				std::bind(
					&SimplexFunctionOptimizer::run_one_simplex_optimization_in_threads,
					this,
					i,
					j,
					std::cref( problem->starting_points()[j] ),
					std::cref( problem->objective_function() ),
					std::ref( solutions[i] )
				)
			);
		}
	}

	// Do the work, in threads:
	MasalaThreadedWorkExecutionSummary const thread_summary(
		MasalaThreadManager::get_instance()->do_work_in_threads( workvec )
	);
	thread_summary.write_summary_to_tracer();

	return solutions;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Function that executes in threads to carry out one simplex optimization.
/// @details Should be called from a mutex-locked context.
void
SimplexFunctionOptimizer::run_one_simplex_optimization_in_threads(
	masala::base::Size const problem_index,
	masala::base::Size const problem_starting_point_index,
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & starting_point,
	std::function< masala::base::Real( Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ) > const & objective_function,
	masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolution_APISP & solution
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;
	using masala::base::Size;
	using masala::base::Real;

	// Current simplex:
	Eigen::Matrix< Real, Eigen::Dynamic, Eigen::Dynamic > simplex;
	Size const ndim( starting_point.size() );
	simplex.resize( ndim + 1, ndim );

	// Initialize the simplex:
	for( Size i(0); i<=ndim; ++i ) {
		for( Size j(0); j<ndim; ++j ) {
			simplex[i,j] = starting_point[j] + ( i == j ? initial_simplex_size_ : 0.0 );
		}
	}
	
	// Loop over iterations:
	bool converged( false );
	TODO TODO TODO;

	// Check for convergence:
	TODO TODO TODO;

	// Package solution:
	TODO TODO TODO;
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
SimplexFunctionOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer const & src
) {
	SimplexFunctionOptimizer const * src_ptr_cast( dynamic_cast< SimplexFunctionOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );

	max_iterations_ = src_ptr_cast->max_iterations_;

	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
SimplexFunctionOptimizer::protected_make_independent() {
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer::protected_make_independent();
}

} // namespace gradient_free
} // namespace optimizers
} // namespace standard_masala_plugins
