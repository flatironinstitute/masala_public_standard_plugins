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

/// @file src/numeric_api/base_classes/optimization/gradient_based/QuasiNewtonianFunctionOptimizerBase.hh
/// @brief Headers for the QuasiNewtonianFunctionOptimizerBase base class.
/// @details The QuasiNewtonianFunctionOptimizerBase class serves as a base class for optimizers that carry out gradient-
/// descent minimization of functions using quasi-Newtonian approaches, such as the DFP, BFGS, and L-BFGS algorithms.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_based_QuasiNewtonianFunctionOptimizerBase_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_based_QuasiNewtonianFunctionOptimizerBase_hh

// Forward declarations:
#include <optimizers/gradient_based/QuasiNewtonianFunctionOptimizerBase.fwd.hh>

// Base headers:
#include <base/managers/engine/MasalaEngineAPI.fwd.hh>
#include <base/types.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.fwd.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/PluginLineOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/real_valued_local/PluginRealValuedFunctionLocalOptimizer.hh>

// External headers:
#include <external/eigen/Eigen/Core>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

/// @brief The QuasiNewtonianFunctionOptimizerBase base class.
/// @details The QuasiNewtonianFunctionOptimizerBase class serves as a base class for optimizers that carry out gradient-
/// descent minimization of functions using quasi-Newtonian approaches, such as the DFP, BFGS, and L-BFGS algorithms.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class QuasiNewtonianFunctionOptimizerBase : public masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	QuasiNewtonianFunctionOptimizerBase() = default;

	/// @brief Copy constructor.
	QuasiNewtonianFunctionOptimizerBase( QuasiNewtonianFunctionOptimizerBase const & ) = default;

	/// @brief Destructor.
	~QuasiNewtonianFunctionOptimizerBase() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the
	/// copy.  Contained objects may still be shared.
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizerSP
	clone() const override;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	QuasiNewtonianFunctionOptimizerBaseSP
	deep_clone() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
	/// may be overridden by derived classes.
	/// @returns { { "Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi_newtonian" }
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
	/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer"} }
    std::vector< std::vector < std::string > >
    get_engine_categories() const override;

	/// @brief Every class can name itself.
	/// @returns "QuasiNewtonianFunctionOptimizerBase".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	std::string class_namespace() const override;

	/// @brief Every class can name itself.
	/// @returns "QuasiNewtonianFunctionOptimizerBase".
	static std::string class_name_static();

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the maximum number of steps that we can take.
	/// @details A setting of 0 means loop until convergence.
	void set_max_iterations( masala::base::Size const setting );

	/// @brief Set a line optimizer to use for the line searches.
	/// @details Used directly, not cloned.  If none is provided (or if this is set to
	/// nullptr), then a BrentAlgorithmLineOptimizer is used by default.
	void
	set_line_optimizer(
		masala::base::managers::engine::MasalaEngineAPICSP line_optimizer_in
	);

	/// @brief Set the tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	void set_tolerance( masala::base::Real const setting );

	/// @brief Set the gradient tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of gradient tolerance).
	void set_gradient_tolerance( masala::base::Real const setting );

	/// @brief Set whether we should throw if iterations are exceeded (true), or just warn
	/// (false, the default).
	void set_throw_if_iterations_exceeded( bool const setting );

	/// @brief Set the minimum absolute value of the determinant of the approximate inverse Hessian matrix, below which we reset the Hessian
	/// to the identity matrix.
	/// @details Quasi-Newtonian methods fail if the Hessian becomes singular.  The default value is 4 times machine precision, and rarely
	/// needs to be adjusted.
	void set_min_inv_hessian_determinant( masala::base::Real const setting );

