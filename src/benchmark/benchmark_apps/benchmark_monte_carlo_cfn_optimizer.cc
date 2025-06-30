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

/// @file src/benchmark/benchmark_apps/benchmark_monte_carlo_cfn_optimizer.hh
/// @brief An application used for benchmarking the MonteCarloCostFunctionNetworkOptimizer.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Core headers
#include <base/types.hh>

// Optimizers API headers
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/MonteCarloCostFunctionNetworkOptimizer_API.hh>
#include <optimizers_api/auto_generated_api/annealing/LogarithmicRepeatAnnealingSchedule_API.hh>

// Numeric API headers
#include <numeric_api/utility/optimization/cost_function_network/util.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>

// Base headers
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/error/ErrorHandling.hh>
#include <base/managers/random/MasalaRandomNumberGenerator.hh>

// STL headers
#include <chrono>
#include <tuple>
#include <sstream>
#include <iomanip>
#include <cmath>

// Program entry point:
int
main(
    // int argc,
    // char * argv[]
    int, char**
) {
    using masala::base::Size;
    using masala::base::Real;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::numeric_api::utility::optimization::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;

    masala::base::managers::threads::MasalaThreadManagerHandle tm( masala::base::managers::threads::MasalaThreadManager::get_instance() );
    masala::base::managers::tracer::MasalaTracerManagerHandle tr( masala::base::managers::tracer::MasalaTracerManager::get_instance() );
    masala::base::managers::random::MasalaRandomNumberGeneratorHandle rg( masala::base::managers::random::MasalaRandomNumberGenerator::get_instance() );

    std::string const appname( "standard_masala_plugins::benchmark::benchmark_apps::benchmark_monte_carlo_cfn_optimizer" );

    try{
        masala::numeric_api::auto_generated_api::registration::register_numeric();
        standard_masala_plugins::optimizers_api::auto_generated_api::registration::register_optimizers();

        Size const total_replicates( 10 ); // Do 10 replicates for each threadcount.
#ifndef NDEBUG
        Size const total_steps( 100000 ); // Do a hundred thousand Monte Carlo steps for each threadcount in debug mode.
#else
        Size const total_steps( 1000000 ); // Do a million Monte Carlo steps for each threadcount in release mode.
#endif

        Size const nthread_total( tm->hardware_threads() );
        if( nthread_total == 0 ) {
            MASALA_THROW( appname, "main", "Could not auto-detect hardware threads!" );
        }
        tr->write_to_tracer( appname, "Detected " + std::to_string(nthread_total) + " hardware threads." );

        // Launch as many threads as we have hardware threads:
        tm->set_total_threads( nthread_total );

        // Prepare the problem that we'll solve over and over:
        CostFunctionNetworkOptimizationProblems_APISP problems(
            masala::make_shared< CostFunctionNetworkOptimizationProblems_API >()
        );
        problems->add_optimization_problem( construct_test_problem( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" ) );

        // Prepare the annealing schedule:
        LogarithmicRepeatAnnealingSchedule_APISP anneal_sched(
            masala::make_shared< LogarithmicRepeatAnnealingSchedule_API >()
        );
        anneal_sched->set_temperature_initial(100.0);
        anneal_sched->set_temperature_initial(0.3);
        anneal_sched->set_n_repeats(3);

        // Prepare the vector of stuff to be done.  Each entry is threadcount, replicate number, output time in microseconds.
        std::vector< std::tuple< Size, Size, Size > > jobs;
        std::vector< Size > jobindices;
        Size counter(0);
        jobs.reserve( total_replicates * nthread_total );
        jobindices.reserve( total_replicates * nthread_total );
        for( Size threadcount(1); threadcount <= nthread_total; ++threadcount ) {
            for( Size ireplicate(0); ireplicate < total_replicates; ++ireplicate ) {
                jobs.push_back( std::make_tuple( threadcount, ireplicate, 0 ) );
                jobindices.push_back( counter );
                ++counter;
            }
        }
        
        // Randomize the order in which jobs will run.
        rg->shuffle_vector( jobindices );

        // Run a problem on a series of thread counts:
        counter = 0;
        for( Size const jobindex : jobindices ) {
            std::tuple< Size, Size, Size > & job( jobs[jobindex] );
            Size const threadcount( std::get<0>(job) );
            tr->write_to_tracer( appname, "Running test problem on " + std::to_string(threadcount) + " threads (test " + std::to_string(counter+1) + " of " + std::to_string(total_replicates * nthread_total) + ")." );

            MonteCarloCostFunctionNetworkOptimizer_APISP mc_opt(
                masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
            );
            mc_opt->set_annealing_schedule( *anneal_sched );
            mc_opt->set_annealing_steps_per_attempt( total_steps );
            mc_opt->set_attempts_per_problem(threadcount);
            mc_opt->set_cpu_threads_to_request(threadcount);
            mc_opt->set_solution_storage_mode("check_on_acceptance");
            mc_opt->set_n_solutions_to_store_per_problem(1);

            // Run the problem:
            std::chrono::time_point< std::chrono::steady_clock > const starttime(
                std::chrono::steady_clock::now()
            );
            std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > const solutions(
                mc_opt->run_cost_function_network_optimizer( *problems )
            );
            std::chrono::time_point< std::chrono::steady_clock > const endtime(
                std::chrono::steady_clock::now()
            );
            std::get<2>(job) = std::chrono::duration_cast< std::chrono::microseconds >( endtime-starttime ).count();
            ++counter;
        }

        // Print the results:
        tr->write_to_tracer( appname, "THREADS\tTIME(us)\tTIME_STDERR\tMONTE_CARLO_STEPS\tSTEPS/MICROSECOND\tSTEPS/US_STDERR\tEXPECTED_STEPS\tEFFICIENCY" );
        tr->write_to_tracer( appname, "-------\t--------\t-----------\t-----------------\t-----------------\t---------------\t--------------\t----------" );
        counter = 0;
        Real avgtime_1(0.0);
        for( Size threadcount(1); threadcount <= nthread_total; ++threadcount ) {
            Real avgtime(0);
            Real std_err(0);
            for( Size ireplicate(0); ireplicate < total_replicates; ++ireplicate ) {
                avgtime += static_cast< Real >( std::get<2>(jobs[counter]) );
                ++counter;
            }
            avgtime /= static_cast<Real>(total_replicates);
            counter -= total_replicates;
            for( Size ireplicate(0); ireplicate < total_replicates; ++ireplicate ) {
                Real const diff(static_cast< Real >( std::get<2>(jobs[counter]) ) - avgtime);
                std_err += diff*diff;
                ++counter;
            }
            std_err = std::sqrt(std_err)/static_cast<Real>(total_replicates);
            if(threadcount == 1) {
                avgtime_1 = avgtime;
            }
            std::ostringstream ss;
            ss << std::setw(7) << threadcount << "\t";
            ss << std::setw(8) << avgtime << "\t";
            ss << std::setw(11) << std_err << "\t";
            ss << std::setw(17) << total_steps * threadcount << "\t";
            Real const actual( static_cast<Real>(total_steps * threadcount)/avgtime );
            ss << std::setw(17) << actual << "\t";
            Real const std_err_actual( actual * std_err / avgtime ); // Propagation of error: if x = C/y, delta_x = x delta_y / y.
            ss << std::setw(15) << std_err_actual << "\t";
            Real const expected( static_cast<Real>(total_steps * threadcount)/avgtime_1 );
            ss << std::setw(14) << expected << "\t" ;
            ss << std::setw(10) << actual/expected ;
            tr->write_to_tracer( appname, ss.str() );
        }

    } catch( masala::base::error::MasalaException const & e ) {
        tr->write_to_tracer( appname, "Caught Masala exception: " + e. message() );
        return 1;
    }

    return 0;
}
