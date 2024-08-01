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

/// @file src/numeric_api/base_classes/optimization/gradient_free/SimplexFunctionOptimizer.hh
/// @brief Headers for the SimplexFunctionOptimizer.
/// @details The SimplexFunctionOptimizer carries out local minimization of an arbitrary function
/// without using gradient information.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_free_SimplexFunctionOptimizer_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_free_SimplexFunctionOptimizer_hh

// Forward declarations:
#include <optimizers/gradient_free/SimplexFunctionOptimizer.fwd.hh>

// External headers:
#include <external/eigen/Eigen/Core>

// Base headers:
#include <base/types.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolution_API.fwd.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/PluginLineOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/real_valued_local/RealValuedFunctionLocalOptimizer.hh>

// STL headers:
#include <limits>
#include <cmath>
#include <functional>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_free {

/// @brief The SimplexFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using an iterative approach of computing gradients and carrying out
/// line searches with a line search algorithm.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class SimplexFunctionOptimizer : public masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	SimplexFunctionOptimizer() = default;

	/// @brief Copy constructor.
	SimplexFunctionOptimizer( SimplexFunctionOptimizer const & ) = default;

	/// @brief Destructor.
	~SimplexFunctionOptimizer() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the
	/// copy.  Contained objects may still be shared.
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizerSP
	clone() const override;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	SimplexFunctionOptimizerSP
	deep_clone() const;

public:

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
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_free", "numeric" }
	std::vector< std::string >
	get_keywords() const override;

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
    get_engine_categories() const override;

	/// @brief Every class can name itself.
	/// @returns "SimplexFunctionOptimizer".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_free".
	std::string class_namespace() const override;

	/// @brief Every class can name itself.
	/// @returns "SimplexFunctionOptimizer".
	static std::string class_name_static();

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_free".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the maximum number of steps that we can take.
	/// @details A setting of 0 means loop until convergence.
	void
	set_max_iterations(
		masala::base::Size const setting
	);

	/// @brief Set the initial size of the simplex.  The initial simplex points will be the starting
	/// point plus a small step in each of the cardinal directions.
	void
	set_initial_simplex_size(
		masala::base::Real const size_in
	);

	/// @brief Set the tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	void set_tolerance( masala::base::Real const setting );

	/// @brief Set whether we should throw if iterations are exceeded (true), or just warn
	/// (false, the default).
	void set_throw_if_iterations_exceeded( bool const setting );

	/// @brief Set the amount by which to expand, when expanding the simplex.  Must be greater than 1.
	void set_expansion_factor( masala::base::Real const setting );

	/// @brief Set the amount by which to contract, when contracting the simplex.  Must be between 0 and 1.
	void set_contraction_factor( masala::base::Real const setting );

public:

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the maximum number of steps that we can take
	/// @details A setting of 0 means loop until convergence.
	masala::base::Size max_iterations() const;

	/// @brief Get the initial size of the simplex.  The initial simplex points will be the starting
	/// point plus a small step in each of the cardinal directions.
	masala::base::Real
	initial_simplex_size() const;

	/// @brief Get the tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real tolerance() const;

	/// @brief Should we throw if iterations are exceeded (true), or just warn
	/// (false, the default)?
	bool throw_if_iterations_exceeded() const;

	/// @brief Get the amount by which to expand, when expanding the simplex.  Must be greater than 1.
	masala::base::Real expansion_factor() const;

	/// @brief Get the amount by which to contract, when contracting the simplex.  Must be between 0 and 1.
	masala::base::Real contraction_factor() const;

public:

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
	get_api_definition() override;

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Assignment: must be implemented by derived classes, which must call the base
	/// class protected_assign().
	/// @details Performs no mutex locking.
	void
	protected_assign(
		masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer const & src
	) override;

	/// @brief Make independent: must be implemented by derived classes, which must call the base
	/// class protected_make_independent().
	/// @details Performs no mutex locking.
	void
	protected_make_independent() override;

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Run the optimizer on a set of gradient-based loss function minimization problems, and produce a set of solutions.
	/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
	/// the problem with the same index.  This version uses the low-memory Broyden–Fletcher–Goldfarb–Shanno algorithm (BFGS)
	/// to carry out gradient-descent minimization.
	std::vector< masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolutions_APICSP >
	run_real_valued_local_optimizer(
		masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblems_API const & problems
	) const override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Function that executes in threads to carry out one simplex optimization.
	/// @details Should be called from a mutex-locked context.
	void
	run_one_simplex_optimization_in_threads(
		masala::base::Size const problem_index,
		masala::base::Size const problem_starting_point_index,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & starting_point,
		std::function< masala::base::Real( Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ) > const & objective_function,
		masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolution_APISP & solution
	) const;

	/// @brief Reflect one vertex across the centroid of the other vertices, scaling by a given factor.
	/// Re-evaluate the scoring function at the new position.
	/// @details Assumes that vertex scores is the same size as the number of rows in the simplex.  Updates
	/// other centroid only if it needs recomputation.  Rescale factor should be negative to flip about centroid,
	/// positive just to scale.
	static
	void
	reflect_vertex(
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > & other_centroid,
		bool const other_centroid_needs_update,
		Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > & simplex,
		masala::base::Size const simplex_vertex_index_to_move,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > & vertex_scores,
		std::function< masala::base::Real( Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & ) > const & objective_function,
		masala::base::Real const rescale_factor
	);

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The maximum number of steps that we're allowed to take.
	/// @details A setting of 0 means loop until convergence.
	masala::base::Size max_iterations_ = 2000;

	/// @brief The initial size of the simplex:
	masala::base::Real initial_simplex_size_ = 0.001;

	/// @brief The tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real tolerance_ = std::sqrt( std::numeric_limits< masala::base::Real >::epsilon() );

	/// @brief Should we throw if iterations are exceeded (true), or just warn
	/// (false, the default)?
	bool throw_if_iterations_exceeded_ = false;

	/// @brief The amount by which to expand, when expanding the simplex.  Must be greater than 1.
	masala::base::Real expansion_factor_ = 2.0;

	/// @brief The amount by which to contract, when contracting the simplex.  Must be between 0 and 1.
	masala::base::Real contraction_factor_ = 0.5;

}; // class SimplexFunctionOptimizer

} // namespace gradient_free
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_free_SimplexFunctionOptimizer_hh