public:

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the maximum number of steps that we can take
	/// @details A setting of 0 means loop until convergence.
	masala::base::Size max_iterations() const;

	/// @brief Get the line optimizer used for the line searches.
	/// @details Could be nullptr, in which case a BrentAlgorithmLineOptimizer
	/// is used by default.
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerCSP
	line_optimizer() const;

	/// @brief Get the tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real tolerance() const;

	/// @brief Get the gradient tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of gradient tolerance).
	masala::base::Real gradient_tolerance() const;

	/// @brief Should we throw if iterations are exceeded (true), or just warn
	/// (false, the default)?
	bool throw_if_iterations_exceeded() const;

	/// @brief Get the minimum absolute value of the determinant of the approximate inverse Hessian matrix, below which we reset the Hessian
	/// to the identity matrix.
	/// @details Quasi-Newtonian methods fail if the Hessian becomes singular.  The default value is 4 times machine precision, and rarely
	/// needs to be adjusted.
	masala::base::Real min_inv_hessian_determinant() const;

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
		masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizer const & src
	) override;

	/// @brief Make independent: must be implemented by derived classes, which must call the base
	/// class protected_make_independent().
	/// @details Performs no mutex locking.
	void
	protected_make_independent() override;

	/// @brief Allow derived classes to access the minimum determinant of the inverse Hessian.
	inline
	masala::base::Real
	protected_min_inv_hessian_determinant() const {
		return min_inv_hessian_determinant_;
	}

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

	/// @brief Run the optimizer on a single gradient-based loss function minimization problem, and produce a single solution.
	/// @details This function executes in threads.  Expected to be called from a mutex-locked context.
	void
	run_one_job_in_threads(
		masala::base::Size const job_index,
		masala::base::Size const problem_index,
		masala::base::Size const start_index,
		masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblem_APICSP const & problem,
		masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerCSP line_optimizer, // Deliberately passed by shared pointer copy.
		masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolution_APISP & solution
	) const;

	/// @brief Update the approximation of the inverse of the Hessian matrix.
	/// @details The update rule differs between the DFP, BFGS, and L-BFGS algorithms.
	/// @note Expected to be called from a mutex-locked context.  Must be implemented by derived classes.
	virtual
	void
	update_inverse_hessian(
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & p_diff,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & grad_diff,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > & scratchvec1,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > & scratchvec2,
		Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > & inv_hessian
	) const;

	/// @brief Determine whether the search has converged, based on the change in coordinates.
	/// @return True for convergence, false otherwise.
	/// @note Static function.
	static
	bool
	search_converged(
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & delta_p,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & p_new,
		masala::base::Real const tolerance
	);

	/// @brief Determine whether the search has converged, based on the change in gradient.
	/// @return True for convergence, false otherwise.
	/// @note Static function.
	static
	bool
	gradient_converged(
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & p_new,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & grad_new,
		masala::base::Real const grad_tolerance,
		masala::base::Real const curscore
	);

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The maximum number of steps that we're allowed to take.
	/// @details A setting of 0 means loop until convergence.
	masala::base::Size max_iterations_ = 2000;

	/// @brief A line optimizer used for the line searches.
	/// @details If none is provided, a BrentAlgorithmLineOptimizer is used.
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerCSP line_optimizer_;

	/// @brief The tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real tolerance_ = std::sqrt( std::numeric_limits< masala::base::Real >::epsilon() );

	/// @brief The gradient tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real gradient_tolerance_ = std::sqrt( std::numeric_limits< masala::base::Real >::epsilon() );

	/// @brief Should we throw if iterations are exceeded (true), or just warn
	/// (false, the default)?
	bool throw_if_iterations_exceeded_ = false;

	/// @brief The minimum absolute value of the determinant of the approximate inverse Hessian matrix, below which we reset the Hessian
	/// to the identity matrix.
	/// @details Quasi-Newtonian methods fail if the Hessian becomes singular.  The default value is 4 times machine precision, and rarely
	/// needs to be adjusted.
	masala::base::Real min_inv_hessian_determinant_ = 4.0 * std::numeric_limits< masala::base::Real >::epsilon();

}; // class QuasiNewtonianFunctionOptimizerBase

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_QuasiNewtonianFunctionOptimizerBase_hh