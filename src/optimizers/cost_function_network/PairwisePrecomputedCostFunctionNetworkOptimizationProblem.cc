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

/// @file src/optimizers/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem.cc
/// @brief Implementation for a pure virtual base class for PairwisePrecomputedCostFunctionNetworkOptimizationProblems.
/// @details PairwisePrecomputedCostFunctionNetworkOptimizationProblems define a numerical cost function network optimization problem to be solved
/// by a suitable Optimizer.  They do not contain any chemistry-specific concepts.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>

// STL headers:
#include <vector>
#include <string>
#include <set>
#include <numeric>

// Base headers:
#include <base/utility/execution_policy/util.hh>
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_TwoInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_ThreeInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_TwoInput.tmpl.hh>
#include <base/utility/container/container_util.tmpl.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Default constructor.
/// @details Needs to be explicit due to use of std::atomic.
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::PairwisePrecomputedCostFunctionNetworkOptimizationProblem() :
    masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem(),
    background_constant_offset_(0.0),
    one_choice_node_constant_offset_(0.0)
{}

/// @brief Copy constructor.
/// @details Needs to be explicit due to use of std::atomic.
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::PairwisePrecomputedCostFunctionNetworkOptimizationProblem(
    PairwisePrecomputedCostFunctionNetworkOptimizationProblem const & src
) :
    masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem( src )
{
    operator=(src);
}

// @brief Assignment operator.
/// @details Needs to be explicit due to use of std::atomic.
PairwisePrecomputedCostFunctionNetworkOptimizationProblem &
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::operator=(
    PairwisePrecomputedCostFunctionNetworkOptimizationProblem const & src
) {
    masala::numeric_api::base_classes::optimization::cost_function_network::PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem::operator=(src);
    std::lock( problem_mutex(), src.problem_mutex() );
    std::lock_guard< std::mutex > lock_this( problem_mutex(), std::adopt_lock );
    std::lock_guard< std::mutex > lock_that( src.problem_mutex(), std::adopt_lock );
    single_node_penalties_ = src.single_node_penalties_;
    pairwise_node_penalties_ = src.pairwise_node_penalties_;
    background_constant_offset_ = src.background_constant_offset_.load();
    one_choice_node_constant_offset_ = src.one_choice_node_constant_offset_.load();
    return *this;
}

/// @brief Make a fully independent copy of this object.
PairwisePrecomputedCostFunctionNetworkOptimizationProblemSP
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::deep_clone() const {
    PairwisePrecomputedCostFunctionNetworkOptimizationProblemSP new_problem( masala::make_shared< PairwisePrecomputedCostFunctionNetworkOptimizationProblem >( *this ) );
    new_problem->make_independent();
    return new_problem;
}

