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

/// @file src/optimizers/cost_function_network/cost_function/GraphBasedCostFunction.cc
/// @brief Implementations for a pure virtual base class for GraphBasedCostFunctions.
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

// Unit header:
#include <optimizers/cost_function_network/cost_function/GraphBasedCostFunction.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
template< typename T >
GraphBasedCostFunction<T>::GraphBasedCostFunction(
	GraphBasedCostFunction<T> const & src
) :
	Parent()
{
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
}

// @brief Assignment operator.
template< typename T >
GraphBasedCostFunction<T> &
GraphBasedCostFunction<T>::operator=(
	GraphBasedCostFunction<T> const & src
) {
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
	return *this;
}

/// @brief Destructor.  Not defaulted since we have to deallocate the matrices.
template< typename T >
GraphBasedCostFunction<T>::~GraphBasedCostFunction() {
	protected_clear();
}


////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction", "GraphBasedCostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
template< typename T >
std::vector< std::vector< std::string > >
GraphBasedCostFunction<T>::get_categories() const {
	std::vector< std::vector< std::string > > outvec( masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::get_categories() );
	outvec[0].push_back("GraphBasedCostFunction");
	return outvec;
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "GraphBasedCostFunction" } }.
template< typename T >
std::vector< std::vector< std::string > >
GraphBasedCostFunction<T>::get_data_representation_categories() const {
	return std::vector< std::vector< std::string > >{ { "CostFunction", "GraphBasedCostFunction" } };
}

