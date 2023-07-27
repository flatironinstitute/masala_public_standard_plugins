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

/// @file src/optimizers_api/utility/cost_function_network/util.cc
/// @brief Implementations of utility functions for cost function network optimization problems.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <numeric_api/utility/optimization/cost_function_network/util.hh>

// Numeric API headers:
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/SquareOfChoicePenaltySumCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction_API.hh>
#include <optimizers_api/auto_generated_api/cost_function_network/cost_function/feature_based/SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/base_classes/optimization/cost_function_network/PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>

// Optimizers headers:
#include <optimizers/cost_function_network/cost_function/feature_based/SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/SquareOfChoicePenaltySumCostFunction.hh>
#include <optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction.hh>

// Base headers:
#include <base/types.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/managers/plugin_module/MasalaPluginAPI.hh>
#include <base/managers/plugin_module/MasalaPlugin.hh>

// STL headers:
#include <vector>

namespace standard_masala_plugins {
namespace optimizers_api {
namespace utility {
namespace cost_function_network {

/// @brief This is a utility function to construct a standard test problem for
/// testing out cost function network optimizers.  This problem has three nodes
/// with three choices per node, for a total of 27 possible solutions.
/// @param[in] name_of_problem_class The class name for the problem container.  Must be derived from
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
/// @param[in] gapped If true, we define the problem for nodes 0, 1, and 3, with only
/// one rotamer at node 2.  If false, we define the problem for nodes 0, 1, and 2.  False
/// by default.
/// @param[in] finalized If true (the default), we return a finalized problem setup.  If
/// false, we leave the problem unfinalized, permitting additional stuff to be added.
/// @details  The solutions and solutions scores are as follows:
/// 0 0 0 -> 71
/// 0 0 1 -> 54
/// 0 0 2 -> 58
/// 0 1 0 -> 96
/// 0 1 1 -> 83
/// 0 1 2 -> 88
/// 0 2 0 -> 55
/// 0 2 1 -> 42
/// 0 2 2 -> 46
/// 1 0 0 -> 76
/// 1 0 1 -> 58
/// 1 0 2 -> 57
/// 1 1 0 -> 100
/// 1 1 1 -> 86
/// 1 1 2 -> 86
/// 1 2 0 -> 54
/// 1 2 1 -> 40
/// 1 2 2 -> 399
/// 2 0 0 -> 38
/// 2 0 1 -> 22
/// 2 0 2 -> 23
/// 2 1 0 -> 67
/// 2 1 1 -> 55
/// 2 1 2 -> 57
/// 2 2 0 -> 18
/// 2 2 1 -> 6  <-- lowest
/// 2 2 2 -> 7
/// If gapped, all solutions shift up by 17.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
construct_test_problem(
    std::string const & name_of_problem_class,
    bool const gapped,
    bool const finalized
) {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

    masala::base::Size const last_node( gapped ? 3 : 2 );

    masala::base::managers::plugin_module::MasalaPluginAPISP my_object(
        masala::base::managers::plugin_module::MasalaPluginModuleManager::get_instance()->create_plugin_object_instance_by_short_name(
            std::vector< std::string >{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem" },
            name_of_problem_class,
            true
        )
    );

    CostFunctionNetworkOptimizationProblem_APISP my_object_cast(
        std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API >( my_object )
    );
    CHECK_OR_THROW( my_object_cast != nullptr, "masala::numeric_api::utility::optimization::cost_function_network", "construct_test_problem", "The returned " + name_of_problem_class + " object was not a CostFunctionNetworkOptimizationProblem." );

    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * problem(
        dynamic_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * >( my_object_cast->get_inner_object().get() )
    );
    CHECK_OR_THROW( problem != nullptr, "masala::numeric_api::utility::optimization::cost_function_network", "construct_test_problem", "The returned " + name_of_problem_class + " object was not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem." );

    // Configure one-node penalties:
    problem->set_onebody_penalty( 0, 0, 25 );
    problem->set_onebody_penalty( 0, 1, 32 );
    problem->set_onebody_penalty( 0, 2, 0 );
    problem->set_onebody_penalty( 1, 0, 15 );
    problem->set_onebody_penalty( 1, 1, 43 );
    problem->set_onebody_penalty( 1, 2, 0 );
    problem->set_onebody_penalty( last_node, 0, 14 );
    problem->set_onebody_penalty( last_node, 1, 5 );
    problem->set_onebody_penalty( last_node, 2, 0 );

    // Configure pairwise two-node penalties:
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 0 ), 5 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 1 ), 3 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 0, 2 ), 9 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 0 ), 4 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 1 ), 1 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 1, 2 ), 2 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 0 ), 1 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 1 ), 3 );
    problem->set_twobody_penalty( std::make_pair( 0, 1 ), std::make_pair( 2, 2 ), 1 );

    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 0, 0 ), 5 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 0, 1 ), 3 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 0, 2 ), 9 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 1, 0 ), 4 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 1, 1 ), 1 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 1, 2 ), 2 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 2, 0 ), 1 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 2, 1 ), 0 );
    problem->set_twobody_penalty( std::make_pair( 0, last_node ), std::make_pair( 2, 2 ), 3 );

    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 0, 0 ), 7 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 0, 1 ), 1 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 0, 2 ), 4 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 1, 0 ), 6 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 1, 1 ), 4 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 1, 2 ), 8 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 2, 0 ), 2 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 2, 1 ), 0 );
    problem->set_twobody_penalty( std::make_pair( 1, last_node ), std::make_pair( 2, 2 ), 3 );

    if( gapped ) {
        // We set some penalties here that become fixed background.  The fixed background should be 17.0.
        problem->set_onebody_penalty( 2, 0, 12.0 );

        // The pairwise penalties for choices at node 0 with node 2 are effectively onebody penalties, so they have to be the same:
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 0, 0 ), 3.0 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 1, 0 ), 3.0 );
        problem->set_twobody_penalty( std::make_pair( 0, 2 ), std::make_pair( 2, 0 ), 3.0 );

        // The pairwise penalties for choices at node 1 with node 2 are also effectively onebody penalties, so they have to be the same:
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 0, 0 ), 2.0 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 1, 0 ), 2.0 );
        problem->set_twobody_penalty( std::make_pair( 1, 2 ), std::make_pair( 2, 0 ), 2.0 );
    }

    if( finalized ) {
        // Finalize the problem.
        problem->finalize();
    }
    
    // Return the problem.
    return my_object_cast;
}

