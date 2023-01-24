/*
    Masala
    Copyright (C) 2022 Vikram K. Mulligan

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

/// @file src/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.cc
/// @brief Implementation for a CostFunctionNetworkOptimizer that solves a cost function network problem using Monte
/// Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.hh>

// Numeric API headers:
#include <numeric_api/base_classes/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem.hh>
#include <numeric_api/base_classes/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {


////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outher vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
MonteCarloCostFunctionNetworkOptimizer::get_categories() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return CostFunctionNetworkOptimizer::get_categories();
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "stochastic" }
std::vector< std::string >
MonteCarloCostFunctionNetworkOptimizer::get_keywords() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	std::vector< std::string > keywords( CostFunctionNetworkOptimizer::get_keywords() );
    keywords.push_back( "monte_carlo" );
    keywords.push_back( "stochastic" );
    return keywords;
}

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.
masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutionCSP
MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem const & problem
) const {
    TODO TODO TODO
}

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
