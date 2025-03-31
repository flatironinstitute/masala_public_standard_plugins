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

/// @file src/optimizers/cost_function_network/cost_function/graph_island_based/LinearGraphIslandCountCostFunction.cc
/// @brief Implementation for a class for LinearGraphIslandCountCostFunctions.
/// @details LinearGraphIslandCountCostFunctions are graph-based cost functions that figure out the
/// number of elements in the islands in the graph, sum the counts in the islands over a minimum, and
/// return the sum.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/graph_island_based/LinearGraphIslandCountCostFunction.hh>

// Optimizers headers:
#include <optimizers/cost_function_network/cost_function/graph_island_based/GraphIslandCountCFScratchSpace.hh>

// STL headers:
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// Numeric headers:
#ifndef NDEBUG
#include <numeric/optimization/cost_function_network/cost_function/CostFunctionScratchSpace.hh>
#endif

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
namespace graph_island_based {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
LinearGraphIslandCountCostFunction::LinearGraphIslandCountCostFunction(
	LinearGraphIslandCountCostFunction const & src
) :
	Parent()
{
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
}

// @brief Assignment operator.
LinearGraphIslandCountCostFunction &
LinearGraphIslandCountCostFunction::operator=(
	LinearGraphIslandCountCostFunction const & src
) {
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
	return *this;
}

/// @brief Make a copy of this object.
masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP
LinearGraphIslandCountCostFunction::clone() const {
	return masala::make_shared< LinearGraphIslandCountCostFunction >( *this );
}

/// @brief Make a copy of this object that is fully independent.
LinearGraphIslandCountCostFunctionSP
LinearGraphIslandCountCostFunction::deep_clone() const {
	LinearGraphIslandCountCostFunctionSP new_object( std::static_pointer_cast< LinearGraphIslandCountCostFunction >( this->clone() ) );
	new_object->make_independent();
	return new_object;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////
	
/// @brief Get a description of the API of this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
LinearGraphIslandCountCostFunction::get_api_definition() {
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
				"over a given size threshold and returns the negated sum.", false, false
			)
		);

		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( LinearGraphIslandCountCostFunction, api_def );