/// @brief Ensure that all data are unique and not shared (i.e. everytihng is deep-cloned.)
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::make_independent() {
    masala::numeric::optimization::OptimizationProblem::make_independent();
    //GNDN
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "OptimizationProblem", "CostFunctionNetworkOptimizationProblem", "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_categories() const {
	return std::vector< std::vector< std::string > > {
		{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem", "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" }
	};
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function_network_optimization_problem", "numeric" }
std::vector< std::string >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_keywords() const {
	return std::vector< std::string > {
		"optimization_problem",
		"cost_function_network_optimization_problem",
		"numeric"
	};
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "OptimizationProblem", "CostFunctionNetworkOptimizationProblem", "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" } }.
std::vector< std::vector< std::string > >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_data_representation_categories() const {
    using namespace std;
    return vector< vector< string > >{ { "OptimizationProblem", "CostFunctionNetworkOptimizationProblem", "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" } };
}

/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
/// is compatible.
/// @returns { "standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer" }
std::vector< std::string >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_compatible_masala_engines() const {
    return std::vector< std::string >{ "standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer" };
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function_network_optimization_problem" }.
std::vector< std::string >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_present_data_representation_properties() const {
    return std::vector< std::string >{ "optimization_problem", "cost_function_network_optimization_problem" };
}

/// @brief Get the properties of this MasalaDataRepresentation that might possibly be present.
/// @details Obviously, this is a non-exhuastive list.
/// @returns { "precomputed", "pairwise_decomposible", "partially_precomputed", "partially_pairwise_decomposible" }.
std::vector< std::string >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_possibly_present_data_representation_properties() const {
    return std::vector< std::string >{ "precomputed", "pairwise_decomposible", "partially_precomputed", "partially_pairwise_decomposible" };
}

/// @brief Get the name of this class.
/// @returns "PairwisePrecomputedCostFunctionNetworkOptimizationProblem".
std::string
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::class_name() const {
    return "PairwisePrecomputedCostFunctionNetworkOptimizationProblem";
}

/// @brief Get the namespace for this class.
/// @returns "masala::numeric::optimization::cost_function_network".
std::string
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::class_namespace() const {
    return "standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the fixed background constant offset.
masala::base::Real
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::background_constant_offset() const {
    return background_constant_offset_;
}

/// @brief Get the constant offset for nodes.
/// @details This is the sum of onebody energies for nodes that have exactly
/// one choice, plus the twobdy energies between those nodes.
masala::base::Real
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::one_choice_node_constant_offset() const {;
    CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "one_choice_node_constant_offset", "The problem setup must be finalized with a call "
        "to finalize() before this function can be called."
    );
    return one_choice_node_constant_offset_;
}

/// @brief Get the total constant offset.
/// @details This is the sum of background_constant_offset() and one_choice_node_constant_offset().
masala::base::Real
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::total_constant_offset() const {
    CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "total_constant_offset", "The problem setup must be finalized with a call "
        "to finalize() before this function can be called."
    );
    return one_choice_node_constant_offset_ + background_constant_offset_;
}

/// @brief Does this pariwise precomputed cost function network optimization problem have any non-pairwise
/// components that must be computed on the fly?
/// @details For now, returns false.  This will be implemented in the future.
bool
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::has_non_pairwise_scores() const {
    // CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "has_non_pairwise_scores", "The problem setup must be finalized with a call "
    //     "to finalize() before this function can be called."
    // );
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Add onebody penalty for a choice at a node.
/// @details If the node has not yet been listed, it's added to the n_choices_by_node_index_ map.
/// If the number of choices at the node is currently less than the node index, the number of
/// choices is increased.
/*virtual*/
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_onebody_penalty(
    masala::base::Size const node_index,
    masala::base::Size const choice_index,
    masala::base::Real const penalty
) {
    std::lock_guard< std::mutex > lock( problem_mutex() );
    std::map< masala::base::Size, masala::base::Size >::iterator it( n_choices_by_node_index().find(node_index) );
    if( it == n_choices_by_node_index().end() ) {
        // Update the number of choices per node:
        n_choices_by_node_index()[node_index] = choice_index + 1;
        // Set the one-body penalty:
        single_node_penalties_[node_index] = create_choice_vector( choice_index, penalty );
    } else {
        // Update the number of choices per node:
        if( it->second <= choice_index ) {
            it->second = choice_index + 1;
        }
        // Set the one-body penalty:
        set_entry_in_vector( single_node_penalties_[node_index], choice_index, penalty );
    }
}

/// @brief Set the two-node penalty for a particular pair of choice indices corresponding to a particular
/// pair of node indices.
/// @param[in] node_indices A pair of node indices.  The lower index should be first.  (This function will
/// throw if it is not, since it makes the choice indices ambiguous).
/// @param[in] choice_indices The corresponding pair of choice indices.  The first entry should be the choice
/// index for the lower-numbered node, and the second should be the choice index for the higher-numbered node.
/// @param[in] penalty The value of the two-node penalty (or, if negative, bonus).
/// @details If a node has not yet been listed, it's added to the n_choices_by_node_index_ map.
/// If the number of choices at the node is currently less than the node index, the number of
/// choices is increased.
/*virtual*/
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_twobody_penalty(
    std::pair< masala::base::Size, masala::base::Size > const & node_indices,
    std::pair< masala::base::Size, masala::base::Size > const & choice_indices,
    masala::base::Real penalty
) {
    using masala::base::Size;
    using masala::base::Real;

    std::lock_guard< std::mutex > lock( problem_mutex() );

    // Sanity check:
    CHECK_OR_THROW_FOR_CLASS(
        node_indices.second > node_indices.first,
        "set_twobody_penalty",
        "This function requires that the second node index be higher than the first.  Got node_index1="
        + std::to_string( node_indices.first ) + ", node_index2=" + std::to_string( node_indices.second ) + "."
    );

    // Update the number of choices per node:
    set_minimum_number_of_choices_at_node_mutex_locked( node_indices.first, choice_indices.first + 1 );
    set_minimum_number_of_choices_at_node_mutex_locked( node_indices.second, choice_indices.second + 1 );

    // Update the penalties:
    std::unordered_map< std::pair< Size, Size >, Eigen::Matrix< Real, Eigen::Dynamic, Eigen::Dynamic >, masala::base::size_pair_hash >::iterator it(
        pairwise_node_penalties_.find( node_indices )
    );
    if( it == pairwise_node_penalties_.end() ) {
        pairwise_node_penalties_[node_indices] = create_choicepair_matrix( choice_indices, penalty );
    } else {
        set_entry_in_matrix( pairwise_node_penalties_[node_indices], choice_indices, penalty );
    }
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given a candidate solution, compute the score.
/// @details The candidate solution is expressed as a vector of choice indices, with
/// one entry per variable position, in order of position indices.  (There may not be
/// entries for every position, though, since not all positions have at least two choices.)
/// @note This uses the one- and two-node penalties cached in this object to make this
/// calculation efficient.  This function does NOT lock the problem mutex.  This is only
/// threadsafe from a read-only context.
masala::base::Real
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::compute_absolute_score(
    std::vector< masala::base::Size > const & candidate_solution
) const {
    using masala::base::Real;
    using masala::base::Size;
    CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "compute_absolute_score", "The problem setup must be finalized before compute_absolute_score() can be called." );

    Real accumulator(
        total_constant_offset() +
        masala::numeric::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem::compute_absolute_score( candidate_solution ) // Handles anything non-pairwise.
    );

    Size const n_pos( candidate_solution.size() );
    std::vector< std::pair< Size, Size > > const variable_positions( n_choices_at_variable_nodes() );
    CHECK_OR_THROW_FOR_CLASS( candidate_solution.size() == variable_positions.size(), "compute_absolute_score",
        "The number of entries in the candidate solution vector (" + std::to_string( candidate_solution.size() ) +
        ") does not match the number of variable nodes with two or more choices (" + std::to_string( variable_positions.size() ) + ")." );
    for( Size i(0); i<n_pos; ++i ) {
        Size const node_i_index( variable_positions[i].first );
        Size const choice_i_index( candidate_solution[i] );
        {
            //Retrieve onebody energy
            std::unordered_map< Size, std::vector< Real > >::const_iterator it( single_node_penalties_.find(node_i_index) );
            if( it != single_node_penalties_.end() && choice_i_index < it->second.size() ) {
                accumulator += it->second[choice_i_index];
            }
        }

        for( Size j(0); j<i; ++j ) {
            Size const node_j_index( variable_positions[j].first );
            Size const choice_j_index( candidate_solution[j] );
            // Retrieve twobody energy:
            std::unordered_map< std::pair< Size, Size >, Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic >, masala::base::size_pair_hash >::const_iterator it( pairwise_node_penalties_.find( std::make_pair( node_j_index, node_i_index ) ) );
            if( it != pairwise_node_penalties_.end() ) {
                Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > const & choicepairs( it->second );
                if( static_cast< Size >( choicepairs.rows() ) > choice_j_index && static_cast< Size >( choicepairs.cols() ) > choice_i_index ) {
                    accumulator += choicepairs( choice_j_index, choice_i_index );
                }
            }
        }
    }

    return accumulator;
}

/// @brief Given a pair of candidate solutions, compute the difference in their scores.
/// @details The candidate solution is expressed as a vector of choice indices, with
/// one entry per variable position, in order of position indices.  (There may not be
/// entries for every position, though, since not all positions have at least two choices.)
/// @note This uses the one- and two-node penalties cached in this object to make this
/// calculation very efficient.  This function does NOT lock the problem mutex.  This is only
/// threadsafe from a read-only context.
masala::base::Real
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::compute_score_change(
    std::vector< masala::base::Size > const & old_solution,
    std::vector< masala::base::Size > const & new_solution
) const {
    using masala::base::Real;
    using masala::base::Size;
    CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "compute_score_change", "The problem setup must be finalized "
        "before compute_score_change() can be called."
    );

    Size const npos( protected_total_variable_nodes() ); //Only safe to call if finalized.
    CHECK_OR_THROW_FOR_CLASS( old_solution.size() == npos, "compute_score_change",
        "The size of the old candidate solution vector was " + std::to_string( old_solution.size() ) + ", but "
        "there are " + std::to_string( npos ) + " variable positions."
    );
    CHECK_OR_THROW_FOR_CLASS( new_solution.size() == npos, "compute_score_change",
        "The size of the new candidate solution vector was " + std::to_string( new_solution.size() ) + ", but "
        "there are " + std::to_string( npos ) + " variable positions."
    );

    std::vector< Size > ivals( npos );
    for( Size i(0); i < npos; ++i ) {
        ivals[i] = i;
    }

    return CostFunctionNetworkOptimizationProblem::compute_score_change( old_solution, new_solution ) + std::transform_reduce(
        MASALA_UNSEQ_EXECUTION_POLICY
        ivals.cbegin(), ivals.cend(), 0.0, std::plus{},
        [this, &old_solution, &new_solution]( Size const i ) {
            if( old_solution[i] != new_solution[i] ) {
                Real accumulator(0.0);
                // Sum onebody energy change:
                std::vector< Real > const * onebody_vec( single_node_penalties_for_variable_nodes_[i] );
                if( onebody_vec != nullptr ) {
                    Real const old_onebody_energy( old_solution[i] < onebody_vec->size() ? (*onebody_vec)[old_solution[i]] : 0.0 );
                    Real const new_onebody_energy( new_solution[i] < onebody_vec->size() ? (*onebody_vec)[new_solution[i]] : 0.0 );
                    accumulator += new_onebody_energy - old_onebody_energy;
                }

                // Sum twobody energy change:
                accumulator += std::transform_reduce(
                    MASALA_UNSEQ_EXECUTION_POLICY
                    interacting_variable_nodes_[i].cbegin(), interacting_variable_nodes_[i].cend(), 0.0, std::plus{},
                    // Note: In the following lines, I need the lambda to capture this if and only if
                    // DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS does a check (i.e. only in debug mode).
#ifdef NDEBUG
                    [i, &old_solution, &new_solution ]( auto const & entry ) {
#else
                    [this, i, &old_solution, &new_solution ]( auto const & entry ) {
#endif
                        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( entry.first != i, "compute_score_change", "Program error, since an interacting residue appeared in its own interacting residue list.  This should not happen." );
                        if( ( old_solution[entry.first] == new_solution[entry.first] ) || ( entry.first < i ) ) {
                            Size const lowernode( std::min( i, entry.first ) ), uppernode( std::max( i, entry.first ) );
                            auto const & mat( *entry.second );
                            Real const old_twobody_energy( ( old_solution[lowernode] < static_cast< Size >(mat.rows()) && old_solution[uppernode] < static_cast< Size >(mat.cols()) ) ? mat( old_solution[lowernode], old_solution[uppernode] ) : 0.0 );
                            Real const new_twobody_energy( ( new_solution[lowernode] < static_cast< Size >(mat.rows()) && new_solution[uppernode] < static_cast< Size >(mat.cols()) ) ? mat( new_solution[lowernode], new_solution[uppernode] ) : 0.0 );
                            return new_twobody_energy - old_twobody_energy;
                        }
                        return 0.0;
                    }
                );

                return accumulator;
            }
            return 0.0;
        }
    );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get a description of the API for the PairwisePrecomputedCostFunctionNetworkOptimizationProblem class.
masala::base::api::MasalaObjectAPIDefinitionCWP
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::get_api_definition() {
    using namespace masala::base::api;
    using masala::base::Size;
    using masala::base::Real;
    
    std::lock_guard< std::mutex > lock( problem_mutex() );

    if( api_definition() == nullptr ) {

        MasalaObjectAPIDefinitionSP api_def(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this,
                "The PairwisePrecomputedCostFunctionNetworkOptimizationProblem class defines a numerical cost function "
                "optimization problem.  This is the problem reduced to numbers, with no chemical classes "
                "or concepts included.",
                false, false
            )
        );

        // Constructors:
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( PairwisePrecomputedCostFunctionNetworkOptimizationProblem, api_def );

        // Getters:
        api_def->add_getter(
            masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
                "background_constant_offset", "Get the fixed background constant offset.",
                "background_contant_offset", "A fixed, constant value added to all energies for all solutions.  Useful for parts "
                "of the problem that are not variable.", false, false,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::background_constant_offset, this )
            )
        );
        api_def->add_getter(
            masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
                "one_choice_node_constant_offset", "Get the constant offset for nodes.  This is the sum of onebody energies "
                "for nodes that have exactly one choice, plus the twobdy energies between those nodes.  Note that this could "
                "be rather slow.",
                "one_choice_node_constant_offset", "A fixed, constant value from all nodes with exactly one choice.", false, false,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::one_choice_node_constant_offset, this )
            )
        );
        api_def->add_getter(
            masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< Real > >(
                "total_constant_offset", "Get the total (background + node) constant offset.",
                "total_constant_offset", "This is the sum of background_constant_offset() and one_choice_node_constant_offset().",
                false, false,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::total_constant_offset, this )
            )
        );
        api_def->add_getter(
            masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
                "has_non_pairwise_scores", "Get whether this problem has any components to its score that are not decomposable as one- "
                "or two-node additive.",
                "has_non_pairwise_scores", "Returns true if there are non-pairwise components that must be computed on the fly, false "
                "otherwise.  (For now, always false.  Non-pairwise functionality will be added in the future.)",
                false, false,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::has_non_pairwise_scores, this )
            )
        );
        api_def->add_getter(
            masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
                "finalized", "Has this problem description been finalized?  That is, is the problem setup "
                "complete and the object locked to now be read-only?",
                "finalized", "True if the object has been finalized, false otherwise.",
                false, false,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::finalized, this )
            )
        );
		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"has_candidate_starting_solutions", "Does this cost function network optimization problem have "
				"one or more candidate solutions defined?  These can be used as starting points for some optimizers, "
				"or can be ignored.",
				"has_candidate_starting_solutions", "True if there is at least one candidate starting solution, false otherwise.",
				false, false, std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::has_candidate_starting_solutions, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< std::vector< std::vector< masala::base::Size > > const & > >(
				"candidate_starting_solutions", "Returns candidate starting solutions for this cost functoin network optimization problem.  "
				"These can be used as starting points for some optimizers, or can be ignored.",
				"candidate_starting_solutions", "A vector of vectors of candidate starting solutions for this problem, where the length of "
				"each vector matches the number of variable positions and the entries are choice indices.",
				false, false, std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::candidate_starting_solutions, this )
			)
		);

        // Setters:
        api_def->add_setter(
            masala::make_shared< setter::MasalaObjectAPISetterDefinition_ZeroInput >(
                "reset", "Completely reset the problem description, deleting all one-node and two-node penalties and "
                "all choices for each node.", false, true,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::reset, this )
            )
        );
        api_def->add_setter(
            masala::make_shared< setter::MasalaObjectAPISetterDefinition_ZeroInput >(
                "finalize", "Indicates that problem setup is complete, locking the one- and two-node penalties and making the object read-only.",
                false, true,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::finalize, this )
            )
        );
        api_def->add_setter(
            masala::make_shared< setter::MasalaObjectAPISetterDefinition_ThreeInput< Size, Size, Real > >(
                "set_onebody_penalty", "Set the one-node penalty for a particular choice index selected at a particular node index.",
                "node_index", "The index of the node for which we're setting a penalty.",
                "choice_index", "The index of the choice at this node for which we're setting a penalty.",
                "penalty", "The value of the penalty (or, if negative, bonus).", true, false,
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_onebody_penalty, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
            )
        );
        api_def->add_setter(
            masala::make_shared< setter::MasalaObjectAPISetterDefinition_ThreeInput< std::pair< Size, Size > const &, std::pair< Size, Size > const &, Real > >(
                "set_twobody_penalty", "Set the two-node penalty for a pair of choices at a pair of nodes.",

                "node_indices", "A pair of node indices.  The lower index should be first.  (This function will "
                "throw if it is not, since it makes the choice indices ambiguous).",

                "choice_indices", "The corresponding pair of choice indices.  The first entry should be the choice "
                "index for the lower-numbered node, and the second should be the choice index for the higher-numbered "
                "node.",

                "penalty", "The value of the penalty (or, if negative, bonus).",

                true, false,

                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_twobody_penalty, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
            )
        );

		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_OneInput< std::vector< masala::base::Size > const & > >(
				"add_candidate_solution", "Add a candidate solution.  This may or may not be used as a starting point by a given solver.  This function "
				"locks the problem mutex.  It throws if the problem has already been finalized.",
				"candidate_solution_in", "The input candidate solution.  This should be a vector of zero-based choice indices, with one "
				"index for each variable node in the problem.", false, false,
				std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::add_candidate_solution, this, std::placeholders::_1 )
			)
		);

        // Work functions
        work_function::MasalaObjectAPIWorkFunctionDefinition_OneInputSP< Real, std::vector< Size > const & > compute_absolute_score_fxn(
            masala::make_shared< work_function::MasalaObjectAPIWorkFunctionDefinition_OneInput< Real, std::vector< Size > const & > >(
                "compute_absolute_score", "Given a candidate solution, compute the score.  "
	            "The candidate solution is expressed as a vector of choice indices, with "
                "one entry per variable position, in order of position indices.  This override "
                "uses precomputed one- and two-node penalties cached in the problem definition to "
                "make this calculation efficient.",
                true, false, false, true,
                "candidate_solution", "The candidate solution, expressed as a vector of choice indices, with "
                "one entry per variable position, in order of position indices.  (There may not be "
                "entries for every position, though, since not all positions have at least two choices.)",
                "score", "The score for this candidate solution, computed by this function.",
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::compute_absolute_score, this, std::placeholders::_1 )
            )
        );
        compute_absolute_score_fxn->set_triggers_no_mutex_lock();
        api_def->add_work_function( compute_absolute_score_fxn );

        work_function::MasalaObjectAPIWorkFunctionDefinition_TwoInputSP<
            Real, std::vector< Size > const &,
            std::vector< Size > const &
        > compute_score_change_fxn(
            masala::make_shared< work_function::MasalaObjectAPIWorkFunctionDefinition_TwoInput< Real, std::vector< Size > const &, std::vector< Size > const & > >(
                "compute_score_change", "Given two candidate solutions, compute the score difference.  "
	            "The candidate solutions are expressed as a vector of choice indices, with "
                "one entry per variable position, in order of position indices. (There may not be "
                "entries for every position, though, since not all positions have at least two choices.)  This "
                "override uses precomputed one- and two-node penalties cached in the problem definition to make "
                "this calculation very efficient.",
                true, false, false, true,
                "old_solution", "The first candidate solution, expressed as a vector of choice indices, with "
                "one entry per variable position, in order of position indices.",
                "new_solution", "The second candidate solution, expressed as a vector of choice indices, with "
                "one entry per variable position, in order of position indices.",
                "delta_score", "The score change from old to new candidate solutions, computed by this function.",
                std::bind( &PairwisePrecomputedCostFunctionNetworkOptimizationProblem::compute_score_change, this, std::placeholders::_1, std::placeholders::_2 )
            )
        );
        compute_score_change_fxn->set_triggers_no_mutex_lock();
        api_def->add_work_function( compute_score_change_fxn );

        api_definition() = api_def; //Make const.
    }

    return api_definition();
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Reset this object.  Assumes mutex has been locked.
/// @details Calls parent protected_reset().
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::protected_reset() {
    single_node_penalties_.clear();
    pairwise_node_penalties_.clear();
    single_node_penalties_for_variable_nodes_.clear();
    interacting_variable_nodes_.clear();
    background_constant_offset_ = 0.0;
    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem::protected_reset();
}