/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
/// is compatible.
/// @returns {"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"}.
template< typename T >
std::vector< std::string >
GraphBasedCostFunction<T>::get_compatible_masala_engines() const {
	return std::vector< std::string >{
		"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"
	};
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible" }.
template< typename T >
std::vector< std::string >
GraphBasedCostFunction<T>::get_present_data_representation_properties() const {
	return std::vector< std::string > {
		"graph_based",
		"cost_function",
		"not_pairwise_decomposible"
	};
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
template< typename T >
std::vector< std::string >
GraphBasedCostFunction<T>::get_absent_data_representation_properties() const {
	return std::vector< std::string > {
		"pairwise_decomposible"
	};
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible" }
template< typename T >
std::vector< std::string >
GraphBasedCostFunction<T>::get_keywords() const {
	std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::get_keywords() );
	outvec.push_back( "graph_based" );
	outvec.push_back( "not_pairwise_decomposible" );
	return outvec;
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible" }
template< typename T >
std::vector< std::string >
GraphBasedCostFunction<T>::get_data_representation_keywords() const {
	std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::get_keywords() );
	outvec.push_back( "graph_based" );
	outvec.push_back( "not_pairwise_decomposible" );
	return outvec;
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get whether nodes' absolute index is one-based (true) or zero-based (false, the default).
/// @note Variable node indexing is always zero-based.
template< typename T >
bool
GraphBasedCostFunction<T>::one_based_absolute_node_indexing() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return use_one_based_node_indexing_;
}

/// @brief Get the total number of nodes.
template< typename T >
masala::base::Size
GraphBasedCostFunction<T>::absolute_node_count() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( full_choice_choice_interaction_graph_.rows() == full_choice_choice_interaction_graph_.cols(),
		"absolute_node_count", "The full choice interaction graph matrix was not square!  This is a program error that ought "
		"not to happen, so please consult a developer."
	);
	if( full_choice_choice_interaction_graph_.rows() == 0 ) return 0;
	return full_choice_choice_interaction_graph_.rows() - static_cast< masala::base::Size >( use_one_based_node_indexing_ );
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set whether nodes' absolute index is one-based (true) or zero-based (false, the default).
/// @details Throws if node-choice pair interacitons have already been input.
/// @note Variable node indexing is always zero-based.
template< typename T >
void
GraphBasedCostFunction<T>::set_one_based_absolute_node_indexing(
	bool const setting
) {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "set_one_based_absolute_node_indexing", "This function cannot be "
		"called after the " + class_name() + " object has been finalized."
	);
	CHECK_OR_THROW_FOR_CLASS( full_choice_choice_interaction_graph_.rows() == 0 && full_choice_choice_interaction_graph_.cols() == 0,
		"set_one_based_absolute_node_indexing", "The set_one_based_absolute_node_indexing() function cannot be called after "
		"the set_absolute_node_count() or declare_node_choice_pair_interaction() functions."
	);
	use_one_based_node_indexing_ = setting;
}

/// @brief Set the total number of nodes.
/// @details If the interaction graph is smaller than this count, it is enlarged.  If it is larger,
/// it is shrunk and any of the choice matrices that need to be deallocated are deallocated.  Throws if
/// object has been finalized.
template< typename T >
void
GraphBasedCostFunction<T>::set_absolute_node_count(
	masala::base::Size const absolute_node_count
) {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	protected_set_absolute_node_count( absolute_node_count );
}

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
template< typename T >
void
GraphBasedCostFunction<T>::declare_node_choice_pair_interaction(
	masala::base::Size const abs_nodeindex_1,
	masala::base::Size const abs_nodeindex_2,
	masala::base::Size const choiceindex_1,
	masala::base::Size const choiceindex_2,
	T const edge_value
) {
	using masala::base::Size;
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "declare_node_choice_pair_interaction", "This function cannot be "
		"called after the " + class_name() + " object has been finalized."
	);
	CHECK_OR_THROW_FOR_CLASS( abs_nodeindex_1 != abs_nodeindex_2, "declare_node_choice_pair_interaction", "Cannot have edges "	
		"between two choices for the same node, but got interactions between node " + std::to_string( abs_nodeindex_1)
		+ " and itself."
	);
	Size const firstindex( std::min( abs_nodeindex_1, abs_nodeindex_2 ) );
	Size const secondindex( std::max( abs_nodeindex_1, abs_nodeindex_2 ) );
	Size const choice1( firstindex == abs_nodeindex_1 ? choiceindex_1 : choiceindex_2 );
	Size const choice2( firstindex == abs_nodeindex_1 ? choiceindex_2 : choiceindex_1 );

	if( secondindex >= static_cast< Size >( full_choice_choice_interaction_graph_.cols() ) ) {
		protected_set_absolute_node_count( abs_nodeindex_2 );
	}

	if( full_choice_choice_interaction_graph_(firstindex, secondindex) == nullptr ) {
		full_choice_choice_interaction_graph_(firstindex, secondindex) = new Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >();
	}

	Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > * choicematrix( full_choice_choice_interaction_graph_(firstindex, secondindex) );
	if( static_cast< Size >( choicematrix->rows() ) <= choice1 || static_cast< Size >( choicematrix->cols() ) <= choice2 ) {
		Size const oldrows( choicematrix->rows() );
		Size const oldcols( choicematrix->cols() );
		Size const newrows( std::max( oldrows, choice1+1 ) );
		Size const newcols( std::max( oldcols, choice2+1 ) );
		choicematrix->conservativeResize( newrows, newcols );
		if( newrows > oldrows ) {
			for( Size i(oldrows); i<newrows; ++i ) {
				for( Size j(0); j<newcols; ++j ) {
					(*choicematrix)(i,j) = T(0);
				}
			}
		}
		if( newcols > oldcols ) {
			for( Size i(0); i<newrows; ++i ) {
				for( Size j(oldcols); j<newcols; ++j ) {
					(*choicematrix)(i,j) = T(0);
				}
			}
		}
	}

	(*choicematrix)(choice1, choice2) = edge_value;
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the total number of nodes.  Protected version, which performs no mutex-locking.  Intended
/// to be called from a mutex-locked context.
/// @details If the interaction graph is smaller than this count, it is enlarged.  If it is larger,
/// it is shrunk and any of the choice matrices that need to be deallocated are deallocated.  Throws if
/// object has been finalized.
template< typename T >
void
GraphBasedCostFunction<T>::protected_set_absolute_node_count(
	masala::base::Size const absolute_node_count
) {
	using masala::base::Size;
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "protected_set_absolute_node_count", "The " + class_name() + " object has "
		"already been finalized.  This function can only be called on an object that has not yet been finalized."
	);
	Size const oldsize( full_choice_choice_interaction_graph_.rows() );
	Size const absolute_row_count( absolute_node_count + static_cast< Size >( use_one_based_node_indexing_ ) );
	CHECK_OR_THROW_FOR_CLASS( oldsize == static_cast< Size >( full_choice_choice_interaction_graph_.cols() ),
		"protected_set_absolute_node_count", "The full choice-choice interaction graph is not square.  This is a program error."
	);
	if( absolute_row_count > oldsize ) {
		full_choice_choice_interaction_graph_.conservativeResize( absolute_row_count, absolute_row_count );
		for( Size i(oldsize); i<absolute_row_count; ++i ) {
			for( Size j(0); j<absolute_row_count; ++j ) {
				full_choice_choice_interaction_graph_(i,j) = nullptr;
			}
		}
		for( Size i(0); i<oldsize; ++i ) {
			for( Size j(oldsize); j<absolute_row_count; ++j ) {
				full_choice_choice_interaction_graph_(i,j) = nullptr;
			}
		}
	} else if( absolute_row_count < oldsize ) {
		for( Size i(absolute_row_count); i<oldsize; ++i ) {
			for( Size j(0); j<oldsize; ++j ) {
				if( full_choice_choice_interaction_graph_(i,j) != nullptr ) {
					delete ( full_choice_choice_interaction_graph_(i,j) );
					full_choice_choice_interaction_graph_(i,j) = nullptr;
				}
			}
			for( Size i(0); i<oldsize; ++i ) {
				for( Size j(absolute_row_count); j<oldsize; ++j ) {
					if( full_choice_choice_interaction_graph_(i,j) != nullptr ) {
						delete ( full_choice_choice_interaction_graph_(i,j) );
						full_choice_choice_interaction_graph_(i,j) = nullptr;
					}
				}
			}
		}
		full_choice_choice_interaction_graph_.conservativeResize( absolute_row_count, absolute_row_count );
	}
}

