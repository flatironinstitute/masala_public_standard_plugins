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

/// @file src/numeric/optimization/cost_function_network/cost_function/feature_based/SumOfUnsatisfiedChoiceFeaturesCostFunction.cc
/// @brief Implementation for a pure virtual base class for SumOfUnsatisfiedChoiceFeaturesCostFunctions.
/// @details SumOfUnsatisfiedChoiceFeaturesCostFunctions define a penalty function which is based on the following:
/// - One or more features are defined for each choice at each node.  (A feature could be a hydrogen bond
/// donor or acceptor, for instance, in a packing problem)
/// - Each feature has an expected minimum and maximum number of connections that can be set.
/// - For each pair of choices, the features that are connected can be set.
/// - At calculation time, the number of unsatisfied features (total features minus features
/// satisfied by connections) is returned.
/// @note Since this class does not implement class_name() or class_namespace()
/// functions required by the MasalaObject base class, it remains pure virtual.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/feature_based/SumOfUnsatisfiedChoiceFeaturesCostFunction.hh>

// STL headers:
#include <vector>
#include <string>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/utility/container/container_util.tmpl.hh>

// Optimizers headers:
#include <optimizers/cost_function_network/cost_function/feature_based/ChoiceFeature.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace feature_based {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
SumOfUnsatisfiedChoiceFeaturesCostFunction::SumOfUnsatisfiedChoiceFeaturesCostFunction(
    SumOfUnsatisfiedChoiceFeaturesCostFunction const & src
) :
    masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction( src )
{
    std::lock( src.mutex(), mutex() );
    std::lock_guard< std::mutex > lockthis( mutex(), std::adopt_lock );
    std::lock_guard< std::mutex > lockthat( src.mutex(), std::adopt_lock );
    assign_mutex_locked( src );
}

// @brief Assignment operator.
SumOfUnsatisfiedChoiceFeaturesCostFunction &
SumOfUnsatisfiedChoiceFeaturesCostFunction::operator=(
    SumOfUnsatisfiedChoiceFeaturesCostFunction const & src
) {
    std::lock( src.mutex(), mutex() );
    std::lock_guard< std::mutex > lockthis( mutex(), std::adopt_lock );
    std::lock_guard< std::mutex > lockthat( src.mutex(), std::adopt_lock );
    assign_mutex_locked( src );
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction", "ChoiceFeatureBasedCostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_categories() const {
	return std::vector< std::vector< std::string > >{
        { "CostFunction", "ChoiceFeatureBasedCostFunction" }
    };
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "not_pairwise_decomposible", "unsatisfied_choice_feature_sum_based" }
std::vector< std::string >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_keywords() const {
	std::vector< std::string > outvec( CostFunction::get_keywords() );
    outvec.push_back( "not_pairwise_decomposible" );
    outvec.push_back( "unsatisfied_choice_feature_sum_based" );
    return outvec;
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "SumOfUnsatisfiedChoiceFeaturesCostFunction" } }.
std::vector< std::vector< std::string > >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_data_representation_categories() const {
    return std::vector< std::vector< std::string > >{ { "CostFunction", "SumOfUnsatisfiedChoiceFeaturesCostFunction" } };
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "not_pairwise_decomposible", "unsatisfied_choice_feature_sum_based" }
std::vector< std::string >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_data_representation_keywords() const {
	std::vector< std::string > outvec( CostFunction::get_data_representation_keywords() );
    outvec.push_back( "not_pairwise_decomposible" );
    outvec.push_back( "unsatisfied_choice_feature_sum_based" );
    return outvec;
}

/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
/// is compatible.
/// @returns {"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"}.
std::vector< std::string >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_compatible_masala_engines() const {
    return std::vector< std::string >{
        "standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"
    };
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "choice_feature_based", "cost_function", "not_pairwise_decomposible" }.
std::vector< std::string >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_present_data_representation_properties() const {
    return std::vector< std::string >{
        "choice_feature_based",
        "cost_function",
        "not_pairwise_decomposible"
    };
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
std::vector< std::string >
SumOfUnsatisfiedChoiceFeaturesCostFunction::get_absent_data_representation_properties() const {
    return std::vector< std::string >{ "pairwise_decomposible" };
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Add a choice feature for a set of nodes, indexed by absolute node index.
/// @details This can only be called prior to object finalization.
///
/// @param[in] absolute_node_index The index of this node (absolute).
/// @param[in] choice_index The index of this choice.
/// @param[in] min_connections_to_satisfy_feature The minimum number of connections that
/// this feature must make in order to be satisfied.
/// @param[in] max_connections_to_satisfy_feature The maximum number of connections that
/// this feature must make in order to be satisfied.
/// @param[in] feature_connection_offset The number of connections that this feature always
/// makes (e.g. to background, or to itself).
///
/// @returns The index of the newly-added choice feature in the vector of choice features for
/// this position.
masala::base::Size
SumOfUnsatisfiedChoiceFeaturesCostFunction::add_choice_feature_by_absolute_node_index(
    masala::base::Size const absolute_node_index,
    masala::base::Size const choice_index,
    masala::base::Size const min_connections_to_satisfy_feature,
    masala::base::Size const max_connections_to_satisfy_feature,
    masala::base::Size const feature_connection_offset
) {
    using masala::base::Size;
    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "add_choice_feature_by_absolute_node_index",
        "Choice features cannot be added after this object has already been finalized!"
    );
    std::pair< Size, Size > key( absolute_node_index, choice_index );
    std::unordered_map<
	    std::pair< masala::base::Size, masala::base::Size >,
		std::vector< ChoiceFeatureSP >,
		masala::base::size_pair_hash
	>::iterator it(
        choice_features_by_absolute_node_and_choice_.find( key )
    );
    if( it == choice_features_by_absolute_node_and_choice_.end() ) {
        choice_features_by_absolute_node_and_choice_[key] = std::vector< ChoiceFeatureSP >{
            masala::make_shared< ChoiceFeature >(
                min_connections_to_satisfy_feature,
                max_connections_to_satisfy_feature,
                feature_connection_offset
            )
        };
        return 0;
    } else {
        it->second.push_back(
            masala::make_shared< ChoiceFeature >(
                min_connections_to_satisfy_feature,
                max_connections_to_satisfy_feature,
                feature_connection_offset
            )
        );
        return it->second.size() - 1;
    }
    return 0; // Keep older compilers happy, though we never reach here.
}

/// @brief Given an absolute node index, declare all features for all choices at that
/// index.
/// @details No choices must have been declared previously, or this function will throw.
/// If this object was previously finalized, this function will throw.  Locks mutex (i.e.
/// threadsafe).
///
/// @param[in] absolute_node_index The index of the node for which we are setting choices.
/// @param[in] min_and_max_connections_by_choice_and_feature A vector indexed by choice index, containing
/// vectors indexed by feature index, containing pairs of min connection count and max connection count
/// for each feature.
///
/// @note All choice features are initialized to offsets of zero.  The increment_offsets_at_node()
/// function can be used to adjust this.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::declare_features_for_node_choices(
    masala::base::Size const absolute_node_index,
    std::vector< std::vector< std::pair< masala::base::Size, masala::base::Size > > > const & min_and_max_connections_by_choice_and_feature
) {
    using masala::base::Size;
    using std::pair;
    using std::vector;

    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "declare_features_for_node_choices",
        "Choice features cannot be declared after this object has already been finalized!"
    );

    for( Size ichoice(0), ichoicemax(min_and_max_connections_by_choice_and_feature.size()); ichoice<ichoicemax; ++ichoice ) {
        pair< Size, Size > key( absolute_node_index, ichoice );
        CHECK_OR_THROW_FOR_CLASS( choice_features_by_absolute_node_and_choice_.count(key) == 0,
            "declare_features_for_node_choices",
            "Cannot declare features for node " + std::to_string( absolute_node_index )
            + ", choice " + std::to_string( ichoice ) + ", since this node/choice combination "
            + "has already been declared!"
        );
        vector< pair< Size, Size > > const & min_and_max_by_feature( min_and_max_connections_by_choice_and_feature[ichoice] );
        if( min_and_max_by_feature.empty() ) { continue; }
        std::vector< ChoiceFeatureSP > vec;
        vec.reserve( min_and_max_by_feature.size() );
        for( Size ifeature(0), ifeaturemax(min_and_max_by_feature.size()); ifeature<ifeaturemax; ++ifeature ) {
            pair< Size, Size > const & min_max( min_and_max_by_feature[ifeature] );
            vec.push_back( masala::make_shared< ChoiceFeature >( min_max.first, min_max.second, 0 ) );
        }
        choice_features_by_absolute_node_and_choice_[key] = vec;
    }
}

