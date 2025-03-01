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
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/GraphBasedCostFunction.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>

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
GraphBasedCostFunction::GraphBasedCostFunction(
	GraphBasedCostFunction const & src
) :
	Parent()
{
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
}

// @brief Assignment operator.
GraphBasedCostFunction &
GraphBasedCostFunction::operator=(
	GraphBasedCostFunction const & src
) {
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
	return *this;
}

/// @brief Destructor.  Not defaulted since we have to deallocate the matrices.
GraphBasedCostFunction::~GraphBasedCostFunction() {
	protected_clear();
}


////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
GraphBasedCostFunction::get_categories() const {
	return masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::get_categories();
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "GraphBasedCostFunction" } }.
std::vector< std::vector< std::string > >
GraphBasedCostFunction::get_data_representation_categories() const {
	return std::vector< std::vector< std::string > >{ { "CostFunction", "GraphBasedCostFunction" } };
}

/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
/// is compatible.
/// @returns {"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"}.
std::vector< std::string >
GraphBasedCostFunction::get_compatible_masala_engines() const {
	return std::vector< std::string >{
		"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"
	};
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible" }.
std::vector< std::string >
GraphBasedCostFunction::get_present_data_representation_properties() const {
	return std::vector< std::string > {
		"graph_based",
		"cost_function",
		"not_pairwise_decomposible"
	};
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
std::vector< std::string >
GraphBasedCostFunction::get_absent_data_representation_properties() const {
	return std::vector< std::string > {
		"pairwise_decomposible"
	};
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible" }
std::vector< std::string >
GraphBasedCostFunction::get_keywords() const {
	std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::get_keywords() );
	outvec.push_back( "graph_based" );
	outvec.push_back( "not_pairwise_decomposible" );
	return outvec;
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible" }
std::vector< std::string >
GraphBasedCostFunction::get_data_representation_keywords() const {
	std::vector< std::string > outvec( masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::get_keywords() );
	outvec.push_back( "graph_based" );
	outvec.push_back( "not_pairwise_decomposible" );
	return outvec;
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
/// @param[in] variable_node_indices A list of all of the absolute node indices
/// for nodes that have more than one choice, indexed by variable node index.
/// @details The base class function simply marks this object as finalized.  Should
/// be overridden, and overrides should call parent class protected_finalize().
void
GraphBasedCostFunction::protected_finalize(
	std::vector< masala::base::Size > const & variable_node_indices
) {
	using masala::base::Size;

	// TODO TODO TODO
	// -- STORE ABS->VAR AND VAR->ABS.

	masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::protected_finalize( variable_node_indices );
}

/// @brief Is this data representation empty?
/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
/// @returns True if no data have been loaded into this data representation, false otherwise.
/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
bool
GraphBasedCostFunction::protected_empty() const {
return full_choice_choice_interaction_graph_.rows() == 0 &&
	full_choice_choice_interaction_graph_.cols() == 0 &&
	Parent::protected_empty();
}

/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
void
GraphBasedCostFunction::protected_clear() {
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

	Parent::protected_clear();
}

/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
void
GraphBasedCostFunction::protected_reset() {
	protected_clear();
	Parent::protected_reset();
}

/// @brief Override of protected_assign().  Calls parent function.
/// @details Throws if src is not a GraphBasedCostFunction.
void
GraphBasedCostFunction::protected_assign(
	masala::base::managers::engine::MasalaDataRepresentation const & src
) {
	using masala::base::Size;

	GraphBasedCostFunction const * const src_cast_ptr( dynamic_cast< GraphBasedCostFunction const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Cannot assign a GraphBasedCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

	protected_clear();

	full_choice_choice_interaction_graph_.resize( src_cast_ptr->full_choice_choice_interaction_graph_.rows(), src_cast_ptr->full_choice_choice_interaction_graph_.cols() );
	for( Size i(0); i<static_cast<Size>(full_choice_choice_interaction_graph_.rows()); ++i ) {
		for( Size j(0); j<static_cast<Size>(full_choice_choice_interaction_graph_.cols()); ++j ) {
			if( src_cast_ptr->full_choice_choice_interaction_graph_(i,j) == nullptr ) {
				full_choice_choice_interaction_graph_(i,j) = nullptr;
			} else {
				full_choice_choice_interaction_graph_(i,j) = new Eigen::Matrix< bool, Eigen::Dynamic, Eigen::Dynamic >( *(src_cast_ptr->full_choice_choice_interaction_graph_(i,j))  );
			}
		}
	}

	Parent::protected_assign( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
void
GraphBasedCostFunction::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