/// @brief Construct a variant of the problem above, satisfiable features on some of the choices.
/// @param[in] name_of_problem_class The class name for the problem container.  Must be derived from
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
/// @param[in] gapped If true, we define the problem for nodes 0, 1, and 3, with only
/// one rotamer at node 2.  If false, we define the problem for nodes 0, 1, and 2.  False
/// by default.
/// @param[in] finalized If true (the default), we return a finalized problem setup.  If
/// false, we leave the problem unfinalized, permitting additional stuff to be added.
/// @details  The solutions and solutions scores are as follows if ungapped:
/// 0 0 0 -> 86
/// 0 0 1 -> 114
/// 0 0 2 -> 118
/// 0 1 0 -> 156
/// 0 1 1 -> 98
/// 0 1 2 -> 223
/// 0 2 0 -> 55
/// 0 2 1 -> 57
/// 0 2 2 -> 61
/// 1 0 0 -> 76
/// 1 0 1 -> 73
/// 1 0 2 -> 72
/// 1 1 0 -> 115
/// 1 1 1 -> 146
/// 1 1 2 -> 146
/// 1 2 0 -> 69
/// 1 2 1 -> 40
/// 1 2 2 -> 99
/// 2 0 0 -> 53
/// 2 0 1 -> 82
/// 2 0 2 -> 83
/// 2 1 0 -> 127
/// 2 1 1 -> 70
/// 2 1 2 -> 192
/// 2 2 0 -> 18
/// 2 2 1 -> 21
/// 2 2 2 -> 22
///
/// The solutions and solutions scores are as follows if gapped:
/// 0 0 0 -> 103
/// 0 0 1 -> 131
/// 0 0 2 -> 135
/// 0 1 0 -> 173
/// 0 1 1 -> 115
/// 0 1 2 -> 240
/// 0 2 0 -> 72
/// 0 2 1 -> 74
/// 0 2 2 -> 78
/// 1 0 0 -> 93
/// 1 0 1 -> 90
/// 1 0 2 -> 89
/// 1 1 0 -> 132
/// 1 1 1 -> 163
/// 1 1 2 -> 163
/// 1 2 0 -> 86
/// 1 2 1 -> 57
/// 1 2 2 -> 116
/// 2 0 0 -> 70
/// 2 0 1 -> 99
/// 2 0 2 -> 100
/// 2 1 0 -> 144
/// 2 1 1 -> 87
/// 2 1 2 -> 209
/// 2 2 0 -> 35
/// 2 2 1 -> 38
/// 2 2 2 -> 39
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
construct_test_problem_with_squared_unsatisfied_feature_penalties(
    std::string const & name_of_problem_class,
    bool const gapped /*=false*/,
    bool const finalized /*=true*/
) {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::cost_function;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::cost_function::feature_based;
    using std::unordered_map;
    using std::vector;
    using std::pair;
    using masala::base::Size;
    using masala::base::size_pair_hash;

    masala::base::Size const last_node( gapped ? 3 : 2 );

    CostFunctionNetworkOptimizationProblem_APISP problem_cast( construct_test_problem( name_of_problem_class, gapped, false ) );
    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * problem(
        dynamic_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * >( problem_cast->get_inner_object().get() )
    );
    CHECK_OR_THROW( problem != nullptr, "masala::numeric_api::utility::optimization::cost_function_network", "construct_test_problem_with_squared_unsatisfied_feature_penalties", "The returned " + name_of_problem_class + " object was not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem." );

    SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_APISP cost_function( masala::make_shared< SquareOfSumOfUnsatisfiedChoiceFeaturesCostFunction_API >() );

    cost_function->add_choice_feature_by_absolute_node_index( 0, 1, 1, 1, 0 );
    cost_function->add_choice_feature_by_absolute_node_index( 1, 0, 1, 1, 0 );
    cost_function->add_choice_feature_by_absolute_node_index( 1, 1, 1, 1, 0 );
    cost_function->add_choice_feature_by_absolute_node_index( 1, 1, 1, 1, 0 ); // Two features for node 1, choice 1.
    cost_function->add_choice_feature_by_absolute_node_index( last_node, 1, 1, 1, 0 );
    cost_function->add_choice_feature_by_absolute_node_index( last_node, 2, 1, 1, 0 );

    unordered_map<Size, vector<vector<unordered_map<pair<Size, Size>, Size, size_pair_hash>>>> connection_data;
    // Node 0:
    connection_data[0] = vector< vector< unordered_map< pair< Size, Size >, Size, size_pair_hash > > >{
        // Choice 0 -- no features:
        {},
        // Choice 1:
        {
            // Feature 1:
            {
                {pair< Size, Size >( 1, 0 ), 1},
                {pair< Size, Size >( 1, 1 ), 1},
                {pair< Size, Size >( last_node, 1 ), 1}
            }
        },
        // Choice 2 -- no features:
        {}
    };
    // Node 1:
    connection_data[1] = vector< vector< unordered_map< pair< Size, Size >, Size, size_pair_hash > > >{
        // Choice 0:
        {
            // Feature 1:
            {
                {pair< Size, Size >( 0, 1 ), 1}
            }
        },
        // Choice 1:
        {
            // Feature 1:
            {
                {pair< Size, Size >( 0, 1 ), 1}
            },
            // Feature 2:
            {
                {pair< Size, Size >( last_node, 1 ), 1}
            }
        },
        // Choice 2 -- no features:
        {}
    };
    // Node 2 (or 3 if gapped):
    connection_data[last_node] = vector< vector< unordered_map< pair< Size, Size >, Size, size_pair_hash > > >{
        // Choice 0 -- no features:
        {},
        // Choice 1:
        {
            // Feature 1:
            {
                {pair<Size, Size >( 0, 1 ), 1 },
                {pair<Size, Size >( 1, 1 ), 1 }
            }
        },
        // Choice 2:
        {
            // Feature 1:
            {
                // Not satisfied by anything.
            }
        }
    };

    cost_function->add_connecting_node_choices_for_features_of_nodes_choices(
        connection_data
    );
    cost_function->set_weight( 15 );

    problem->add_cost_function( cost_function->get_inner_object() );

    if( finalized ) {
        problem->finalize();
    }

    return problem_cast;
}

/// @brief Construct a variant of the problem above, with penalties on each of the choices and a desired
/// penalty count that makes what was previously the third-lowest energy solution the new lowest-energy
/// solution.  This emulates what is done in Rosetta with the voids_penalty scoreterm.
/// @param[in] name_of_problem_class The class name for the problem container.  Must be derived from
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
/// @param[in] gapped If true, we define the problem for nodes 0, 1, and 3, with only
/// one rotamer at node 2.  If false, we define the problem for nodes 0, 1, and 2.  False
/// by default.
/// @param[in] finalized If true (the default), we return a finalized problem setup.  If
/// false, we leave the problem unfinalized, permitting additional stuff to be added.
/// @details  The solutions and solutions scores are as follows if ungapped:
/// 0 0 0 -> 80
/// 0 0 1 -> 103
/// 0 0 2 -> 107
/// 0 1 0 -> 132
/// 0 1 1 -> 183
/// 0 1 2 -> 188
/// 0 2 0 -> 59
/// 0 2 1 -> 78
/// 0 2 2 -> 82
/// 1 0 0 -> 80
/// 1 0 1 -> 94
/// 1 0 2 -> 93
/// 1 1 0 -> 125
/// 1 1 1 -> 167
/// 1 1 2 -> 167
/// 1 2 0 -> 55
/// 1 2 1 -> 65
/// 1 2 2 -> 64
/// 2 0 0 -> 39
/// 2 0 1 -> 47
/// 2 0 2 -> 48
/// 2 1 0 -> 83
/// 2 1 1 -> 119
/// 2 1 2 -> 121
/// 2 2 0 -> 18  <-- lowest
/// 2 2 1 -> 22
/// 2 2 2 -> 23
///
/// The solutions and solutions scores are as follows if gapped:
/// 0 0 0 -> 113
/// 0 0 1 -> 152
/// 0 0 2 -> 156
/// 0 1 0 -> 177
/// 0 1 1 -> 244
/// 0 1 2 -> 249
/// 0 2 0 -> 88
/// 0 2 1 -> 123
/// 0 2 2 -> 127
/// 1 0 0 -> 109
/// 1 0 1 -> 139
/// 1 0 2 -> 138
/// 1 1 0 -> 166
/// 1 1 1 -> 224
/// 1 1 2 -> 224
/// 1 2 0 -> 80
/// 1 2 1 -> 106
/// 1 2 2 -> 105
/// 2 0 0 -> 64
/// 2 0 1 -> 88
/// 2 0 2 -> 89
/// 2 1 0 -> 120
/// 2 1 1 -> 172
/// 2 1 2 -> 174
/// 2 2 0 -> 39  <-- lowest
/// 2 2 1 -> 59
/// 2 2 2 -> 60
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
construct_test_problem_with_squared_choice_count_penalties(
    std::string const & name_of_problem_class,
    bool const gapped,
    bool const finalized
) {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::cost_function;

    masala::base::Size const last_node( gapped ? 3 : 2 );

    CostFunctionNetworkOptimizationProblem_APISP problem_cast( construct_test_problem( name_of_problem_class, gapped, false) );
    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * problem(
        dynamic_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * >( problem_cast->get_inner_object().get() )
    );
    CHECK_OR_THROW( problem != nullptr, "masala::numeric_api::utility::optimization::cost_function_network", "construct_test_problem_with_squared_choice_count_penalties", "The returned " + name_of_problem_class + " object was not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem." );

    SquareOfChoicePenaltySumCostFunction_APISP cost_func( masala::make_shared< SquareOfChoicePenaltySumCostFunction_API >() );

    cost_func->set_constant_offset( -3.0 );
    cost_func->set_penalties_for_all_choices_at_node( 0, std::vector< masala::base::Real >{ 3.0, 2.0, 1.0 } );
    cost_func->set_penalties_for_all_choices_at_node( 1, std::vector< masala::base::Real >{ 2.0, 5.0, 1.0 } );
    if( gapped ) {
        cost_func->set_penalties_for_all_choices_at_node( 2, std::vector< masala::base::Real >{ 2.0 } );
    }
    cost_func->set_penalties_for_all_choices_at_node( last_node, std::vector< masala::base::Real >{ 1.0, 5.0, 5.0 } );

    problem->add_cost_function( cost_func->get_inner_object() );

    if( finalized ) {
        // Finalize the problem.
        problem->finalize();
    }
    
    // Return the problem.
    return problem_cast;
}

