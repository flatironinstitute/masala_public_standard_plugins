/*
	Masala
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

/// @file src/numeric_api/base_classes/optimization/cost_function_network/PairwisePrecomputedCFNProblemScratchSpace.hh
/// @brief Header for a PairwisePrecomputedCFNProblemScratchSpace class.
/// @details PairwisePrecomputedCFNProblemScratchSpaces allow data to be cached and reused from one evaluation of a
/// PairwisePrecomputedCostFunctionNetworkOptimizationProblem to another.  They store the scratch spaces for the
/// individual cost functions.
/// @note These objects are intended to be used by a single thread, and are not threadsafe.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_optimizers_cost_function_network_PairwisePrecomputedCFNProblemScratchSpace_hh
#define StandardMaslaPlugins_src_optimizers_cost_function_network_PairwisePrecomputedCFNProblemScratchSpace_hh

// Forward declarations:
#include <optimizers/cost_function_network/PairwisePrecomputedCFNProblemScratchSpace.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/PluginPairwisePrecomputedCFNProblemScratchSpace.hh>

// Base headers:
#include <base/types.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

/// @brief A PairwisePrecomputedCFNProblemScratchSpace class.
/// @details PairwisePrecomputedCFNProblemScratchSpaces allow data to be cached and reused from one evaluation of a
/// PairwisePrecomputedCostFunctionNetworkOptimizationProblem to another.  They store the scratch spaces for the
/// individual cost functions.
/// @note These objects are intended to be used by a single thread, and are not threadsafe.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class PairwisePrecomputedCFNProblemScratchSpace : public masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCFNProblemScratchSpace {

	typedef masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCFNProblemScratchSpace Parent;
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCFNProblemScratchSpaceSP ParentSP;
	typedef masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCFNProblemScratchSpaceCSP ParentCSP;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	PairwisePrecomputedCFNProblemScratchSpace() = delete;

	/// @brief Settings constructor.  Finalizes this object with the function call.
	PairwisePrecomputedCFNProblemScratchSpace(
		masala::base::Size const n_variable_nodes,
		std::vector< masala::numeric::optimization::cost_function_network::cost_function::CostFunctionCSP > const & cost_functions
	);

	/// @brief Copy constructor.
	PairwisePrecomputedCFNProblemScratchSpace( PairwisePrecomputedCFNProblemScratchSpace const & /*src*/ ) = default;

	// @brief Assignment operator.
	PairwisePrecomputedCFNProblemScratchSpace &
	operator=( PairwisePrecomputedCFNProblemScratchSpace const & /*src*/ ) = default;

	/// @brief Destructor.
	~PairwisePrecomputedCFNProblemScratchSpace() override = default;
	
public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the name for this class.
	/// @returns "PairwisePrecomputedCFNProblemScratchSpace".
	std::string
	class_name() const;

	/// @brief Get the namespace for this class.
	/// @returns "standard_masala_plugins::optimizers::cost_function_network".
	std::string
	class_namespace() const;

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get indices of all variable nodes.  Preallocated for convenience and speed.
	inline std::vector< masala::base::Size > const & ivals() const { return ivals_; }

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////


public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES.
////////////////////////////////////////////////////////////////////////////////

	/// @brief Indices of all variable nodes.  Preallocated for convenience and speed.
	std::vector< masala::base::Size > ivals_;

}; // class PairwisePrecomputedCFNProblemScratchSpace

} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // StandardMaslaPlugins_src_optimizers_cost_function_network_PairwisePrecomputedCFNProblemScratchSpace_hh