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

/// @file src/numeric_api/base_classes/optimization/gradient_based/BrentAlgorithmLineOptimizer.hh
/// @brief Headers for the BrentAlgorithmLineOptimizer.
/// @details The BrentAlgorithmLineOptimizer carries out gradient-free optimization of a function
/// along a line.  It uses the algorithm of Richard P. Brent described in "Algorithms for Minimization
/// Without Derivatives" (1973).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_based_BrentAlgorithmLineOptimizer_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_based_BrentAlgorithmLineOptimizer_hh

// Forward declarations:
#include <optimizers/gradient_based/BrentAlgorithmLineOptimizer.fwd.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/real_valued_local/LineOptimizer.hh>

// STL headers:
#include <cmath>
#include <limits>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

/// @brief The BrentAlgorithmLineOptimizer, which carries out gradient-free optimization of a function
/// along a line.  It uses the algorithm of Richard P. Brent described in "Algorithms for Minimization
/// Without Derivatives" (1973).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class BrentAlgorithmLineOptimizer : public masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	BrentAlgorithmLineOptimizer() = default;

	/// @brief Copy constructor.
	BrentAlgorithmLineOptimizer( BrentAlgorithmLineOptimizer const & ) = default;

	/// @brief Destructor.
	~BrentAlgorithmLineOptimizer() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the
	/// copy.  Contained objects may still be shared.
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerSP
	clone() const override;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	BrentAlgorithmLineOptimizerSP
	deep_clone() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
	/// may be overridden by derived classes.
	/// @returns { { "LineOptimizer", "BrentAlgorithmLineOptimizer" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "line_optimizer", "lightweight", "numeric", "brent_algorithm" }
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
	/// @returns { {"LineOptimizer", "BrentAlgorithmLineOptimizer"} }
    std::vector< std::vector < std::string > >
    get_engine_categories() const override;

	/// @brief Every class can name itself.
	/// @returns "BrentAlgorithmLineOptimizer".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	std::string class_namespace() const override;

	/// @brief Every class can name itself.
	/// @returns "BrentAlgorithmLineOptimizer".
	static std::string class_name_static();

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real tolerance() const;

	/// @brief Get the maximum number of iterations.
	/// @details Defaults to 1000.  A setting of 0 means to loop until convergence, regardless
	/// the number of iterations taken.
	masala::base::Size max_iters() const;

	/// @brief Get the step size for initially bracketing x.  Set to 0.001 by default.
	masala::base::Real initial_stepsize() const;

	/// @brief Should we throw if iterations are exceeded (true), or just warn
	/// (false, the default)?
	bool throw_if_iterations_exceeded() const;

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	void set_tolerance( masala::base::Real const setting );

	/// @brief Set the maximum number of iterations.
	/// @details Defaults to 1000.  A setting of 0 means to loop until convergence, regardless
	/// the number of iterations taken.
	void set_max_iters( masala::base::Size const setting );

	/// @brief GSet the step size for initially bracketing x.  Set to 0.001 by default.
	void set_initial_stepsize( masala::base::Real const setting );

	/// @brief Set whether we should we throw if iterations are exceeded (true), or just warn
	/// (false, the default).
	void set_throw_if_iterations_exceeded( bool const setting );

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
		masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizer const & src
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
	/// @details Must be implemented by derived classes.  The solution is a pair of (x, f(x)) where x minimizes f.
	/// @param[in] fxn The function to minimize.
	/// @param[out] x The value of x that (locally) minimizes f(x).
	/// @param[out] fxn_at_x The value of f(x) where x (locally) minimizes f(x).
	void
	run_line_optimizer(
		std::function< masala::base::Real( masala::base::Real ) > const & fxn,
		masala::base::Real & x,
		masala::base::Real & fxn_at_x
	) const override;

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

private:

	/// @brief The tolerance for determining whether or not we've finished our search.
	/// @details The default is the square root of machine precision (the theoretical lower limit for
	/// any sensible value of tolerance).
	masala::base::Real tolerance_ = std::sqrt( std::numeric_limits< masala::base::Real >::denorm_min() );

	/// @brief Maximum number of iterations.
	/// @details Defaults to 1000.
	masala::base::Size max_iters_ = 1000;

	/// @brief Step size for initially bracketing x.  Set to 0.001 by default.
	masala::base::Real initial_stepsize_ = 1.0e-3;

	/// @brief Should we throw if iterations are exceeded (true), or just warn
	/// (false, the default)?
	bool throw_if_iterations_exceeded_ = false;

}; // class BrentAlgorithmLineOptimizer

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_BrentAlgorithmLineOptimizer_hh