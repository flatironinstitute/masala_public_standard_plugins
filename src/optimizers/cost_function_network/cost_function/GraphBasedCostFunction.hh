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

/// @file src/optimizers/cost_function_network/cost_function/GraphBasedCostFunction.hh
/// @brief Headers for a pure virtual base class for GraphBasedCostFunctions.
/// @details GraphBasedCostFunctions define a penalty function which is based on the following:
/// - A graph of interacting choices at all nodes is computed.
/// - During an optimization trajectory, the set of edges based on the currently-selected choices is maintained and updated.
/// - Derived classes compute something from this graph.  (The initial application is to compute the size of islands, and then
/// to sum some function of each island's size; this is used to promote mutually-connected structures like hydrogen bond networks
/// when designing peptides and proteins, and is analogous to Rosetta's hbnet scoring term.)
/// @note This class has been updated to be a template class.  Originally, it stored a boolean graph, but it makes
/// sense to allow other types of values to occupy edges (for instance, to support Rosetta's interdigitation_bonus
/// scoring term).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphBasedCostFunction_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphBasedCostFunction_hh

// Forward declarations:
#include <optimizers/cost_function_network/cost_function/GraphBasedCostFunction.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/cost_function/PluginCostFunction.hh>

// Numeric headers:

// Base headers:
#include <base/types.hh>
#include <base/error/ErrorHandling.hh>

// External headers:
#include <external/eigen/Eigen/Core>

// STL headers:

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

/// @brief A pure virtual base class for GraphBasedCostFunctions.
/// @details GraphBasedCostFunctions define a penalty function which is based on the following:
/// - A graph of interacting choices at all nodes is computed.
/// - During an optimization trajectory, the set of edges based on the currently-selected choices is maintained and updated.
/// - Derived classes compute something from this graph.  (The initial application is to compute the size of islands, and then
/// to sum some function of each island's size; this is used to promote mutually-connected structures like hydrogen bond networks
/// when designing peptides and proteins, and is analogous to Rosetta's hbnet scoring term.)
/// @note This class has been updated to be a template class.  Originally, it stored a boolean graph, but it makes
/// sense to allow other types of values to occupy edges (for instance, to support Rosetta's interdigitation_bonus
/// scoring term).
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
template< typename T >
class GraphBasedCostFunction : public masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction {
	
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction Parent;
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionSP ParentSP;
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionCSP ParentCSP;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	GraphBasedCostFunction() = default;

	/// @brief Copy constructor.
	GraphBasedCostFunction(
		GraphBasedCostFunction const & src
	);

	// @brief Assignment operator.
	GraphBasedCostFunction &
	operator=( GraphBasedCostFunction const & src );

	/// @brief Destructor.  Not defaulted since we have to deallocate the matrices.
	~GraphBasedCostFunction() override;

	/// @brief This class is pure virtual, and does not define the clone function.
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP clone() const override = 0;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all
	/// cost functions; may be overridden by derived classes.
	/// @returns { { "CostFunction", "GraphBasedCostFunction" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the category for this MasalaDataRepresentation.
	/// @returns { { "CostFunction", "GraphBasedCostFunction" } }.
	std::vector< std::vector< std::string > >
	get_data_representation_categories() const override;

	/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
	/// is compatible.
	/// @returns {"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"}.
	std::vector< std::string >
	get_compatible_masala_engines() const override;

	/// @brief Get the properties of this MasalaDataRepresentation.
	/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible" }.
	std::vector< std::string >
	get_present_data_representation_properties() const override;

	/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
	/// non-exhaustive list.
	/// @returns { "pairwise_decomposible" }.
	std::vector< std::string >
	get_absent_data_representation_properties() const override;

	/// @brief Get the keywords for this plugin class.
	/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the keywords for this MasalaDataRepresentation.
	/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible" }
	std::vector< std::string >
	get_data_representation_keywords() const override;

	/// @brief This class is pure virtual, and does not define the name function.
	std::string class_name() const override = 0;

