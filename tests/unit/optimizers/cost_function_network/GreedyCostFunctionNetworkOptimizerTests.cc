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

/// @file tests/unit/optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizerTests.cc
/// @brief Unit tests for the GreedyCostFunctionNetworkOptimizer class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/cost_function_network/GreedyCostFunctionNetworkOptimizer_API.hh>
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>
#include <optimizers_api/utility/cost_function_network/util.hh>

// Masala numeric headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>
#include <numeric_api/utility/optimization/cost_function_network/util.hh>

// Masala base headers:
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <sstream>
#include <iomanip>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace cost_function_network {

TEST_CASE( "Instantiate an GreedyCostFunctionNetworkOptimizer.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::GreedyCostFunctionNetworkOptimizer_API][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        optimizers_api::auto_generated_api::cost_function_network::GreedyCostFunctionNetworkOptimizer_APISP greedyopt(
            masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::GreedyCostFunctionNetworkOptimizer_API >()
        );
        greedyopt->write_to_tracer( "Instantiated a GreedyCostFunctionNetworkOptimizer." );
    }() );
}

TEST_CASE( "Solve a simple problem with the GreedyCostFunctionNetworkOptimizer.", "[standard_masala_plugins::optimizers_api::auto_generated_api::annealing::ConstantAnnealingSchedule_API][standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::GreedyCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::base::managers::tracer;
	using masala::base::Size;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    masala::base::managers::threads::MasalaThreadManager::get_instance()->set_total_threads(5);
    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );


	std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

	REQUIRE_NOTHROW([&](){

		CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
		auto test_problem(
			masala::numeric_api::utility::optimization::cost_function_network::construct_test_problem(
				"PairwisePrecomputedCostFunctionNetworkOptimizationProblem", false, false
			)
		);
		test_problem->add_candidate_solution( std::vector< Size >{ 0, 0, 0 } );
		test_problem->add_candidate_solution( std::vector< Size >{ 1, 1, 2 } );
		test_problem->finalize();
		problem_container->add_optimization_problem(
			test_problem
		);
		
		GreedyCostFunctionNetworkOptimizer_APISP greedyopt(
			masala::make_shared< GreedyCostFunctionNetworkOptimizer_API >()
		);
		greedyopt->set_cpu_threads_to_request(2);
		
		solutions = greedyopt->run_cost_function_network_optimizer( *problem_container );


	}() );

	tracer->write_to_tracer( "GreedyCostFunctionNetworkOptimizerTests", "Got " + std::to_string( solutions[0]->n_solutions() ) + " solutions." );
	tracer->write_to_tracer( "GreedyCostFunctionNetworkOptimizerTests", "SOLUTION\tTIMES_SEEN\tSCORE\tCHOICE_SELECTION" );
	tracer->write_to_tracer( "GreedyCostFunctionNetworkOptimizerTests", "--------\t----------\t-----\t----------------" );

	for( masala::base::Size i(0); i<solutions[0]->n_solutions(); ++i ) {
		CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions[0]->solution(i) ) );
		CHECK( solution != nullptr );
		std::ostringstream ss;
		ss << std::setw(8) << i << "\t"
			<< std::setw(10) << solution->n_times_solution_was_produced() << "\t"
			<< std::setw(5) << solution->solution_score() << "\t"
			<< "[" << masala::base::utility::container::container_to_string( solution->solution_at_variable_positions(), ",") << "]";
		tracer->write_to_tracer( "GreedyCostFunctionNetworkOptimizerTests", ss.str() );
	}

	CHECK( solutions.size() == 1 );
	CHECK( solutions[0]->n_solutions() == 1 );
	CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 6.0 ) < 1.0e-8 );
	CHECK( solutions[0]->solution(0)->solution_is_valid() );
	CHECK( solutions[0]->solution(0)->n_times_solution_was_produced() == 2 );

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}


} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
