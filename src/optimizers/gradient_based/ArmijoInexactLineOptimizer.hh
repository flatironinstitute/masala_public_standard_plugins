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

/// @file src/numeric_api/base_classes/optimization/gradient_based/ArmijoInexactLineOptimizer.hh
/// @brief Headers for the ArmijoInexactLineOptimizer.
/// @details The ArmijoInexactLineOptimizer carries out gradient-free optimization of a function
/// along a line by the method of Larry Armijo, Pacific J. Math. 16(1): 1-3 (1966).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_based_ArmijoInexactLineOptimizer_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_based_ArmijoInexactLineOptimizer_hh

// Forward declarations:
#include <optimizers/gradient_based/ArmijoInexactLineOptimizer.fwd.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/real_valued_local/PluginLineOptimizer.hh>

// STL headers:
#include <cmath>
#include <limits>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

/// @details The ArmijoInexactLineOptimizer carries out gradient-free optimization of a function
/// along a line by the method of Larry Armijo, Pacific J. Math. 16(1): 1-3 (1966).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class ArmijoInexactLineOptimizer : public masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	ArmijoInexactLineOptimizer() = default;

	/// @brief Copy constructor.
	ArmijoInexactLineOptimizer( ArmijoInexactLineOptimizer const & ) = default;

	/// @brief Destructor.
	~ArmijoInexactLineOptimizer() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the
	/// copy.  Contained objects may still be shared.
	masala::numeric_api::base_classes::optimization::real_valued_local::PluginLineOptimizerSP
	clone() const override;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	ArmijoInexactLineOptimizerSP
	deep_clone() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
	/// may be overridden by derived classes.
	/// @returns { { "LineOptimizer", "ArmijoInexactLineOptimizer" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "line_optimizer", "lightweight", "numeric", "armijo", "inexact" }
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
	/// @returns { {"LineOptimizer", "ArmijoInexactLineOptimizer"} }
    std::vector< std::vector < std::string > >
    get_engine_categories() const override;

	/// @brief Keywords for engines.
	/// @returns { "line_optimizer", "lightweight", "numeric", "armijo", "inexact" }
    std::vector < std::string >
    get_engine_keywords() const override;

	/// @brief Every class can name itself.
	/// @returns "ArmijoInexactLineOptimizer".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	std::string class_namespace() const override;

	/// @brief Every class can name itself.
	/// @returns "ArmijoInexactLineOptimizer".
	static std::string class_name_static();

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the intial step size, as a multiple of the gradient.  Defaults to 1.0
	masala::base::Real initial_stepsize() const;

	/// @brief Get the value of tau used when shrinking the step size (where alpha_{i+1} = alpha_i * tau).
	/// Defaults to 0.5, the value used by Armijo in his initial publication.
	masala::base::Real stepsize_decrease_factor() const;

	/// @brief Get the value of c in the Armijo condition, f(x0+alpha*dir) <= f(x0) + alpha*c*m.  Defaults to
	/// 0.5, the value used by Armijo in his initial publication.
	masala::base::Real function_decrease_factor() const;

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the intial step size, as a multiple of the gradient.  Defaults to 1.0
	void set_initial_stepsize( masala::base::Real const setting );

	/// @brief Set the value of tau used when shrinking the step size (where alpha_{i+1} = alpha_i * tau).
	/// Defaults to 0.5, the value used by Armijo in his initial publication.
	void set_stepsize_decrease_factor( masala::base::Real const setting );

	/// @brief Set the value of c in the Armijo condition, f(x0+alpha*dir) <= f(x0) + alpha*c*m.  Defaults to
	/// 0.5, the value used by Armijo in his initial publication.
	void set_function_decrease_factor( masala::base::Real const setting );

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
		masala::numeric::optimization::real_valued_local::LineOptimizer const & src
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

	/// @brief Run the line optimizer on a single line optimization problem, and produce a single solution.
	/// @details Must be implemented by derived classes.  The solution is a pair of (x, f(x)) where x minimizes f.  Note
	/// that this function locks the object mutex, so this object is intended to be used to minimize a single function
	/// at a time (unlike other optimizers that take a vector of minimization problems to carry out in parallel).
	/// @param[in] fxn The function to (approximately) minimize.
	/// @param[in] x0 The starting point for the search.
	/// @param[in] fxn_at_x0 The value of the function at the starting point for the search.
	/// @param[in] grad_of_fxn_at_x0 The gradient of the function at the starting point for the search.
	/// @param[in] search_dir The search direction, which may or may not match the negative gradient of the starting point.
	/// @param[out] x The output value of x that adequately reduces f(x).
	/// @param[out] fxn_at_x The output value of f(x) where x adequately reduces f(x).
	void
	run_line_optimizer(
		std::function< masala::base::Real( Eigen::VectorXd const & ) > const & fxn,
		Eigen::VectorXd const & x0,
		masala::base::Real const fxn_at_x0,
		Eigen::VectorXd const & grad_of_fxn_at_x0,
		Eigen::VectorXd const & search_dir,
		Eigen::VectorXd & x,
		masala::base::Real & fxn_at_x
	) const override;

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

private:

	/// @brief Intial step size, as a multiple of the gradient.  Defaults to 1.0
	masala::base::Real initial_stepsize_ = 1.0;

	/// @brief The value of tau used when shrinking the step size (where alpha_{i+1} = alpha_i * tau).
	/// Defaults to 0.5, the value used by Armijo in his initial publication.
	masala::base::Real stepsize_decrease_factor_ = 0.5;

	/// @brief The value of c in the Armijo condition, f(x0+alpha*dir) <= f(x0) + alpha*c*m.  Defaults to
	/// 0.5, the value used by Armijo in his initial publication.
	masala::base::Real function_decrease_factor_ = 0.5;


}; // class ArmijoInexactLineOptimizer

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_ArmijoInexactLineOptimizer_hh