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

/// @file src/optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCFScratchSpace.cc
/// @brief Implementations for the GraphIslandCountCFScratchSpace class.
/// @details GraphIslandCountCFScratchSpaces allow data to be cached and reused from one evaluation of a
/// GraphIslandCountCostFunction to another.  They are intended to be used with one particular cost function instance for
/// one particular problem.  The GraphIslandCountCFScratchSpace class derives from CostFunctionScratchSpace,
/// and permits cost function scratch spaces to be defined in plug-in libraries.
/// @note These objects are intended to be used by a single thread, and are not threadsafe.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Headers:
#include <optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCFScratchSpace.hh>

// Numeric headers:

// Base headers:
#include <base/error/ErrorHandling.hh>

// STL headers:

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace graph_island_based {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Options constructor.
GraphIslandCountCFScratchSpace::GraphIslandCountCFScratchSpace(
	masala::base::Size const n_absolute_nodes,
	masala::base::Size const n_variable_nodes,
	std::vector< masala::base::Size > const & n_interaction_graph_edges_by_abs_node
) :
	Parent()
{
	using masala::base::Size;

	CHECK_OR_THROW( n_interaction_graph_edges_by_abs_node.size() == n_absolute_nodes, class_namespace_static() + "::" + class_name_static(),
		"GraphIslandCountCFScratchSpace", "Expected " + std::to_string(n_absolute_nodes) + " entries in the n_interaction_graph_edges_by_abs_node "
		"vector, but got " + std::to_string(n_interaction_graph_edges_by_abs_node.size()) + "."
	);
	vec1_.resize( n_variable_nodes, 0 );
	vec2_.resize( n_variable_nodes, 0 );
	current_candidate_solution_ = &vec1_;
	last_accepted_candidate_solution_ = &vec2_;

	vec3_.resize( n_absolute_nodes, 1 );
	vec4_.resize( n_absolute_nodes, 1 );
	island_sizes_ = &vec3_;
	last_accepted_island_sizes_ = &vec4_;

	vec5_.resize( n_absolute_nodes, 0 );
	vec6_.resize( n_absolute_nodes, 0 );
	vec7_.resize(n_absolute_nodes);
	vec8_.resize(n_absolute_nodes);
	for( Size i(0); i<n_absolute_nodes; ++i ) {
		vec7_[i].resize( n_interaction_graph_edges_by_abs_node[i], 0 );
		vec8_[i].resize( n_interaction_graph_edges_by_abs_node[i], 0 );
	}
	nedges_for_node_in_connectivity_graph_ = &vec5_;
	last_accepted_nedges_for_node_in_connectivity_graph_ = &vec6_;
	edges_for_node_in_connectivity_graph_ = &vec7_;
	last_accepted_edges_for_node_in_connectivity_graph_ = &vec8_;
}

/// @brief Make a copy of this object.
masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunctionScratchSpaceSP
GraphIslandCountCFScratchSpace::clone() const {
	return masala::make_shared< GraphIslandCountCFScratchSpace >( *this );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the name of this class.
/// @returns "GraphIslandCountCFScratchSpace".
std::string
GraphIslandCountCFScratchSpace::class_name_static() {
	return "GraphIslandCountCFScratchSpace";
}

/// @brief Get the namespace of this class.
/// @returns "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based".
std::string
GraphIslandCountCFScratchSpace::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based";
}

/// @brief Get the name of this class.
/// @returns "GraphIslandCountCFScratchSpace".
std::string
GraphIslandCountCFScratchSpace::class_name() const {
	return "GraphIslandCountCFScratchSpace";
}

/// @brief Get the namespace of this class.
/// @returns "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based".
std::string
GraphIslandCountCFScratchSpace::class_namespace() const {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based";
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the current state to the last accepted state.
void
GraphIslandCountCFScratchSpace::copy_last_accepted_to_current() {
	using masala::base::Size;
	(*current_candidate_solution_) = (*last_accepted_candidate_solution_);
	(*island_sizes_) = (*last_accepted_island_sizes_);
	copy_last_accepted_connectivity_graph_to_current();
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy the last accepted connectivity graph to that for the current state, then update it for the given state.
void
GraphIslandCountCFScratchSpace::update_connectivity_graph_for_current(
	std::vector< masala::base::Size > const & solution_in
) {
	copy_last_accepted_connectivity_graph_to_current();
	set_current_candidate_solution( solution_in );

	TODO TODO TODO UPDATE CURRENT GIVEN NEW SOLUTION;
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Accept the last move.
void
GraphIslandCountCFScratchSpace::protected_accept_last_move() {
	if( move_made_ ) {
		std::swap( last_accepted_candidate_solution_, current_candidate_solution_ );
		std::swap( last_accepted_island_sizes_, island_sizes_ );
		std::swap( last_accepted_nedges_for_node_in_connectivity_graph_, nedges_for_node_in_connectivity_graph_ );
		std::swap( last_accepted_edges_for_node_in_connectivity_graph_, edges_for_node_in_connectivity_graph_ );
		move_made_ = false;
		move_accepted_ = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the current candidate solution.  Throws if solution sizes don't match in debug mode.
/// @details Also updates the current 
void
GraphIslandCountCFScratchSpace::set_current_candidate_solution(
	std::vector< masala::base::Size > const & solution_in
) {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( solution_in.size() == current_candidate_solution_->size(), "set_current_candidate_solution",
		"Size mismatch in input candidate solution size (" + std::to_string( solution_in.size() ) + ") versus last current "
		"candidate solution (" + std::to_string( current_candidate_solution_->size() ) + ")."
	);
	move_made_ = true;
	(*current_candidate_solution_) = solution_in;
}

/// @brief Set the current connectivity graph to that of the last accepted state.
void
GraphIslandCountCFScratchSpace::copy_last_accepted_connectivity_graph_to_current() {
	using masala::base::Size;
	(*nedges_for_node_in_connectivity_graph_) = (*last_accepted_nedges_for_node_in_connectivity_graph_);
	for( Size i(0); i<edges_for_node_in_connectivity_graph_->size(); ++i ) {
		(*edges_for_node_in_connectivity_graph_)[i] = (*last_accepted_edges_for_node_in_connectivity_graph_)[i];
	}
}

} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