		// Getters:
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"one_based_absolute_node_indexing",
				"Get whether nodes' absolute index is one-based (true) or zero-based (false, the default).  Note that variable "
				"node indexing is always zero-based.",
				"one_based_absolute_node_indexing", "True if the absolute node index is one-based, false if it is zero-based (the default).",
				false, false,
				std::bind(
					&LinearGraphIslandCountCostFunction::one_based_absolute_node_indexing,
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
					&LinearGraphIslandCountCostFunction::absolute_node_count,
					this
				)
			)
		);
		api_def->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > >(
				"min_island_size",
				"Get the minimum number of nodes in an island in order for that island to contribute to the penalty function "
				"value.  If the number of nodes is greater than or equal to this value, this value is subtracted from the count"
				".  These values are summed and negated to compute the penalty value.",
				"min_island_size", "The minimum island size.",
				false, false,
				std::bind(
					&LinearGraphIslandCountCostFunction::min_island_size,
					this
				)
			)
		);

		// Setters:
		api_def->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > >(
				"set_min_island_size",
				"Set the minimum number of nodes in an island in order for that island to contribute to the penalty function "
				"value.  If the number of nodes is greater than or equal to this value, this value is subtracted from the count"
				".  These values summed and negated to compute the penalty value.",
				"min_island_size_in", "The minimum island size to set.",
				false, false,
				std::bind(
					&LinearGraphIslandCountCostFunction::set_min_island_size,
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
					&LinearGraphIslandCountCostFunction::set_one_based_absolute_node_indexing,
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
					&LinearGraphIslandCountCostFunction::set_absolute_node_count,
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
					&LinearGraphIslandCountCostFunction::declare_node_choice_pair_interaction,
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
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ZeroInput< masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpaceSP > >(
				"generate_cost_function_scratch_space", "Generate a GraphIslandCountCFScratchSpace object, which serves as a thread-local scratch "
				"space for repeated reevaluation of this cost function.",
				true, false, true, false,
				"cost_function_scratch_space", "A GraphIslandCountCFScratchSpace object, which serves as a thread-local scratch "
				"space for repeated reevaluation of this cost function.",
				std::bind( &LinearGraphIslandCountCostFunction::generate_cost_function_scratch_space, this )
			)
		);

		MasalaObjectAPIWorkFunctionDefinition_TwoInputSP < Real, std::vector< Size > const &, masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * > compute_fxn(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_TwoInput < Real, std::vector< Size > const &, masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * > >(
				"compute_cost_function", "Compute the cost function: find the size of each island in the interaction graph over "
				"threshold, sum them, and negate the result.  No mutex-locking is performed.",
				true, false, false, true,
				"candidate_solution", "The candidate solution, expressed as a vector of choices for the variable nodes only.",
				"scratch_space", "A pointer to scratch space for accelerating this calculation, or nullptr.  Should be nullptr for this class.",
				"cost_function_value", "The value of the cost function, computed for the current candidate solution.",
				std::bind(
					&LinearGraphIslandCountCostFunction::compute_cost_function,
					this,
					std::placeholders::_1,
					std::placeholders::_2
				)
			)
		);
		compute_fxn->set_triggers_no_mutex_lock();
		api_def->add_work_function( compute_fxn );

		MasalaObjectAPIWorkFunctionDefinition_ThreeInputSP < Real, std::vector< Size > const &, std::vector< Size > const &, masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * > compute_diff_fxn(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ThreeInput < Real, std::vector< Size > const &, std::vector< Size > const &, masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * > >(
				"compute_cost_function_difference", "Compute the cost function difference: for each of two input vectors, find the size of each "
				"island in the interaction graph over threshold, sum them, negate the result, and return the difference.  "
				"No mutex-locking is performed.",
				true, false, false, true,
				"candidate_solution_old", "The old candidate solution, expressed as a vector of choices for the variable nodes only.",
				"candidate_solution_new", "The new candidate solution, expressed as a vector of choices for the variable nodes only.",
				"scratch_space", "A pointer to scratch space for accelerating this calculation, or nullptr.  Should be nullptr for this class.",
				"cost_function_difference", "The difference of the cost function, computed for the two candidate solutions.",
				std::bind(
					&LinearGraphIslandCountCostFunction::compute_cost_function_difference,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				)
			)
		);
		compute_diff_fxn->set_triggers_no_mutex_lock();
		api_def->add_work_function( compute_diff_fxn );

        api_definition_mutex_locked() = api_def;
    }
    return api_definition_mutex_locked();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction", "LinearGraphIslandCountCostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
