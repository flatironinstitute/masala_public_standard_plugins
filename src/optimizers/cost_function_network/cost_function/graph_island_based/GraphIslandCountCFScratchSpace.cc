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

	changed_variable_node_count_and_indices_.first = 0;
	changed_variable_node_count_and_indices_.second.resize( n_variable_nodes, 0 );

	drop_list_.resize( n_absolute_nodes, std::make_pair(0,0) );
	add_list_.resize( n_absolute_nodes, std::make_pair(0,0) );
}

/// @brief Make a copy of this object.
masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpaceSP
GraphIslandCountCFScratchSpace::clone() const {
	return masala::make_shared< GraphIslandCountCFScratchSpace >( *this );
}

/// @brief Make a fully independent copy of this object.
GraphIslandCountCFScratchSpaceSP
GraphIslandCountCFScratchSpace::deep_clone() const {
	GraphIslandCountCFScratchSpaceSP new_obj( masala::make_shared< GraphIslandCountCFScratchSpace >( *this ) );
	new_obj->protected_make_independent();
	return new_obj;
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

/// @brief Set the size of the drop and add lists to zero.
void
GraphIslandCountCFScratchSpace::clear_drop_and_add_lists() {
	drop_list_size_ = 0;
	add_list_size_ = 0;
}

/// @brief Indicate a connection between two absolute node indices to drop.
void
GraphIslandCountCFScratchSpace::indicate_drop(
	std::pair< masala::base::Size, masala::base::Size > const & pair_to_drop
) {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( drop_list_size_ < drop_list_.size(), "indicate_drop", "Drop list bounds exceeded.  This is a program error." );
	drop_list_[drop_list_size_] = pair_to_drop;
	++drop_list_size_;
}

/// @brief Indicate a connection between two absolute node indices to add.
void
GraphIslandCountCFScratchSpace::indicate_add(
	std::pair< masala::base::Size, masala::base::Size > const & pair_to_add
) {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( add_list_size_ < add_list_.size(), "indicate_add", "Add list bounds exceeded.  This is a program error." );
	add_list_[add_list_size_] = pair_to_add;
	++add_list_size_;
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy the last accepted connectivity graph to that for the current state, then copy in a new current state.
/// @note This does NOT update the connectivity graph for the current state.
void
GraphIslandCountCFScratchSpace::prepare_connectivity_graph_for_current(
	std::vector< masala::base::Size > const & solution_in
) {
	copy_last_accepted_connectivity_graph_to_current();
	set_current_candidate_solution( solution_in );
}

/// @brief Count the number of variable node indices that have changed from previous to current, update the internally-
/// stored count and indices, and return a reference to the count and indices.
std::pair< masala::base::Size, std::vector< masala::base::Size > > const &
GraphIslandCountCFScratchSpace::changed_variable_node_count_and_indices() {
	using masala::base::Size;
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( current_candidate_solution_->size() == last_accepted_candidate_solution_->size(),
		"changed_variable_node_count_and_indices", "Size mismatch between current (" + std::to_string( current_candidate_solution_->size() )
		+ ") and last accepted (" + std::to_string( last_accepted_candidate_solution_->size() ) + ") candidate solutions."
	);
	changed_variable_node_count_and_indices_.first = 0;
	for( Size i(0); i<current_candidate_solution_->size(); ++i ) {
		if( (*current_candidate_solution_)[i] != (*last_accepted_candidate_solution_)[i] ) {
			changed_variable_node_count_and_indices_.second[changed_variable_node_count_and_indices_.first] = i;
			++(changed_variable_node_count_and_indices_.first);
		}
	}
	return changed_variable_node_count_and_indices_;
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

/// @brief Make this object fully independent by deep-cloning all contained data.
void
GraphIslandCountCFScratchSpace::protected_make_independent() {
	if( current_candidate_solution_ != nullptr && *current_candidate_solution_ == vec2_ ) {
		current_candidate_solution_ = &vec2_;
		last_accepted_candidate_solution_ = &vec1_;
	} else {
		current_candidate_solution_ = &vec1_;
		last_accepted_candidate_solution_ = &vec2_;
	}

	if( island_sizes_ != nullptr && *island_sizes_ == vec4_ ) {
		island_sizes_ = &vec4_;
		last_accepted_island_sizes_ = &vec3_;
	} else {
		island_sizes_ = &vec3_;
		last_accepted_island_sizes_ = &vec4_;
	}

	if( nedges_for_node_in_connectivity_graph_ != nullptr && *nedges_for_node_in_connectivity_graph_ == vec6_ ) {
		nedges_for_node_in_connectivity_graph_ = &vec6_;
		last_accepted_nedges_for_node_in_connectivity_graph_ = &vec5_;
	} else {
		nedges_for_node_in_connectivity_graph_ = &vec5_;
		last_accepted_nedges_for_node_in_connectivity_graph_ = &vec6_;
	}

	if( edges_for_node_in_connectivity_graph_ != nullptr && *edges_for_node_in_connectivity_graph_ == vec8_ ) {
		edges_for_node_in_connectivity_graph_ = &vec8_;
		last_accepted_edges_for_node_in_connectivity_graph_ = &vec7_;
	} else {
		edges_for_node_in_connectivity_graph_ = &vec7_;
		last_accepted_edges_for_node_in_connectivity_graph_ = &vec8_;
	}

	Parent::protected_make_independent();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

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