/// @brief Get the number of nodes, with no mutex-locking.
/// @note This is the total number of rows of the full_choice_choice_interaction_graph_ matrix, which may have an extra row
/// and column if we are using 1-based numbering.
template< typename T >
masala::base::Size
GraphBasedCostFunction<T>::protected_n_nodes_absolute() const {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( full_choice_choice_interaction_graph_.rows() == full_choice_choice_interaction_graph_.cols(),
		"protected_n_nodes_absolute", "The full choice interaction graph was not square.  This is a program error that ought not to "
		"happen, so please consult a developer."
	);
	return full_choice_choice_interaction_graph_.rows();
}

/// @brief Get the number of variable nodes, with no mutex-locking.
template< typename T >
masala::base::Size
GraphBasedCostFunction<T>::protected_n_nodes_variable() const {
	return n_variable_nodes_;
}

/// @brief Given an absolute node index, get the variable node index.
/// @details Throws if not yet finalized.  Does not lock mutex.  Returns a pair of
/// <is variable node, variable node index if variable or 0 otherwise>.
template< typename T >
std::pair< bool, masala::base::Size > const &
GraphBasedCostFunction<T>::protected_varnode_from_absnode(
	masala::base::Size const absnode_index
) const {
	using masala::base::Size;
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( absnode_index < protected_n_nodes_absolute(), "protected_varnode_from_absnode",
		"Absolute node index " + std::to_string(absnode_index) + " is out of range."
	);
	return varnodes_by_absnode_[absnode_index];
}

/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
/// @param[in] variable_node_indices A list of all of the absolute node indices
/// for nodes that have more than one choice, indexed by variable node index.
/// @details The base class function simply marks this object as finalized.  Should
/// be overridden, and overrides should call parent class protected_finalize().
template< typename T >
void
GraphBasedCostFunction<T>::protected_finalize(
	std::vector< masala::base::Size > const & variable_node_indices
) {
	using masala::base::Size;

	n_variable_nodes_ = variable_node_indices.size();

	varnodes_by_absnode_.resize( full_choice_choice_interaction_graph_.rows() );
	std::fill( varnodes_by_absnode_.begin(), varnodes_by_absnode_.end(), std::make_pair(false, 0) );
	for( Size i(0); i<variable_node_indices.size(); ++i ) {
		varnodes_by_absnode_[ variable_node_indices[i] ] = std::make_pair( true, i );
	}

	masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::protected_finalize( variable_node_indices );
}

/// @brief Is this data representation empty?
/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
/// @returns True if no data have been loaded into this data representation, false otherwise.
/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
template< typename T >
bool
GraphBasedCostFunction<T>::protected_empty() const {
return full_choice_choice_interaction_graph_.rows() == 0 &&
	full_choice_choice_interaction_graph_.cols() == 0 &&
	Parent::protected_empty();
}

/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
template< typename T >
void
GraphBasedCostFunction<T>::protected_clear() {
	using masala::base::Size;

	// Deallocate inner matrices:
	for( Size i(0); i<static_cast<Size>(full_choice_choice_interaction_graph_.rows()); ++i ) {
		for( Size j(0); j<static_cast<Size>(full_choice_choice_interaction_graph_.cols()); ++j ) {
			if( full_choice_choice_interaction_graph_(i, j) != nullptr ) {
				delete ( full_choice_choice_interaction_graph_(i, j) );
				full_choice_choice_interaction_graph_(i, j) = nullptr;
			}
		}
	}
	full_choice_choice_interaction_graph_.resize(0, 0);

	varnodes_by_absnode_.clear();

	Parent::protected_clear();
}

/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
template< typename T >
void
GraphBasedCostFunction<T>::protected_reset() {
	protected_clear();
	use_one_based_node_indexing_ = false;
	Parent::protected_reset();
}

/// @brief Override of protected_assign().  Calls parent function.
/// @details Throws if src is not a GraphBasedCostFunction.
template< typename T >
void
GraphBasedCostFunction<T>::protected_assign(
	masala::base::managers::engine::MasalaDataRepresentation const & src
) {
	using masala::base::Size;

	GraphBasedCostFunction<T> const * const src_cast_ptr( dynamic_cast< GraphBasedCostFunction<T> const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Cannot assign a GraphBasedCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

	protected_clear();

	use_one_based_node_indexing_ = src_cast_ptr->use_one_based_node_indexing_;

	full_choice_choice_interaction_graph_.resize( src_cast_ptr->full_choice_choice_interaction_graph_.rows(), src_cast_ptr->full_choice_choice_interaction_graph_.cols() );
	for( Size i(0); i<static_cast<Size>(full_choice_choice_interaction_graph_.rows()); ++i ) {
		for( Size j(0); j<static_cast<Size>(full_choice_choice_interaction_graph_.cols()); ++j ) {
			if( src_cast_ptr->full_choice_choice_interaction_graph_(i,j) == nullptr ) {
				full_choice_choice_interaction_graph_(i,j) = nullptr;
			} else {
				full_choice_choice_interaction_graph_(i,j) = new Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic >( *(src_cast_ptr->full_choice_choice_interaction_graph_(i,j))  );
			}
		}
	}

	varnodes_by_absnode_ = src_cast_ptr->varnodes_by_absnode_;

	Parent::protected_assign( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
template< typename T >
void
GraphBasedCostFunction<T>::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

template class GraphBasedCostFunction< bool >;
template class GraphBasedCostFunction< signed long int >;
template class GraphBasedCostFunction< masala::base::Size >;
template class GraphBasedCostFunction< masala::base::Real >;

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
