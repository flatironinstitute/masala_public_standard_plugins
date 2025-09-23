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

/// @file src/optimizers/cost_function_network/RandomCostFunctionNetworkOptimizer.hh
/// @brief Headers for a CostFunctionNetworkOptimizer that solves a cost function network problem by picking a
/// solution at random.
/// @details This is intended as a control during methods development, to compare a new solver to just choosing
/// a solution at random.  This is NOT intended to be a useful production solver.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_RandomCostFunctionNetworkOptimizer_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_RandomCostFunctionNetworkOptimizer_hh

// Forward declarations:
#include <optimizers/cost_function_network/RandomCostFunctionNetworkOptimizer.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/PluginCostFunctionNetworkOptimizer.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/annealing/AnnealingScheduleBase_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.fwd.hh>

// Base headers:
#include <base/types.hh>
#include <base/managers/plugin_module/MasalaPluginAPI.fwd.hh>
#include <base/managers/random/MasalaRandomNumberGenerator.fwd.hh>

// STL headers:
#include <mutex>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

/// @brief A CostFunctionNetworkOptimizer that solves a cost function network problem by picking a solution at random.
/// @details This is intended as a control during methods development, to compare a new solver to just choosing
/// a solution at random.  This is NOT intended to be a useful production solver.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class RandomCostFunctionNetworkOptimizer : public masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	RandomCostFunctionNetworkOptimizer() = default;

	/// @brief Copy constructor.
	/// @details Needed since we define a mutex.
	RandomCostFunctionNetworkOptimizer( RandomCostFunctionNetworkOptimizer const & );

	/// @brief Assignment operator.
	/// @details Needed since we define a mutex.
	RandomCostFunctionNetworkOptimizer &
	operator=( RandomCostFunctionNetworkOptimizer const & );

	/// @brief Make a copy of this object that's wholly independent.
	RandomCostFunctionNetworkOptimizerSP
	deep_clone() const;

	/// @brief Destructor.
	~RandomCostFunctionNetworkOptimizer() override = default;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
	/// may be overridden by derived classes.
	/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outher vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
	/// by derived classes.
	/// @returns { "optimizer", "cost_function_network", "numeric", "random", "stochastic" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the category that this MasalaEngine fits into.
	/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
	std::vector< std::vector< std::string > >
	get_engine_categories() const override;

	/// @brief Get the keywords that this MasalaEngine has.
	/// @returns { "optimizer", "cost_function_network", "numeric", "random", "stochastic" }
	std::vector< std::string >
	get_engine_keywords() const override;

	/// @brief Get the class name.
	/// @returns "RandomCostFunctionNetworkOptimizer".
	std::string class_name() const override;

	/// @brief Get the class namespace.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	std::string class_namespace() const override;

	/// @brief Get the class name (static version).
	/// @returns "RandomCostFunctionNetworkOptimizer".
	static std::string class_name_static();

	/// @brief Get the class namespace (static version).
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the API definition for this object.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the number of times to try each problem.
	/// @details Minimum is 1.
	void set_attempts_per_problem( masala::base::Size const attempts_in );

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the number of times to try each problem.
	/// @details Minimum is 1.
	masala::base::Size attempts_per_problem() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
	/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
	/// the problem with the same index.
	std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
	run_cost_function_network_optimizer(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
	) const override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Assign src to this object.  Must be implemented by derived classes.  Performs no mutex-locking.  Derived classes should call their parent's protected_assign().
	void protected_assign( PluginCostFunctionNetworkOptimizer const & src ) override;

	/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
	void protected_make_independent() override;

	/// @brief Set a template cost function network optimization problem data representation, configured by the user but with no data entered.
	/// @details This can optionally be passed in, in which case the get_template_preferred_cfn_data_representation() function can be
	/// used to retrieve a deep clone.  This allows the solver to cache its preferred data representation with its setup.
	/// @note This version performs no mutex-locking, and is called by set_template_preferred_cfn_data_representation(), which does lock the mutex.
	/// This version just calls the base class version; there are no special checks here.
	void
	protected_set_template_preferred_cfn_data_representation(
		masala::base::managers::engine::MasalaDataRepresentationAPICSP const & representation_in
	) override;

	/// @brief If the template preferred CFN data representation has not been set, return a default CFN data representation.
	/// @details This version returns a PairwisePrecomputedCostFunctionNetworkOptimizationProblem, with default configuration.  Performs no mutex-locking.
	masala::base::managers::engine::MasalaDataRepresentationAPISP
	protected_get_default_template_preferred_cfn_data_representation() const override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief The API description.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_description_;

	/// @brief The number of times to attempt each problem.
	masala::base::Size attempts_per_problem_ = 1;

}; // class RandomCostFunctionNetworkOptimizer

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins

#endif // StandardMaslaPlugins_src_optimizers_cost_function_network_RandomCostFunctionNetworkOptimizer_hh