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

/// @file src/optimizers/cost_function_network/cost_function/SquareRootOfGraphIslandCountCostFunction.cc
/// @brief Implementation for a class for SquareRootOfGraphIslandCountCostFunctions.
/// @details SquareRootOfGraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph, sum the counts in the islands over a minimum, and
/// return the square root of the sum.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/SquareRootOfGraphIslandCountCostFunction.hh>

// STL headers:
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// Base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_FourInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_TwoInput.tmpl.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
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
SquareRootOfGraphIslandCountCostFunction::SquareRootOfGraphIslandCountCostFunction(
	SquareRootOfGraphIslandCountCostFunction const & src
) :
	Parent()
{
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
}

// @brief Assignment operator.
SquareRootOfGraphIslandCountCostFunction &
SquareRootOfGraphIslandCountCostFunction::operator=(
	SquareRootOfGraphIslandCountCostFunction const & src
) {
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
	return *this;
}

/// @brief Make a copy of this object.
masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP
SquareRootOfGraphIslandCountCostFunction::clone() const {
	return masala::make_shared< SquareRootOfGraphIslandCountCostFunction >( *this );
}

/// @brief Make a copy of this object that is fully independent.
SquareRootOfGraphIslandCountCostFunctionSP
SquareRootOfGraphIslandCountCostFunction::deep_clone() const {
	SquareRootOfGraphIslandCountCostFunctionSP new_object( std::static_pointer_cast< SquareRootOfGraphIslandCountCostFunction >( this->clone() ) );
	new_object->make_independent();
	return new_object;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////
	
/// @brief Get a description of the API of this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
SquareRootOfGraphIslandCountCostFunction::get_api_definition() {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::base::api;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;

	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	if( api_definition_mutex_locked() == nullptr ) {
		
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this, "A cost function which computes the sum of the sizes of islands "
				"over a given size threshold and returns the negated sum of the square roots.",
				false, false
			)
		);

		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( SquareRootOfGraphIslandCountCostFunction, api_def );

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"one_based_absolute_node_indexing",
				"Get whether nodes' absolute index is one-based (true) or zero-based (false, the default).  Note that variable "
				"node indexing is always zero-based.",
				"one_based_absolute_node_indexing", "True if the absolute node index is one-based, false if it is zero-based (the default).",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::one_based_absolute_node_indexing,
					this
				)
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"absolute_node_count",
				"Get the total number of nodes.",
				"absolute_node_count", "The total number of nodes.",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::absolute_node_count,
					this
				)
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"min_island_size",
				"Get the minimum number of nodes in an island in order for that island to contribute to the penalty function "
				"value.  If the number of nodes is greater than or equal to this value, this value is subtracted from the count "
				"and the result's square root is computed.  The square roots are summed and negated to compute the penalty value.",
				"min_island_size", "The minimum island size.",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::min_island_size,
					this
				)
			)
		);

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_min_island_size",
				"Set the minimum number of nodes in an island in order for that island to contribute to the penalty function "
				"value.  If the number of nodes is greater than or equal to this value, this value is subtracted from the count "
				"and the result's square root is computed.  The square roots are summed and negated to compute the penalty value.",
				"min_island_size_in", "The minimum island size to set.",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::set_min_island_size,
					this,
					std::placeholders::_1
				)
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< bool > >(
				"set_one_based_absolute_node_indexing",
				"Set whether nodes' absolute index is one-based (true) or zero-based (false, the default).  Note that variable "
				"node indexing is always zero-based.  Throws if node-choice pair interacitons have already been input.",
				"one_based_absolute_node_indexing_in", "The setting: true if the absolute node index is one-based, false if "
				"it is zero-based (the default).",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::set_one_based_absolute_node_indexing,
					this,
					std::placeholders::_1
				)
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_absolute_node_count",
				"Set the total number of nodes.  If the interaction graph is smaller than this count, it is "
				"enlarged.  If it is larger, it is shrunk and any of the choice matrices that need to be "
				"deallocated are deallocated.  Throws if object has been finalized.",
				"absolute_node_count", "The total number of nodes to set.",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::set_absolute_node_count,
					this,
					std::placeholders::_1
				)
			)
		);
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_FourInput< Size, Size, Size, Size > >(
				"declare_node_choice_pair_interaction",
				"Declare that two particular choices at two different absolute node indices interact. If the node pair "
				"has not yet been declared, this declares it.  If the size of the matrix at the two absolute residue "
				"indices is smaller than the choice indices, this resizes the matrix to the size of the choice indices.",
				"abs_nodeindex_1", "The absolute index of the first node (variable or not).",
				"abs_nodeindex_2", "The absolute index of the second node (variable or not).",
				"choiceindex_1", "The absolute index of the choice at the first node (or 0 for a non-variable node).",
				"choiceindex_2", "The absolute index of the choice at the second node (or 0 for a non-variable node).",
				false, false,
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::declare_node_choice_pair_interaction,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					std::placeholders::_4
				)
			)
		);

		// Work functions:
		api_def->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput < Real, std::vector< Size > const & > >(
				"compute_cost_function", "Compute the cost function: find the size of each island in the interaction graph over "
				"threshold, compute the square root of the sizes, sum them, and negate the result.  No mutex-locking is performed.",
				true, false, false, true,
				"cost_function_value", "The value of the cost function, computed for the current candidate solution.",
				"candidate_solution", "The candidate solution, expressed as a vector of choices for the variable nodes only.",
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::compute_cost_function,
					this,
					std::placeholders::_1
				)
			)
		);
		api_def->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_TwoInput < Real, std::vector< Size > const &, std::vector< Size > const & > >(
				"compute_cost_function_difference", "Compute the cost function difference: for each of two input vectors, find the size of each "
				"island in the interaction graph over threshold, compute the square root of the sizes, sum them, negate the result, and return the difference.  "
				"No mutex-locking is performed.",
				true, false, false, true,
				"cost_function_difference", "The difference of the cost function, computed for the two candidate solutions.",
				"candidate_solution_old", "The old candidate solution, expressed as a vector of choices for the variable nodes only.",
				"candidate_solution_new", "The new candidate solution, expressed as a vector of choices for the variable nodes only.",
				std::bind(
					&SquareRootOfGraphIslandCountCostFunction::compute_cost_function_difference,
					this,
					std::placeholders::_1,
					std::placeholders::_2
				)
			)
		);

        api_definition_mutex_locked() = api_def;
    }
    return api_definition_mutex_locked();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction", "SquareRootOfGraphIslandCountCostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
