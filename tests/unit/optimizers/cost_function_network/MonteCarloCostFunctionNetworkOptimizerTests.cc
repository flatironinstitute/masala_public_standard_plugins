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

/// @file tests/unit/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizerTests.cc
/// @brief Unit tests for the MonteCarloCostFunctionNetworkOptimizer class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/cost_function_network/MonteCarloCostFunctionNetworkOptimizer_API.hh>
#include <optimizers_api/auto_generated_api/annealing/ConstantAnnealingSchedule_API.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>

// Core headers:

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace cost_function_network {

TEST_CASE( "Instantiate an MonteCarloCostFunctionNetworkOptimizer", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
            masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API >()
        );
        mcopt->write_to_tracer( "Instantiated a MonteCarloCostFunctionNetworkOptimizer." );
    }() );
}

TEST_CASE( "Solve a simple problem with the MonteCarloCostFunctionNetworkOptimizer", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

    std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

    REQUIRE_NOTHROW([&](){
        PairwisePrecomputedCostFunctionNetworkOptimizationProblem_APISP problem( masala::make_shared< PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API >() );

        // We'll create a simple problem with 27 possible solutions:
        // 0 0 0 -> 71
        // 0 0 1 -> 54
        // 0 0 2 -> 58
        // 0 1 0 -> 96
        // 0 1 1 -> 83
        // 0 1 2 -> 88
        // 0 2 0 -> 55
        // 0 2 1 -> 42
        // 0 2 2 -> 46
        // 1 0 0 -> 76
        // 1 0 1 -> 58
        // 1 0 2 -> 57
        // 1 1 0 -> 100
        // 1 1 1 -> 86
        // 1 1 2 -> 86
        // 1 2 0 -> 54
        // 1 2 1 -> 40
        // 1 2 2 -> 399
        // 2 0 0 -> 38
        // 2 0 1 -> 22
        // 2 0 2 -> 23
        // 2 1 0 -> 67
        // 2 1 1 -> 55
        // 2 1 2 -> 57
        // 2 2 0 -> 18
        // 2 2 1 -> 6  <-- lowest
        // 2 2 2 -> 7
        problem->set_onebody_penalty( 0, 0, 25 );
        problem->set_onebody_penalty( 0, 1, 32 );
        problem->set_onebody_penalty( 0, 2, 0 );
        problem->set_onebody_penalty( 1, 0, 15 );
        problem->set_onebody_penalty( 1, 1, 43 );
        problem->set_onebody_penalty( 1, 2, 0 );
        problem->set_onebody_penalty( 2, 0, 14 );
        problem->set_onebody_penalty( 2, 1, 5 );
        problem->set_onebody_penalty( 2, 2, 0 );

        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 0 ), 5 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 1 ), 3 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 2 ), 9 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 0 ), 4 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 1 ), 1 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 2 ), 2 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 0 ), 1 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 1 ), 3 );
        problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 2 ), 1 );

        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 0 ), 5 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 1 ), 3 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 2 ), 9 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 0 ), 4 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 1 ), 1 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 2 ), 2 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 0 ), 1 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 1 ), 0 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 2 ), 3 );

        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 0 ), 7 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 1 ), 1 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 2 ), 4 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 0 ), 6 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 1 ), 4 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 2 ), 8 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 0 ), 2 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 1 ), 0 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 2 ), 3 );

        CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
        problem_container->add_optimization_problem( problem );
        
        MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
            masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
        );
        mcopt->set_attempts_per_problem(5);
        mcopt->set_n_solutions_to_store_per_problem(5);
        mcopt->set_cpu_threads_to_request(5);

        ConstantAnnealingSchedule_APISP annealing_schedule( masala::make_shared< ConstantAnnealingSchedule_API >() );
        annealing_schedule->set_temperature(0.9 /*kcal/mol*/); // Hottish.
        mcopt->set_annealing_schedule( *annealing_schedule );
        
        solutions = mcopt->run_cost_function_network_optimizer( *problem_container );
    }() );

    CHECK( solutions.size() == 1 );
    CHECK( solutions[0]->n_solutions() == 5 );
    CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 6.0 ) < 1.0e-8 );
    CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 7.0 ) < 1.0e-8 );
    CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 18.0 ) < 1.0e-8 );
    CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 22.0 ) < 1.0e-8 );
    CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 23.0 ) < 1.0e-8 );
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
