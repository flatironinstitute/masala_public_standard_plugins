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

/// @file tests/unit/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizerTests.cc
/// @brief Unit tests for the MonteCarloCostFunctionNetworkOptimizer class.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/cost_function_network/MonteCarloCostFunctionNetworkOptimizer_API.hh>
#include <optimizers_api/auto_generated_api/annealing/ConstantAnnealingSchedule_API.hh>
#include <optimizers_api/auto_generated_api/annealing/LinearAnnealingSchedule_API.hh>
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

TEST_CASE( "Instantiate an MonteCarloCostFunctionNetworkOptimizer.", "[standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][instantiation]" ) {
    REQUIRE_NOTHROW([&](){
        optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
            masala::make_shared< optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API >()
        );
        mcopt->write_to_tracer( "Instantiated a MonteCarloCostFunctionNetworkOptimizer." );
    }() );
}

TEST_CASE( "Solve a simple problem with the MonteCarloCostFunctionNetworkOptimizer with a constant annealing schedule.", "[standard_masala_plugins::optimizers_api::auto_generated_api::annealing::ConstantAnnealingSchedule_API][standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::base::managers::tracer;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    masala::base::managers::threads::MasalaThreadManager::get_instance()->set_total_threads(5);
    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );

    for( auto const multimutation : std::vector< bool >{ true, false } ) {
        if( multimutation ) {
            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using multi-mutations." );
        } else {
            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using single mutations." );
        }
        std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

        REQUIRE_NOTHROW([&](){

            CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
            problem_container->add_optimization_problem(
                masala::numeric_api::utility::optimization::cost_function_network::construct_test_problem( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" )
            );
            
            MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
                masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
            );
            mcopt->set_use_multimutation( multimutation );
            mcopt->set_solution_storage_mode( "check_at_every_step" );
            mcopt->set_annealing_steps_per_attempt(100000);
            mcopt->set_attempts_per_problem(5);
            mcopt->set_n_solutions_to_store_per_problem(5);
            mcopt->set_cpu_threads_to_request(5);

            ConstantAnnealingSchedule_APISP annealing_schedule( masala::make_shared< ConstantAnnealingSchedule_API >() );
            annealing_schedule->set_temperature(0.9 /*kcal/mol*/); // Hottish.
            mcopt->set_annealing_schedule( *annealing_schedule );
            
            solutions = mcopt->run_cost_function_network_optimizer( *problem_container );


        }() );

        tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Got " + std::to_string( solutions[0]->n_solutions() ) + " solutions." );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "SOLUTION\tTIMES_SEEN\tSCORE\tDR_SCORE\tS_SCORE\tCHOICE_SELECTION" );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "--------\t----------\t-----\t--------\t-------\t----------------" );

        for( masala::base::Size i(0); i<solutions[0]->n_solutions(); ++i ) {
            CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions[0]->solution(i) ) );
            CHECK( solution != nullptr );
            std::ostringstream ss;
            ss << std::setw(8) << i << "\t"
                << std::setw(10) << solution->n_times_solution_was_produced() << "\t"
                << std::setw(5) << solution->solution_score() << "\t"
				<< std::setw(8) << solution->solution_score_data_representation_approximation() << "\t"
				<< std::setw(7) << solution->solution_score_solver_approximation() << "\t"
                << "[" << masala::base::utility::container::container_to_string( solution->solution_at_variable_positions(), ",") << "]";
            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ss.str() );
        }

        CHECK( solutions.size() == 1 );
        CHECK( solutions[0]->n_solutions() == 5 );
        CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 6.0 ) < 1.0e-8 );
        CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 7.0 ) < 1.0e-8 );
        CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 18.0 ) < 1.0e-8 );
        CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 22.0 ) < 1.0e-8 );
        CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 23.0 ) < 1.0e-8 );
    }

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Solve a simple problem with the MonteCarloCostFunctionNetworkOptimizer with a linear annealing schedule.", "[standard_masala_plugins::optimizers_api::auto_generated_api::annealing::LinearAnnealingSchedule_API][standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::base::managers::tracer;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    masala::base::managers::threads::MasalaThreadManager::get_instance()->set_total_threads(5);
    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );

    for( auto const gapped : std::vector< bool >{ false, true } ) {
        for( auto const multimutation : std::vector< bool >{ true, false } ) {
            if( multimutation ) {
                tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using multi-mutations." );
            } else {
                tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using single mutations." );
            }
            std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

            REQUIRE_NOTHROW([&](){

                CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
                problem_container->add_optimization_problem(
                    masala::numeric_api::utility::optimization::cost_function_network::construct_test_problem( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem", gapped )
                );

                MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
                    masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
                );
                mcopt->set_use_multimutation( multimutation );
                mcopt->set_solution_storage_mode( "check_at_every_step" );
                mcopt->set_annealing_steps_per_attempt(100000);
                mcopt->set_attempts_per_problem(5);
                mcopt->set_n_solutions_to_store_per_problem(5);
                mcopt->set_cpu_threads_to_request(3);

                LinearAnnealingSchedule_APISP annealing_schedule( masala::make_shared< LinearAnnealingSchedule_API >() );
                mcopt->set_annealing_schedule( *annealing_schedule );
                
                solutions = mcopt->run_cost_function_network_optimizer( *problem_container );

            }() );

            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ( gapped ? "Gapped results" : "Ungapped results" ) );
            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Got " + std::to_string( solutions[0]->n_solutions() ) + " solutions." );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "SOLUTION\tTIMES_SEEN\tSCORE\tDR_SCORE\tS_SCORE\tCHOICE_SELECTION" );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "--------\t----------\t-----\t--------\t-------\t----------------" );

            for( masala::base::Size i(0); i<solutions[0]->n_solutions(); ++i ) {
                CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions[0]->solution(i) ) );
                CHECK( solution != nullptr );
                std::ostringstream ss;
                ss << std::setw(8) << i << "\t"
                    << std::setw(10) << solution->n_times_solution_was_produced() << "\t"
                    << std::setw(5) << solution->solution_score() << "\t"
					<< std::setw(8) << solution->solution_score_data_representation_approximation() << "\t"
					<< std::setw(7) << solution->solution_score_solver_approximation() << "\t"
                    << "[" << masala::base::utility::container::container_to_string( solution->solution_at_variable_positions(), ",") << "]";
                tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ss.str() );
            }

            CHECK( solutions.size() == 1 );
            CHECK( solutions[0]->n_solutions() == 5 );
            
            masala::base::Real const gap_offset( gapped ? 17.0 : 0.0 );
            CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 6.0 - gap_offset ) < 1.0e-8 );
            CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 7.0 - gap_offset ) < 1.0e-8 );
            CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 18.0 - gap_offset ) < 1.0e-8 );
            CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 22.0 - gap_offset ) < 1.0e-8 );
            CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 23.0 - gap_offset ) < 1.0e-8 );
        }
    }

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Solve a problem with a squared nonlinear count function using the MonteCarloCostFunctionNetworkOptimizer with a linear annealing schedule.", "[masala::numeric_api::auto_generated_api::optimization::cost_function_network::cost_function::SquareOfChoicePenaltySumCostFunction_API][standard_masala_plugins::optimizers_api::auto_generated_api::annealing::LinearAnnealingSchedule_API][standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::base::managers::tracer;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    masala::base::managers::threads::MasalaThreadManager::get_instance()->set_total_threads(5);
    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );

    for( auto const gapped : std::vector< bool >{ false, true } ) {
        for( auto const multimutation : std::vector< bool >{ true, false } ) {
            if( multimutation ) {
                tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using multi-mutations." );
            } else {
                tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using single mutations." );
            }
            std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

            REQUIRE_NOTHROW([&](){

                CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
                problem_container->add_optimization_problem(
                    standard_masala_plugins::optimizers_api::utility::cost_function_network::construct_test_problem_with_squared_choice_count_penalties(  "PairwisePrecomputedCostFunctionNetworkOptimizationProblem", gapped )
                );

                MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
                    masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
                );
                mcopt->set_use_multimutation( multimutation );
                mcopt->set_solution_storage_mode( "check_at_every_step" );
                mcopt->set_annealing_steps_per_attempt(100000);
                mcopt->set_attempts_per_problem(5);
                mcopt->set_n_solutions_to_store_per_problem(5);
                mcopt->set_cpu_threads_to_request(3);

                LinearAnnealingSchedule_APISP annealing_schedule( masala::make_shared< LinearAnnealingSchedule_API >() );
                mcopt->set_annealing_schedule( *annealing_schedule );
                
                solutions = mcopt->run_cost_function_network_optimizer( *problem_container );

            }() );

            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ( gapped ? "Gapped results" : "Ungapped results" ) );
            tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Got " + std::to_string( solutions[0]->n_solutions() ) + " solutions." );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "SOLUTION\tTIMES_SEEN\tSCORE\tDR_SCORE\tS_SCORE\tCHOICE_SELECTION" );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "--------\t----------\t-----\t--------\t-------\t----------------" );

            for( masala::base::Size i(0); i<solutions[0]->n_solutions(); ++i ) {
                CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions[0]->solution(i) ) );
                CHECK( solution != nullptr );
                std::ostringstream ss;
                ss << std::setw(8) << i << "\t"
                    << std::setw(10) << solution->n_times_solution_was_produced() << "\t"
                    << std::setw(5) << solution->solution_score() << "\t"
					<< std::setw(8) << solution->solution_score_data_representation_approximation() << "\t"
					<< std::setw(7) << solution->solution_score_solver_approximation() << "\t"
                    << "[" << masala::base::utility::container::container_to_string( solution->solution_at_variable_positions(), ",") << "]";
                tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ss.str() );
            }

            CHECK( solutions.size() == 1 );
            CHECK( solutions[0]->n_solutions() == 5 );

            if( gapped ) {
                CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 39.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 59.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 60.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 64.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 80.0 ) < 1.0e-8 );
            } else {
                CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 18.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 22.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 23.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 39.0 ) < 1.0e-8 );
                CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 47.0 ) < 1.0e-8 );
            }
        }
    }

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Solve a problem with a sum of count function using the MonteCarloCostFunctionNetworkOptimizer with a linear annealing schedule.", "[masala::numeric_api::auto_generated_api::optimization::cost_function_network::cost_function::FunctionOfIntegerPenaltySumCostFunction_API][standard_masala_plugins::optimizers_api::auto_generated_api::annealing::LinearAnnealingSchedule_API][standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::base::managers::tracer;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    masala::base::managers::threads::MasalaThreadManager::get_instance()->set_total_threads(5);
    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );

    for( auto const gapped : std::vector< bool >{ false, true } ) {
        for( auto const multimutation : std::vector< bool >{ true, false } ) {
			for( auto const greedy : std::vector< bool >{ false, true } ) {

				if( multimutation ) {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using multi-mutations." );
				} else {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using single mutations." );
				}
				if( greedy ) {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using greedy refinement." );
				} else {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Without greedy refinement." );
				}
				std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

				REQUIRE_NOTHROW([&](){

					CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
					problem_container->add_optimization_problem(
						standard_masala_plugins::optimizers_api::utility::cost_function_network::construct_test_problem_with_function_of_integer_penalty_sum_penalties( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem", gapped )
					);

					MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
						masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
					);
					mcopt->set_use_multimutation( multimutation );
					mcopt->set_solution_storage_mode( "check_at_every_step" );
					mcopt->set_annealing_steps_per_attempt( greedy ? 10000 : 100000 );
					mcopt->set_attempts_per_problem(5);
					mcopt->set_n_solutions_to_store_per_problem(5);
					mcopt->set_cpu_threads_to_request(3);
					mcopt->set_do_greedy_refinement( greedy );
					mcopt->set_greedy_refinement_mode( "refine_all" );

					LinearAnnealingSchedule_APISP annealing_schedule( masala::make_shared< LinearAnnealingSchedule_API >() );
					mcopt->set_annealing_schedule( *annealing_schedule );
					
					solutions = mcopt->run_cost_function_network_optimizer( *problem_container );

				}() );

				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ( gapped ? "Gapped results" : "Ungapped results" ) );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Got " + std::to_string( solutions[0]->n_solutions() ) + " solutions." );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "SOLUTION\tTIMES_SEEN\tSCORE\tDR_SCORE\tS_SCORE\tCHOICE_SELECTION" );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "--------\t----------\t-----\t--------\t-------\t----------------" );

				for( masala::base::Size i(0); i<solutions[0]->n_solutions(); ++i ) {
					CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions[0]->solution(i) ) );
					CHECK( solution != nullptr );
					std::ostringstream ss;
					ss << std::setw(8) << i << "\t"
						<< std::setw(10) << solution->n_times_solution_was_produced() << "\t"
						<< std::setw(5) << solution->solution_score() << "\t"
						<< std::setw(8) << solution->solution_score_data_representation_approximation() << "\t"
						<< std::setw(7) << solution->solution_score_solver_approximation() << "\t"
						<< "[" << masala::base::utility::container::container_to_string( solution->solution_at_variable_positions(), ",") << "]";
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ss.str() );
				}

				CHECK( solutions.size() == 1 );
				CHECK( solutions[0]->n_solutions() == (greedy ? 1 : 5 ) );

				if( gapped ) {
					CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 39.0 ) < 1.0e-8 );
					if( !greedy ) {
						CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 45.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 55.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 57.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 62.0 ) < 1.0e-8 );
					}
				} else {
					CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 7.0 ) < 1.0e-8 );
					if( !greedy ) {
						CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 21.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 33.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 38.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 40.0 ) < 1.0e-8 );
					}
				}
			}
        }
    }

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

TEST_CASE( "Solve a problem with satisfiable features using the MonteCarloCostFunctionNetworkOptimizer with a linear annealing schedule.", "[masala::numeric_api::auto_generated_api::optimization::cost_function_network::cost_function::feature_based::SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_API][standard_masala_plugins::optimizers_api::auto_generated_api::annealing::LinearAnnealingSchedule_API][standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::MonteCarloCostFunctionNetworkOptimizer_API][optimization]" ) {
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::annealing;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::base::managers::tracer;
    
    optimizers_api::auto_generated_api::registration::register_optimizers();
    masala::numeric_api::auto_generated_api::registration::register_numeric();

    masala::base::managers::threads::MasalaThreadManager::get_instance()->set_total_threads(5);
    MasalaTracerManagerHandle tracer( MasalaTracerManager::get_instance() );

    for( auto const gapped : std::vector< bool >{ false, true } ) {
        for( auto const multimutation : std::vector< bool >{ true, false } ) {
			for( auto const greedy : std::vector< bool >{ false, true } ) {
				if( multimutation ) {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using multi-mutations." );
				} else {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using single mutations." );
				}
				if( greedy ) {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Using greedy refinement." );
				} else {
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Without greedy refinement." );
				}
				std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions;

				REQUIRE_NOTHROW([&](){

					CostFunctionNetworkOptimizationProblems_APISP problem_container( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );
					problem_container->add_optimization_problem(
						standard_masala_plugins::optimizers_api::utility::cost_function_network::construct_test_problem_with_squared_unsatisfied_feature_penalties( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem", gapped )
					);

					MonteCarloCostFunctionNetworkOptimizer_APISP mcopt(
						masala::make_shared< MonteCarloCostFunctionNetworkOptimizer_API >()
					);
					mcopt->set_use_multimutation( multimutation );
					mcopt->set_solution_storage_mode( "check_at_every_step" );
					mcopt->set_annealing_steps_per_attempt( greedy ? 10000 : 100000 );
					mcopt->set_attempts_per_problem(5);
					mcopt->set_n_solutions_to_store_per_problem(5);
					mcopt->set_cpu_threads_to_request(3);
					mcopt->set_do_greedy_refinement( greedy );
					mcopt->set_greedy_refinement_mode( "refine_top" );

					LinearAnnealingSchedule_APISP annealing_schedule( masala::make_shared< LinearAnnealingSchedule_API >() );
					mcopt->set_annealing_schedule( *annealing_schedule );
					
					solutions = mcopt->run_cost_function_network_optimizer( *problem_container );

				}() );

				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ( gapped ? "Gapped results" : "Ungapped results" ) );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "Got " + std::to_string( solutions[0]->n_solutions() ) + " solutions." );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "SOLUTION\tTIMES_SEEN\tSCORE\tDR_SCORE\tS_SCORE\tCHOICE_SELECTION" );
				tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", "--------\t----------\t-----\t--------\t-------\t----------------" );

				for( masala::base::Size i(0); i<solutions[0]->n_solutions(); ++i ) {
					CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions[0]->solution(i) ) );
					CHECK( solution != nullptr );
					std::ostringstream ss;
					ss << std::setw(8) << i << "\t"
						<< std::setw(10) << solution->n_times_solution_was_produced() << "\t"
						<< std::setw(5) << solution->solution_score() << "\t"
						<< std::setw(8) << solution->solution_score_data_representation_approximation() << "\t"
						<< std::setw(7) << solution->solution_score_solver_approximation() << "\t"
						<< "[" << masala::base::utility::container::container_to_string( solution->solution_at_variable_positions(), ",") << "]";
					tracer->write_to_tracer( "MonteCarloCostFunctionNetworkOptimizerTests", ss.str() );
				}

				CHECK( solutions.size() == 1 );
				CHECK( solutions[0]->n_solutions() == ( greedy ? 1 : 5 ) );

				if( !gapped ) {
					CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 18.0 ) < 1.0e-8 );
					if( !greedy ) {
						CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 21.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 22.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 40.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 53.0 ) < 1.0e-8 );
					}
				} else {
					CHECK( std::abs( solutions[0]->solution(0)->solution_score() - 18.0 - 17.0 ) < 1.0e-8 );
					if( !greedy ) {
						CHECK( std::abs( solutions[0]->solution(1)->solution_score() - 21.0 - 17.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(2)->solution_score() - 22.0 - 17.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(3)->solution_score() - 40.0 - 17.0 ) < 1.0e-8 );
						CHECK( std::abs( solutions[0]->solution(4)->solution_score() - 53.0 - 17.0 ) < 1.0e-8 );
					}
				}
			}
        }
    }

    optimizers_api::auto_generated_api::registration::unregister_optimizers();
    masala::numeric_api::auto_generated_api::registration::unregister_numeric();
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
