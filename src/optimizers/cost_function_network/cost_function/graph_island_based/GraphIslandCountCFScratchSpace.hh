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
		masala::base::Size const n_variable_nodes
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

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Set the current candidate solution.  Throws if solution sizes don't match in debug mode.
	void set_current_candidate_solution( std::vector< masala::base::Size > const & solution_in );

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Accept the last move.
	void protected_accept_last_move() override;

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

	/// @brief Has a move been made since the last accept?
	bool move_made_ = false;

	/// @brief Has at least one move been accepted?
	bool move_accepted_ = false;

}; // class GraphIslandCountCFScratchSpace

} // namespace graph_island_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_graph_island_based_GraphIslandCountCFScratchSpace_hh