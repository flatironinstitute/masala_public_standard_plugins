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
#include <base/utility/container/container_util.tmpl.hh> // COMMENT ME OUT.  FOR DEBUGGING ONLY.

// Optimizers headers:

// STL headers:
#include <vector>
#include <string>
// #include <iostream> // COMMENT ME OUT.  FOR DEBUGGING ONLY.

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_free {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Clone operation: copy this object and return a shared pointer to the
/// copy.  Contained objects may still be shared.
masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizerSP
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
/// @returns { { "Optimizer", "PluginRealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
SimplexFunctionOptimizer::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "Optimizer", "PluginRealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer" }
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
/// @returns { {"Optimizer", "PluginRealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer"} }
std::vector< std::vector < std::string > >
SimplexFunctionOptimizer::get_engine_categories() const {
	return std::vector< std::vector < std::string > >{ { "Optimizer", "PluginRealValuedFunctionLocalOptimizer", "SimplexFunctionOptimizer" } };
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

/// @brief Set the number of times that we should restart the simplex search to avoid
/// false convergence.  Defaults to 3.
void
SimplexFunctionOptimizer::set_outer_iterations(
	masala::base::Size const n_outer_iterations_in
) {
	std::lock_guard< std::mutex > lock( mutex() );
	CHECK_OR_THROW_FOR_CLASS( n_outer_iterations_in > 0, "set_outer_iterations", "The number of outer iterations must be greater than 0." );
	outer_iterations_ = n_outer_iterations_in;
}

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

/// @brief Set the amount by which to expand, when expanding the simplex.  Must be greater than 1.
void
SimplexFunctionOptimizer::set_expansion_factor(
	masala::base::Real const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	CHECK_OR_THROW_FOR_CLASS( setting > 1.0, "set_expansion_factor", "The expansion "
		"factor must be greater than 1.  Got " + std::to_string(setting) + "."
	);
	expansion_factor_ = setting;
}

/// @brief Set the amount by which to contract, when contracting the simplex.  Must be between 0 and 1.
void
SimplexFunctionOptimizer::set_contraction_factor(
	masala::base::Real const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	CHECK_OR_THROW_FOR_CLASS( setting > 0.0 && setting < 1.0,
		"set_contraction_factor", "The contraction factor must be greater than 0 and less than 1.  "
		"Got " + std::to_string(setting) + "."
	);
	contraction_factor_ = setting;
}


/// @brief Set the amount by which to shrink, when shrinking the simplex about the best vertex.  Must
/// be between 0 and 1.
void
SimplexFunctionOptimizer::set_shrink_factor(
	masala::base::Real const setting
) {
	std::lock_guard< std::mutex > lock( mutex() );
	CHECK_OR_THROW_FOR_CLASS( setting > 0.0 && setting < 1.0,
		"set_shrink_factor", "The shrink factor must be greater than 0 and less than 1.  "
		"Got " + std::to_string(setting) + "."
	);
	shrink_factor_ = setting;
}

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the number of times that we should restart the simplex search to avoid
/// false convergence.  Defaults to 3.
masala::base::Size
SimplexFunctionOptimizer::outer_iterations() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return outer_iterations_;
}

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

/// @brief Get the amount by which to expand, when expanding the simplex.  Must be greater than 1.
masala::base::Real
SimplexFunctionOptimizer::expansion_factor() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return expansion_factor_;
}

/// @brief Get the amount by which to contract, when contracting the simplex.  Must be between 0 and 1.
masala::base::Real
SimplexFunctionOptimizer::contraction_factor() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return contraction_factor_;
}

