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

/// @file src/benchmark/benchmark_apps/benchmark_monte_carlo_cfn_optimizer.hh
/// @brief An application used for benchmarking the MonteCarloCostFunctionNetworkOptimizer.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Core headers
#include <base/types.hh>

// Optimizers API headers
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/MonteCarloCostFunctionNetworkOptimizer_API.hh>
#include <optimizers_api/auto_generated_api/annealing/LinearAnnealingSchedule_API.hh>

// Numeric API headers
#include <numeric_api/utility/optimization/cost_function_network/util.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>

// Base headers
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/error/ErrorHandling.hh>

// Program entry point:
int
main(
    // int argc,
    // char * argv[]
    int, char**
) {
    using masala::base::Size;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::numeric_api::utility::optimization::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;

    masala::base::managers::threads::MasalaThreadManagerHandle tm( masala::base::managers::threads::MasalaThreadManager::get_instance() );
    masala::base::managers::tracer::MasalaTracerManagerHandle tr( masala::base::managers::tracer::MasalaTracerManager::get_instance() );

    std::string const appname( "standard_masala_plugins::benchmark::benchmark_apps::benchmark_monte_carlo_cfn_optimizer" );

    try{
        standard_masala_plugins::optimizers_api::auto_generated_api::registration::register_optimizers();

        Size const nthread_total( tm->hardware_threads() );
        if( nthread_total == 0 ) {
            MASALA_THROW( appname, "main", "Could not auto-detect hardware threads!" );
        }
        tr->write_to_tracer( appname, "Detected " + std::to_string(nthread_total) + " hardware threads." );

        // Launch as many threads as we have hardware threads:
        tm->set_total_threads( nthread_total );

        // Prepare the problem that we'll solve over and over:
        PairwisePrecomputedCostFunctionNetworkOptimizationProblems_APISP problems(
            masala::make_shared< PairwisePrecomputedCostFunctionNetworkOptimizationProblems_API >()
        );
        problems->add_optimization_problem( construct_test_problem() );

        // Run a problem on a series of thread counts:
        for( Size threadcount(1); threadcount<=nthread_total; ++threadcount ) {
            tr->write_to_tracer( appname, "Running test problem on " + std::to_string(threadcount) + " threads." );
            LinearAnnealingSchedule_APISP anneal_sched(
                masala::make_shared< LinearAnnealingSchedule_API >()
            );
            MonteCarloCostFunctionNetworkOptimizer_APISP mc_opt(
                masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
            );
            mc_opt->set_annealing_schedule( anneal_sched );
            mc_opt->set_annealing_steps_per_attempt(1000000);
            mc_opt->set_attempts_per_problem(threadcount);
            mc_opt->set_cpu_threads_to_request(threadcount);
            mc_opt->set_solution_storage_mode("check_at_every_step");
            mc_opt->set_n_solutions_to_store_per_problem(5);

            // Run the problem:
            std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > const solutions(
                mc_opt->run_cost_function_network_optimizer( *problems )
            );

            // TODO ANALYZE SOLUTIONS.
        }

    } catch( masala::base::error::MasalaException const e ) {
        tr->write_to_tracer( appname, "Caught Masala exception: " + e. message() );
        return 1;
    }

    return 0;
}