LinearGraphIslandCountCostFunction::get_categories() const {
	std::vector< std::vector< std::string > > outvec( Parent::get_categories() );
	outvec[0].push_back( "LinearGraphIslandCountCostFunction" );
	return outvec;
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction", "LinearGraphIslandCountCostFunction" } }.
std::vector< std::vector< std::string > >
LinearGraphIslandCountCostFunction::get_data_representation_categories() const {
	return std::vector< std::vector< std::string > >{ { "CostFunction", "GraphBasedCostFunction", "GraphIslandCountCostFunction", "LinearGraphIslandCountCostFunction" } };
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based", "linear" }
std::vector< std::string >
LinearGraphIslandCountCostFunction::get_data_representation_keywords() const {
	std::vector< std::string > outvec( Parent::get_data_representation_keywords() );
	outvec.push_back( "linear" );
	return outvec;
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based", "linear" }.
std::vector< std::string >
LinearGraphIslandCountCostFunction::get_present_data_representation_properties() const {
	return std::vector< std::string >{ "graph_based", "cost_function", "not_pairwise_decomposible", "graph_island_count_based", "linear" };
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
std::vector< std::string >
LinearGraphIslandCountCostFunction::get_absent_data_representation_properties() const {
	return std::vector< std::string >{ "pairwise_decomposible" };
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "graph_based", "not_pairwise_decomposible", "graph_island_count_based", "linear" }
std::vector< std::string >
LinearGraphIslandCountCostFunction::get_keywords() const {
	std::vector< std::string > outvec( Parent::get_keywords() );
	outvec.push_back( "linear" );
	return outvec;
}

/// @brief Get the name of this class ("LinearGraphIslandCountCostFunction").
/// @details Static version.
std::string
LinearGraphIslandCountCostFunction::class_name_static() {
	return "LinearGraphIslandCountCostFunction";
}

/// @brief Get the name of this class ("LinearGraphIslandCountCostFunction").
std::string
LinearGraphIslandCountCostFunction::class_name() const {
	return class_name_static();
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based").
/// @details Static version.
std::string
LinearGraphIslandCountCostFunction::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function::graph_island_based";
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizersn::cost_function_network::cost_function").
std::string
LinearGraphIslandCountCostFunction::class_namespace() const {
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
/// @note No mutex-locking is performed!  The scratch_space pointer should be null.
masala::base::Real
LinearGraphIslandCountCostFunction::compute_cost_function(
	std::vector< masala::base::Size > const & candidate_solution,
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * scratch_space
) const {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( scratch_space != nullptr, "compute_cost_function", "Expected a non-null pointer for the scratch space." );
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( dynamic_cast< GraphIslandCountCFScratchSpace * >( scratch_space ) != nullptr, "compute_cost_function", "Expected "
		"a pointer to a GraphIslandCountCFScratchSpace object, but got a pointer to a " + scratch_space->class_name() + " object instead."
	);
	using masala::base::Size;
	using masala::base::Real;
	Size const n_nodes( protected_n_nodes_absolute() );
	GraphIslandCountCFScratchSpace * scratch_space_cast( static_cast< GraphIslandCountCFScratchSpace * >(scratch_space) );
	protected_compute_island_sizes( candidate_solution, *scratch_space_cast );
	std::vector< Size > const & island_sizes( scratch_space_cast->island_sizes_const() );
	Size accumulator(0);
	for( Size i(0); i<n_nodes; ++i ) {
		if( island_sizes[i] >= protected_min_island_size() ) {
			Size const cursize( island_sizes[i] + 1 - protected_min_island_size() );
			accumulator += cursize;
		}
	}
	return -1.0*protected_weight()*static_cast<Real>(accumulator);
}

/// @brief Given an old selection of choices at variable nodes and a new selection,
/// compute the cost function difference.
/// @details This must be implemented by derived classes.
/// @note No mutex-locking is performed!  The scratch_space pointer should be null.
masala::base::Real
LinearGraphIslandCountCostFunction::compute_cost_function_difference(
	std::vector< masala::base::Size > const & candidate_solution_old,
	std::vector< masala::base::Size > const & candidate_solution_new,
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * scratch_space
) const {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( scratch_space != nullptr, "compute_cost_function_difference", "Expected a non-null pointer for the scratch space." );
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( dynamic_cast< GraphIslandCountCFScratchSpace * >( scratch_space ) != nullptr, "compute_cost_function_difference", "Expected "
		"a pointer to a GraphIslandCountCFScratchSpace object, but got a pointer to a " + scratch_space->class_name() + " object instead."
	);
	return compute_cost_function(candidate_solution_new, scratch_space) - compute_cost_function(candidate_solution_old, scratch_space);
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
LinearGraphIslandCountCostFunction::protected_finalize(
	std::vector< masala::base::Size > const & variable_node_indices
) {
	// TODO ANY NEEDED FINALIZATION HERE

	Parent::protected_finalize( variable_node_indices );
}

/// @brief Override of assign_protected_assignmutex_locked().  Calls parent function.
/// @details Throws if src is not a LinearGraphIslandCountCostFunction.
void
LinearGraphIslandCountCostFunction::protected_assign(
	masala::base::managers::engine::MasalaDataRepresentation const & src
) {
	LinearGraphIslandCountCostFunction const * const src_cast_ptr( dynamic_cast< LinearGraphIslandCountCostFunction const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Cannot assign a LinearGraphIslandCountCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

	// TODO COPY DATA HERE.

	Parent::protected_assign( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
void
LinearGraphIslandCountCostFunction::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

/// @brief Is this data representation empty?
/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
/// @returns True if no data have been loaded into this data representation, false otherwise.
/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
bool
LinearGraphIslandCountCostFunction::protected_empty() const {
	return // TODO STUFF HERE &&
		Parent::protected_empty();
}

/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
void
LinearGraphIslandCountCostFunction::protected_clear() {
	// TODO CLEAR DATA HERE
	Parent::protected_clear();
}

/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
void
LinearGraphIslandCountCostFunction::protected_reset() {
	protected_clear();
	Parent::protected_reset();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
