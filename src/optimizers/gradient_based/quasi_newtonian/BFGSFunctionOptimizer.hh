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

/// @file src/numeric_api/base_classes/optimization/gradient_based/BFGSFunctionOptimizer.hh
/// @brief Headers for the BFGSFunctionOptimizer.
/// @details The BFGSFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using the quasi-Newtonian Broyden–Fletcher–Goldfarb–Shanno
/// algorithm.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_based_quasi_newtonian_BFGSFunctionOptimizer_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_based_quasi_newtonian_BFGSFunctionOptimizer_hh

// Forward declarations:
#include <optimizers/gradient_based/quasi_newtonian/BFGSFunctionOptimizer.fwd.hh>

// Base headers:
#include <base/managers/engine/MasalaEngineAPI.fwd.hh>
#include <base/types.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.fwd.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/PluginLineOptimizer.fwd.hh>

// Parent header:
#include <optimizers/gradient_based/QuasiNewtonianFunctionOptimizerBase.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {
namespace quasi_newtonian {

/// @brief The BFGSFunctionOptimizer, which carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using the quasi-Newtonian Broyden–Fletcher–Goldfarb–Shanno
/// algorithm.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class BFGSFunctionOptimizer : public standard_masala_plugins::optimizers::gradient_based::QuasiNewtonianFunctionOptimizerBase {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	BFGSFunctionOptimizer() = default;

	/// @brief Copy constructor.
	BFGSFunctionOptimizer( BFGSFunctionOptimizer const & ) = default;

	/// @brief Destructor.
	~BFGSFunctionOptimizer() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the
	/// copy.  Contained objects may still be shared.
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginRealValuedFunctionLocalOptimizerSP
	clone() const override;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	BFGSFunctionOptimizerSP
	deep_clone() const;

public:

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
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi-newtonian", "bfgs" }
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
	/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "QuasiNewtonianFunctionOptimizer", "BFGSFunctionOptimizer"} }
    std::vector< std::vector < std::string > >
    get_engine_categories() const override;

	/// @brief Every class can name itself.
	/// @returns "BFGSFunctionOptimizer".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based::quasi_newtonian".
	std::string class_namespace() const override;

	/// @brief Every class can name itself.
	/// @returns "BFGSFunctionOptimizer".
	static std::string class_name_static();

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based::quasi_newtonian".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// SETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

public:

////////////////////////////////////////////////////////////////////////////////
// GETTER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

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

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Update the approximation of the inverse of the Hessian matrix.
	/// @details The update rule differs between the DFP, BFGS, and L-BFGS algorithms.
	/// @note Expected to be called from a mutex-locked context.  Must be implemented for derived classes.
	inline
	void
	update_inverse_hessian(
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & p_diff,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > const & grad_diff,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > & scratchvec1,
		Eigen::Vector< masala::base::Real, Eigen::Dynamic > & scratchvec2,
		Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > & inv_hessian
	) const override {
		using masala::base::Real;
		using Eigen::Vector;
		using Eigen::Matrix;
		using Eigen::Dynamic;

		Real const p_diff_dot_grad_diff( p_diff.dot( grad_diff ) );

		scratchvec1 = inv_hessian * grad_diff;
		Real const grad_diff_inv_hess_grad_diff( grad_diff.dot( scratchvec1 ) );

		scratchvec2 = p_diff / p_diff_dot_grad_diff - scratchvec1 / grad_diff_inv_hess_grad_diff;

		inv_hessian += ( p_diff * ( p_diff.transpose() ) / ( p_diff_dot_grad_diff ) )
			- ( scratchvec1 * ( scratchvec1.transpose() ) / grad_diff_inv_hess_grad_diff )
			+ grad_diff_inv_hess_grad_diff * scratchvec2 * ( scratchvec2.transpose() );
		
		if( protected_min_inv_hessian_determinant() > 0.0 && inv_hessian.determinant() < protected_min_inv_hessian_determinant() ) {
			write_to_tracer( "Singular inverse Hessian approximation detected.  Resetting to identity matrix." );
			inv_hessian.setIdentity( inv_hessian.rows(), inv_hessian.cols() );
		}
	}

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

}; // class BFGSFunctionOptimizer

} // namespace quasi_newtonian
} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_quasi_newtonian_BFGSFunctionOptimizer_hh