/// @brief Get the amount by which to shrink, when shrinking the simplex about the best vertex.  Must
/// be between 0 and 1.
masala::base::Real
SimplexFunctionOptimizer::shrink_factor() const {
	std::lock_guard< std::mutex > lock( mutex() );
	return shrink_factor_;
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
	using namespace masala::base::api::work_function;
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;
	using namespace masala::numeric_api::auto_generated_api::optimization;
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
				"set_threads_to_request", "Set the number of threads requested by this optimizer.  The actual number "
				"may be smaller if there is less work to do, or if there are fewer threads available.  Note that the "
				"only thing parallelized are different problems or different starting points of the same problem.  Within "
				"a given trajectory, there is no parallelism.",
				"setting", "The number of threads to request.  Different starting points of the same problem or "
				"different starting points of different problems can be carried out simultaneously in threads.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::set_threads_to_request, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_outer_iterations", "Set the number of times that we should restart the simplex search to avoid false convergence.  Defaults to 3.",
				"n_outer_iterations_in", "The number of times that we should restart the simplex search to avoid false convergence.",
				false, false, std::bind( &SimplexFunctionOptimizer::set_outer_iterations, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_max_iterations", "Set the maximum number of inner steps that we can take (i.e. the maximum number of objective function evaluations).  A setting of 0 means loop until convergence.",
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
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_expansion_factor", "Set the amount by which to expand, "
				"when expanding the simplex.  Must be greater than 1.  Defaults to 2.0.",
				"setting", "The amount by which to expand, when expanding the simplex.  "
				"Must be greater than 1.  Defaults to 2.0.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::set_expansion_factor, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_contraction_factor", "Set the amount by which to contract, "
				"when contracting the simplex.  Must be between 0 and 1.  Defaults to 0.5.",
				"setting", "The amount by which to contract, when contracting the simplex.  "
				"Must be between 0 and 1.  Defaults to 0.5.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::set_contraction_factor, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_shrink_factor", "Set the amount by which to shrink, when shrinking the simplex about "
				"the best vertex.  Must be between 0 and 1.  Must be between 0 and 1.  Defaults to 0.5.",
				"setting", "The amount by which to shrink, when contracting the simplex about the best vertex.  "
				"Must be between 0 and 1.  Defaults to 0.5.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::set_shrink_factor, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"threads_to_request", "Get the number of threads requested by this optimizer.  Different starting "
				"points of the same problem or different starting points of different problems can be carried out "
				"simultaneously in threads.",
				"threads_to_request", "The number of threads requested by this optimizer.  The actual number "
				"may be smaller if there is less work to do, or if there are fewer threads available.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::threads_to_request, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"outer_iterations", "Get the number of times that we should restart the simplex search to avoid false convergence.  Defaults to 3.",
				"outer_iterations", "The number of times that we should restart the simplex search to avoid false convergence.",
				false, false, std::bind( &SimplexFunctionOptimizer::outer_iterations, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"max_iterations", "Get the maximum number of inner steps that we can take (i.e. the maximum number of objective function evaluations).  A setting of 0 means loop until convergence.",
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
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"expansion_factor", "Get the amount by which to expand, "
				"when expanding the simplex.",
				"expansion_factor", "The amount by which to expand, when expanding the simplex.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::expansion_factor, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"contraction_factor", "Get the amount by which to contract, "
				"when contracting the simplex.",
				"setting", "The amount by which to contract, when contracting the simplex.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::contraction_factor, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
				"shrink_factor", "Get the amount by which to shrink, "
				"when shrinking the simplex about the best vertex.",
				"setting", "The amount by which to shrink, when shrinking the simplex about the best vertex.",
				false, false,
				std::bind( &SimplexFunctionOptimizer::shrink_factor, this )
			)
		);

		// Work functions
		api_def->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP >, RealValuedFunctionLocalOptimizationProblems_API const & > >(
				"run_real_valued_local_optimizer", "Run the optimizer on a set of loss function "
				"local minimization problems, and produce a set of solutions.",
				true, false, true, false,
				"problems", "A set of local optimization problems to solve.  Each must implement a loss function "
				"and provide at least one starting point.  Gradients are not needed.",
				"solutions_vector", "A vector of solutions objects.  Each solutions set in the vector "
				"of solutions corresponds to the problem with the same index.  The various solutions in the "
				"set come from different starting points defined in the problem.",
				std::bind( &SimplexFunctionOptimizer::run_real_valued_local_optimizer, this, std::placeholders::_1 )
			)
		);
		api_def->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< std::vector< OptimizationSolutions_APICSP >, OptimizationProblems_API const & > >(
				"run_optimizer", "Run the optimizer on a set of loss function local minimization problems, "
				"and produce a set of solutions.",
				true, false, true, false,
				"problems", "A set of local optimization problems to solve.  Each must implement a loss function "
				"and provide at least one starting point.  Gradients are not needed.",
				"solutions_vector", "A vector of solutions objects.  Each solutions set in the vector "
				"of solutions corresponds to the problem with the same index.  The various solutions in the "
				"set come from different starting points defined in the problem.",
				std::bind( &SimplexFunctionOptimizer::run_optimizer, this, std::placeholders::_1 )
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

	MasalaThreadedWorkRequest workvec( protected_threads_to_request() );
	std::vector< std::vector< RealValuedFunctionLocalOptimizationSolution_APISP > > solutions( problems.n_problems() );

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
		Size const nstartingpoints( problem->starting_points().size() );
		solutions[i].resize( nstartingpoints );
		for( Size j(0); j<nstartingpoints; ++j ) {
			solutions[i][j] = masala::make_shared< RealValuedFunctionLocalOptimizationSolution_API >();
			solutions[i][j]->set_problem(problem);
			solutions[i][j]->set_starting_point_and_index( problem->starting_points()[j], j );
			workvec.add_job(
				std::bind(
					&SimplexFunctionOptimizer::run_one_simplex_optimization_in_threads,
					this,
					i,
					j,
					std::cref( problem->starting_points()[j] ),
					std::cref( problem->objective_function() ),
					std::ref( solutions[i][j] )
				)
			);
		}
	}

	// Do the work, in threads:
	MasalaThreadedWorkExecutionSummary const thread_summary(
		MasalaThreadManager::get_instance()->do_work_in_threads( workvec )
	);
	thread_summary.write_summary_to_tracer();

	// Repackage solutions:
	std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP > outsolsvec(problems.n_problems());
	for( Size i(0); i<problems.n_problems(); ++i ) {
		RealValuedFunctionLocalOptimizationSolutions_APISP sols( masala::make_shared< RealValuedFunctionLocalOptimizationSolutions_API >() );
		for( Size j(0); j<solutions[i].size(); ++j ) {
			sols->add_optimization_solution(solutions[i][j]);
		}
		outsolsvec[i] = sols; // Nonconst to const.
	}

	return outsolsvec;
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
	CHECK_OR_THROW_FOR_CLASS( ndim > 1, "run_one_simplex_optimization_in_threads",
		"The " + class_name() + " requires at least a 2-dimensional search space."
	);
	simplex.resize( ndim + 1, ndim );
	Eigen::Vector< Real, Eigen::Dynamic > simplex_scores;
	simplex_scores.resize( ndim + 1 );
	Size worst_index, second_worst_index, best_index, new_best_index, old_worst_index;
	Eigen::Vector< Real, Eigen::Dynamic > trial_point, old_worst_point, other_centroid;
	trial_point.resize( ndim );
	old_worst_point.resize( ndim );
	other_centroid.resize( ndim );
	Real old_worst_score, trial_score, maxscore, minscore;
	Real const tinyval( std::numeric_limits< masala::base::Real >::epsilon() );

	// Initialize the simplex:
	for( Size i(0); i<=ndim; ++i ) {
		for( Size j(0); j<ndim; ++j ) {
			simplex(i,j) = starting_point(j) + ( i == j ? initial_simplex_size_ : 0.0 );
		}
	}

	// Storage for best, worst, and second-worst indices:
	worst_index = 0; second_worst_index = 0; best_index = 0;
	
	// Loop over iterations:
	bool converged( false );
	Size iter_count(0);
	// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} ******** BEGIN ******** " << std::endl;
	for( Size outer_iter( 0 ); outer_iter < outer_iterations_; ++outer_iter ) {
		// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} START OUTER ITER " << outer_iter << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.

		// Reset the simplex for subsequent outer iterations:
		if( outer_iter > 0 ) {
			for( Size i(0); i<=ndim; ++i ) {
				if( i == best_index ) { continue; }
				for( Size j(0); j<ndim; ++j ) {
					simplex(i,j) = simplex(best_index,j) + ( i == j ? initial_simplex_size_ : 0.0 );
				}
			}
		}

		// Find best, worst, and second-worst indices:
		for( masala::base::Size i(0); i<=ndim; ++i ) {
			if( outer_iter > 0 && i == best_index ) { continue; }
			++iter_count;
			if( iter_count > max_iterations_ ) {
				// Leave converged at last setting.
				break;
			}
			simplex_scores(i) = objective_function( simplex.row(i).transpose() );
		}
		best_index = 0;
		worst_index = 0;
		second_worst_index = 0;
		for( Size j(1); j<=ndim; ++j ) {
			if( simplex_scores(j) < simplex_scores(best_index) ) {
				best_index = j;
			}
			if( simplex_scores(j) > simplex_scores(worst_index) ) {
				worst_index = j;
			} else if( simplex_scores(j) > simplex_scores(second_worst_index) && j != worst_index ) {
				second_worst_index = j;
			}
		}

		while( true ) {

			// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} START LOOP AT INNER ITER " << iter_count << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.
			// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} " << simplex << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.

			// Compute relative tolerance and decide whether to exit:
			maxscore = simplex_scores.maxCoeff();
			minscore = simplex_scores.minCoeff();
			if( 2.0*std::abs( maxscore - minscore ) / (std::abs( maxscore ) + std::abs( minscore ) + tinyval ) < tolerance_ ) {
				converged = true;
				break;
			}

			// Decide whether to exit due to max iterations:
			if( iter_count > max_iterations_ ) {
				converged = false;
				break;
			}

			// Store old worst:
			old_worst_index = worst_index;
			old_worst_score = simplex_scores(worst_index);
			old_worst_point = simplex.row( old_worst_index );

			// Reflect worst across other points:
			reflect_vertex( other_centroid, true, simplex, old_worst_index, simplex_scores, objective_function, -1.0 );
			// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} REFLECT " << simplex << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.
			++iter_count;
			if( simplex_scores(old_worst_index) < simplex_scores(second_worst_index) && simplex_scores(best_index) < simplex_scores(old_worst_index) ) {
				worst_index = second_worst_index;
				second_worst_index = find_second_worst_index( best_index, worst_index, simplex_scores );
				continue;
			}
			if( iter_count > max_iterations_ ) {
				continue;
			}

			// If now best, expand:
			if( simplex_scores(old_worst_index) < simplex_scores(best_index) ) {
				trial_score = simplex_scores(old_worst_index);
				trial_point = simplex.row( old_worst_index );
				simplex.row(old_worst_index) = old_worst_point;
				reflect_vertex( other_centroid, false, simplex, old_worst_index, simplex_scores, objective_function, -1.0*expansion_factor_ );
				// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} EXPAND " << simplex << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.
				++iter_count;
				if( simplex_scores(old_worst_index) >= trial_score ) {
					simplex.row( old_worst_index ) = trial_point;
					simplex_scores(old_worst_index) = trial_score;
				}
				best_index = old_worst_index;
				worst_index = second_worst_index;
				second_worst_index = find_second_worst_index( best_index, worst_index, simplex_scores );
				continue;
			}

			// If we reach here, we know that the reflected point is worse than the
			// second-worst (i.e. is still worst).
			// If worse than second-worst but better than old worst, contract on the
			// outside; otherwise, if worse than old worst, contract on inside:
			bool const on_inside( simplex_scores(old_worst_index) >= old_worst_score ? true : false );
			if( on_inside ) {
				trial_score = old_worst_score;
				simplex.row(old_worst_index) = old_worst_point;
			} else {
				trial_score = simplex_scores( old_worst_index );
			}
			reflect_vertex( other_centroid, false, simplex, old_worst_index, simplex_scores, objective_function, contraction_factor_ );
			// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} CONTRACT " << simplex << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.
			++iter_count;
			if( simplex_scores(old_worst_index) < trial_score ) {
				continue;
			}
			simplex.row( old_worst_index ) = old_worst_point;
			simplex_scores(old_worst_index) = old_worst_score;
			if( iter_count > max_iterations_ ) {
				continue;
			}

			// If not better than old worst, reset and contract about best point:
			new_best_index = 0; worst_index = 0; second_worst_index = 0;
			for( Size i(0); i<=ndim; ++i ) {
				if( i == best_index ) { continue; }
				simplex.row(i) = shrink_factor_ * (simplex.row(i) - simplex.row(best_index)) + simplex.row(best_index);
				simplex_scores(i) = objective_function( simplex.row(i).transpose() );
				++iter_count;
				if( iter_count > max_iterations_ ) {
					continue;
				}
				if( i > 0 ) {
					if( simplex_scores(i) < simplex_scores(new_best_index) ) { new_best_index = i; }
					if( simplex_scores(i) > simplex_scores(worst_index) ) { worst_index = i; }
					else if( i != worst_index && simplex_scores(i) > simplex_scores(second_worst_index) ) { second_worst_index = i; }
				}
			}
			best_index = new_best_index;
			// std::cout << "{" << masala::base::managers::threads::MasalaThreadManager::get_instance()->get_thread_manager_thread_id() << "} SHRINK " << simplex << std::endl; // COMMENT ME OUT.  FOR DEBUGGING ONLY.
		}
	}

	// Check for convergence:
	if( !converged ) {
		if( throw_if_iterations_exceeded_ ) {
			MASALA_THROW(
				class_namespace_static() + "::" + class_name_static(),
				"run_one_simplex_optimization_in_threads",
				"Optimization inner iterations for the " + class_name_static() + " exceeded for problem " +
				std::to_string( problem_index ) + ", starting point " + std::to_string( problem_starting_point_index ) + "!"
			);
		} else {
			write_to_tracer( "Warning: optimization inner iterations for the " + class_name_static() + " exceeded for problem " +
				std::to_string( problem_index ) + ", starting point " + std::to_string( problem_starting_point_index ) + "!"
			);
		}
	}

	// Package solution:
	solution->set_converged( converged );
	solution->set_iterations( iter_count );
	solution->set_n_times_solution_was_produced(1);
	solution->set_solution_point( simplex.row( best_index ).transpose() );
	solution->set_solution_score( simplex_scores(best_index) );
	solution->set_solution_score_data_representation_approximation( simplex_scores(best_index) );
	solution->set_solution_score_solver_approximation( simplex_scores(best_index) );
}

/// @brief Find the second-worst entry in a vector, given the positions of the best and worst.
/*static*/
masala::base::Real
SimplexFunctionOptimizer::find_second_worst_index(
	masala::base::Size const best_index,
	masala::base::Size const worst_index,
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & simplex_scores
) {
	using masala::base::Real;
	using masala::base::Size;

	Real worst_val_seen( simplex_scores(best_index) );
	Size worst_position_seen( best_index );

	for( Size i(0), ndim(simplex_scores.size()); i<ndim; ++i ) {
		if( i == best_index || i == worst_index ) continue;
		if( simplex_scores(i) > worst_val_seen ) {
			worst_val_seen = simplex_scores(i);
			worst_position_seen = i;
		}
	}

	return worst_position_seen; // Since we're skipping the worst, this will be second-worst.
}

/// @brief Reflect one vertex across the centroid of the other vertices, scaling by a given factor.
/// Re-evaluate the scoring function at the new position.
/// @details Assumes that vertex scores is the same size as the number of rows in the simplex.  Updates
/// other centroid only if it needs recomputation.  Rescale factor should be negative to flip about centroid,
/// positive just to scale.
/*static*/
void
SimplexFunctionOptimizer::reflect_vertex(
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > & other_centroid,
	bool const other_centroid_needs_update,
	Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > & simplex,
	masala::base::Size const simplex_vertex_index_to_move,
	Eigen::Vector< masala::base::Real, Eigen::Dynamic > & vertex_scores,
	std::function< masala::base::Real( Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ) > const & objective_function,
	masala::base::Real const rescale_factor
) {
	using masala::base::Size;
	using masala::base::Real;

	Size const ndim( vertex_scores.size() - 1 );
	DEBUG_MODE_CHECK_OR_THROW( static_cast< Size >( simplex.rows() ) == ndim + 1
		&& static_cast< Size >( simplex.cols() ) == ndim,
		class_namespace_static() + "::" + class_name_static(),
		"reflect_vertex",
		"Expected matrix to be " + std::to_string( ndim + 1 ) + " by " +
		std::to_string( ndim ) + ", but got " + std::to_string( simplex.rows() ) + " by "
		+ std::to_string( simplex.cols() ) + "."
	);

	// Update centroid of other vertices if needed:
	if( other_centroid_needs_update ) {
		for( Size i(0); i<static_cast<Size>(other_centroid.size()); ++i ) { other_centroid(i) = 0.0; }
		for( Size i(0); i<ndim+1; ++i ) {
			if( i == simplex_vertex_index_to_move ) { continue; }
			for( Size j(0); j<ndim; ++j ) {
				other_centroid(j) += simplex(i,j);
			}
		}
		other_centroid /= ndim;
	}

	simplex.row( simplex_vertex_index_to_move ) = rescale_factor * ( simplex.row( simplex_vertex_index_to_move ) - other_centroid.transpose() ) + other_centroid.transpose();
	vertex_scores( simplex_vertex_index_to_move ) = objective_function( simplex.row( simplex_vertex_index_to_move ).transpose() );
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assignment: must be implemented by derived classes, which must call the base
/// class protected_assign().
/// @details Performs no mutex locking.
void
SimplexFunctionOptimizer::protected_assign(
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer const & src
) {
	SimplexFunctionOptimizer const * src_ptr_cast( dynamic_cast< SimplexFunctionOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_ptr_cast != nullptr, "protected_assign", "Cannot assign an object of type " + src.class_name() + " to an object of type " + class_name() + "." );

	max_iterations_ = src_ptr_cast->max_iterations_;

	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer::protected_assign( src );
}

/// @brief Make independent: must be implemented by derived classes, which must call the base
/// class protected_make_independent().
/// @details Performs no mutex locking.
void
SimplexFunctionOptimizer::protected_make_independent() {
	using namespace masala::numeric_api::base_classes::optimization::real_valued_local;
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer::protected_make_independent();
}

} // namespace gradient_free
} // namespace optimizers
} // namespace standard_masala_plugins