/// @brief Indicates that problem setup is complete, locking the one- and two-node penalties
/// and making the object read-only.  Must be called from a mutex-locked context.
/// @details Calls parent protected_finalize().
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::protected_finalize() {
    move_twobody_energies_involving_one_choice_nodes_to_onebody_for_variable_nodes();
    one_choice_node_constant_offset_ = compute_one_choice_node_constant_offset();
    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem::protected_finalize();
    set_up_interacting_node_vector(); // Must come after base class protected_finalize().
    set_up_single_node_penalties_for_variable_nodes_vector(); // Must also come after base class protected_finalize();
    write_to_tracer( "Finalized problem description." );
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Calculate the constant offset for nodes.
/// @details This is the sum of onebody energies for nodes that have exactly
/// one choice, plus the twobdy energies between those nodes.
/// @note This function should be called from a mutex-locked context.
masala::base::Real
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::compute_one_choice_node_constant_offset() {
    using masala::base::Real;
    using masala::base::Size;

    // Accumulators for onebody and twobody energies:
    Real accumulator1( 0.0 ), accumulator2( 0.0 );
    
    // The set of nodes with only one choice:
    std::set< Size > one_choice_nodes;

    // Find all of the nodes with only one choice:
    for( std::map< Size, Size >::const_iterator it( n_choices_by_node_index().begin() );
        it != n_choices_by_node_index().end();
        ++it
    ) {
        // Ensure that this node hasn't already been added:
        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( one_choice_nodes.count( it->first ) == 0,
            "compute_one_choice_node_constant_offset",
            "Node " + std::to_string( it->first ) + " is already in the set of one-choice nodes!"
        );

        // If there's only one choice, add the node:
        if( it->second == 1 ) {
            one_choice_nodes.insert(it->first);
        }
    }

    // Accumulate the onebody energies of one-choice nodes:
    for( std::unordered_map< Size, std::vector< Real > >::const_iterator it( single_node_penalties_.cbegin() );
        it != single_node_penalties_.cend();
        ++it
    ) {
        if( one_choice_nodes.count( it->first ) != 0 ) {
            DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS(
                it->second.size() <= 1,
                "one_choice_node_constant_offset",
                "Program error: multiple choice assignments found in single-node energies!"
            );
            if( it->second.size() == 1 ) {
                accumulator1 += it->second[0]; // Add onebody energies of nodes with only one choice.
            }
        }
    }
    write_to_tracer( "Sum of one-body energies of nodes with only one choice: " + std::to_string( accumulator1 ) );

    // Accumulate the twobody energies of pairs of one-choice nodes:
    for( std::unordered_map< std::pair< Size, Size >, Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic >, masala::base::size_pair_hash >::const_iterator it( pairwise_node_penalties_.cbegin() );
        it != pairwise_node_penalties_.cend();
        ++it
    ) {
        if( one_choice_nodes.count( it->first.first ) != 0 && one_choice_nodes.count( it->first.second ) != 0 ) {
            DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS(
                it->second.rows() <= 1 && it->second.cols() <= 1,
                "one_choice_node_constant_offset",
                "Program error: multiple choice assignments found in pairwise node energies at two positions "
                "that are supposed to have one choice each!"
            );
            if( it->second.rows() == 1 && it->second.cols() == 1 ) {
                accumulator2 += it->second(0,0); // Add twobody energies of pairs of nodes with only one choice.
            }
        }
    }
    write_to_tracer( "Sum of two-body energies between nodes with only one choice: " + std::to_string( accumulator2 ) );

    // Sum the onebody and twobody energies of nodes with only one choice:
    Real total_accumulator( accumulator1 + accumulator2 );
    write_to_tracer( "Total node background: " + std::to_string( total_accumulator ) );

    return total_accumulator;
}


/// @brief Find all twobody energies involving one node with one choice and another node with more than
/// one choice.  Transfer all of these to the onebody energies of the variable node, deleting the corresponding
/// twobody energy.
/// @note This function should be called from a mutex-locked context.  It is called from protected_finalized().
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::move_twobody_energies_involving_one_choice_nodes_to_onebody_for_variable_nodes() {
    using masala::base::Real;
    using masala::base::Size;

    std::set< Size > one_choice_nodes;
    //write_to_tracer( "NODE\tCHOICE_COUNT" );
    for( std::map< Size, Size >::const_iterator it( n_choices_by_node_index().begin() );
        it != n_choices_by_node_index().end();
        ++it
    ) {
        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( one_choice_nodes.count( it->first ) == 0, "move_twobody_energies_involving_one_choice_nodes_to_onebody_for_variable_nodes", "Node " + std::to_string( it->first ) + " is already in the set of one-choice nodes!" );
        //write_to_tracer( std::to_string(it->first) + "\t" + std::to_string(it->second) );
        if( it->second == 1 ) {
            one_choice_nodes.insert(it->first);
        }
    }

    for( std::unordered_map< std::pair< Size, Size >, Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic >, masala::base::size_pair_hash >::iterator it( pairwise_node_penalties_.begin() );
        it!= pairwise_node_penalties_.end();
        // no increment here
    ) {
        std::pair< Size, Size > const & node_indices( it->first );
        Size other_node;
        bool other_node_is_first;
        if( one_choice_nodes.count( node_indices.first ) != 0 && one_choice_nodes.count( node_indices.second ) == 0 ) {
            other_node = node_indices.second;
            other_node_is_first = false;
        } else if ( one_choice_nodes.count( node_indices.second ) != 0 && one_choice_nodes.count( node_indices.first ) == 0 ) {
            other_node = node_indices.first;
            other_node_is_first = true;
        } else {
            ++it; //Increment here.
            continue; // Neither has one choice, or both have one choice.
        }

        // Ensure that there are onebody energies for the other node.
        std::unordered_map< Size, std::vector< Real > >::iterator it_onebodynode( single_node_penalties_.find( other_node ) );
        if( it_onebodynode == single_node_penalties_.end() ) {
            single_node_penalties_[ other_node ] = std::vector< Real >{};
            it_onebodynode = single_node_penalties_.find( other_node );
        }
        std::vector< Real > & onebody_choice_penalties_for_other( it_onebodynode->second );

        // Update the onebody energies for the multi-choice node's choices:
        auto & mat( it->second );
        //Sanity check:
        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( (other_node_is_first ? mat.cols() : mat.rows() ) == 1,
            "move_twobody_energies_involving_one_choice_nodes_to_onebody_for_variable_nodes",
            "Program error: got additional choice for a single-choice node when iterating."
        );
        for( Size choiceindex(0); choiceindex < static_cast< Size >( other_node_is_first ? mat.rows() : mat.cols() ); ++choiceindex ) {
            add_to_vector_index( onebody_choice_penalties_for_other, choiceindex, ( other_node_is_first ? mat(choiceindex, 0) : mat(0, choiceindex) ) );
        }

        // Delete the twobody energy and update the iterator.
        it = pairwise_node_penalties_.erase(it);
    }
}

/// @brief Set up the vector that maps variable node index to a pointer to the vector of one-body penalties
/// for the choices for that node.
/// @note This function should be called from a mutex-locked context.  It is called from protected_finalized().
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_up_single_node_penalties_for_variable_nodes_vector() {
    using masala::base::Real;
    using masala::base::Size;

    DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "set_up_single_node_penalties_for_variable_nodes_vector", "This function can only be called after base class finalization." );
    DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( single_node_penalties_for_variable_nodes_.empty(), "set_up_single_node_penalties_for_variable_nodes_vector", "The single_node_penalties_for_variable_nodes_ vector was not empty!" );

    std::vector< std::pair< Size, Size > > const var_nodes_and_choices( n_choices_at_variable_nodes() );
    single_node_penalties_for_variable_nodes_.resize( var_nodes_and_choices.size(), nullptr );

    for( Size i(0), imax(var_nodes_and_choices.size()); i<imax; ++i ) {
        Size const absnode_index( var_nodes_and_choices[i].first );
        std::unordered_map<masala::base::Size, std::vector<masala::base::Real>>::const_iterator const it( single_node_penalties_.find( absnode_index ) );
        if( it != single_node_penalties_.cend() ) {
            single_node_penalties_for_variable_nodes_[i] = &it->second;
        }
    }
}

