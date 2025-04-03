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

/// @file src/optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCFScratchSpace.hh
/// @brief Headers for the GraphIslandCountCFScratchSpace class.
/// @details GraphIslandCountCFScratchSpaces allow data to be cached and reused from one evaluation of a
/// GraphIslandCountCostFunction to another.  They are intended to be used with one particular cost function instance for
/// one particular problem.  The GraphIslandCountCFScratchSpace class derives from CostFunctionScratchSpace,
/// and permits cost function scratch spaces to be defined in plug-in libraries.
/// @note These objects are intended to be used by a single thread, and are not threadsafe.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_GraphIslandCountCFScratchSpace_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_GraphIslandCountCFScratchSpace_hh

// Forward declarations:
#include <optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCFScratchSpace.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/cost_function/PluginCostFunctionScratchSpace.hh>

// Numeric headers:

// Base headers:
#include <base/types.hh>

// STL headers:
#include <vector>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace graph_island_based {

/// @brief The GraphIslandCountCFScratchSpace class.
/// @details GraphIslandCountCFScratchSpaces allow data to be cached and reused from one evaluation of a
/// GraphIslandCountCostFunction to another.  They are intended to be used with one particular cost function instance for
/// one particular problem.  The GraphIslandCountCFScratchSpace class derives from CostFunctionScratchSpace,
/// and permits cost function scratch spaces to be defined in plug-in libraries.
/// @note These objects are intended to be used by a single thread, and are not threadsafe.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class GraphIslandCountCFScratchSpace : public masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionScratchSpace {

	typedef masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionScratchSpace Parent;
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionScratchSpaceSP ParentSP;
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionScratchSpaceCSP ParentCSP;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	GraphIslandCountCFScratchSpace() = delete;

	/// @brief Options constructor.  Calls finalize
	GraphIslandCountCFScratchSpace(
		masala::base::Size const n_absolute_nodes,
		masala::base::Size const n_variable_nodes,
		std::vector< masala::base::Size > const & n_interaction_graph_edges_by_abs_node
	);

	/// @brief Copy constructor.
	GraphIslandCountCFScratchSpace( GraphIslandCountCFScratchSpace const & /*src*/ ) = default;

	// @brief Assignment operator.
	GraphIslandCountCFScratchSpace &
	operator=( GraphIslandCountCFScratchSpace const & /*src*/ ) = default;

	/// @brief Destructor.
	~GraphIslandCountCFScratchSpace() override = default;

	/// @brief Make a copy of this object.
	masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionScratchSpaceSP
	clone() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the name of this class.
	/// @returns "GraphIslandCountCFScratchSpace".
	static
	std::string
	class_name_static();

	/// @brief Get the namespace of this class.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based".
	static
	std::string
	class_namespace_static();

	/// @brief Get the name of this class.
	/// @returns "GraphIslandCountCFScratchSpace".
	std::string
	class_name() const;

	/// @brief Get the namespace of this class.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based".
	std::string
	class_namespace() const;

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Access the island sizes (connected component node counts).  Const access version.
	inline std::vector< masala::base::Size > const & island_sizes_const() const { return *island_sizes_; }

	/// @brief Access the island sizes (connected component node counts).  Nonconst access version.
	inline std::vector< masala::base::Size > & island_sizes() { return *island_sizes_; }

	/// @brief Has at least one move been accepted?
	inline bool at_least_one_move_accepted() const { return move_accepted_; }

	/// @brief Access the last accepted candidate solution vector.  Const access version.
	inline std::vector< masala::base::Size > const & last_accepted_candidate_solution_const() const { return *last_accepted_candidate_solution_; }

	/// @brief Const access to the vector of number of edges by absolute node index in the last accepted connectivity graph.
	inline std::vector< masala::base::Size > const & last_accepted_nedges_for_node_in_connectivity_graph_const() const { return *last_accepted_nedges_for_node_in_connectivity_graph_; }

	/// @brief Const access to the vector of vectors of connected node indices by absolute node index in the last accepted connectivity graph.
	inline std::vector< std::vector< masala::base::Size > > const & last_accepted_edges_for_node_in_connectivity_graph_const() const { return *last_accepted_edges_for_node_in_connectivity_graph_; }

	/// @brief Const access to the vector of number of edges by absolute node index in the current connectivity graph.
	inline std::vector< masala::base::Size > const & nedges_for_node_in_connectivity_graph_const() const { return *nedges_for_node_in_connectivity_graph_; }

	/// @brief Const access to the vector of vectors of connected node indices by absolute node index in the current connectivity graph.
	inline std::vector< std::vector< masala::base::Size > > const & edges_for_node_in_connectivity_graph_const() const { return *edges_for_node_in_connectivity_graph_; }

	/// @brief Non-const access to the vector of number of edges by absolute node index in the current connectivity graph.
	inline std::vector< masala::base::Size > & nedges_for_node_in_connectivity_graph() { return *nedges_for_node_in_connectivity_graph_; }

	/// @brief Non-const access to the vector of vectors of connected node indices by absolute node index in the current connectivity graph.
	inline std::vector< std::vector< masala::base::Size > > & edges_for_node_in_connectivity_graph() { return *edges_for_node_in_connectivity_graph_; }

	/// @brief Get the size of the drop list.
	inline masala::base::Size drop_list_size() const { return drop_list_size_; }

	/// @brief Get the size of the add list.
	inline masala::base::Size add_list_size() const { return add_list_size_; }

	/// @brief Const-access the drop list.
	inline std::vector< std::pair< masala::base::Size, masala::base::Size > > const & drop_list() const { return drop_list_; }

	/// @brief Const-access the add list.
	inline std::vector< std::pair< masala::base::Size, masala::base::Size > > const & add_list() const { return add_list_; }

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the current state to the last accepted state.
	void copy_last_accepted_to_current();

	/// @brief Set the current candidate solution.  Throws if solution sizes don't match in debug mode.
	void set_current_candidate_solution( std::vector< masala::base::Size > const & solution_in );

	/// @brief Set the size of the drop and add lists to zero.
	void clear_drop_and_add_lists();

	/// @brief Indicate a connection between two absolute node indices to drop.
	void indicate_drop( std::pair< masala::base::Size, masala::base::Size > const & pair_to_drop );

	/// @brief Indicate a connection between two absolute node indices to add.
	void indicate_add( std::pair< masala::base::Size, masala::base::Size > const & pair_to_add );

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Copy the last accepted connectivity graph to that for the current state, then copy in a new current state.
	/// @note This does NOT update the connectivity graph for the current state.
	void prepare_connectivity_graph_for_current( std::vector< masala::base::Size > const & solution_in );

	/// @brief Count the number of variable node indices that have changed from previous to current, update the internally-
	/// stored count and indices, and return a reference to the count and indices.
	std::pair< masala::base::Size, std::vector< masala::base::Size > > const &
	changed_variable_node_count_and_indices();

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Accept the last move.
	void protected_accept_last_move() override;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the last accepted connectivity graph to that for the current state.
	void copy_last_accepted_connectivity_graph_to_current();

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief Storage for current and last candidate solutions.  To avoid unnecessary memory copying,
	/// these are allocated and referred to by two pointers that can swap.
	std::vector< masala::base::Size > vec1_;
	std::vector< masala::base::Size > vec2_;

	/// @brief The current candidate solution.  A pointer that swaps between vec1_ and vec2_.
	std::vector< masala::base::Size > * current_candidate_solution_;

	/// @brief The last accepted candidate solution.  A pointer that swaps between vec2_ and vec1_.
	std::vector< masala::base::Size > * last_accepted_candidate_solution_;

	/// @brief Storage for current and last island sizes.  To avoid unnecessary memory copying,
	/// these are allocated and referred to by two pointers that can swap.
	std::vector< masala::base::Size > vec3_;
	std::vector< masala::base::Size > vec4_;

	/// @brief The sizes of all of the connected components.  Points to vec3_ or vec4_.
	std::vector< masala::base::Size > * island_sizes_;

	/// @brief The last accepted sizes of all of the connected components.  Points to vec4_ or vec3_.
	std::vector< masala::base::Size > * last_accepted_island_sizes_;

	/// @brief Storage for the number of edges for a given node in the connectivity graph, and for the list of edges.
	std::vector< masala::base::Size > vec5_;
	std::vector< masala::base::Size > vec6_;
	std::vector< std::vector< masala::base::Size > > vec7_;
	std::vector< std::vector< masala::base::Size > > vec8_;

	/// @brief The number of edges for a given node in the connectivity graph, and for the list of edges.
	/// Again, these are vectors that alternate between pointing at vec5_ and vec6_, and vec7_ and vec8_, respectively.
	std::vector< masala::base::Size > * nedges_for_node_in_connectivity_graph_;
	std::vector< masala::base::Size > * last_accepted_nedges_for_node_in_connectivity_graph_;
	std::vector< std::vector< masala::base::Size > > * edges_for_node_in_connectivity_graph_;
	std::vector< std::vector< masala::base::Size > > * last_accepted_edges_for_node_in_connectivity_graph_;

	/// @brief Has a move been made since the last accept?
	bool move_made_ = false;

	/// @brief Has at least one move been accepted?
	bool move_accepted_ = false;

	/// @brief The number of changed variable nodes and the changed variable node indices.
	std::pair< masala::base::Size, std::vector< masala::base::Size > > changed_variable_node_count_and_indices_;

	/// @brief A temporary list of absolute node pair interactions to drop.
	std::vector< std::pair< masala::base::Size, masala::base::Size > > drop_list_;

	/// @brief A temporary list of absolute node pair interactions to add.
	std::vector< std::pair< masala::base::Size, masala::base::Size > > add_list_;

	/// @brief The size of the drop list.  (Smaller than the vector to avoid reallocation.)
	masala::base::Size drop_list_size_ = 0;

	/// @brief The size of the add list.  (Smaller than the vector to avoid reallocation.)
	masala::base::Size add_list_size_ = 0;

}; // class GraphIslandCountCFScratchSpace

} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_GraphIslandCountCFScratchSpace_hh