SquareRootOfGraphIslandCountCostFunction::get_categories() const {
	std::vector< std::vector< std::string > > outvec( Parent::get_categories() );
	outvec[0].push_back( "SquareRootOfGraphIslandCountCostFunction" );
	return outvec;
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction", "SquareRootOfGraphIslandCountCostFunction" } }.
std::vector< std::vector< std::string > >
SquareRootOfGraphIslandCountCostFunction::get_data_representation_categories() const {
	return std::vector< std::vector< std::string > >{ { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction", "SquareRootOfGraphIslandCountCostFunction" } };
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based", "square_root" }
std::vector< std::string >
SquareRootOfGraphIslandCountCostFunction::get_data_representation_keywords() const {
	std::vector< std::string > outvec( Parent::get_data_representation_keywords() );
	outvec.push_back( "graph_island_count_based" );
	outvec.push_back( "square_root" );
	return outvec;
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based", "square_root" }.
std::vector< std::string >
SquareRootOfGraphIslandCountCostFunction::get_present_data_representation_properties() const {
	return std::vector< std::string >{ "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based", "square_root" };
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
std::vector< std::string >
SquareRootOfGraphIslandCountCostFunction::get_absent_data_representation_properties() const {
	return std::vector< std::string >{ "pairwise_decomposible" };
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based", "square_root" }
std::vector< std::string >
SquareRootOfGraphIslandCountCostFunction::get_keywords() const {
	std::vector< std::string > outvec( Parent::get_keywords() );
	outvec.push_back( "graph_island_count_based" );
	outvec.push_back( "square_root" );
	return outvec;
}

/// @brief Get the name of this class ("SquareRootOfGraphIslandCountCostFunction").
/// @details Static version.
std::string
SquareRootOfGraphIslandCountCostFunction::class_name_static() {
	return "SquareRootOfGraphIslandCountCostFunction";
}

/// @brief Get the name of this class ("SquareRootOfGraphIslandCountCostFunction").
std::string
SquareRootOfGraphIslandCountCostFunction::class_name() const {
	return class_name_static();
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function").
/// @details Static version.
std::string
SquareRootOfGraphIslandCountCostFunction::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function";
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizersn::cost_function_network::cost_function").
std::string
SquareRootOfGraphIslandCountCostFunction::class_namespace() const {
	return class_namespace_static();
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


/// @brief Given a selection of choices at variable nodes, compute the cost function.
/// @details This must be implemented by derived classes.
/// @note No mutex-locking is performed!
masala::base::Real
SquareRootOfGraphIslandCountCostFunction::compute_cost_function(
	std::vector< masala::base::Size > const & candidate_solution
) const {
	using masala::base::Size;
	using masala::base::Real;
	Size const n_nodes( protected_n_nodes_absolute() );
	Size * island_sizes = static_cast<Size *>( alloca( sizeof(Size) * n_nodes ) ); // Since this is stack-allocated with alloca, will automatically be deallocated at function's end.  DO NOT FREE.
	protected_compute_island_sizes( candidate_solution, island_sizes );
	Real accumulator(0);
	for( Size i(0); i<n_nodes; ++i ) {
		if( island_sizes[i] >= protected_min_island_size() ) {
			Size const cursize( island_sizes[i] + 1 - protected_min_island_size() );
			accumulator += std::sqrt( static_cast<masala::base::Real>( cursize ) );
		}
	}
	return -1.0*protected_weight()*accumulator;
}

/// @brief Given an old selection of choices at variable nodes and a new selection,
/// compute the cost function difference.
/// @details This must be implemented by derived classes.
/// @note No mutex-locking is performed!
masala::base::Real
SquareRootOfGraphIslandCountCostFunction::compute_cost_function_difference(
	std::vector< masala::base::Size > const & candidate_solution_old,
	std::vector< masala::base::Size > const & candidate_solution_new
) const {
	return compute_cost_function(candidate_solution_new) - compute_cost_function(candidate_solution_old);
}

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
SquareRootOfGraphIslandCountCostFunction::protected_finalize(
	std::vector< masala::base::Size > const & variable_node_indices
) {
	// TODO ANY NEEDED FINALIZATION HERE

	Parent::protected_finalize( variable_node_indices );
}

/// @brief Override of assign_protected_assignmutex_locked().  Calls parent function.
/// @details Throws if src is not a SquareRootOfGraphIslandCountCostFunction.
void
SquareRootOfGraphIslandCountCostFunction::protected_assign(
	masala::base::managers::engine::MasalaDataRepresentation const & src
) {
	SquareRootOfGraphIslandCountCostFunction const * const src_cast_ptr( dynamic_cast< SquareRootOfGraphIslandCountCostFunction const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Cannot assign a SquareRootOfGraphIslandCountCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

	// TODO COPY DATA HERE.

	Parent::protected_assign( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
void
SquareRootOfGraphIslandCountCostFunction::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

/// @brief Is this data representation empty?
/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
/// @returns True if no data have been loaded into this data representation, false otherwise.
/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
bool
SquareRootOfGraphIslandCountCostFunction::protected_empty() const {
	return // TODO STUFF HERE &&
		Parent::protected_empty();
}

/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
void
SquareRootOfGraphIslandCountCostFunction::protected_clear() {
	// TODO CLEAR DATA HERE
	Parent::protected_clear();
}

/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
void
SquareRootOfGraphIslandCountCostFunction::protected_reset() {
	protected_clear();
	Parent::protected_reset();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