/// @brief Set up the interacting_variable_nodes_ data structure, listing, for each variable node, the
/// nodes that interact and providing (raw) pointers to their choice interaction matrices.
/// @note This function should be called from a mutex-locked context.  It is called from protected_finalized().
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_up_interacting_node_vector() {
    using masala::base::Size;

    DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "set_up_interacting_node_vector", "This function can only be called after base class finalization." );
    DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( interacting_variable_nodes_.empty(), "set_up_interacting_node_vector", "The interacting_variables_nodes_ vector was not empty!" );

    std::vector< std::pair< Size, Size > > const var_nodes_and_choices( n_choices_at_variable_nodes() );
    std::unordered_map< Size, Size > var_node_by_abs_node;
    for( Size i(0), imax(var_nodes_and_choices.size()); i<imax; ++i ) {
        var_node_by_abs_node[ var_nodes_and_choices[i].first ] = i;
    }
    interacting_variable_nodes_.resize( var_nodes_and_choices.size() );
    
    for( auto const & entry : pairwise_node_penalties_ ) {
        DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( entry.first.first != entry.first.second, "set_up_interacting_node_vector", "In the pairwise_node_penalties_ map, "
            "a node was found that interacts with itself.  This should not be possible.  Program error."
        );
        if( entry.second.rows() < 2 || entry.second.cols() < 2 ) continue; // Single-choice nodes.
        Size const varnode_i( var_node_by_abs_node.at(entry.first.first) );
        Size const varnode_j( var_node_by_abs_node.at(entry.first.second) );
        interacting_variable_nodes_[varnode_i].push_back( std::make_pair( varnode_j, &entry.second ) );
        interacting_variable_nodes_[varnode_j].push_back( std::make_pair( varnode_i, &entry.second ) );
    }
}

/// @brief Create a vector of choice indices just large enough to store a given choice index.
/// Set all entries to zero except for that index.
/*static*/
std::vector< masala::base::Real >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::create_choice_vector(
    masala::base::Size const choice_index,
    masala::base::Real const choice_penalty
) {
    std::vector< masala::base::Real > outvec( choice_index + 1, 0.0 );
    outvec[choice_index] = choice_penalty;
    return outvec;
}

/// @brief Create an Eigen matrix just large enough to store a given pair of indices.  Fill it
/// with zeros, except for the one entry specified.
/*static*/
Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic >
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::create_choicepair_matrix(
    std::pair< masala::base::Size, masala::base::Size > const & indices,
    masala::base::Real const value
) {
    Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > outmatrix( indices.first + 1, indices.second + 1 );
    outmatrix.setZero();
    outmatrix( indices.first, indices.second ) = value;
    return outmatrix;
}

/// @brief Given a vector with a certain number of entries, set the value of entry N.  If the
/// vector length is less than N+1, extend the vector, padding it with zeros.
/*static*/
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_entry_in_vector(
    std::vector< masala::base::Real > & vec,
    masala::base::Size const index,
    masala::base::Real const value
) {
    if( vec.size() > index ) {
        vec[index] = value;
    } else {
        vec.resize( index+1, 0.0 );
        vec[index] = value;
    }
}