/// @brief Construct a variant of the problem above, with some of the choices in a "countable"
/// category and a desired count of those choices.  That makes what was previously the second-lowest
/// energy solution the new lowest-energy solution.  This emulates what is done in Rosetta with the
/// aa_composition scoreterm.
/// @param[in] name_of_problem_class The class name for the problem container.  Must be derived from
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
/// @param[in] gapped If true, we define the problem for nodes 0, 1, and 3, with only
/// one rotamer at node 2.  If false, we define the problem for nodes 0, 1, and 2.  False
/// by default.
/// @param[in] finalized If true (the default), we return a finalized problem setup.  If
/// false, we leave the problem unfinalized, permitting additional stuff to be added.
/// @details  The solutions and solutions scores are as follows if ungapped:
/// 0 0 0 -> 96
/// 0 0 1 -> 79
/// 0 0 2 -> 73
/// 0 1 0 -> 111
/// 0 1 1 -> 98
/// 0 1 2 -> 88
/// 0 2 0 -> 70
/// 0 2 1 -> 57
/// 0 2 2 -> 46
/// 1 0 0 -> 91
/// 1 0 1 -> 73
/// 1 0 2 -> 57
/// 1 1 0 -> 100
/// 1 1 1 -> 86
/// 1 1 2 -> 108
/// 1 2 0 -> 54
/// 1 2 1 -> 40
/// 1 2 2 -> 61
/// 2 0 0 -> 63
/// 2 0 1 -> 47
/// 2 0 2 -> 38
/// 2 1 0 -> 82
/// 2 1 1 -> 70
/// 2 1 2 -> 57
/// 2 2 0 -> 33
/// 2 2 1 -> 21
/// 2 2 2 -> 7  <-- lowest
///
/// The solutions and solutions scores are as follows if gapped:
/// 0 0 0 -> 88
/// 0 0 1 -> 71
/// 0 0 2 -> 97
/// 0 1 0 -> 135
/// 0 1 1 -> 122
/// 0 1 2 -> 193
/// 0 2 0 -> 94
/// 0 2 1 -> 81
/// 0 2 2 -> 151
/// 1 0 0 -> 115
/// 1 0 1 -> 97
/// 1 0 2 -> 162
/// 1 1 0 -> 205
/// 1 1 1 -> 191
/// 1 1 2 -> 301
/// 1 2 0 -> 159
/// 1 2 1 -> 145
/// 1 2 2 -> 254
/// 2 0 0 -> 55
/// 2 0 1 -> 39  <-- lowest
/// 2 0 2 -> 62
/// 2 1 0 -> 106
/// 2 1 1 -> 94
/// 2 1 2 -> 162
/// 2 2 0 -> 57
/// 2 2 1 -> 45
/// 2 2 2 -> 112
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
construct_test_problem_with_function_of_integer_penalty_sum_penalties(
    std::string const & name_of_problem_class,
    bool const gapped /*=false*/,
    bool const finalized /*=true*/
) {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
    using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
    using namespace standard_masala_plugins::optimizers_api::auto_generated_api::cost_function_network::cost_function;

    masala::base::Size const last_node( gapped ? 3 : 2 );

    CostFunctionNetworkOptimizationProblem_APISP problem_cast( construct_test_problem(name_of_problem_class, gapped, false) );
    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * problem(
        dynamic_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem * >( problem_cast->get_inner_object().get() )
    );
    CHECK_OR_THROW( problem != nullptr, "masala::numeric_api::utility::optimization::cost_function_network", "construct_test_problem_with_squared_choice_count_penalties", "The returned " + name_of_problem_class + " object was not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem." );

    FunctionOfIntegerPenaltySumCostFunction_APISP cost_func( masala::make_shared< FunctionOfIntegerPenaltySumCostFunction_API >() );

    cost_func->set_penalty_function( std::vector< masala::base::Real >{ 25.0, 15.0, 0.0, 22.0 } );

    cost_func->set_penalties_for_all_choices_at_node( 0, std::vector< signed long int >{ 0, 1, 0 } );
    cost_func->set_penalties_for_all_choices_at_node( 1, std::vector< signed long int >{ 0, 1, 1 } );
    if( gapped ) {
        cost_func->set_penalties_for_all_choices_at_node( 2, std::vector< signed long int >{ 2 } );
    }
    cost_func->set_penalties_for_all_choices_at_node( last_node, std::vector< signed long int >{ 0, 0, 1 } );
    cost_func->set_penalty_function_behaviour_high_by_string("quadratic");
    cost_func->set_penalty_function_behaviour_low_by_string("quadratic");

    problem->add_cost_function( cost_func->get_inner_object() );

    if( finalized ) {
        // Finalize the problem.
        problem->finalize();
    }
    
    // Return the problem.
    return problem_cast;
}

} // namespace cost_function_network
} // namespace utility
} // namespace optimizers_api
} // namespace standard_masala_plugins
