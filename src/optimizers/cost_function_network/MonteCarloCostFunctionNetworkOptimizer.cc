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
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer::MonteCarloCostFunctionNetworkOptimizer(
    MonteCarloCostFunctionNetworkOptimizer const & src
) :
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer( src )
{}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer &
MonteCarloCostFunctionNetworkOptimizer::operator=( MonteCarloCostFunctionNetworkOptimizer const & src ) {
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::operator=( src );
    return *this;
}

/// @brief Make a copy of this object that's wholly independent.
MonteCarloCostFunctionNetworkOptimizerSP
MonteCarloCostFunctionNetworkOptimizer::deep_clone() const {
    MonteCarloCostFunctionNetworkOptimizerSP new_optimizer( masala::make_shared< MonteCarloCostFunctionNetworkOptimizer >(*this) );
    new_optimizer->make_independent();
    return new_optimizer;
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
MonteCarloCostFunctionNetworkOptimizer::make_independent() {
    //GNDN
}


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

/// @brief Get the class name.
/// @returns "MonteCarloCostFunctionNetworkOptimizer".
std::string
MonteCarloCostFunctionNetworkOptimizer::class_name() const {
    return "MonteCarloCostFunctionNetworkOptimizer";
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
std::string
MonteCarloCostFunctionNetworkOptimizer::class_namespace() const {
    return "standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
MonteCarloCostFunctionNetworkOptimizer::get_api_definition() {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    if( api_description_ == nullptr ) {
        masala::base::api::MasalaObjectAPIDefinitionSP api_description(
            masala::make_shared< masala::base::api::MasalaObjectAPIDefinition >(
                *this,
                "An optimizer for cost function network problems that uses a Monte Carlo search to optimize.  This "
                "performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to "
                "pick a node at random and change its setting at random, compute the change in overall energy or score, "
                "and accept or reject the move based on the difference in energy and the Metropolis criterion.",
                false
            )
        );

        // Convert nonconst to const:
        api_description_ = api_description;
    }
    return api_description_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolution_APICSP
MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API const & problem
) const {
    //TODO TODO TODO
    return nullptr; // TODO TODO TODO
}

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
