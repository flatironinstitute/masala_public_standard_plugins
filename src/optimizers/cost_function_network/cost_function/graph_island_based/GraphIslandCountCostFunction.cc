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

/// @file src/optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCostFunction.cc
/// @brief Implementation for a class for GraphIslandCountCostFunctions.
/// @details GraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCostFunction.hh>

// Optimizers headers:
#include <optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCFScratchSpace.hh>

// STL headers:
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers
//#include <ostream> // DELETE ME -- FOR DEBUGGING ONLY

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace graph_island_based {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
GraphIslandCountCostFunction::GraphIslandCountCostFunction(
	GraphIslandCountCostFunction const & src
) :
	Parent()
{
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
}

// @brief Assignment operator.
GraphIslandCountCostFunction &
GraphIslandCountCostFunction::operator=(
	GraphIslandCountCostFunction const & src
) {
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
	return *this;
}

// /// @brief Make a copy of this object.
// masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP
// GraphIslandCountCostFunction::clone() const {
// 	return masala::make_shared< GraphIslandCountCostFunction >( *this );
// }

// /// @brief Make a copy of this object that is fully independent.
// GraphIslandCountCostFunctionSP
// GraphIslandCountCostFunction::deep_clone() const {
// 	GraphIslandCountCostFunctionSP new_object( std::static_pointer_cast< GraphIslandCountCostFunction >( this->clone() ) );
// 	new_object->make_independent();
// 	return new_object;
// }

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
GraphIslandCountCostFunction::get_categories() const {
	std::vector< std::vector< std::string > > outvec( GraphBasedCostFunction::get_categories() );
	outvec[0].push_back("GraphIslandCountCostFunction");
	return outvec;
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction" } }.
std::vector< std::vector< std::string > >
GraphIslandCountCostFunction::get_data_representation_categories() const {
	return std::vector< std::vector< std::string > >{ { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction" } };
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based" }
std::vector< std::string >
GraphIslandCountCostFunction::get_data_representation_keywords() const {
	std::vector< std::string > outvec( Parent::get_data_representation_keywords() );
	outvec.push_back( "graph_island_count_based" );
	return outvec;
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based" }.
std::vector< std::string >
GraphIslandCountCostFunction::get_present_data_representation_properties() const {
	return std::vector< std::string >{ "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based" };
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
std::vector< std::string >
GraphIslandCountCostFunction::get_absent_data_representation_properties() const {
	return std::vector< std::string >{ "pairwise_decomposible" };
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based" }
std::vector< std::string >
GraphIslandCountCostFunction::get_keywords() const {
	std::vector< std::string > outvec( Parent::get_keywords() );
	outvec.push_back( "graph_island_count_based" );
	return outvec;
}

/// @brief Get the name of this class ("GraphIslandCountCostFunction").
/// @details Static version.
std::string
GraphIslandCountCostFunction::class_name_static() {
	return "GraphIslandCountCostFunction";
}

/// @brief Get the name of this class ("GraphIslandCountCostFunction").
std::string
GraphIslandCountCostFunction::class_name() const {
	return class_name_static();
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based").
/// @details Static version.
std::string
GraphIslandCountCostFunction::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based";
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizersn::cost_function_network::cost_function").
std::string
GraphIslandCountCostFunction::class_namespace() const {
	return class_namespace_static();
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the minimum number of nodes that must be in a connected island in the connection graph in order
/// for the island to be counted.
/// @details The default minimum size is 2 nodes.
masala::base::Size
GraphIslandCountCostFunction::min_island_size() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return protected_min_island_size();
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the minimum number of nodes that must be in a connected island in the connection graph in order
/// for the island to be counted.
/// @details The default minimum size is 2 nodes.
void
GraphIslandCountCostFunction::set_min_island_size(
	masala::base::Size const setting
) {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "set_min_island_size", "This function cannot be set after the " +
		class_name() + " object has been finalized."
	);
	min_island_size_ = setting;
}

/// @brief Declare that two particular choices at two different absolute node indices interact, and set the boolean value
/// for the edge to "true".
/// @details If the node pair has not yet been declared, this declares it.  If the size of the matrix at the two
/// absolute residue indices is smaller than the choice indices, this resizes the matrix to the size of the choice
/// indices.  Calls the parent class declare_node_choice_pair_interaction() function and passes "true" for the fifth
/// parameter.
/// @param[in] abs_nodeindex_1 The absolute index of the first node (variable or not).
/// @param[in] abs_nodeindex_2 The absolute index of the second node (variable or not).
/// @param[in] choiceindex_1 The absolute index of the choice at the first node (or 0 for a non-variable node).
/// @param[in] choiceindex_2 The absolute index of the choice at the second node (or 0 for a non-variable node).
void
GraphIslandCountCostFunction::declare_node_choice_pair_interaction(
	masala::base::Size const abs_nodeindex_1,
	masala::base::Size const abs_nodeindex_2,
	masala::base::Size const choiceindex_1,
	masala::base::Size const choiceindex_2
) {
	Parent::declare_node_choice_pair_interaction( abs_nodeindex_1, abs_nodeindex_2, choiceindex_1, choiceindex_2, true );
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Generate a GraphIslandCountCFScratchSpace for this cost function.
masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpaceSP
GraphIslandCountCostFunction::generate_cost_function_scratch_space() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "generate_cost_function_scratch_space", "This " + class_name() + " object must be finalized "
		"before this function can be called."
	);
	return masala::make_shared< GraphIslandCountCFScratchSpace >( protected_n_nodes_absolute(), protected_n_nodes_variable(), n_interaction_graph_edges_by_abs_node_ );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Compute a vector of island sizes.
/// @details Uses a depth-first algorithm.  Throws if object not finalized first.  Performs no mutex-locking.
/// @param[in] candidate_solution The current solution, as a vector of variable node choice indices.
/// @param[inout] scratch_space A reference to the thread-local scratch space for repeated evaluation of this GraphIslandCountCostFunction.
void
GraphIslandCountCostFunction::protected_compute_island_sizes(
	std::vector< masala::base::Size > const & candidate_solution,
	GraphIslandCountCFScratchSpace & scratch_space
) const {
	using masala::base::Size;

	Size const nnodes( protected_n_nodes_absolute() ); // Will include extra if zero-based.
	bool const use_onebased( protected_use_one_based_node_indexing() );
	if( nnodes == 0 || (use_onebased && nnodes == 1) ) return; // Do nothing if we have no nodes.

	if( scratch_space.at_least_one_move_accepted() && scratch_space.last_accepted_candidate_solution_const() == candidate_solution ) {
		scratch_space.copy_last_accepted_to_current();
		return;
	}

	// Objects in scratch space we will use:
	std::vector< Size > & island_sizes( scratch_space.island_sizes() );

	// Compute the current connectivity graph.  This is stack-allocated, and should be small, though it is worst case O(N^2) in memory.
	// N will likely be << 1000; N^2 will likely be less than a megabyte of memory.  If this ever becomes an issue, we can
	// revisit this.  This is also unlikely to be an issue due to sparsity of the graph: we are only allocating space for the edges
	// in the interaction graph, not for edges between all nodes.
	// Size * const nedges_for_node_in_connectivity_graph = static_cast< Size * >( alloca( sizeof(Size) * nnodes ) );
	// Size ** const edges_for_node_in_connectivity_graph = static_cast< Size ** >( alloca( sizeof(Size *) * nnodes ) );

	std::vector< Size > & nedges_for_node_in_connectivity_graph( scratch_space.nedges_for_node_in_connectivity_graph() );
	std::vector< std::vector< Size > > & edges_for_node_in_connectivity_graph( scratch_space.edges_for_node_in_connectivity_graph() );
	
	if( (!scratch_space.at_least_one_move_accepted()) ) {
		// Compute from scratch if we have accepted no moves.
		for( Size i(0); i<nnodes; ++i ) {
			nedges_for_node_in_connectivity_graph[i] = 0;
		}
		for( auto const & entry : interacting_abs_node_indices_ ) {
			Eigen::Matrix< bool, Eigen::Dynamic, Eigen::Dynamic > const * const ij_matrix( protected_choice_choice_interaction_graph_for_nodepair( entry.first, entry.second ) );
			DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( ij_matrix != nullptr, "protected_compute_island_sizes", "The interaction graph matirx was null.  "
					"This is a program error: it ought not to be able to happen.  Please consult a developer."
			);
			std::pair< bool, Size > const & varnode_i( protected_varnode_from_absnode( entry.first ) );
			Size const choice_i( varnode_i.first ? candidate_solution[varnode_i.second] : 0 );
			std::pair< bool, Size > const & varnode_j( protected_varnode_from_absnode( entry.second ) );
			Size const choice_j( varnode_j.first ? candidate_solution[varnode_j.second] : 0 );
			if(
				static_cast<Size>(ij_matrix->rows()) > choice_i &&
				static_cast<Size>(ij_matrix->cols()) > choice_j &&
				(*ij_matrix)( choice_i, choice_j )
			) {
				edges_for_node_in_connectivity_graph[entry.first][ nedges_for_node_in_connectivity_graph[entry.first] ] = entry.second;
				edges_for_node_in_connectivity_graph[entry.second][ nedges_for_node_in_connectivity_graph[entry.second] ] = entry.first;
				nedges_for_node_in_connectivity_graph[entry.first] += 1;
				nedges_for_node_in_connectivity_graph[entry.second] += 1;
			}
		}
		scratch_space.set_current_candidate_solution( candidate_solution );
	} else {
		// Otherwise, update from last accepted.
		scratch_space.prepare_connectivity_graph_for_current( candidate_solution );
		std::pair< Size, std::vector< Size > > const & changed_variable_nodes( scratch_space.changed_variable_node_count_and_indices() );
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( changed_variable_nodes.first > 0, "protected_compute_island_sizes", "Expected at least one changed node!" );
		for( Size i(0); i<changed_variable_nodes.first; ++i ) {
			scratch_space.clear_drop_and_add_lists();
			Size const abs_node_index( protected_absnode_from_varnode( changed_variable_nodes.second[i] ) );
			Size const old_choiceindex( scratch_space.last_accepted_candidate_solution_const()[i] );
			Size const new_choiceindex( candidate_solution[i] );
			for( Size j(0); j<n_interaction_graph_edges_by_abs_node_[abs_node_index]; ++j ) {
				Size const other_abs_node( interaction_partners_of_abs_node_[abs_node_index][j] );
				std::pair< bool, Size > const & other_var_node( protected_varnode_from_absnode( other_abs_node ) );
				Size const old_choiceindex_other( other_var_node.first ? scratch_space.last_accepted_candidate_solution_const()[other_var_node.second] : 0 );
				Size const new_choiceindex_other( other_var_node.first ? candidate_solution[other_var_node.second] : 0 );
				Size const firstnode( std::min(abs_node_index, other_abs_node) );
				Size const secondnode( std::max(abs_node_index, other_abs_node) );
				Size const old_firstchoice( abs_node_index < other_abs_node ? old_choiceindex : old_choiceindex_other );
				Size const new_firstchoice( abs_node_index < other_abs_node ? new_choiceindex : new_choiceindex_other );
				Size const old_secondchoice( abs_node_index >= other_abs_node ? old_choiceindex : old_choiceindex_other );
				Size const new_secondchoice( abs_node_index >= other_abs_node ? new_choiceindex : new_choiceindex_other );
				Eigen::Matrix< bool, Eigen::Dynamic, Eigen::Dynamic > const * const ij_matrix(
					protected_choice_choice_interaction_graph_for_nodepair( firstnode, secondnode )
				);
				DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( ij_matrix != nullptr, "protected_compute_island_sizes", "Program error.  Expected a non-null pointer to the I/J matrix." );
				bool const connected_old(
					static_cast<Size>(ij_matrix->rows()) > old_firstchoice && static_cast<Size>(ij_matrix->cols()) > old_secondchoice ?
					(*ij_matrix)( old_firstchoice, old_secondchoice ) :
					false
				);
				bool const connected_new(
					static_cast<Size>(ij_matrix->rows()) > new_firstchoice && static_cast<Size>(ij_matrix->cols()) > new_secondchoice ?
					(*ij_matrix)( new_firstchoice, new_secondchoice ) :
					false
				);
				if( connected_old && (!connected_new) ) {
					scratch_space.indicate_drop( std::make_pair( firstnode, secondnode ) );
				} else if( connected_new && (!connected_old) ) {
					scratch_space.indicate_add( std::make_pair( firstnode, secondnode ) );
				}
			}

			for( Size i(0); i<scratch_space.drop_list_size(); ++i ) {
				write_to_tracer( "Dropping " + std::to_string( scratch_space.drop_list()[i].first ) + "-" + std::to_string( scratch_space.drop_list()[i].second ) ); // DELETE ME -- FOR DEBUGGING ONLY
				do_drop( scratch_space.drop_list()[i], scratch_space.nedges_for_node_in_connectivity_graph(), scratch_space.edges_for_node_in_connectivity_graph() );
			}
			for( Size i(0); i<scratch_space.add_list_size(); ++i ) {
				write_to_tracer( "Adding " + std::to_string( scratch_space.add_list()[i].first ) + "-" + std::to_string( scratch_space.add_list()[i].second ) ); // DELETE ME -- FOR DEBUGGING ONLY
				do_add( scratch_space.add_list()[i], scratch_space.nedges_for_node_in_connectivity_graph(), scratch_space.edges_for_node_in_connectivity_graph() );
			}

			// DELETE THE FOLLOWING -- FOR DEBUGGING ONLY:
			std::stringstream ss;
			ss << "Node\tOld_Choice\tNew_Choice\tOld_Connections\tNew_Connections" << std::endl;
			for(Size i( static_cast<Size>(protected_use_one_based_node_indexing()) ); i<scratch_space.nedges_for_node_in_connectivity_graph().size(); ++i ) {
				ss << i << "\t" << scratch_space.last_accepted_candidate_solution_const()[i-static_cast<Size>(protected_use_one_based_node_indexing())] << "\t" << candidate_solution[i-static_cast<Size>(protected_use_one_based_node_indexing())] << "\t";
				for(Size j(0); j<scratch_space.last_accepted_nedges_for_node_in_connectivity_graph_const()[i]; ++j) {
					if(j>0) { ss << ","; }
					ss << scratch_space.last_accepted_edges_for_node_in_connectivity_graph_const()[i][j];
				}
				ss << "\t";
				for(Size j(0); j<scratch_space.nedges_for_node_in_connectivity_graph_const()[i]; ++j) {
					if(j>0) { ss << ","; }
					ss << scratch_space.edges_for_node_in_connectivity_graph_const()[i][j];
				}
				ss << std::endl;
			}
			write_to_tracer( ss.str() );
		}
	}

	// Storage for whether we have discovered each node.  Automatically deallocated at function's end since
	// this is stack-allocated with alloca().
	bool * const node_discovered = static_cast< bool * >( alloca( sizeof(bool) * nnodes ) );

	// Initialize the island_sizes array to be all 1.  We change it to 0 when a node is incorporated into an island
	// (unless it is the first node in the island, in which case its entry stores the number of connected components).
	for( Size i(0); i<nnodes; ++i ) {
		island_sizes[i] = 1;
		node_discovered[i] = false;
	}
	if( use_onebased ) {
		island_sizes[0] = 0;
		node_discovered[0] = true;
	}

	// Scratch space used by this function:
	// We use this to make a list of nodes to check in the depth-first search in order to avoid a recursive function.
	// - node_sizearray is used to store indices of nodes to be checked.
	// - stackend indicates the address in node_sizearray that's one past the end of the stack to be checked.
	Size stackend(0);
	Size * node_sizearray = static_cast< Size * >( alloca( sizeof(Size) * nnodes ) );
	
	for( Size i( static_cast<Size>(protected_use_one_based_node_indexing())); i<nnodes; ++i ) {
		// std::cout << "NODE " << i << ": " << ( node_discovered[i] ? "DISCOVERED" : "UNDISCOVERED" ) << std::endl; // DELETE ME -- FOR DEBUGGING ONLY.
		if( node_discovered[i] ) { continue; } // This position is already part of an island.
		
		node_sizearray[0] = i;
		node_discovered[i] = true;
		stackend = 1;

		while( stackend > 0 ) {
			// std::cout << "Stack array: "; // DELETE ME -- FOR DEBUGGING ONLY.
			// for( Size j(0); j<stackend; ++j ) { std::cout << node_sizearray[j] << " "; } // DELETE ME -- FOR DEBUGGING ONLY.
			// std::cout << std::endl; // DELETE ME -- FOR DEBUGGING ONLY.

			--stackend;
			// The following function:
			/// - Finds all the nodes that are connected to the node given by the second argument.
			/// - Skips those that have already been visited.
			/// - Appends the rest to the node_sizearray, incrementing stackend.
			/// - Increments the ith element of island_sizes with the number of connected nodes appended.
			/// - Sets the connected nodes to 0 in island_sizes, and true in node_discovered.
			push_connected_undiscovered_nodes(
				i, node_sizearray[stackend], stackend,
				node_sizearray, island_sizes, node_discovered,
				nedges_for_node_in_connectivity_graph, edges_for_node_in_connectivity_graph
			);
		}
	}

#ifndef NDEBUG
	// Sanity check in debug mode: make sure all nodes were discovered, and that the sum of all island sizes is nnodes.
	Size accumulator(0);
	for( Size i(0); i<nnodes; ++i ) {
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( node_discovered[i], "protected_compute_island_sizes", "Node " + std::to_string(i)
			+ " was somehow not discovered by this function!  This is a program error that ought not to happen.  Please consult "
			"a developer."
		);
		accumulator += island_sizes[i];
	}
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( accumulator + static_cast<Size>(use_onebased) == nnodes, "protected_compute_island_sizes", "Expected the sum of all island "
		"sizes to be " + std::to_string( nnodes - static_cast<Size>(use_onebased) ) +  " but it was " + std::to_string( accumulator ) + ".  This is a program error "
		"that ought not to happen.  Please consult a developer."
	);
#endif
}

/// @brief Get the minimum number of nodes that must be in a connected island in the connection graph in order
/// for the island to be counted.  This version performs no mutex-locking.
/// @details The default minimum size is 2 nodes.
masala::base::Size
GraphIslandCountCostFunction::protected_min_island_size() const {
	return min_island_size_;
}

/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
/// @param[in] variable_node_indices A list of all of the absolute node indices
/// for nodes that have more than one choice, indexed by variable node index.
/// @details The base class function simply marks this object as finalized.  Should
/// be overridden, and overrides should call parent class protected_finalize().
void
GraphIslandCountCostFunction::protected_finalize(
	std::vector< masala::base::Size > const & variable_node_indices
) {
	using masala::base::Size;

	// Compute the interacting node pairs
	interacting_abs_node_indices_.clear();
	n_interaction_graph_edges_by_abs_node_.clear();
	interaction_partners_of_abs_node_.clear();
	masala::base::Size const nnodes( protected_n_nodes_absolute() );
	n_interaction_graph_edges_by_abs_node_.resize( nnodes, 0 );
	interaction_partners_of_abs_node_.resize(nnodes);
	for( Size i( static_cast<Size>(protected_use_one_based_node_indexing()) ); i<nnodes-1; ++i ) {
		for( Size j(i+1); j<nnodes; ++j ) {
			if( protected_choice_choice_interaction_graph_for_nodepair(i,j) != nullptr ) {
				interacting_abs_node_indices_.push_back( std::make_pair(i,j) );
				++n_interaction_graph_edges_by_abs_node_[i];
				++n_interaction_graph_edges_by_abs_node_[j];
				interaction_partners_of_abs_node_[i].push_back(j);
				interaction_partners_of_abs_node_[j].push_back(i);
			}
		}
	}
	interacting_abs_node_indices_.shrink_to_fit();

	Parent::protected_finalize( variable_node_indices );
}

/// @brief Override of assign_protected_assignmutex_locked().  Calls parent function.
/// @details Throws if src is not a GraphIslandCountCostFunction.
void
GraphIslandCountCostFunction::protected_assign(
	masala::base::managers::engine::MasalaDataRepresentation const & src
) {
	GraphIslandCountCostFunction const * const src_cast_ptr( dynamic_cast< GraphIslandCountCostFunction const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Cannot assign a GraphIslandCountCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

	min_island_size_ = src_cast_ptr->min_island_size_;
	interacting_abs_node_indices_ = src_cast_ptr->interacting_abs_node_indices_;
	n_interaction_graph_edges_by_abs_node_ = src_cast_ptr->n_interaction_graph_edges_by_abs_node_;
	// TODO COPY DATA HERE.

	Parent::protected_assign( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
void
GraphIslandCountCostFunction::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

/// @brief Is this data representation empty?
/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
/// @returns True if no data have been loaded into this data representation, false otherwise.
/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
bool
GraphIslandCountCostFunction::protected_empty() const {
	return // TODO STUFF HERE &&
		Parent::protected_empty();
}

/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
void
GraphIslandCountCostFunction::protected_clear() {
	// TODO CLEAR DATA HERE
	interacting_abs_node_indices_.clear();
	n_interaction_graph_edges_by_abs_node_.clear();
	Parent::protected_clear();
}

/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
void
GraphIslandCountCostFunction::protected_reset() {
	protected_clear();
	min_island_size_ = 2;
	Parent::protected_reset();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief This function:
/// - Finds all the nodes that are connected to the node given by the second argument.
/// - Skips those that have already been visited.
/// - Appends the rest to the node_sizearray, incrementing stackend.
/// - Increments the ith element of island_sizes with the number of connected nodes appended.
/// - Sets the connected nodes to 0 in island_sizes, and true in node_discovered.
void
GraphIslandCountCostFunction::push_connected_undiscovered_nodes(
	masala::base::Size const root_of_current_island,
	masala::base::Size const current_node,
	masala::base::Size & stackend,
	masala::base::Size * node_sizearray,
	std::vector< masala::base::Size > & island_sizes,
	bool * node_discovered,
	std::vector< masala::base::Size > const & nedges_for_node_in_connectivity_graph,
	std::vector< std::vector< masala::base::Size > > const & edges_for_node_in_connectivity_graph
) const {
	using masala::base::Size;
	std::vector< Size > const & edges_for_curnode( edges_for_node_in_connectivity_graph[current_node] );
	for( Size iother_index( 0 ); iother_index < nedges_for_node_in_connectivity_graph[current_node] ; ++iother_index ) {
		Size const iother( edges_for_curnode[iother_index] );
		if( node_discovered[iother] == false ) {
			// If the current choices at iother and current_node interact...
			island_sizes[iother] = 0;
			++(island_sizes[root_of_current_island]);
			node_discovered[iother] = true;
			node_sizearray[stackend] = iother;
			++stackend;
		}
	}
}

/// @brief Drop an edge from the connectivity graph.
void
GraphIslandCountCostFunction::do_drop(
	std::pair< masala::base::Size, masala::base::Size > const & pair_to_drop,
	std::vector< masala::base::Size > & nedges_for_node_in_connectivity_graph,
	std::vector< std::vector< masala::base::Size > > & edges_for_node_in_connectivity_graph
) const {
	using masala::base::Size;

	for( Size i(0); i<nedges_for_node_in_connectivity_graph[pair_to_drop.first]; ++i ) {
		if( edges_for_node_in_connectivity_graph[pair_to_drop.first][i] == pair_to_drop.second ) {
			for( Size j(i+1); j<nedges_for_node_in_connectivity_graph[pair_to_drop.first]; ++j ) {
				edges_for_node_in_connectivity_graph[pair_to_drop.first][j-1]=edges_for_node_in_connectivity_graph[pair_to_drop.first][j];
			}
			--(nedges_for_node_in_connectivity_graph[pair_to_drop.first]);
			break;
		}
	}
	for( Size i(0); i<nedges_for_node_in_connectivity_graph[pair_to_drop.second]; ++i ) {
		if( edges_for_node_in_connectivity_graph[pair_to_drop.second][i] == pair_to_drop.first ) {
			for( Size j(i+1); j<nedges_for_node_in_connectivity_graph[pair_to_drop.second]; ++j ) {
				edges_for_node_in_connectivity_graph[pair_to_drop.second][j-1]=edges_for_node_in_connectivity_graph[pair_to_drop.second][j];
			}
			--(nedges_for_node_in_connectivity_graph[pair_to_drop.second]);
			break;
		}
	}
}

/// @brief Add an edge to the connectivity graph.
void
GraphIslandCountCostFunction::do_add(
	std::pair< masala::base::Size, masala::base::Size > const & pair_to_add,
	std::vector< masala::base::Size > & nedges_for_node_in_connectivity_graph,
	std::vector< std::vector< masala::base::Size > > & edges_for_node_in_connectivity_graph
) const {
	using masala::base::Size;
	bool found1(false);
#ifndef NDEBUG
	bool found2(false);
#endif
	for( Size i(0); i<nedges_for_node_in_connectivity_graph[pair_to_add.first]; ++i ) {
		if( edges_for_node_in_connectivity_graph[pair_to_add.first][i] == pair_to_add.second ) {
			found1 = true;
			break;
		}
	}
#ifndef NDEBUG
	for( Size i(0); i<nedges_for_node_in_connectivity_graph[pair_to_add.second]; ++i ) {
		if( edges_for_node_in_connectivity_graph[pair_to_add.second][i] == pair_to_add.first ) {
			found2 = true;
			break;
		}
	}
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( found1 == found2, "do_add", "Program error: asymmetric edge found." );
#endif
	if( found1 ) {
		edges_for_node_in_connectivity_graph[pair_to_add.first][nedges_for_node_in_connectivity_graph[pair_to_add.first]] = pair_to_add.second;
		edges_for_node_in_connectivity_graph[pair_to_add.second][nedges_for_node_in_connectivity_graph[pair_to_add.second]] = pair_to_add.first;
		++(nedges_for_node_in_connectivity_graph[pair_to_add.first]);
		++(nedges_for_node_in_connectivity_graph[pair_to_add.second]);
	}

}

} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
