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

/// @file src/numeric_api/base_classes/optimization/cost_function_network/PairwisePrecomputedCFNProblemScratchSpace.cc
/// @brief Implementations for a PairwisePrecomputedCFNProblemScratchSpace class.
/// @details PairwisePrecomputedCFNProblemScratchSpaces allow data to be cached and reused from one evaluation of a
/// PairwisePrecomputedCostFunctionNetworkOptimizationProblem to another.  They store the scratch spaces for the
/// individual cost functions.
/// @note These objects are intended to be used by a single thread, and are not threadsafe.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Headers:
#include <optimizers/cost_function_network/PairwisePrecomputedCFNProblemScratchSpace.hh>


namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Settings constructor.
PairwisePrecomputedCFNProblemScratchSpace::PairwisePrecomputedCFNProblemScratchSpace(
	masala::base::Size const /*n_variable_nodes*/,
	std::vector< masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP > const & /*cost_functions*/
) :
	Parent( cost_functions )
{}

} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins