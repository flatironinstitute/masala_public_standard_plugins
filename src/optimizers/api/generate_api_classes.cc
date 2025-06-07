/*
    Standard Masala Plugins
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

/// @file src/optimizers/api/generate_api_classes.cc
/// @brief Utility functions to generate all of the classes in the optimizers library that define
/// an API.  Used for auto-generating the optimizers API.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Project header
#include <optimizers/api/generate_api_classes.hh>

// Optimizers headers
#include <optimizers/annealing/ConstantAnnealingSchedule.hh>
#include <optimizers/annealing/LinearAnnealingSchedule.hh>
#include <optimizers/annealing/LinearRepeatAnnealingSchedule.hh>
#include <optimizers/annealing/LogarithmicAnnealingSchedule.hh>
#include <optimizers/annealing/LogarithmicRepeatAnnealingSchedule.hh>
#include <optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.hh>
#include <optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizer.hh>
#include <optimizers/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>
#include <optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/SquareOfChoicePenaltySumCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/graph_island_based/SquareOfGraphIslandCountCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/graph_island_based/SquareRootOfGraphIslandCountCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/graph_island_based/LinearGraphIslandCountCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/graph_island_based/LogOfGraphIslandCountCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/feature_based/ChoiceFeature.hh>
#include <optimizers/cost_function_network/cost_function/feature_based/SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction.hh>
// #include <optimizers/gradient_based/BFGSFunctionOptimizer.hh>
#include <optimizers/gradient_based/GradientDescentFunctionOptimizer.hh>
#include <optimizers/gradient_free/SimplexFunctionOptimizer.hh>
#include <optimizers/gradient_based/ArmijoInexactLineOptimizer.hh>
#include <optimizers/gradient_based/BrentAlgorithmLineOptimizer.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace api {

    /// @brief Generate an instance of each of the classes in core that have a defined API, and
    /// return a vector of owning pointers to these instances.
    /// @details Used for auto-generating the core API description (as a JSON file).
    std::vector< masala::base::MasalaObjectSP >
    generate_api_classes() {
        std::vector< masala::base::MasalaObjectSP > outvec;

        using namespace standard_masala_plugins::optimizers;

        // Add to this vector whenever a class is added with a defined API:
        outvec.emplace_back( masala::make_shared< annealing::ConstantAnnealingSchedule >() );
        outvec.emplace_back( masala::make_shared< annealing::LinearAnnealingSchedule >() );
        outvec.emplace_back( masala::make_shared< annealing::LinearRepeatAnnealingSchedule >() );
        outvec.emplace_back( masala::make_shared< annealing::LogarithmicAnnealingSchedule >() );
        outvec.emplace_back( masala::make_shared< annealing::LogarithmicRepeatAnnealingSchedule >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::MonteCarloCostFunctionNetworkOptimizer >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::GreedyCostFunctionNetworkOptimizer >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::SquareOfChoicePenaltySumCostFunction >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::graph_island_based::SquareOfGraphIslandCountCostFunction >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::graph_island_based::SquareRootOfGraphIslandCountCostFunction >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::graph_island_based::LinearGraphIslandCountCostFunction >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::graph_island_based::LogOfGraphIslandCountCostFunction >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::feature_based::ChoiceFeature >() );
        outvec.emplace_back( masala::make_shared< cost_function_network::cost_function::feature_based::SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction >() );
        // outvec.emplace_back( masala::make_shared< gradient_based::BFGSFunctionOptimizer >() );
        outvec.emplace_back( masala::make_shared< gradient_based::GradientDescentFunctionOptimizer >() );
        outvec.emplace_back( masala::make_shared< gradient_free::SimplexFunctionOptimizer >() );
        outvec.emplace_back( masala::make_shared< gradient_based::ArmijoInexactLineOptimizer >() );
        outvec.emplace_back( masala::make_shared< gradient_based::BrentAlgorithmLineOptimizer >() );
        // ADD MORE ENTRIES HERE

        return outvec;
    }

} // namespace api
} // namespace optimizers
} // namespace standard_masala_plugins {

