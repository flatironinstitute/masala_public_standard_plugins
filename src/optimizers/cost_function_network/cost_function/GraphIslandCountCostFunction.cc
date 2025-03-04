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

/// @file src/optimizers/cost_function_network/cost_function/GraphIslandCountCostFunction.cc
/// @brief Implementation for a class for GraphIslandCountCostFunctions.
/// @details GraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/GraphIslandCountCostFunction.hh>

// STL headers:
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/utility/container/container_util.tmpl.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

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

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function").
/// @details Static version.
std::string
GraphIslandCountCostFunction::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function";
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

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


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
	// TODO ANY NEEDED FINALIZATION HERE

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


} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
