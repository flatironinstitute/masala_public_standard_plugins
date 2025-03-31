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
    masala::base::Size const n_variable_nodes
) :
	Parent()
{
	island_sizes_.resize( n_absolute_nodes );
	std::fill( island_sizes_.begin(), island_sizes_.end(), 1 );

    vec1_.resize( n_variable_nodes );
    vec2_.resize( n_variable_nodes );
    std::fill( vec1_.begin(), vec1_.end(), 0 );
    std::fill( vec2_.begin(), vec2_.end(), 0 );

    current_candidate_solution_ = &vec1_;
    last_accepted_candidate_solution_ = &vec2_;
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
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Accept the last move.
void
GraphIslandCountCFScratchSpace::protected_accept_last_move() {
    std::swap( last_accepted_candidate_solution_, current_candidate_solution_ );
}

} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
