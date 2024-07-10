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

/// @file src/numeric_api/base_classes/optimization/gradient_based/GradientDescentFunctionOptimizer.hh
/// @brief Headers for the GradientDescentFunctionOptimizer.
/// @details The GradientDescentFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using an iterative approach of computing gradients and carrying out
/// line searches with a line search algorithm.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_gradient_based_GradientDescentFunctionOptimizer_hh
#define Standard_Masala_Plugins_src_optimizers_gradient_based_GradientDescentFunctionOptimizer_hh

// Forward declarations:
#include <optimizers/gradient_based/GradientDescentFunctionOptimizer.fwd.hh>

// Base headers:
#include <base/types.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.fwd.hh>
#include <numeric_api/base_classes/optimization/real_valued_local/LineOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/real_valued_local/RealValuedFunctionLocalOptimizer.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace gradient_based {

/// @brief The GradientDescentFunctionOptimizer carries out gradient-descent minimization of an arbitrary function
/// for which gradients are available using an iterative approach of computing gradients and carrying out
/// line searches with a line search algorithm.  This is relatively inefficient compared to quasi-Newtonian
/// methods like DFP, BFGS, or L-BFGS.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class GradientDescentFunctionOptimizer : public masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	GradientDescentFunctionOptimizer() = default;

	/// @brief Copy constructor.
	GradientDescentFunctionOptimizer( GradientDescentFunctionOptimizer const & ) = default;

	/// @brief Destructor.
	~GradientDescentFunctionOptimizer() override = default;

	/// @brief Clone operation: copy this object and return a shared pointer to the
	/// copy.  Contained objects may still be shared.
	masala::numeric_api::base_classes::optimization::real_valued_local::RealValuedFunctionLocalOptimizerSP
	clone() const override;

	/// @brief Deep clone operation: copy this object and return a shared pointer to the
	/// copy, making sure that all contained objects are also copied.
	GradientDescentFunctionOptimizerSP
	deep_clone() const;

public:

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
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "optimizer", "real_valued", "local_optimizer", "gradient_based", "numeric", "quasi-newtonian", "l-bfgs" }
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
	/// @returns { {"Optimizer", "RealValuedFunctionLocalOptimizer", "GradientDescentFunctionOptimizer"} }
    std::vector< std::vector < std::string > >
    get_engine_categories() const override;

	/// @brief Every class can name itself.
	/// @returns "GradientDescentFunctionOptimizer".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::gradient_based".
	std::string class_namespace() const override;

	/// @brief Every class can name itself.
	/// @returns "GradientDescentFunctionOptimizer".
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
	void
	set_max_iterations(
		masala::base::Size const setting
	);

	/// @brief Set a line optimizer to use for the line searches.
	/// @details Used directly, not cloned.  If none is provided (or if this is set to
	/// nullptr), then a BrentAlgorithmLineOptimizer is used by default.
	void
	set_line_optimizer(
		masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP const & line_optimizer_in
	);

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
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP
	line_optimizer() const;

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

	/// @brief Run a single local optimization problem in a thread.  This function runs in parallel
	/// in threads.  This function is called from a mutex-locked context.
	/// @param[in] problem The problem to solve.
	/// @param[in] line_optimizer The line optimizer to use when solving this problem.
	/// @param[out] solutions The solutions container pointer.  This will be updated to point to a new
	/// solutions container object, containing a single solution.
	void
	run_real_valued_local_optimizer_on_one_problem(
		masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationProblem_APICSP problem,
		masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP line_optimizer,
		masala::numeric_api::auto_generated_api::optimization::real_valued_local::RealValuedFunctionLocalOptimizationSolutions_APICSP & solutions
	) const;

	/// @brief Generate the Brent optimizer used by default if another line optimizer is not provided.
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP
	generate_brent_optimizer() const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The maximum number of steps that we're allowed to take.
	/// @details A setting of 0 means loop until convergence.
	masala::base::Size max_iterations_ = 2000;

	/// @brief A line optimizer used for the line searches.
	/// @details If none is provided, a BrentAlgorithmLineOptimizer is used.
	masala::numeric_api::base_classes::optimization::real_valued_local::LineOptimizerCSP line_optimizer_;

}; // class GradientDescentFunctionOptimizer

} // namespace gradient_based
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_gradient_based_GradientDescentFunctionOptimizer_hh