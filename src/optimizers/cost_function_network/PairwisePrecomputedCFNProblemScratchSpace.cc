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

// STL headers:
#include <string>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Settings constructor.  Finalizes this object with the function call.
PairwisePrecomputedCFNProblemScratchSpace::PairwisePrecomputedCFNProblemScratchSpace(
	masala::base::Size const n_variable_nodes,
	std::vector< masala::numeric::optimization::cost_function_network::cost_function::CostFunctionCSP > const & cost_functions
) :
	Parent( cost_functions )
{
	ivals_.resize( n_variable_nodes );
	for( masala::base::Size i(0); i<n_variable_nodes; ++i ) {
		ivals_[i] = i;
	}
	finalize();
}

/// @brief Make a copy of this object.
masala::numeric::optimization::cost_function_network::CFNProblemScratchSpaceSP
PairwisePrecomputedCFNProblemScratchSpace::clone() const {
	return masala::make_shared< PairwisePrecomputedCFNProblemScratchSpace >( *this );
}

/// @brief Make a fully independent copy of this object.
PairwisePrecomputedCFNProblemScratchSpaceSP
PairwisePrecomputedCFNProblemScratchSpace::deep_clone() const {
	PairwisePrecomputedCFNProblemScratchSpaceSP new_obj( masala::make_shared< PairwisePrecomputedCFNProblemScratchSpace >( *this ) );
	new_obj->protected_make_independent();
	return new_obj;
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the name for this class.
/// @returns "PairwisePrecomputedCFNProblemScratchSpace".
std::string
PairwisePrecomputedCFNProblemScratchSpace::class_name() const {
	return "PairwisePrecomputedCFNProblemScratchSpace";
}

/// @brief Get the namespace for this class.
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
std::string
PairwisePrecomputedCFNProblemScratchSpace::class_namespace() const {
	return ".standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Make this object fully independent.
void
PairwisePrecomputedCFNProblemScratchSpace::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins