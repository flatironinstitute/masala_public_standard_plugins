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

/// @file src/optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.cc
/// @brief Implementation for a CostFunctionNetworkOptimizer that solves a cost function network problem using Monte
/// Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/MonteCarloCostFunctionNetworkOptimizer.hh>

// Optimizers headers:
#include <optimizers/cost_function_network/GreedyCostFunctionNetworkOptimizer.hh>


// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/annealing/AnnealingScheduleBase_API.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/setter_annotation/OwnedSingleObjectSetterAnnotation.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/threads/MasalaThreadedWorkRequest.hh>
#include <base/managers/threads/MasalaThreadedWorkExecutionSummary.hh>
#include <base/managers/random/MasalaRandomNumberGenerator.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <iostream>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// ENUM HELPER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given a greedy optimization refinement mode, get its name string.
std::string
greedy_refinement_name_from_mode(
	MCOptimizerGreedyRefinementMode const greedy_mode
) {
	switch( greedy_mode ) {
		case MCOptimizerGreedyRefinementMode::REFINE_BEST_COLLECTED_FROM_ALL_TRAJECTORIES :
			return "refine_top";
		case MCOptimizerGreedyRefinementMode::REFINE_BEST_COLLECTED_FROM_ALL_TRAJECTORIES_KEEPING_ORIGINAL :
			return "refine_top_keeping_original";
		case MCOptimizerGreedyRefinementMode::REFINE_BEST_OF_EACH_TRAJECTORY :
			return "refine_all";
		default:
			MASALA_THROW( "standard_masala_plugins::optimizers::cost_function_network", "greedy_refinement_name_from_mode",
				"An unrecognized greedy refinement mode was passed to this function!"
			);
	}
	return "";
}

/// @brief Given a greedy optimization mode name string, get its enum.
/// @details Returns MCOptimizerGreedyRefinementMode::INVALID_MODE if the string could not be
/// parsed as a valid mode.
MCOptimizerGreedyRefinementMode
greedy_refinement_mode_from_name(
	std::string const & greedy_mode_string
) {
	using masala::base::Size;
	for( Size i(1); i<=static_cast<Size>(MCOptimizerGreedyRefinementMode::N_MODES); ++i ) {
		if( greedy_refinement_name_from_mode( static_cast< MCOptimizerGreedyRefinementMode >( i ) ) == greedy_mode_string ) {
			return static_cast< MCOptimizerGreedyRefinementMode >( i );
		}
	}
	return MCOptimizerGreedyRefinementMode::INVALID_MODE;
}

/// @brief List all greedy optimization modes, as a comma-separated list.
std::string
get_all_greedy_refinement_modes() {
	using masala::base::Size;
	std::stringstream ss;
	for( Size i(1); i<=static_cast<Size>(MCOptimizerGreedyRefinementMode::N_MODES); ++i ) {
		if( i != 1 ) {
			ss << ", ";
		}
		if( i == static_cast<Size>(MCOptimizerGreedyRefinementMode::N_MODES) ) {
			ss << "and ";
		}
		ss << greedy_refinement_name_from_mode( static_cast< MCOptimizerGreedyRefinementMode >( i ) );
	}
	return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer::MonteCarloCostFunctionNetworkOptimizer(
    MonteCarloCostFunctionNetworkOptimizer const & src
) :
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer( src )
{
    std::lock_guard< std::mutex > lock( src.optimizer_mutex_ );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
    attempts_per_problem_ = src.attempts_per_problem_;
    n_solutions_to_store_per_problem_ = src.n_solutions_to_store_per_problem_;
    annealing_steps_per_attempt_ = src.annealing_steps_per_attempt_;
    annealing_schedule_ = ( src.annealing_schedule_ == nullptr ? nullptr : src.annealing_schedule_->deep_clone() );

    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->reset_call_count();
    }

    solution_storage_mode_ = src.solution_storage_mode_;
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
MonteCarloCostFunctionNetworkOptimizer &
MonteCarloCostFunctionNetworkOptimizer::operator=( MonteCarloCostFunctionNetworkOptimizer const & src ) {
    std::lock( optimizer_mutex_, src.optimizer_mutex_ );
    std::lock_guard< std::mutex > lock1( optimizer_mutex_, std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.optimizer_mutex_, std::adopt_lock );
    masala::numeric_api::base_classes::optimization::cost_function_network::CostFunctionNetworkOptimizer::operator=( src );
    cpu_threads_to_request_ = src.cpu_threads_to_request_;
    attempts_per_problem_ = src.attempts_per_problem_;
    annealing_steps_per_attempt_ = src.annealing_steps_per_attempt_;
    n_solutions_to_store_per_problem_ = src.n_solutions_to_store_per_problem_;
    annealing_schedule_ = ( src.annealing_schedule_ == nullptr ? nullptr : src.annealing_schedule_->deep_clone() );
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->reset_call_count();
    }
    solution_storage_mode_ = src.solution_storage_mode_;
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
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_ = annealing_schedule_->deep_clone();
    }
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
/// @returns { "optimizer", "cost_function_network", "numeric", "monte_carlo", "simulated_annealing", "stochastic" }
std::vector< std::string >
MonteCarloCostFunctionNetworkOptimizer::get_keywords() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	std::vector< std::string > keywords( CostFunctionNetworkOptimizer::get_keywords() );
    keywords.push_back( "monte_carlo" );
    keywords.push_back( "simulated_annealing" );
    keywords.push_back( "stochastic" );
    return keywords;
}

/// @brief Get the category that this MasalaEngine fits into.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }.
std::vector< std::vector< std::string > >
MonteCarloCostFunctionNetworkOptimizer::get_engine_categories() const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return CostFunctionNetworkOptimizer::get_engine_categories();
}

/// @brief Get the class name.
/// @returns "MonteCarloCostFunctionNetworkOptimizer".
std::string
MonteCarloCostFunctionNetworkOptimizer::class_name() const {
    return class_name_static();
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
std::string
MonteCarloCostFunctionNetworkOptimizer::class_namespace() const {
    return class_namespace_static();
}

/// @brief Get the class name (static version).
/// @returns "MonteCarloCostFunctionNetworkOptimizer".
//static
std::string
MonteCarloCostFunctionNetworkOptimizer::class_name_static() {
    return "MonteCarloCostFunctionNetworkOptimizer";
}

/// @brief Get the class namespace (static version).
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
//static
std::string
MonteCarloCostFunctionNetworkOptimizer::class_namespace_static() {
    return "standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC STATIC ENUM FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the storage modes, as a list separated by a delimiter.
/// @param delimiter The delimiter to use.  (For instance, ", " for a comma-separated list).
/// @param include_and If true, the string "and " is inserted before the last entry.
/*static*/
std::string
MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_strings(
    std::string const & delimiter,
    bool const include_and
) {
    using masala::base::Size;
    std::ostringstream ss;
    for( Size i(1); i<=static_cast<Size>(MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::NUM_SOLUTION_STORAGE_MODES); ++i ) {
        if( i > 1 ) {
            ss << delimiter;
        }
        if( include_and && static_cast<Size>(MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::NUM_SOLUTION_STORAGE_MODES) > 2 ) {
            ss << "and ";
        }
        ss << solution_storage_mode_string_from_enum( static_cast<MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode>(i) );
    }
    return ss.str();
}

/// @brief Given a solution storage mode as an enum, get the corresponding string.
/// @param mode_enum The solution storage mode.
/// @note Throws if the mode is INVALID.
/*static*/
std::string
MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_string_from_enum(
    MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const mode_enum
) {
    switch(mode_enum) {
        case MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::CHECK_AT_EVERY_STEP :
            return "check_at_every_step";
        case MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::CHECK_ON_ACCEPTANCE :
            return "check_on_acceptance";
        default:
            MASALA_THROW( class_namespace_static() + "::" + class_name_static(), "solution_storage_mode_string_from_enum", "Invalid solution storage mode!" );
    }
    return "invalid"; // Should never reach here.
}

/// @brief Given a solution storage mode as a string, get the corresponding enum.
/// @param mode_string The solution storage mode, expressed as a string.
/// @returns The corresponding enum, or INVALID if the string could not be parsed.
/*static*/
MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode
MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_enum_from_string(
    std::string const & mode_string
) {
    using masala::base::Size;
    for( Size i(1); i<=static_cast<Size>(MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::NUM_SOLUTION_STORAGE_MODES); ++i ) {
        if( solution_storage_mode_string_from_enum(static_cast<MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode>(i)) == mode_string ) {
            return static_cast<MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode>(i);
        }
    }
    return MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::INVALID_MODE;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
MonteCarloCostFunctionNetworkOptimizer::get_api_definition() {
    using namespace masala::base::managers::plugin_module;
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using namespace masala::base::api::getter;
    using namespace masala::base::api::work_function;
    using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    if( api_description_ == nullptr ) {
        MasalaObjectAPIDefinitionSP api_description(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this,
                "An optimizer for cost function network problems that uses a Monte Carlo search to optimize.  This "
                "performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to "
                "pick a node at random and change its setting at random, compute the change in overall energy or score, "
                "and accept or reject the move based on the difference in energy and the Metropolis criterion.",
                false, false
            )
        );

        // Constructors:
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( MonteCarloCostFunctionNetworkOptimizer, api_description );

        // Setters:
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_cpu_threads_to_request", "Sets the number of threads to request when running problems in parallel.",
				"threads_in", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_cpu_threads_to_request, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_attempts_per_problem", "Sets the number of times to try each problem.",
				"attempts_in", "The number of times to try each problem.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_attempts_per_problem, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_n_solutions_to_store_per_problem", "Sets the maximum number of solutions to return for each problem.",
				"n_solutions_in", "The maximum number solutions to return for each problem.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_n_solutions_to_store_per_problem, this, std::placeholders::_1 )
			)
		);
		{
            std::string const available_annealing_schedules(
				masala::base::managers::plugin_module::MasalaPluginModuleManager::get_instance()->get_short_names_of_plugins_by_category_cs_list(
					std::vector< std::string >{ "AnnealingSchedule" }, true
				)
			);

			MasalaObjectAPISetterDefinition_OneInputSP< MasalaPluginAPI const & > annealing_sched_setter(
				masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< MasalaPluginAPI const & > > (
					"set_annealing_schedule", "Sets the annealing schedule to use for the problem.  Available annealing schedule types include: "
					+ available_annealing_schedules + ".",
					"annealing_schedule_in", "The annealing schedule to use.  Cloned on input.", false, false,
					std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule, this, std::placeholders::_1 )
				)
			);
			setter_annotation::OwnedSingleObjectSetterAnnotationSP owned_annotation(
				masala::make_shared< setter_annotation::OwnedSingleObjectSetterAnnotation >()
			);
			owned_annotation->set_plugin_manager_info(
				std::vector< std::string >{ "AnnealingSchedule" },
				std::vector< std::string >{ "annealing_schedule" }
			);
			annealing_sched_setter->add_setter_annotation( owned_annotation );
			api_description->add_setter( annealing_sched_setter );

			api_description->add_setter(
				masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
					"set_annealing_schedule_by_name", "Sets the annealing schedule, by name.  Creates an annealing schedule and leaves it with "
					"its default configuration (so the set_annealing_schedule() setter should be used instead if you wish to configure "
					"the annealing schedule).  The name need not include namespace unless there is a name conflict.  Available annealing schedules include: "
					+ available_annealing_schedules + ".",
					"annealing_schedule_name", "The name of the annealing schedule.  Must be one of: " + available_annealing_schedules + ".",
					false, false, std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule_by_name, this, std::placeholders::_1 )
				)
			);
		}
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_annealing_steps_per_attempt", "Sets the length of the Monte Carlo trajectory performed for each attempt of each problem.",
				"steps_in", "The number of steps in the Monte Carlo trajectory.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_annealing_steps_per_attempt, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > > (
				"set_solution_storage_mode", "Sets the solution storage mode.  The 'check_at_every_step' option (default) promotes diversity "
                "at the expense of slower computation, checking every solution considered to see whether it should be stored. The "
                "'check_on_acceptance' option only checks whether to store a solution when it is accepted.",
				"storage_mode_in", "A string representing the solution storage mode.  Options are " + solution_storage_mode_strings( ", ", true ),
                false, false,
				std::bind(
                    static_cast< void(MonteCarloCostFunctionNetworkOptimizer::*)(std::string const &) >(
                        &MonteCarloCostFunctionNetworkOptimizer::set_solution_storage_mode
                    ), this, std::placeholders::_1
                )
			)
		);
        api_description->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput < bool > >(
                "set_use_multimutation", "Set whether we're using multimutations.  If true, we select the number of mutation positions from a "
                "Poisson distribution.  If false, we only mutate one node at a time.  True by default.  Note that we actually take a Poisson "
                "distribution and add 1, since we don't want 0 mutations.",
                "use_multimutation", "True if we should use multimutations, false if we should do one mutation at a time.",
                false, false, std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_use_multimutation, this, std::placeholders::_1 )
            )
        );
        api_description->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput < masala::base::Real > >(
                "set_multimutation_probability_of_one_mutation", "Set the probability of having 1 mutation.  Must be a value between 0 and 1.  "
                "Default 0.75.  Used to find the value of lambda for the Poisson distribution.  Since we add 1 to the value that comes out of "
                "the Poisson distribution, the value of P(0) is set to this value: "
                "lambda^k exp(-lambda) / k!, "
                "P(0) = exp(-lambda), "
                "-ln( P(0) ) = lambda.  "
                "Note that this function throws if outside of the range (0, 1].",
                "probability_in", "The probability of having one mutation in multimutation mode.  Must be in the range (0, 1].",
                false, false, std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_multimutation_probability_of_one_mutation, this, std::placeholders::_1 )
            )
        );
        api_description->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< bool const > >(
                "set_do_greedy_refinement", "Set whether we do greedy optimization at the end on each solution "
				"found by the Monte Carlo search.  False by default.",
                "do_greedy_refinement_in", "True if we're doing greedy refinement, false otherwise.",
                false, false,
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::set_do_greedy_refinement, this, std::placeholders::_1 )
            )
        );
        api_description->add_setter(
            masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
                "set_greedy_refinement_mode", "Set the greedy refinement mode, as a string.  Allowed modes are: "
				+ get_all_greedy_refinement_modes() + ".  Briefly, refine_top will perform greedy optimization refinement "
				"on the top solutions collected from Monte Carlo trajectories (returning only refined solutions), refine_top_keeping_original will do the same, "
				"but will return both the original MC trajectory top solutions and the refined solutions in the same "
				"output list, and refine_all will refine the top solutions from each Monte Carlo trajectory (returning only refined solutions)."
				,
                "greedy_refinement_mode_in", "The greedy refinement mode.  Allowed options are " + get_all_greedy_refinement_modes() + ".",
                false, false,
                std::bind(
					static_cast<
						void(MonteCarloCostFunctionNetworkOptimizer::*)(std::string const &)
					> (
						&MonteCarloCostFunctionNetworkOptimizer::set_greedy_refinement_mode
					),
					this,
					std::placeholders::_1
				)
            )
        );

		// Getters:
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"cpu_threads_to_request", "Gets the number of threads to request when running problems in parallel.",
				"cpu_threads_to_request", "The number of CPU threads to request.  This is a maximum; fewer are requested if there are fewer "
				"problem replicates to try.  A setting of 0 means \"request all available\".", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::cpu_threads_to_request, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"attempts_per_problem", "Gets the number of times to try each problem.",
				"attempts_per_problem", "The number of times to try each problem.  Minimum 1.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::attempts_per_problem, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"n_solutions_to_store_per_problem", "Gets the maximum number of solutions that will be returned for each problem.",
				"n_solutions_to_store_per_problem", "The maximum number of solutions that will be returned for each problem.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::n_solutions_to_store_per_problem, this )
			)
		);
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"annealing_steps_per_attempt", "Gets the length of the Monte Carlo trajectory performed for each attempt of each problem.",
				"steps", "The number of steps in the Monte Carlo trajectory.", false, false,
				std::bind( &MonteCarloCostFunctionNetworkOptimizer::annealing_steps_per_attempt, this )
			)
		);
        api_description->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & > > (
                "annealing_schedule", "Allows const access to the annealing schedule, to allow its parameters to be examined.  Not threadsafe!",
                "annealing_schedule", "A const reference to the annealing schedule.", false, false,
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::annealing_schedule, this )
            )
        );
        api_description->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< std::string > >(
                "solution_storage_mode_string", "Get the solution storage mode, as a string.",
                "solution_storage_mode", "A string representing the solution storage mode.",
                false, false,
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_string, this )
            )
        );
        api_description->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
                "use_multimutation", "Get whether we're using multimutations.  If true, we select the number of mutation positions from a Poisson "
                "distribution.  If false, we only mutate one node at a time.  True by default.",
                "use_multimutation", "True if we're using multimutations, false if we're doing one mutation at a time.",
                false, false,
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::use_multimutation, this )
            )
        );
        api_description->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< masala::base::Real > >(
                "multimutation_probability_of_one_mutation", "Get the probability of having 1 mutation.  Must be a value between 0 and 1.  Default 0.75.",
                "multimutation_probability_of_one_mutation", "The probability of having exactly one mutation if multimutations are being used.",
                false, false,
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::multimutation_probability_of_one_mutation, this )
            )
        );
        api_description->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
                "do_greedy_refinement", "Get whether we do greedy optimization at the end on each solution "
				"found by the Monte Carlo search.  False by default.",
                "do_greedy_refinement", "True if we're doing greedy refinement, false otherwise.",
                false, false,
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::do_greedy_refinement, this )
            )
        );
        api_description->add_getter(
            masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< std::string > >(
                "greedy_refinement_mode_string", "Get the greedy refinement mode, as a string.  Possible modes are: "
				+ get_all_greedy_refinement_modes() + ".  Briefly, refine_top will perform greedy optimization refinement "
				"on the top solutions collected from Monte Carlo trajectories (returning only refined solutions), refine_top_keeping_original will do the same, "
				"but will return both the original MC trajectory top solutions and the refined solutions in the same "
				"output list, and refine_all will refine the top solutions from each Monte Carlo trajectory (returning only refined solutions)."
				,
                "greedy_refinement_mode_string", "The greedy refinement mode, as a string.  Possible outputs are " + get_all_greedy_refinement_modes() + ".",
                false, false,
                std::bind(
					&MonteCarloCostFunctionNetworkOptimizer::greedy_refinement_mode_string,
					this
				)
            )
        );

        // Work functions:
        api_description->add_work_function(
            masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_ZeroInput< masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API & > > (
                "annealing_schedule_nonconst", "Allows nonconst access to the annealing schedule, to allow its parameters to be configured.  Not threadsafe!",
                false, false, false, false,
                "annealing_schedule", "A nonconst reference to the annealing schedule.",
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::annealing_schedule_nonconst, this )
            )
        );
        api_description->add_work_function(
            masala::make_shared<
                MasalaObjectAPIWorkFunctionDefinition_OneInput<
                    std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >,
                    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const &
                >
            >(
                "run_cost_function_network_optimizer", "Run the optimizer on a cost function network optimization problem, and produce a solution.",
                true, false, true, false,
                "problems", "A set of problems to run.",
                "solutions", "A vector of solution sets.  Each CostFunctionNetworkOptimizationSolutions object contains the set of solutions for the problem "
                "in the input vector with the corresponding index.  There may be multiple solutions, depending on settings.",
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer, this, std::placeholders::_1 )
            )
        );

        // Convert nonconst to const:
        api_description_ = api_description;
    }
    return api_description_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the number of threads to request.