/// @brief Increment all choices at a specified set of nodes.
/// @details This can only be called prior to object finalization.  Locks mutex (i.e. threadsafe).
/// If node or choices have not yet been declared, this function throws.
/// @param[in] offset_increments A map whose key is the absolute node index, of vectors indexed by
/// choice index, of vectors indexed by feature index, of offset increments.  The offsets of features
/// will be increased by these increments.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::increment_offsets(
    std::unordered_map< masala::base::Size, std::vector< std::vector< masala::base::Size > > > const & offset_increments
) {
    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "increment_offsets",
        "Choice feature offsets cannot be incremented after this object has already been finalized!"
    );

    for( auto const & it : offset_increments ) {
        increment_offsets_at_node_mutex_locked( it.first, it.second );
    }
}

/// @brief For all choices at a given node, increment the offsets.
/// @details This can only be called prior to object finalization.  Locks mutex (i.e. threadsafe).
/// If node or choices have not yet been declared, this function throws.
///
/// @param[in] absolute_node_index The index of the node for which we are updating choices.
/// @param[in] offset_increments The amount by which we are incrementing the choices, provided as
/// a vector indexed by choice index of vectors indexed by choice feature index.  Any choices or features
/// not yet declared trigger an exception.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::increment_offsets_at_node(
    masala::base::Size const absolute_node_index,
    std::vector< std::vector< masala::base::Size > > const & offset_increments
) {
    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "increment_offsets_at_node",
        "Choice feature offsets cannot be incremented after this object has already been finalized!"
    );

	increment_offsets_at_node_mutex_locked( absolute_node_index, offset_increments );
}

/// @brief Given a node and a choice, add node/choice pairs that satisfy one or more of its features.
/// @details The node and choice and features must already have been added, or else this throws.  This function
/// is threadsafe (i.e. it locks the mutex), but can only be called before this object is finalized.
///
/// @param[in] absolute_node_index The node for which we are adding feature connections.
/// @param[in] choice_index The choice for which we are adding feature connections.
/// @param[in] connecting_node_choices_by_feature A vector indexed by feature index for the node and choice given by
/// absolute_node_index and choice_index, pointing to maps indexed by other node/choice pairs, in turn pointing to
/// the number of connections that this feature makes to those node/choice pairs.  The number of connections to those
/// node/choice pairs will be incremented by this amount, or, if there are no connections to those node/choice pairs,
/// will be set to this amount.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::add_connecting_node_choices_for_features_of_node_choice(
    masala::base::Size const absolute_node_index,
    masala::base::Size const choice_index,
    std::vector< std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > > const & connecting_node_choices_by_feature
) {
    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(),
        "add_connecting_node_choices_for_features_of_node_choice",
        "Choice feature connections cannot be added after this " + class_name()
        + "object has already been finalized!"
    );

    add_connecting_node_choices_for_features_of_node_choice_mutex_locked( absolute_node_index, choice_index, connecting_node_choices_by_feature );
}

/// @brief Given a node, add node/choice pairs that satisfy its choices' features.
/// @details The node and choice and features must already have been added, or else this throws.  This function
/// is threadsafe (i.e. it locks the mutex), but can only be called before this object is finalized.
///
/// @param[in] absolute_node_index The node for which we are adding feature connections.
/// @param[in] connecting_node_connections_by_choice_and_feature A vector indexed by choice index, containing vectors
/// indexed by feature index, pointing to maps indexed by other node/choice pairs, in turn pointing to
/// the number of connections that this feature makes to those node/choice pairs.  The number of connections to those
/// node/choice pairs will be incremented by this amount, or, if there are no connections to those node/choice pairs,
/// will be set to this amount.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::add_connecting_node_choices_for_features_of_node_choices(
    masala::base::Size const absolute_node_index,
    std::vector< std::vector< std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > > > const & connecting_node_connections_by_choice_and_feature
) {
    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(),
        "add_connecting_node_choices_for_features_of_node_choices",
        "Choice feature connections cannot be added after this " + class_name()
        + "object has already been finalized!"
    );
    
    for( masala::base::Size i(0), imax(connecting_node_connections_by_choice_and_feature.size()); i<imax; ++i ) {
        add_connecting_node_choices_for_features_of_node_choice_mutex_locked( absolute_node_index, i, connecting_node_connections_by_choice_and_feature[i] );
    }
}

/// @brief Add node/choice pairs that satisfy several nodes' features.
/// @details The nodes, choices, and featuers must already have been added, or else this throws.  This function
/// is threadsafe (i.e. it locks the mutex) but it can only be called before this object is finalized.
///
/// @param[in] connecting_node_connections_by_node_and_choice_and_feature A map indexed by node index, containing vectors
/// indexed by choice index, containing vectors indexed by feature index, containing maps indexed by other node/choice pairs,
/// containing the number of connections that the node/choice/feature makes to the inner node/choice pairs.  The number of
/// connections for the outer node/choice/feature will be incremented by this amount, or, if there are no connections to those
/// node/choice pairs, will be set to this amount.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::add_connecting_node_choices_for_features_of_nodes_choices(
    std::unordered_map<
        masala::base::Size,
        std::vector<
            std::vector<
                std::unordered_map<
                    std::pair< masala::base::Size, masala::base::Size >,
                    masala::base::Size,
                    masala::base::size_pair_hash
                >
            >
        >
    > const & connecting_node_connections_by_node_and_choice_and_feature
) {
    using std::unordered_map;
    using std::vector;
    using std::pair;
    using masala::base::Size;
    using masala::base::size_pair_hash;
    typedef unordered_map< Size, vector< vector< unordered_map< pair< Size, Size >, Size, size_pair_hash > > > > datastruct;

    std::lock_guard< std::mutex > lock( mutex() );
    CHECK_OR_THROW_FOR_CLASS( !protected_finalized(),
        "add_connecting_node_choices_for_features_of_nodes_choices",
        "Choice feature connections cannot be added after this " + class_name()
        + "object has already been finalized!"
    );

    for(
        datastruct::const_iterator it( connecting_node_connections_by_node_and_choice_and_feature.begin() );
        it != connecting_node_connections_by_node_and_choice_and_feature.end();
        ++it
    ) {
        masala::base::Size const absolute_node_index( it->first );
        auto const & connecting_node_connections_by_choice_and_feature( it->second );
        for( masala::base::Size i(0), imax(connecting_node_connections_by_choice_and_feature.size()); i<imax; ++i ) {
            add_connecting_node_choices_for_features_of_node_choice_mutex_locked( absolute_node_index, i, connecting_node_connections_by_choice_and_feature[i] );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given a selection of choices at variable nodes, compute the cost function.
/// @note No mutex-locking is performed!
masala::base::Real
SumOfUnsatisfiedChoiceFeaturesCostFunction::compute_cost_function(
    std::vector< masala::base::Size > const & candidate_solution
) const {
    using masala::base::Real;
    return protected_weight() * static_cast< Real >( protected_compute_cost_function_no_weight( candidate_solution ) );
}

/// @brief Given an old selection of choices at variable nodes and a new selection,
/// compute the cost function difference.
/// @note No mutex-locking is performed!
masala::base::Real
SumOfUnsatisfiedChoiceFeaturesCostFunction::compute_cost_function_difference(
    std::vector< masala::base::Size > const & candidate_solution_old,
    std::vector< masala::base::Size > const & candidate_solution_new
) const {
    using masala::base::Real;
    return protected_weight() * ( static_cast< Real >( protected_compute_cost_function_no_weight( candidate_solution_new ) ) - static_cast< Real >( protected_compute_cost_function_no_weight( candidate_solution_old ) ) );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given a selection of choices at variable nodes, compute the number of unsatisfied features   .
/// @note No mutex-locking is performed, and the result is not multiplied by the weight.
masala::base::Size
SumOfUnsatisfiedChoiceFeaturesCostFunction::protected_compute_cost_function_no_weight(
    std::vector< masala::base::Size > const & candidate_solution
) const {
    using std::unordered_map;
    using std::pair;
    using std::vector;
    using masala::base::Size;
    using masala::base::Real;
    using masala::base::size_pair_hash;

    DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS(
        protected_finalized(),
        "protected_compute_cost_function_no_weight",
        "This object must be finalized before calling this function!"
    );

    Size unsatisfied_choice_features(0);
    std::vector< std::vector< ChoiceFeature const * > const * > selected_and_fixed;
    selected_and_fixed.reserve( candidate_solution.size() + fixed_choice_features_by_absolute_node_and_choice_.size() );
    for( Size i(0), imax(candidate_solution.size()); i<imax; ++i ) {
        pair< Size, Size > const key( std::make_pair(i, candidate_solution[i]) );
        auto const it( choice_features_by_variable_node_and_choice_.find( key ) );
        if( it != choice_features_by_variable_node_and_choice_.end() ) {
            selected_and_fixed.push_back( &(it->second) );
        }
    }
    for( auto it( fixed_choice_features_by_absolute_node_and_choice_.begin() ); it != fixed_choice_features_by_absolute_node_and_choice_.end(); ++it ) {
        selected_and_fixed.push_back( &(it->second) );
    }

    // Loop over all positions and choices currently selected, plus all fixed positions:
    for(
        auto it( selected_and_fixed.begin() );
        it != selected_and_fixed.end();
        ++it
    ) {
        // Loop over all choice features for position and choice:
        for( auto const choicefeature : (**it) ) {
            // Loop over all entries in the candidate solution.  Count the number of connections to this feature.
            Size const connection_count( choicefeature->offset() + choicefeature->n_connections_to_feature_from_nodes_and_choices( candidate_solution ) );
            if( !choicefeature->is_satisfied( connection_count ) ) {
                ++unsatisfied_choice_features;
            }
        }
    }

    //write_to_tracer( "Unsatisfied features: " + std::to_string( unsatisfied_choice_features ) ); //DELETE ME.

    return unsatisfied_choice_features;
}

/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
/// @param[in] variable_node_indices A list of all of the absolute node indices
/// for nodes that have more than one choice, indexed by variable node index.
/// @details The base class function simply marks this object as finalized.  Should
/// be overridden, and overrides should call parent class protected_finalize().
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::protected_finalize(
    std::vector< masala::base::Size > const & variable_node_indices
) {
    using masala::base::Size;

    // Make a map of variable node indices indexed by absolute node index:
    variable_node_indices_by_absolute_node_index_.clear();
    for( Size var_index(0), var_index_max(variable_node_indices.size()); var_index < var_index_max; ++var_index ) {
        variable_node_indices_by_absolute_node_index_[ variable_node_indices[var_index] ] = var_index;
    }

    // Copy data from choice_features_by_absolute_node_and_choice_ to choice_features_by_variable_node_and_choice_.
    // Identify ChoiceFeatures that are now part of fixed background, and store these in a list.
    for(
        std::unordered_map< std::pair< Size, Size >, std::vector< ChoiceFeatureSP >, masala::base::size_pair_hash >::iterator it( choice_features_by_absolute_node_and_choice_.begin() );
        it != choice_features_by_absolute_node_and_choice_.end();
        ++it
    ) {
        std::vector< ChoiceFeatureSP > & choice_feature_csp_vec( it->second );
        std::vector< ChoiceFeature const * > choice_feature_csp_vec_copy( choice_feature_csp_vec.size() );

        //Finalize all choice features, and copy to a vector of raw pointers:
        for(
            Size choice_feature_vec_index(0), choice_feature_vec_index_max( choice_feature_csp_vec.size() );
            choice_feature_vec_index < choice_feature_vec_index_max;
            ++choice_feature_vec_index
        ) {
            choice_feature_csp_vec.at(choice_feature_vec_index)->finalize( variable_node_indices_by_absolute_node_index_ );
            choice_feature_csp_vec_copy[choice_feature_vec_index] = choice_feature_csp_vec.at(choice_feature_vec_index).get();
        }
        Size const abs_node_index( it->first.first );
        Size const choice_index( it->first.second );
        if( masala::base::utility::container::has_value( variable_node_indices, abs_node_index ) ) {
            // If this is a variable node.
            choice_features_by_variable_node_and_choice_[ std::make_pair( variable_node_indices_by_absolute_node_index_.at( abs_node_index ), choice_index ) ] = choice_feature_csp_vec_copy;
        } else {
            // If this is a fixed node.
            fixed_choice_features_by_absolute_node_and_choice_[ std::make_pair( abs_node_index, choice_index ) ] = choice_feature_csp_vec_copy;
        }
    }

    //Do NOT clear choice_features_by_absolute_node_and_choice_, since the shared pointers reside here, and the
    //choice_features_by_variable_node_and_choice_ map stores raw pointers.

    masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::protected_finalize( variable_node_indices );
}

/// @brief Override of assign_mutex_locked().  Calls parent function.
/// @details Throws if src is not a SumOfUnsatisfiedChoiceFeaturesCostFunction.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::assign_mutex_locked(
    CostFunction const & src
) {
    SumOfUnsatisfiedChoiceFeaturesCostFunction const * const src_cast_ptr( dynamic_cast< SumOfUnsatisfiedChoiceFeaturesCostFunction const * >( &src ) );
    CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "assign_mutex_locked", "Cannot assign a SumOfUnsatisfiedChoiceFeaturesCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

    choice_features_by_absolute_node_and_choice_ = src_cast_ptr->choice_features_by_absolute_node_and_choice_;
    choice_features_by_variable_node_and_choice_ = src_cast_ptr->choice_features_by_variable_node_and_choice_;
    fixed_choice_features_by_absolute_node_and_choice_ = src_cast_ptr->fixed_choice_features_by_absolute_node_and_choice_;

    masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::assign_mutex_locked( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::make_independent_mutex_locked() {
    using masala::base::Size;
    using std::unordered_map;
    using std::pair;
    using masala::base::size_pair_hash;
    using std::vector;

    for( unordered_map< pair< Size, Size >, vector< ChoiceFeatureSP >, size_pair_hash >::iterator it(choice_features_by_absolute_node_and_choice_.begin());
        it != choice_features_by_absolute_node_and_choice_.end();
        ++it
    ) {
        std::vector< ChoiceFeatureSP > & vec( it->second );
        for( Size i(0), imax(vec.size()); i<imax; ++i ) {
            vec[i] = vec[i]->deep_clone();
        }
        if( protected_finalized() ) {
            auto it2( fixed_choice_features_by_absolute_node_and_choice_.find( it->first ) );
            if( it2 == fixed_choice_features_by_absolute_node_and_choice_.end() ) {
                pair<Size, Size> const var_node_index_and_choice( variable_node_indices_by_absolute_node_index_.at( it->first.first ), it->first.second );
                it2 = choice_features_by_variable_node_and_choice_.find( var_node_index_and_choice );
                DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( it2 != choice_features_by_variable_node_and_choice_.end(),
                    "make_independent_mutex_locked", "Could not find absolute node "
                    + std::to_string(it->first.first) + ", choice " + std::to_string(it->first.second)
                    + " in either the variable nodes or the fixed nodes.  This is a program error."
                );
            }
            std::vector< ChoiceFeature const * > & vec2( it2->second );
            DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( vec.size() == vec2.size(), "make_independent_mutex_locked",
                "Expected vector size match.  Got vec.size() == " + std::to_string(vec.size()) + ", vec2.size() == "
                + std::to_string(vec2.size()) + ".  This is a program error."
            );
            for( Size i2(0), i2max(vec2.size()); i2<=i2max; ++i2 ) {
                vec2[i2] = vec[i2].get();
            }
        }
    }
    masala::numeric_api::base_classes::optimization::cost_function_network::cost_function::PluginCostFunction::make_independent_mutex_locked();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief For all choices at a given node, increment the offsets.
/// @details This version assumes that the mutex has already been locked.
///
/// @param[in] absolute_node_index The index of the node for which we are updating choices.
/// @param[in] offset_increments The amount by which we are incrementing the choices, provided as
/// a vector indexed by choice index of vectors indexed by choice feature index.  Any choices or features
/// not yet declared trigger an exception.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::increment_offsets_at_node_mutex_locked(
    masala::base::Size const absolute_node_index,
    std::vector< std::vector< masala::base::Size > > const & offset_increments
) {
    using masala::base::Size;
    using std::unordered_map;
    using std::pair;
    using std::vector;
    using masala::base::size_pair_hash;
    typedef unordered_map< pair< Size, Size >, vector< ChoiceFeatureSP >, size_pair_hash >::iterator my_map_iterator;

    for( Size ichoice(0), ichoicemax(offset_increments.size()); ichoice<ichoicemax; ++ichoice ) {
        vector< Size > const & offset_increments_by_feature( offset_increments[ichoice] );
        if( offset_increments_by_feature.empty() ) { continue; }
        pair< Size, Size > const key( absolute_node_index, ichoice );
        my_map_iterator it( choice_features_by_absolute_node_and_choice_.find( key ) );
        CHECK_OR_THROW_FOR_CLASS( it != choice_features_by_absolute_node_and_choice_.end(),
            "increment_offsets_at_node",
            "Node " + std::to_string( absolute_node_index ) + ", choice " + std::to_string( ichoice )
            + " was specified, but no such node/choice combination has been added yet!  Nodes and choices "
            "must be declared before they are incremented."
        );
        std::vector< ChoiceFeatureSP > & vec( it->second );
        CHECK_OR_THROW_FOR_CLASS( vec.size() == offset_increments_by_feature.size(),
            "increment_offsets_at_node", std::to_string(offset_increments_by_feature.size())
            + " features to increment were specified for node " + std::to_string( absolute_node_index )
            + ", choice " + std::to_string( ichoice ) + ", but there are " + std::to_string( vec.size() )
            + " features for this node/choice combination.  One offset must be specified per feature."
        );
        for( Size ifeature(0), ifeaturemax( vec.size() ); ifeature<ifeaturemax; ++ifeature ) {
            vec[ifeature]->increment_offset( offset_increments_by_feature[ifeature] );
        }
    }
}

/// @brief Given a node and a choice, add node/choice pairs that satisfy one or more of its features.
/// @details The node and choice and features must already have been added, or else this throws.  This
/// version is used internally, and assumes that the mutex has already been locked.
///
/// @param[in] absolute_node_index The node for which we are adding feature connections.
/// @param[in] choice_index The choice for which we are adding feature connections.
/// @param[in] connecting_node_choices_by_feature A vector indexed by feature index for the node and choice given by
/// absolute_node_index and choice_index, pointing to maps indexed by other node/choice pairs, in turn pointing to
/// the number of connections that this feature makes to those node/choice pairs.  The number of connections to those
/// node/choice pairs will be incremented by this amount, or, if there are no connections to those node/choice pairs,
/// will be set to this amount.
void
SumOfUnsatisfiedChoiceFeaturesCostFunction::add_connecting_node_choices_for_features_of_node_choice_mutex_locked(
    masala::base::Size const absolute_node_index,
    masala::base::Size const choice_index,
    std::vector< std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > > const & connecting_node_choices_by_feature
) {
    using masala::base::Size;
    using std::pair;
    using std::unordered_map;
    using masala::base::size_pair_hash;

    if( connecting_node_choices_by_feature.empty() ) { return; }

    pair< Size, Size > const this_key( absolute_node_index, choice_index );
    auto it( choice_features_by_absolute_node_and_choice_.find( this_key ) );
    CHECK_OR_THROW_FOR_CLASS( it != choice_features_by_absolute_node_and_choice_.end(),
        "add_connecting_node_choices_for_features_of_node_choice_mutex_locked",
        "Node " + std::to_string( absolute_node_index ) + ", choice " + std::to_string( choice_index )
        + " has not been added to this " + class_name() + " object.  Node/choice pairs must "
        "be added before this function is called."
    );
    std::vector< ChoiceFeatureSP > & feature_vec( it->second );
    CHECK_OR_THROW_FOR_CLASS(
        connecting_node_choices_by_feature.size() == feature_vec.size(),
        "add_connecting_node_choices_for_features_of_node_choice_mutex_locked",
        "Node " + std::to_string( absolute_node_index ) + ", choice " + std::to_string( choice_index )
        + "has " + std::to_string( feature_vec.size() ) + " features, but a vector of "
        + std::to_string( connecting_node_choices_by_feature.size() )
        + "features was provided to this function."
    );

    for( Size ifeature(0), ifeaturemax(connecting_node_choices_by_feature.size()); ifeature<ifeaturemax; ++ifeature ) {
        ChoiceFeature & feature( *(feature_vec[ifeature]) );
        unordered_map< pair< Size, Size >, Size, size_pair_hash > const & nodechoice_to_connection_map( connecting_node_choices_by_feature[ifeature] );
        for( unordered_map< pair< Size, Size >, Size, size_pair_hash >::const_iterator it2( nodechoice_to_connection_map.begin() ); it2 != nodechoice_to_connection_map.end(); ++it2 ) {
            feature.increment_other_node_and_choice_that_satisfies_this( it2->first.first, it2->first.second, it2->second );
        }
    }
}

} // namespace feature_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
