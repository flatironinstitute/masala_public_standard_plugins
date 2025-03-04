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

/// @file src/optimizers/cost_function_network/cost_function/GraphIslandCountCostFunction.hh
/// @brief Headers for a class for GraphIslandCountCostFunctions.
/// @details GraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphIslandCountCostFunction_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphIslandCountCostFunction_hh

// Forward declarations:
#include <optimizers/cost_function_network/cost_function/GraphIslandCountCostFunction.fwd.hh>

// Parent header:
#include <optimizers/cost_function_network/cost_function/GraphBasedCostFunction.hh>

// Numeric headers:

// Base headers:
#include <base/types.hh>
#include <base/hash_types.hh>

// STL headers:
#include <unordered_map>
#include <utility> //For std::pair.

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

/// @brief A class for GraphIslandCountCostFunctions.
/// @details GraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class GraphIslandCountCostFunction : public standard_masala_plugins::optimizers::cost_function_network::cost_function::GraphBasedCostFunction {

	typedef standard_masala_plugins::optimizers::cost_function_network::cost_function::GraphBasedCostFunction Parent;
	typedef standard_masala_plugins::optimizers::cost_function_network::cost_function::GraphBasedCostFunctionSP ParentSP;
	typedef standard_masala_plugins::optimizers::cost_function_network::cost_function::GraphBasedCostFunctionCSP ParentCSP;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	GraphIslandCountCostFunction() = default;

	/// @brief Copy constructor.
	GraphIslandCountCostFunction(
		GraphIslandCountCostFunction const & src
	);

	// @brief Assignment operator.
	GraphIslandCountCostFunction &
	operator=( GraphIslandCountCostFunction const & src );

	/// @brief Destructor.
	~GraphIslandCountCostFunction() override = default;

	// /// @brief Make a copy of this object.
	// masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP
	// clone() const override;

	// /// @brief Make a copy of this object that is fully independent.
	// GraphIslandCountCostFunctionSP
	// deep_clone() const;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all
	/// cost functions; may be overridden by derived classes.
	/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the category for this MasalaDataRepresentation.
	/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction" } }.
	std::vector< std::vector< std::string > >
	get_data_representation_categories() const override;

	/// @brief Get the keywords for this MasalaDataRepresentation.
	/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based" }
	std::vector< std::string >
	get_data_representation_keywords() const override;

	/// @brief Get the properties of this MasalaDataRepresentation.
	/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based" }.
	std::vector< std::string >
	get_present_data_representation_properties() const override;

	/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
	/// non-exhaustive list.
	/// @returns { "pairwise_decomposible" }.
	std::vector< std::string >
	get_absent_data_representation_properties() const override;

	/// @brief Get the keywords for this plugin class.
	/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the name of this class ("GraphIslandCountCostFunction").
	/// @details Static version.
	static std::string class_name_static();

	/// @brief Get the name of this class ("GraphIslandCountCostFunction").
	std::string class_name() const override;

	/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function").
	/// @details Static version.
	static std::string class_namespace_static();

	/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function").
	std::string class_namespace() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////


public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
	/// @param[in] variable_node_indices A list of all of the absolute node indices
	/// for nodes that have more than one choice, indexed by variable node index.
	/// @details The base class function simply marks this object as finalized.  Should
	/// be overridden, and overrides should call parent class protected_finalize().
	void
	protected_finalize(
		std::vector< masala::base::Size > const & variable_node_indices
	) override;

	/// @brief Override of protected_assign().  Calls parent function.
	/// @details Throws if src is not a GraphIslandCountCostFunction.
	void protected_assign( masala::base::managers::engine::MasalaDataRepresentation const & src ) override;

	/// @brief Make this object fully independent.  Assumes mutex was already locked.
	/// Should be called by overrides.
	void
	protected_make_independent() override;

	/// @brief Is this data representation empty?
	/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
	/// @returns True if no data have been loaded into this data representation, false otherwise.
	/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
	bool
	protected_empty() const override;

	/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
	/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
	void
	protected_clear() override;

	/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
	/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
	void
	protected_reset() override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////


}; // class GraphIslandCountCostFunction

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphIslandCountCostFunction_hh