/// @details The default setting of 0 means "request all available".
void
MonteCarloCostFunctionNetworkOptimizer::set_cpu_threads_to_request(
    masala::base::Size const threads_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    cpu_threads_to_request_ = threads_in;
}

/// @brief Set the number of times to try each problem.
/// @details Minimum is 1.
void
MonteCarloCostFunctionNetworkOptimizer::set_attempts_per_problem(
    masala::base::Size const attempts_in
) {
    CHECK_OR_THROW_FOR_CLASS( attempts_in > 0, "set_attempts_per_problem", "The number of attempts per problem must be greater than zero." );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    attempts_per_problem_ = attempts_in;
}

/// @brief Set the number of solutions to return for each problem.
void
MonteCarloCostFunctionNetworkOptimizer::set_n_solutions_to_store_per_problem(
    masala::base::Size const n_solutions_in
) {
    CHECK_OR_THROW_FOR_CLASS( n_solutions_in > 0, "set_n_solutions_to_store_per_problem", "The number of solutions to return per problem must be greater than zero." );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    n_solutions_to_store_per_problem_ = n_solutions_in;
}

/// @brief Set the annealing schedule to use for annealing.
/// @details Cloned on input.  Throws if the plugin module passed in is not an annealing schedule.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule(
	masala::base::managers::plugin_module::MasalaPluginAPI const & schedule_in
) {
	using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
	AnnealingScheduleBase_API const * anneal_sched_ptr( dynamic_cast< AnnealingScheduleBase_API const * >( &schedule_in ) );
	CHECK_OR_THROW_FOR_CLASS( anneal_sched_ptr != nullptr, "set_annealing_schedule", "The " + schedule_in.inner_class_name() + " object passed to this function was not an AnnealingScheduleBase-derived class." );
	std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	annealing_schedule_ = anneal_sched_ptr->deep_clone();
	annealing_schedule_->set_final_time_index( annealing_steps_per_attempt_ );
	annealing_schedule_->reset_call_count();
}

/// @brief Set the annealing schedule by name.
/// @details Namespace is not required unless the name is not unique.  Throws if
/// the name is not found at all in the plugin manager.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_schedule_by_name(
    std::string const & schedule
) {
    using namespace masala::base::managers::plugin_module;
    using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
    MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );
    MasalaPluginAPISP plugin(
        plugman->create_plugin_object_instance_by_short_name( std::vector< std::string >{ "AnnealingSchedule" }, schedule, true )
    );
    AnnealingScheduleBase_APISP annsched( std::dynamic_pointer_cast< AnnealingScheduleBase_API >( plugin ) );
    CHECK_OR_THROW_FOR_CLASS( annsched != nullptr, "set_annealing_schedule_by_name", "Program error getting annealing schedule of type \""
        + schedule + "\".  The returned object was not an annealing schedule."
    );
    {
        std::lock_guard< std::mutex > lock( optimizer_mutex_ );
        annealing_schedule_ = annsched;
        annealing_schedule_->set_final_time_index( annealing_steps_per_attempt_ );
        annealing_schedule_->reset_call_count();
    }
}

/// @brief Set the numer of Monte Carlo moves to make in each attempt.
void
MonteCarloCostFunctionNetworkOptimizer::set_annealing_steps_per_attempt(
    masala::base::Size const steps_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    annealing_steps_per_attempt_ = steps_in;
    if( annealing_schedule_ != nullptr ) {
        annealing_schedule_->set_final_time_index( steps_in );
        annealing_schedule_->reset_call_count();
    }
}

/// @brief Set the solution storage mode, by enum.
void
MonteCarloCostFunctionNetworkOptimizer::set_solution_storage_mode(
    MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const solution_storage_mode_in
) {
    CHECK_OR_THROW_FOR_CLASS(
        solution_storage_mode_in != MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::INVALID_MODE,
        "set_solution_storage_mode", "An invalid mode was passed to this function!"
    );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    solution_storage_mode_ = solution_storage_mode_in;
}

/// @brief Set the solution storage mode, by string.
void
MonteCarloCostFunctionNetworkOptimizer::set_solution_storage_mode(
    std::string const & solution_storage_mode_string_in
) {
    MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const mode_enum(
        solution_storage_mode_enum_from_string( solution_storage_mode_string_in )
    );
    CHECK_OR_THROW_FOR_CLASS(
        mode_enum != MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::INVALID_MODE,
        "set_solution_storage_mode", "Could not parse \"" + solution_storage_mode_string_in +
        "as a valid solution storage mode!"
    );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    solution_storage_mode_ = mode_enum;
}

/// @brief Set whether we're using multimutations.
/// @details If true, we select the number of mutation positions from a Poisson distribution.  If false, we only
/// mutate one node at a time.  True by default.
/// @note We actually take a Poisson distribution and add 1, since we don't want 0 mutations.
void
MonteCarloCostFunctionNetworkOptimizer::set_use_multimutation(
    bool const setting
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    use_multimutation_ = setting;
}

/// @brief Set the probability of having 1 mutation.  Must be a value between 0 and 1.  Default 0.75.
/// @details Used to find the value of lambda for the Poisson distribution.  Since we add 1 to the value
/// that comes out of the Poisson distribution, the value of P(0) is set to this value:
/// P(k) = lambda^k exp(-lambda) / k!
/// P(0) = exp(-lambda)
/// -ln( P(0) ) = lambda
/// @note Throws if outside of the range (0, 1].
void
MonteCarloCostFunctionNetworkOptimizer::set_multimutation_probability_of_one_mutation(
    masala::base::Real const probability_in
) {
    CHECK_OR_THROW_FOR_CLASS( probability_in > 0.0 && probability_in <= 1.0,
        "set_multimutation_probability_of_one_mutation",
        "The probability of 1 mutation must be in the range (0, 1].  Got a probability of "
        + std::to_string( probability_in ) + ", though!"
    );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    multimutation_probability_of_one_mutation_ = probability_in;
}

/// @brief Set whether we do greedy optimization at the end on each solution found by the Monte Carlo search.
/// False by default.
void
MonteCarloCostFunctionNetworkOptimizer::set_do_greedy_refinement(
	bool const do_greedy_refinement_in
) {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	do_greedy_refinement_ = do_greedy_refinement_in;
}

/// @brief Set the greedy refinement mode.
void
MonteCarloCostFunctionNetworkOptimizer::set_greedy_refinement_mode(
	MCOptimizerGreedyRefinementMode const mode_in
) {
	CHECK_OR_THROW_FOR_CLASS( mode_in != MCOptimizerGreedyRefinementMode::INVALID_MODE && mode_in <= MCOptimizerGreedyRefinementMode::N_MODES,
		"set_greedy_refinment_mode", "An invalid greedy refinement mode was passed to this function."
	);
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	greedy_refinement_mode_ = mode_in;
}

/// @brief Set the greedy refinement mode, by string.
void
MonteCarloCostFunctionNetworkOptimizer::set_greedy_refinement_mode(
	std::string const & mode_name_in
) {
	MCOptimizerGreedyRefinementMode const mode_in( greedy_refinement_mode_from_name( mode_name_in ) );
	CHECK_OR_THROW_FOR_CLASS( mode_in != MCOptimizerGreedyRefinementMode::INVALID_MODE,
		"set_greedy_refinement_mode", "Could not parse the string \"" + mode_name_in + "\" as a "
		"valid greedy refinement mode.  Allowed modes are: " + get_all_greedy_refinement_modes() + "."
	);
    set_greedy_refinement_mode( mode_in );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the number of threads to request.
/// @details The default setting of 0 means "request all available".
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::cpu_threads_to_request() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return cpu_threads_to_request_;
}

/// @brief Get the number of times to try each problem.
/// @details Minimum is 1.
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::attempts_per_problem() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return attempts_per_problem_;
}

/// @brief Get the number of solutions to return for each problem.
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::n_solutions_to_store_per_problem() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return n_solutions_to_store_per_problem_;
}

/// @brief Get the numer of Monte Carlo moves to make in each attempt.
masala::base::Size
MonteCarloCostFunctionNetworkOptimizer::annealing_steps_per_attempt() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return annealing_steps_per_attempt_;
}

/// @brief Const access to the annealing schedule (to allow its configuration to be examined).
/// @details The annealing schedule must be set before this is called.  Throws otherwise.
masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const &
MonteCarloCostFunctionNetworkOptimizer::annealing_schedule() const {
    CHECK_OR_THROW_FOR_CLASS( annealing_schedule_ != nullptr, "annealing_schedule", "The annealing schedule must be set before it can be accessed." );
    return *annealing_schedule_;
}

/// @brief Get the solution storage mode, by enum.
MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode
MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_enum() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return solution_storage_mode_;
}

/// @brief Get the solution storage mode, by string.
std::string
MonteCarloCostFunctionNetworkOptimizer::solution_storage_mode_string() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return solution_storage_mode_string_from_enum( solution_storage_mode_ );
}

/// @brief Get whether we're using multimutations.
/// @details If true, we select the number of mutation positions from a Poisson distribution.  If false, we only
/// mutate one node at a time.  True by default.
/// @note We actually take a Poisson distribution and add 1, since we don't want 0 mutations.
bool
MonteCarloCostFunctionNetworkOptimizer::use_multimutation() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return use_multimutation_;
}

/// @brief Get the probability of having 1 mutation.  Must be a value between 0 and 1.  Default 0.75.
masala::base::Real
MonteCarloCostFunctionNetworkOptimizer::multimutation_probability_of_one_mutation() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return multimutation_probability_of_one_mutation_;
}

/// @brief Get whether we do greedy optimization at the end on each solution found by the Monte Carlo search.
/// False by default.
bool
MonteCarloCostFunctionNetworkOptimizer::do_greedy_refinement() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	return do_greedy_refinement_;
}

/// @brief Get the greedy refinement mode.
MCOptimizerGreedyRefinementMode
MonteCarloCostFunctionNetworkOptimizer::greedy_refinement_mode() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	return greedy_refinement_mode_;
}

/// @brief Get the greedy refinement mode string.
std::string
MonteCarloCostFunctionNetworkOptimizer::greedy_refinement_mode_string() const {
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
	return greedy_refinement_name_from_mode( greedy_refinement_mode_ );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Access the annealing schedule by nonconst reference (to allow it to be configured).
/// @details The annealing schedule must be set before this is called.  Throws otherwise.
masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API &
MonteCarloCostFunctionNetworkOptimizer::annealing_schedule_nonconst() {
    CHECK_OR_THROW_FOR_CLASS( annealing_schedule_ != nullptr, "annealing_schedule", "The annealing schedule must be set before it can be accessed." );
    std::lock_guard< std::mutex > lock( optimizer_mutex_ );
    return *annealing_schedule_;
}

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
/// the problem with the same index.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
MonteCarloCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
) const {
    using namespace masala::base::managers::threads;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( optimizer_mutex_ );

    CHECK_OR_THROW_FOR_CLASS( annealing_schedule_ != nullptr, "run_cost_function_network_optimizer", "An annealing schedule must be set before calling this function." );
    annealing_schedule_->reset_call_count();
    annealing_schedule_->set_final_time_index( annealing_steps_per_attempt_ );

    // Create storange for solutions.
    Size const nproblems( problems.n_problems() );
    std::vector< CostFunctionNetworkOptimizationSolutions_APISP > solutions_by_problem;
    std::vector< std::mutex > solution_mutexes( nproblems );
    solutions_by_problem.reserve( nproblems );
    for( Size i(0); i<nproblems; ++i ) {
        masala::numeric_api::auto_generated_api::optimization::OptimizationSolutions_APISP new_solutions_container_uncast(
            problems.problem(i)->create_solutions_container()
        );
        CostFunctionNetworkOptimizationSolutions_APISP new_solutions_container(
            std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >(
                new_solutions_container_uncast
            )
        );
        CHECK_OR_THROW_FOR_CLASS( new_solutions_container != nullptr, "run_cost_function_network_optimizer", "Problem "
            + std::to_string(i) + " created a " + new_solutions_container_uncast->inner_class_name() + " container, but this function "
            "only works with CostFunctionNetworkOptimizationSolutions containers.  Program error.  Please consult a developer, as "
            "this ought not to happen."
        );
        solutions_by_problem.push_back( new_solutions_container );
    }
    solutions_by_problem.shrink_to_fit();

    // Create work vector.
    MasalaThreadedWorkRequest work_request( cpu_threads_to_request_ );
    work_request.reserve( nproblems * attempts_per_problem_ );
    for( Size i(0); i<nproblems; ++i ) {
#ifndef NDEBUG
        // Redundant check that this is a cost function network optimization problem in debug mode.
        CostFunctionNetworkOptimizationProblem_APICSP problem_cast( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >(problems.problem(i)) );
        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( problem_cast != nullptr, "run_cost_function_network_optimizer", "Program error: problem "
            + std::to_string(i) + " is not enclosed in a CostFunctionNetworkOptimizationProblem_API!  It is a " + problems.problem(i)->inner_class_name() + " encapsulated in a "
            + problems.problem(i)->class_name() + "."
        );
#else
        // Just assume that this is the right problem type in release mode.
        CostFunctionNetworkOptimizationProblem_APICSP problem_cast( std::static_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >(problems.problem(i)) );
#endif
        for( Size j(0); j<attempts_per_problem_; ++j ) {
            work_request.add_job(
                std::bind( &MonteCarloCostFunctionNetworkOptimizer::run_mc_trajectory, this,
                    j, // replicate index
                    i, // problem index
                    annealing_steps_per_attempt_, // Steps in the MC search.
                    n_solutions_to_store_per_problem_, // Solutions per problem.
                    std::cref( *annealing_schedule_ ), // A copy of the annealing schedule.
                    problem_cast, // The problem description.
                    std::ref( *(solutions_by_problem[i]) ), // The storage for the collection of solutions.
                    ( n_solutions_to_store_per_problem_ > 1 ? solution_storage_mode_ : MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::CHECK_ON_ACCEPTANCE ), // The solution storage mode
                    use_multimutation_, // Do we do more than one mutation at a time?
                    multimutation_probability_of_one_mutation_, // Probability of doing just one mutation, in multimutation mode.
					do_greedy_refinement_, // Do greedy refinement?
					greedy_refinement_mode_, // Greedy refinement mode.
                    std::ref( solution_mutexes[i] ) // A mutex for locking the solution storage for the problem.
                )
            );
        }
    }

    // Do the work.
    MasalaThreadedWorkExecutionSummary const threading_summary( MasalaThreadManager::get_instance()->do_work_in_threads( work_request ) );
    threading_summary.write_summary_to_tracer();

	// Do the greedy refinement, if we're doing that.
	if( do_greedy_refinement_ && greedy_refinement_mode_ != MCOptimizerGreedyRefinementMode::REFINE_BEST_OF_EACH_TRAJECTORY ) {
		write_to_tracer( "Carrying out greedy refinement of all solutions found." );
		carry_out_greedy_refinement( problems, solutions_by_problem, greedy_refinement_mode_ );
	}

    // Nonconst to const requires a silly extra step:
    std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > const_solutions_by_problem( nproblems );
    for( Size i(0); i<nproblems; ++i ) {
        const_solutions_by_problem[i] = solutions_by_problem[i];
    }

    return const_solutions_by_problem;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Perform greedy refinement on all solutions found.
void
MonteCarloCostFunctionNetworkOptimizer::carry_out_greedy_refinement(
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems,
	std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP > & solutions_by_problem,
	MCOptimizerGreedyRefinementMode const greedy_mode
) const {
	using namespace masala::base::managers::threads;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	using masala::base::Size;

	// Sanity check:
	Size const nprob( problems.n_problems() );
	CHECK_OR_THROW_FOR_CLASS( nprob == solutions_by_problem.size(), "carry_out_greedy_refinement", "The number of problems and solutions objects didn't match!" );

	// Prepare a vector of jobs to do.
	MasalaThreadedWorkRequest work_vector( cpu_threads_to_request_ );
	std::vector< std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > > greedy_solutions; // Will ultimately have one solution per solutions object.

	for( Size iprob(0); iprob<nprob; ++iprob ) {
		CostFunctionNetworkOptimizationProblem_APICSP problem_cast(
			std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >( problems.problem( iprob ) )
		);
		CHECK_OR_THROW_FOR_CLASS( problem_cast != nullptr, "carry_out_greedy_refinement", "Optimization problem " + std::to_string(iprob) + " is not a cost function network optimization problem." );

		Size const nsols( solutions_by_problem[iprob]->n_solutions() );
		greedy_solutions.emplace_back( nsols, nullptr );
		for( Size jsol(0); jsol<nsols; ++jsol ) {

			CostFunctionNetworkOptimizationSolution_APICSP mc_solution_cast(
				std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( solutions_by_problem[iprob]->solution( jsol ) )
			);
			CHECK_OR_THROW_FOR_CLASS( mc_solution_cast != nullptr, "carry_out_greedy_refinement",
				"MC solution " + std::to_string( jsol ) + " of problem " + std::to_string( iprob )
				+ " was not a cost function network optimization solution."
			);

			// Prepare the vector of work to do in threads:
			work_vector.add_job(
				std::bind(
					&MonteCarloCostFunctionNetworkOptimizer::do_one_greedy_refinement_in_threads,
					this,
					problem_cast,
					std::ref(greedy_solutions[iprob][jsol]),
                    mc_solution_cast->solution_at_variable_positions(),
					mc_solution_cast->n_times_solution_was_produced()
				)
			);
		}
	}

	// Run the work vector in threads:
	MasalaThreadedWorkExecutionSummary const threading_summary( MasalaThreadManager::get_instance()->do_work_in_threads( work_vector ) );
    threading_summary.write_summary_to_tracer();

	// Repackage greedy solutions into solutions objects, preserving or not preserving the old solutions:
	for( Size iprob(0); iprob < nprob; ++iprob ) {
		CostFunctionNetworkOptimizationSolutions_API & cursols( *solutions_by_problem[iprob] );
		Size const noldsols( cursols.n_solutions() );
		Size n_to_keep;
		if( greedy_mode != MCOptimizerGreedyRefinementMode::REFINE_BEST_COLLECTED_FROM_ALL_TRAJECTORIES_KEEPING_ORIGINAL ) {
			n_to_keep = noldsols;
			for( Size isol( noldsols ); isol > 0; --isol ) {
				cursols.remove_optimization_solution( isol-1 );
			}
		} else {
			n_to_keep = noldsols * 2;
		}

		Size const nnewsol( greedy_solutions[iprob].size() );
		CHECK_OR_THROW_FOR_CLASS( nnewsol == noldsols, "carry_out_greedy_refinement", "Program error.  Expected number of new solutions to match number of old solutions." );
		for( Size jsol(0); jsol < nnewsol; ++jsol ) {
			CHECK_OR_THROW_FOR_CLASS( greedy_solutions[iprob][jsol]->n_solutions() == 1, "carry_out_greedy_refinement",
				"Program error.  Expected 1 solution from greedy refinement for problem " + std::to_string(iprob) + ", Monte Carlo solution "
				+ std::to_string(jsol) + ", but got " + std::to_string( greedy_solutions[iprob][jsol]->n_solutions() ) + "."
			);
			CostFunctionNetworkOptimizationSolution_APICSP curgreedysol(
				std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >(
					greedy_solutions[iprob][jsol]->solution(0)
				)
			);
			CHECK_OR_THROW_FOR_CLASS( curgreedysol != nullptr, "carry_out_greedy_refinement", "Program error.  The solution from greedy refinement of problem "
				+ std::to_string(iprob) + ", Monte Carlo solution " + std::to_string(jsol) + " is not a cost function network optimization solution."
			);
			CostFunctionNetworkOptimizationProblem_APICSP curgreedysolprob(
				std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >(
					curgreedysol->problem()
				)
			);
			CHECK_OR_THROW_FOR_CLASS( curgreedysolprob != nullptr, "carry_out_greedy_refinement", "Program error.  Expected a CostFunctionNetworkOptimizationProblem "
				"class defining the greedy optimization problem, but got " + curgreedysol->problem()->inner_class_name() + "."
			);
			cursols.merge_in_lowest_scoring_solutions(
				{ std::make_tuple( curgreedysol->solution_at_variable_positions(), curgreedysol->solution_score(), curgreedysol->n_times_solution_was_produced() ) },
				n_to_keep,
				curgreedysolprob
			);
		}
	}
}

/// @brief Carry out a single greedy optimization/
/// @details This function runs in threads.
void
MonteCarloCostFunctionNetworkOptimizer::do_one_greedy_refinement_in_threads(
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP greedy_problem,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP & greedy_solutions,
    std::vector< masala::base::Size > const & starting_point,
	masala::base::Size const n_times_seen
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	//write_to_tracer( "************ n_times_seen: " + std::to_string(n_times_seen) ); // DELETE ME

	CHECK_OR_THROW_FOR_CLASS( greedy_solutions == nullptr, "do_one_greedy_refinement_in_threads", "Program error.  Expected solutiuons object to be nullptr." );

	GreedyCostFunctionNetworkOptimizer greedyopt;
	greedyopt.set_cpu_threads_to_request( 1 );
	greedyopt.set_n_times_seen_multiplier( n_times_seen );
    greedyopt.add_optimizer_starting_state( starting_point );
	CostFunctionNetworkOptimizationSolutions_APICSP sols( greedyopt.run_cost_function_network_optimizer_on_one_problem( greedy_problem ) );

	//write_to_tracer( "************ n_times_seen_by_greedy: " + std::to_string(sols[0]->solution(0)->n_times_solution_was_produced()) ); // DELETE ME

	greedy_solutions = sols;
}

/// @brief Run a single Monte Carlo trajectory.
/// @details This function runs in threads.
/// @param[in] replicate_index The index of this replicate for this problem.
/// @param[in] problem_index The index of this problem.
/// @param[in] annealing_steps The number of steps in the trajectory.
/// @param[in] n_solutions_to_store The number of solutions to store.
/// @param[in] annealing_schedule The temperature generator (already configured with the number of steps).
/// @param[in] problem The description of the problem.  This may or may not be a specialized problem like a
/// PrecomputedPairwiseCostFunctionNetworkOptimizationProblem.
/// @param[in] solutions Storage for a collection of solutions.  Should be unique to problem.
/// @param[in] solution_storage_mode The mode for storing solutions.
/// @param[in] use_multimutation If true, we do N mutations, where N is chosen from a Poisson distribution.
/// If false, we do one mutation at a time.
/// @param[in] multimutation_probability_of_one_mutation The probability of just doing one mutation in
/// multimutation mode.
/// @param do_greedy Are we doing greedy refinement?
/// @param greedy_mode The mode for greedy refinement.
/// @param solutions_mutex A mutex for the collection of solutions.
void
MonteCarloCostFunctionNetworkOptimizer::run_mc_trajectory(
    masala::base::Size const replicate_index,
    masala::base::Size const problem_index,
    masala::base::Size const annealing_steps,
    masala::base::Size const n_solutions_to_store,
    masala::numeric_api::auto_generated_api::optimization::annealing::AnnealingScheduleBase_API const & annealing_schedule,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APICSP problem,
    masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_API & solutions,
    MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode const solution_storage_mode,
    bool const use_multimutation,
    masala::base::Real const multimutation_probability_of_one_mutation,
	bool const do_greedy,
	MCOptimizerGreedyRefinementMode const greedy_mode,
    std::mutex & solutions_mutex
) const {
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
    using namespace masala::base::managers::random;
    using masala::base::Real;
    using masala::base::Size;

    // Compute lambda for the Poisson distribution for multiple moves.
    DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS(
        multimutation_probability_of_one_mutation > 0.0 && multimutation_probability_of_one_mutation <= 1.0,
        "run_mc_trajectory",
        "The multimutation probability of one mutations is supposed to be in the interval (0, 1], but "
        "got " + std::to_string( multimutation_probability_of_one_mutation ) + " as the value!"
    );
    masala::base::Real const poisson_lambda( -std::log( multimutation_probability_of_one_mutation ) );

    // Make a copy of the annealing schedule.
    AnnealingScheduleBase_APISP annealing_schedule_copy( annealing_schedule.deep_clone() );
    annealing_schedule_copy->reset_call_count();

    // Store local solutions as a vector of tuples of (solution vector, score, count of times seen)
    std::vector<
        std::tuple<
            std::vector< Size >, // Solution vector.
            Real, // Solution score.
            Size // Number of times seen.
        >
    > local_solutions;
    local_solutions.reserve( n_solutions_to_store );

    /// Selection for the solution:
    std::vector< std::pair< Size, Size > > const n_choices_per_variable_node( problem->n_choices_at_variable_nodes() ); // First index of each pair is node index, second is number of choices.  Only variable nodes are included.
    Size const n_variable_nodes( n_choices_per_variable_node.size() );
    std::vector< std::pair< Size, Size > > n_choices_per_variable_node_using_variable_node_indices( n_variable_nodes );
    for( Size i(0); i<n_variable_nodes; ++i ) {
        n_choices_per_variable_node_using_variable_node_indices[i] = std::pair< Size, Size >( i, n_choices_per_variable_node[i].second );
    }
    std::vector< Size > current_solution( n_variable_nodes ), last_accepted_solution( n_variable_nodes );

    // Get handle to random generator.
    MasalaRandomNumberGeneratorHandle const randgen( MasalaRandomNumberGenerator::get_instance() );

    // Initialize choices randomly:
    for( Size i(0); i<n_variable_nodes; ++i ) {
        current_solution[i] = randgen->uniform_size_distribution( 0, n_choices_per_variable_node[i].second - 1 );
        last_accepted_solution[i] = current_solution[i];
    }
    // Note: these will accumulate numerical errors.
    Real last_accepted_absolute_score( problem->compute_absolute_score( current_solution ) );
    Real candidate_absolute_score( last_accepted_absolute_score );
    // write_to_tracer( "Initial score = " + std::to_string( last_accepted_absolute_score ) ); // DELETE ME

    // Store the starting state as a solution encountered:
    determine_whether_to_store_solution(
        current_solution, candidate_absolute_score, local_solutions, n_solutions_to_store, true /*forcing this solution to be stored*/
    );

    // Main loop over all steps of the annealing trajectory.
    for( Size step_index(0); step_index < annealing_steps; ++step_index ) {
        if( use_multimutation ) {
            make_mc_multimove( current_solution, n_choices_per_variable_node_using_variable_node_indices, poisson_lambda, randgen );
        } else {
            make_mc_move( current_solution, n_choices_per_variable_node, randgen );
        }
        Real const deltaE( problem->compute_score_change( last_accepted_solution, current_solution ) ); // TODO -- option for doing this without mutex lock.
        candidate_absolute_score += deltaE;
        // write_to_tracer( "Move " + std::to_string( step_index ) +
        //     + " old = [" + masala::base::utility::container::container_to_string( last_accepted_solution, "," )
        //     + "] new = [" + masala::base::utility::container::container_to_string( current_solution, "," )
        //     + "] deltaE = " + std::to_string(deltaE) ); // DELETE ME

        // Decide whether to store this solution.  (Even solutions we might not accept, we examine.)
        if( solution_storage_mode == MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::CHECK_AT_EVERY_STEP ) {
            determine_whether_to_store_solution(
                current_solution, candidate_absolute_score, local_solutions, n_solutions_to_store, false
            );
        }

        // Apply the Metropolis criterion to accept or reject the move:
        if( randgen->apply_metropolis_criterion( deltaE, annealing_schedule_copy->temperature() ) ) {
            last_accepted_solution = current_solution;
            last_accepted_absolute_score = candidate_absolute_score;
            //write_to_tracer( "Accepting move " + std::to_string( step_index ) + ".  Current score = " + std::to_string( last_accepted_absolute_score ) + "." ); // DELETE ME            
            if( solution_storage_mode == MonteCarloCostFunctionNetworkOptimizerSolutionStorageMode::CHECK_ON_ACCEPTANCE ) {
                determine_whether_to_store_solution(
                    current_solution, candidate_absolute_score, local_solutions, n_solutions_to_store, false
                );
            }
        } else {
            current_solution = last_accepted_solution;
            candidate_absolute_score = last_accepted_absolute_score;
        }
    }

	// Greedy refinement -- necessarily kills threaded performance a bit due to shared pointers.  However,
	// this is just once per MC trajectory (at the end), and shared pointer stuff is just for setup and
	// teardown of threaded work.
	if( do_greedy && greedy_mode == MCOptimizerGreedyRefinementMode::REFINE_BEST_OF_EACH_TRAJECTORY ) {
		Size const nsol( local_solutions.size() );

		// Set up nsol solutions containers, each to hold one greedy solution.
		std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > greedy_solutions( nsol, nullptr );

		for( Size isol(0); isol<nsol; ++isol ) {
			do_one_greedy_refinement_in_threads( problem, greedy_solutions[isol],  std::get<0>( local_solutions[isol] ), std::get<2>( local_solutions[isol] ) );
		}

		{ // Mutex lock scope
			std::lock_guard< std::mutex > lock( solutions_mutex );
			for( Size isol(0); isol < nsol; ++isol ) {
				CHECK_OR_THROW_FOR_CLASS( greedy_solutions[isol]->n_solutions() == 1, "run_mc_trajectory",
					"Program error.  Expected 1 solution from greedy refinement for Monte Carlo solution "
					+ std::to_string(isol) + ", but got " + std::to_string( greedy_solutions[isol]->n_solutions() ) + "."
				);
				CostFunctionNetworkOptimizationSolution_APICSP curgreedysol(
					std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >(
						greedy_solutions[isol]->solution(0)
					)
				);
				CHECK_OR_THROW_FOR_CLASS( curgreedysol != nullptr, "carry_out_greedy_refinement", "Program error.  "
					"The solution from greedy refinement of Monte Carlo solution " + std::to_string(isol)
					+ " is not a cost function network optimization solution."
				);
				CostFunctionNetworkOptimizationProblem_APICSP curgreedysolprob(
					std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >(
						curgreedysol->problem()
					)
				);
				CHECK_OR_THROW_FOR_CLASS( curgreedysolprob != nullptr, "carry_out_greedy_refinement", "Program error.  "
					"Expected a CostFunctionNetworkOptimizationProblem class defining the greedy optimization problem, "
					"but got " + curgreedysol->problem()->inner_class_name() + "."
				);

				solutions.merge_in_lowest_scoring_solutions(
					{ std::make_tuple( curgreedysol->solution_at_variable_positions(), curgreedysol->solution_score(), curgreedysol->n_times_solution_was_produced() ) },
					n_solutions_to_store,
					curgreedysolprob
				);
			}
		} // Mutex lock scope.
	} else { // Mutex lock scope
        std::lock_guard< std::mutex > lock( solutions_mutex );
        solutions.merge_in_lowest_scoring_solutions( local_solutions, n_solutions_to_store, problem );
    } // End mutex lock scope.

    // Minimal output.
    write_to_tracer(
        "Completed replicate " + std::to_string( replicate_index ) +
        " of cost function network optimization problem " +
        std::to_string( problem_index ) + "."
    );
}

/// @brief Make a Monte Carlo move.
/// @param current_solution The current solution, as a vector of choice indices for all variable positions.  Changed by this operation.
/// @param n_choices_per_variable_node Number of choices per variable node, in the same order as current_solution.  The pairs are
/// (node index, number of choices).  The node index is ABSOLUTE.
/// @param randgen The handle of the Masala random generator.
/*static*/
void
MonteCarloCostFunctionNetworkOptimizer::make_mc_move(
    std::vector< masala::base::Size > & current_solution,
    std::vector< std::pair< masala::base::Size, masala::base::Size > > const & n_choices_per_variable_node,
    masala::base::managers::random::MasalaRandomNumberGeneratorHandle const randgen
) {
    using masala::base::Real;
    using masala::base::Size;
    if( n_choices_per_variable_node.size() == 0 ) return;

    Size const index_to_change( randgen->uniform_size_distribution( 0, current_solution.size() - 1 ) );
    Size new_choice( randgen->uniform_size_distribution( 0, n_choices_per_variable_node[index_to_change].second - 2 ) );
    if( new_choice >= current_solution[index_to_change] ) {
        ++new_choice;
    }
    current_solution[index_to_change] = new_choice;
}

/// @brief Make a Monte Carlo move that introduces many mutations, where the number of mutations is sampled from a
/// Poisson distribution.
/// @param current_solution The current solution, as a vector of choice indices for all variable positions.  Changed by this operation.
/// @param n_choices_per_variable_node Number of choices per variable node, in the same order as current_solution.  The pairs are
/// (node index, number of choices).  The node index is based on VARIABLE nodes.
/// @param poisson_lambda The parameter lambda in the Poisson distribution of the number of mutations to introduce.
/// @param randgen The handle of the Masala random generator.
/*static*/
void
MonteCarloCostFunctionNetworkOptimizer::make_mc_multimove(
    std::vector< masala::base::Size > & current_solution,
    std::vector< std::pair< masala::base::Size, masala::base::Size > > const & n_choices_per_variable_node,
    masala::base::Real const poisson_lambda,
    masala::base::managers::random::MasalaRandomNumberGeneratorHandle const randgen
) {
    using masala::base::Real;
    using masala::base::Size;
    Size const n_variable_nodes( n_choices_per_variable_node.size() );
    if( n_variable_nodes == 0 ) return;

    Size const n_mutations( std::min( n_variable_nodes, randgen->poisson_size_distribution( poisson_lambda ) + 1 ) );

    // TEMPORARY -- DELETE THE FOLLOWING:
    // std::stringstream ss;
    // ss << "Performing " << n_mutations << " mutations with lambda = " << poisson_lambda << ":\n";
    // for( Size i(0); i<current_solution.size(); ++i ) { ss << current_solution[i] << "\t"; }
    // ss << "\n";
    
    std::vector< std::pair< Size, Size > > variable_node_subset( randgen->random_sample( n_mutations, n_choices_per_variable_node ) );
    for( std::pair< Size, Size > const & entry : variable_node_subset ) {
        Size new_choice( randgen->uniform_size_distribution( 0, n_choices_per_variable_node[ entry.first ].second - 2 ) );
        if( new_choice >= current_solution[ entry.first ] ) {
            ++new_choice;
        }
        current_solution[ entry.first ] = new_choice;
    }

    // TEMPORARY -- DELETE THE FOLLOWING:
    // for( Size i(0); i<current_solution.size(); ++i ) { ss << current_solution[i] << "\t"; }
    // std::cout << ss.str() << std::endl;
}

/// @brief Determine whether to add the current solution to the set of solutions stored for this replicate attempt.
/// @details We maintain a list of N solutions for each replicate attempt.  If this solution has been encountered before
/// and is already in the list, we increment the count for the number of times it has been seen.  If not, and if fewer
/// than N solutions have been stored, we append the solution in a CostFunctionNetworkOptimizationSolution container.  If
/// the solution has not been seen, N solutions are stored, and this solution is lower-energy than the highest-energy
/// solution, we replace the highest-energy solution with this one.
/// @param[in] current_solution The solution that we are considering, represented as a vector of choice indices where each
/// entry in the vector corresponds to a variable node (in order).
/// @param[in] current_absolute_score The absolute score of this solution.
/// @param[inout] solutions The container of solutions.  This should be a thread-local copy.  This is a vector of tuples, where
/// each tuple is ( solution vector for variable nodes, solution score, number of times solution was seen ).
/// @param[in] n_solutions_to_store The number of solutions to store.
/// @param[in] force_store If true, we always store this solution.  If false, we use conditional logic.
/*static*/
void
MonteCarloCostFunctionNetworkOptimizer::determine_whether_to_store_solution(
    std::vector< masala::base::Size > const & current_solution,
    masala::base::Real const current_absolute_score,
    std::vector< std::tuple < std::vector< masala::base::Size >, masala::base::Real, masala::base::Size > > & solutions,
    masala::base::Size const n_solutions_to_store,
    bool const force_store
) {
    using masala::base::Size;
    using masala::base::Real;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

    // If the solution has already been seen, increment the number of times we have seen it.
    // Simultaneously, find the highest score solution that we have stored.
    bool first(true);
    Real highestE(0.0);
    Size highestE_index(0);
    for( Size i(0), imax(solutions.size()); i<imax; ++i ) {
        if( std::get<0>(solutions[i]) == current_solution ) {
            std::get<2>(solutions[i]) += 1;
            return;
        }
        if( first || std::get<1>(solutions[i]) > highestE ) {
            first = false;
            highestE = std::get<1>(solutions[i]);
            highestE_index = i;
        }
    }

    // If we reach here, we've not yet seen this solution.  If we're supposed to store more solutions
    // than we are currently storing, store this one.
    if( solutions.size() < n_solutions_to_store ) {
        solutions.push_back( std::make_tuple( current_solution, current_absolute_score, 1 ) );
        return;
    }

    // If we reach here, we have a full solution vector.  We only store this solution (and kick out the highest-energy solution)
    // if this solution is lower energy than the highest energy.
    if( force_store || current_absolute_score < highestE ) {
        std::get<0>(solutions[highestE_index]) = current_solution;
        std::get<1>(solutions[highestE_index]) = current_absolute_score;
        std::get<2>(solutions[highestE_index]) = 1;
    }
}


} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins
