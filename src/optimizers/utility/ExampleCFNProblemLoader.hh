/*
    Masala
    Copyright (C) 2026 Vikram K. Mulligan

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

/// @file src/optimizers/utility/ExampleCFNProblemLoader.hh
/// @brief Headers for a utility class that loads 100 example CFN problems, which are small enough to solve
/// using the Toulbar2 deterministic solver.
/// @details These problems are intended for unit-testing new CFN solvers.  Pretty much any CFN solver
/// should be able to find the optimal solution for these.  The solutions are also produced by the loader.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_utility_ExampleCFNProblemLoader_hh
#define Standard_Masala_Plugins_src_optimizers_utility_ExampleCFNProblemLoader_hh

// Forward declarations:
#include <optimizers/utility/ExampleCFNProblemLoader.fwd.hh>

// Parent headers:
#include <base/managers/plugin_module/MasalaPlugin.hh>

// Numeric API headers:
#include <numeric_api/base_classes/optimization/cost_function_network/PluginCostFunctionNetworkOptimizer.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.fwd.hh>

// STL headers:
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace utility {

/// @brief A utility class that loads 100 example CFN problems, which are small enough to solve
/// using the Toulbar2 deterministic solver.
/// @details These problems are intended for unit-testing new CFN solvers.  Pretty much any CFN solver
/// should be able to find the optimal solution for these.  The solutions are also produced by the loader.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class ExampleCFNProblemLoader : public masala::base::managers::plugin_module::MasalaPlugin {

public:

	typedef masala::base::managers::plugin_module::MasalaPlugin Parent;
	typedef masala::base::managers::plugin_module::MasalaPluginSP ParentSP;
	typedef masala::base::managers::plugin_module::MasalaPluginCSP ParentCSP;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION, DESTRUCTION, ETC.
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	ExampleCFNProblemLoader() = default;

	/// @brief Copy constructor.  Must be explicit due to mutex.
	ExampleCFNProblemLoader( ExampleCFNProblemLoader const & src );

	/// @brief Assignment operator.  Must be explicit due to mutex.
	ExampleCFNProblemLoader & operator=( ExampleCFNProblemLoader const & src );

	/// @brief Virtual destructor.
	~ExampleCFNProblemLoader() override = default;

	/// @brief Every class can name itself.
	/// @returns "ExampleCFNProblemLoader".
	std::string class_name() const override;

	/// @brief Every class can provide its own namespace.
	/// @returns "standard_masala_plugins::optimizers::utility".
	std::string class_namespace() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get an object describing the API for this object.
	/// @note This is a weak pointer rather than a shared pointer since the
	/// original object is expected to hold on to its API definition (which includes
	/// function pointers to the functions of the instance).  Querying whether the
	/// weak pointer can be converted to a shared pointer serves on a check as to
	/// whether it is safe to use the function pointers.  Not ideal, but better than
	/// nothing.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

	/// @brief Get the category or categories for this plugin class.
	/// @details Pure virtual; must be implemented by derived classes.
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	/// @returns This override returns {{"Utility", "CFN_Utility", "ExampleCFNProblemLoader"}}.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.
	/// @details Pure virtual; must be implemented by derived classes.
	/// @returns This override returns { "utility", "cost_function_network", "example_problem", "example_problem_loader" }.
	std::vector< std::string >
	get_keywords() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Returns a container of 400 problems.  Throws if problems and solutions  have not already been
	/// loaded and cached.
	/// @note These problems are not finalized.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
	get_problems() const;

	/// @brief Returns a container of n problems (where 0 < n <= 400).  Throws if problems and solutions 
	/// have not already been loaded and cached.
	/// @param[in] n_problems The number of problems to return.  Must be in the range [1, 400].  If smaller than
	/// 400, then the first 400 problems are returned.
	/// @note These problems are not finalized.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
	get_problems( masala::base::Size const n_problems ) const;

	/// @brief Returns a container of 400 solutions.  Throws if problems and solutions have not already been
	/// loaded and cached.
	/// @note These solutions are not finalized.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP
	get_solutions() const;

	/// @brief Returns a container of n solutions (where 0 < n <= 400).  Throws if problems and solutions 
	/// have not already been loaded and cached.
	/// @param[in] n_solutions The number of solutions to return.  Must be in the range [1, 400].  If smaller than
	/// 400, then the first 400 solutions are returned.
	/// @note These solutions are not finalized.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP
	get_solutions( masala::base::Size const n_solutions ) const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Load problems and solutions from disk, and cache them in this object in a format compatible with a given optimizer.
	/// @details Throws if already initialized.
	void initialize( masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer const & optimizer );

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Assign src to this.  Performs no mutex locking.  Derived classes should
	/// override this, and the overrides should call the parent class protected_assign().
	virtual void protected_assign( ExampleCFNProblemLoader const & src );

	/// @brief Clone the first N of the cached problems and package the clones into a problems container.
	/// @details Throws if problems and solutions have not yet been loaded.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
	protected_get_problems( masala::base::Size const n_problems = 400 ) const;

	/// @brief Clone the first N of the cached solutions and package the clones into a problems container.
	/// @details Throws if problems and solutions have not yet been loaded.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP
	protected_get_solutions( masala::base::Size const n_solutions = 400 ) const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

private:

	/// @brief A mutex for this class.
	mutable std::mutex mutex_;

	/// @brief The API definition for this instance of this class.  Nullptr until get_api_definition() is called.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_definition_;

	/// @brief The problems, loaded from disk.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP problems_;

	/// @brief The solutions, loaded from disk.
	std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP > solutions_;

	/// @brief The problem names, loaded from disk.
	std::vector< std::string > problem_names_;

}; // class MasalaObject

} // namespace utility
} // namespace optimizers
} // namespace standard_masala_plugins

#endif //Standard_Masala_Plugins_src_optimizers_utility_ExampleCFNProblemLoader_hh