/// @brief Given a matrix with certain dimensions, set the value of an entry.  If the matrix
/// is too small, resize it appropriately, padding with zeros.
/*static*/
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::set_entry_in_matrix(
    Eigen::Matrix< masala::base::Real, Eigen::Dynamic, Eigen::Dynamic > & mat,
    std::pair< masala::base::Size, masala::base::Size > const & indices,
    masala::base::Real const value
) {
    using masala::base::Size;

    Size const oldrows( mat.rows() ), oldcols( mat.cols() );
    if( indices.first >= oldrows ) {
        if( indices.second >= oldcols ) {
            mat.conservativeResize( indices.first + 1, indices.second + 1 );
            for( Size y(0); y <= indices.first; ++y ) {
                for( Size x( y < oldrows ? oldcols : 0 ); x <= indices.second; ++x ) {
                    mat(y,x) = 0.0;
                }
            }
        } else {
            mat.conservativeResize( indices.first + 1, Eigen::NoChange );
            for( Size y(oldrows); y <= indices.first; ++y ) {
                for( Size x(0); x < oldcols; ++x ) {
                    mat(y,x) = 0.0;
                }
            }
        }
    } else if( indices.second >= oldcols ) {
        mat.conservativeResize( Eigen::NoChange, indices.second + 1 );
        for( Size y(0); y < oldrows; ++y ) {
            for( Size x( oldcols ); x <= indices.second; ++x ) {
                mat(y,x) = 0.0;
            }
        }
    }

    mat( indices.first, indices.second ) = value;
}

/// @brief Given a vector, add a value to the Nth entry, or, if the vector has fewer than N entries,
/// expand it with zero padding, then set the last entry to the value.
/*static*/
void
PairwisePrecomputedCostFunctionNetworkOptimizationProblem::add_to_vector_index(
    std::vector< masala::base::Real > & vec,
    masala::base::Size const index,
    masala::base::Real const value
) {
    if( index < vec.size() ) {
        vec[index] += value;
    } else {
        vec.resize( index+1, 0.0 );
        vec[index] = value;
    }
}

} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