	/// @brief This class is pure virtual, and does not define the namespace function.
	std::string class_namespace() const override = 0;

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get whether nodes' absolute index is one-based (true) or zero-based (false, the default).
	/// @note Variable node indexing is always zero-based.
	bool one_based_absolute_node_indexing() const;

	/// @brief Get the total number of nodes.
	masala::base::Size absolute_node_count() const;

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set whether nodes' absolute index is one-based (true) or zero-based (false, the default).
	/// @details Throws if node-choice pair interacitons have already been input.
	/// @note Variable node indexing is always zero-based.
	void
	set_one_based_absolute_node_indexing(
		bool const setting
	);

	/// @brief Set the total number of nodes.
	/// @details If the interaction graph is smaller than this count, it is enlarged.  If it is larger,
	/// it is shrunk and any of the choice matrices that need to be deallocated are deallocated.  Throws if
	/// object has been finalized.
	void
	set_absolute_node_count(
		masala::base::Size const absolute_node_count
	);

	/// @brief Declare that two particular choices at two different absolute node indices interact, and set a value
	/// for the edge.
	/// @details If the node pair has not yet been declared, this declares it.  If the size of the matrix at the two
	/// absolute residue indices is smaller than the choice indices, this resizes the matrix to the size of the choice
	/// indices.
	/// @param[in] abs_nodeindex_1 The absolute index of the first node (variable or not).
	/// @param[in] abs_nodeindex_2 The absolute index of the second node (variable or not).
	/// @param[in] choiceindex_1 The absolute index of the choice at the first node (or 0 for a non-variable node).
	/// @param[in] choiceindex_2 The absolute index of the choice at the second node (or 0 for a non-variable node).
	/// @param[in] edge_value The value for this edge.
	void
	declare_node_choice_pair_interaction(
		masala::base::Size const abs_nodeindex_1,
		masala::base::Size const abs_nodeindex_2,
		masala::base::Size const choiceindex_1,
		masala::base::Size const choiceindex_2,
		T const edge_value
	);

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Given a selection of choices at variable nodes, compute the cost function.
	/// @details This must be implemented by derived classes.
	/// @note No mutex-locking is performed!
	masala::base::Real
	compute_cost_function(
		std::vector< masala::base::Size > const & candidate_solution,
		masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * scratch_space
	) const override = 0;

	/// @brief Given an old selection of choices at variable nodes and a new selection,
	/// compute the cost function difference.
	/// @details This must be implemented by derived classes.
	/// @note No mutex-locking is performed!
	masala::base::Real
	compute_cost_function_difference(
		std::vector< masala::base::Size > const & candidate_solution_old,
		std::vector< masala::base::Size > const & candidate_solution_new,
		masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * scratch_space
	) const override = 0;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the total number of nodes.  Protected version, which performs no mutex-locking.  Intended
	/// to be called from a mutex-locked context.
	/// @details If the interaction graph is smaller than this count, it is enlarged.  If it is larger,
	/// it is shrunk and any of the choice matrices that need to be deallocated are deallocated.  Throws if
	/// object has been finalized.
	void
	protected_set_absolute_node_count(
		masala::base::Size const absolute_node_count
	);

	/// @brief Get the number of nodes, with no mutex-locking.
	/// @note This is the total number of rows of the full_choice_choice_interaction_graph_ matrix, which may have an extra row
	/// and column if we are using 1-based numbering.
	masala::base::Size
	protected_n_nodes_absolute() const;

	/// @brief Are we using one-based (true) or zero-based (false) node indexing?
	/// @details Performs no mutex locking.
	inline bool protected_use_one_based_node_indexing() const { return use_one_based_node_indexing_; }

	/// @brief Get a pointer to the choice-choice interaction graph for a pair of nodes.
	/// @details Returns nullptr if that's the entry in the full choice interaction graph.
	/// Indices can be in any order.  Does not lock mutex.
	inline
	Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > const *
	protected_choice_choice_interaction_graph_for_nodepair(
		masala::base::Size const node1, masala::base::Size const node2
	) const {
		using masala::base::Size;

		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( node1 != node2, "protected_choice_choice_interaction_graph_for_nodepair", "Got " + std::to_string( node1 )
			+ " for both node indices.  Node indices must be different."
		);
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( node1 < node2, "protected_choice_choice_interaction_graph_for_nodepair", "Node 1 must be less than node 2." );
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( (!use_one_based_node_indexing_) || ( node1 > 0 && node2 > 0), "protected_choice_choice_interaction_graph_for_nodepair",
			"Got a node index of zero, but absolute node indices are one-based."
		);
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( node1 < static_cast< Size >( full_choice_choice_interaction_graph_.rows() ), "protected_choice_choice_interaction_graph_for_nodepair",
			"Node index " + std::to_string(node1) + " is out of range.  The full choice-choice interaction graph matrix is " +
			std::to_string(full_choice_choice_interaction_graph_.rows()) + " by " + std::to_string(full_choice_choice_interaction_graph_.cols())
			+ "."
		);
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( node2 < static_cast< Size >( full_choice_choice_interaction_graph_.rows() ), "protected_choice_choice_interaction_graph_for_nodepair",
			"Node index " + std::to_string(node2) + " is out of range.  The full choice-choice interaction graph matrix is " +
			std::to_string(full_choice_choice_interaction_graph_.rows()) + " by " + std::to_string(full_choice_choice_interaction_graph_.cols())
			+ "."
		);

		return full_choice_choice_interaction_graph_( node1, node2 );
	}

	/// @brief Given an absolute node index, get the variable node index.
	/// @details Throws if not yet finalized.  Does not lock mutex.  Returns a pair of
	/// <is variable node, variable node index if variable or 0 otherwise>.
	std::pair< bool, masala::base::Size > const &
	protected_varnode_from_absnode(
		masala::base::Size const absnode_index
	) const;

	/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
	/// @param[in] variable_node_indices A list of all of the absolute node indices
	/// for nodes that have more than one choice, indexed by variable node index.
	/// @details The base class function simply marks this object as finalized.  Should
	/// be overridden, and overrides should call parent class protected_finalize().
	void
	protected_finalize(
		std::vector< masala::base::Size > const & variable_node_indices
	) override;

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

	/// @brief Override of assign_mutex_locked().  Calls parent function.
	/// @details Throws if src is not a GraphBasedCostFunction.
	void protected_assign( masala::base::managers::engine::MasalaDataRepresentation const & src ) override;

	/// @brief Make this object fully independent.  Assumes mutex was already locked.
	/// Should be called by overrides.
	void
	protected_make_independent() override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief Are we using zero-based absolute node indices (false, the default) or one-based (true)?
	bool use_one_based_node_indexing_ = false;

	/// @brief The full choice-choice interaction graph.  This is a matrix indexed by node pairs of
	/// pointers to type T matricies indexed by choice pairs.  If the other matrix has an entry that
	/// is nullptr, it means that no choices at those two nodes have an interaction.
	/// @details The outer matrix is indexed by ABSOLUTE node, not variable node.  Nodes with only one
	/// choice are allowed (and produce single-column or single-row matrices for their interaction graph).
	/// @note The inner matrices are held by raw pointer. The GraphBasedCostFunction class is responsible for
	/// deallocating these on destruction.
	Eigen::Matrix<
		Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > *,
		Eigen::Dynamic,
		Eigen::Dynamic
	> full_choice_choice_interaction_graph_;

	// Computed by finalization function:

	/// @brief A vector of variable node indices by absolute node index.
	/// @details This is computed by the protected_finalize() function.  For every absolute node,
	/// we store < true, varnode_index > if it is a variable node, and < false, 0 > if it is not.
	/// If the indexing is 1-based, then the 0 entry also gets <true, varnode_index>.
	std::vector< std::pair< bool, masala::base::Size > > varnodes_by_absnode_;

}; // class GraphBasedCostFunction

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_GraphBasedCostFunction_hh