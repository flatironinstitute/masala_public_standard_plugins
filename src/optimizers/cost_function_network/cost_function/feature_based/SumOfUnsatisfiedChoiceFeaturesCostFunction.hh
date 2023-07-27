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

/// @file src/numeric/optimization/cost_function_network/cost_function/feature_based/SumOfUnsatisfiedChoiceFeaturesCostFunction.hh
/// @brief Header for a pure virtual base class for SumOfUnsatisfiedChoiceFeaturesCostFunctions.
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

#ifndef Masala_src_numeric_optimization_cost_function_network_cost_function_feature_based_SumOfUnsatisfiedChoiceFeaturesCostFunction_hh
#define Masala_src_numeric_optimization_cost_function_network_cost_function_feature_based_SumOfUnsatisfiedChoiceFeaturesCostFunction_hh

// Forward declarations:
#include <numeric/optimization/cost_function_network/cost_function/feature_based/SumOfUnsatisfiedChoiceFeaturesCostFunction.fwd.hh>

// Parent header:
#include <numeric/optimization/cost_function_network/cost_function/CostFunction.hh>

// Numeric headers:
#include <numeric/optimization/cost_function_network/cost_function/feature_based/ChoiceFeature.fwd.hh>

// Base headers:
#include <base/types.hh>
#include <base/hash_types.hh>

// STL headers:
#include <unordered_map>
#include <utility> //For std::pair.

namespace masala {
namespace numeric {
namespace optimization {
namespace cost_function_network {
namespace cost_function {
namespace feature_based {

/// @brief A pure virtual base class for SumOfUnsatisfiedChoiceFeaturesCostFunctions.
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
class SumOfUnsatisfiedChoiceFeaturesCostFunction : public masala::numeric::optimization::cost_function_network::cost_function::CostFunction {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	SumOfUnsatisfiedChoiceFeaturesCostFunction() = default;

	/// @brief Copy constructor.
	SumOfUnsatisfiedChoiceFeaturesCostFunction(
		SumOfUnsatisfiedChoiceFeaturesCostFunction const & src
	);

	// @brief Assignment operator.
	SumOfUnsatisfiedChoiceFeaturesCostFunction &
	operator=( SumOfUnsatisfiedChoiceFeaturesCostFunction const & src );

	/// @brief Destructor.
	~SumOfUnsatisfiedChoiceFeaturesCostFunction() override = default;

	/// @brief This class is pure virtual, and does not define the clone function.
	CostFunctionSP clone() const override = 0;

	/// @brief This class is pure virtual, and does not define the make independent function.
	void make_independent() override = 0;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all
	/// cost functions; may be overridden by derived classes.
	/// @returns { { "CostFunction", "ChoiceFeatureBasedCostFunction" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.
	/// @returns { "optimization_problem", "cost_function", "numeric", "not_pairwise_decomposible", "unsatisfied_choice_feature_sum_based" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief Get the category for this MasalaDataRepresentation.
	/// @returns { { "CostFunction", "ChoiceFeatureBasedCostFunction" } }.
	std::vector< std::vector< std::string > >
	get_data_representation_categories() const override;

	/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
	/// is compatible.
	/// @returns {"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"}.
	std::vector< std::string >
	get_compatible_masala_engines() const override;

	/// @brief Get the properties of this MasalaDataRepresentation.
	/// @returns { "choice_feature_based", "cost_function", "not_pairwise_decomposible" }.
	std::vector< std::string >
	get_present_data_representation_properties() const override;

	/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
	/// non-exhaustive list.
	/// @returns { "pairwise_decomposible" }.
	std::vector< std::string >
	get_absent_data_representation_properties() const override;

	/// @brief This class is pure virtual, and does not define the name function.
	std::string class_name() const override = 0;

	/// @brief This class is pure virtual, and does not define the namespace function.
	std::string class_namespace() const override = 0;

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////


public:

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
	add_choice_feature_by_absolute_node_index(
		masala::base::Size const absolute_node_index,
		masala::base::Size const choice_index,
		masala::base::Size const min_connections_to_satisfy_feature,
		masala::base::Size const max_connections_to_satisfy_feature,
		masala::base::Size const feature_connection_offset
	);

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
	declare_features_for_node_choices(
		masala::base::Size const absolute_node_index,
		std::vector< std::vector< std::pair< masala::base::Size, masala::base::Size > > > const & min_and_max_connections_by_choice_and_feature
	);

	/// @brief Increment all choices at a specified set of nodes.
	/// @details This can only be called prior to object finalization.  Locks mutex (i.e. threadsafe).
	/// If node or choices have not yet been declared, this function throws.
	/// @param[in] offset_increments A map whose key is the absolute node index, of vectors indexed by
	/// choice index, of vectors indexed by feature index, of offset increments.  The offsets of features
	/// will be increased by these increments.
	void
	increment_offsets(
		std::unordered_map< masala::base::Size, std::vector< std::vector< masala::base::Size > > > const & offset_increments
	);

	/// @brief For all choices at a given node, increment the offsets.
	/// @details This can only be called prior to object finalization.  Locks mutex (i.e. threadsafe).
	/// If node or choices have not yet been declared, this function throws.
	///
	/// @param[in] absolute_node_index The index of the node for which we are updating choices.
	/// @param[in] offset_increments The amount by which we are incrementing the choices, provided as
	/// a vector indexed by choice index of vectors indexed by choice feature index.  Any choices or features
	/// not yet declared trigger an exception.
	void
	increment_offsets_at_node(
		masala::base::Size const absolute_node_index,
		std::vector< std::vector< masala::base::Size > > const & offset_increments
	);

	/// @brief Given a node and a choice, add node/choice pairs that satisfy its features.
	/// @details The node and choice and features must already have been added, or else this throws.  This function
	/// is threadsafe (i.e. it locks the mutex), but can only be called before this object is finalized.
	///
	/// @param[in] absolute_node_index The node for which we are adding feature connections.
	/// @param[in] choice_index The choice for which we are adding feature connections.
	/// @param[in] connecting_node_connections_by_feature A vector indexed by feature index for the node and choice given by
	/// absolute_node_index and choice_index, pointing to maps indexed by other node/choice pairs, in turn pointing to
	/// the number of connections that this feature makes to those node/choice pairs.  The number of connections to those
	/// node/choice pairs will be incremented by this amount, or, if there are no connections to those node/choice pairs,
	/// will be set to this amount.
	void
	add_connecting_node_choices_for_features_of_node_choice(
		masala::base::Size const absolute_node_index,
		masala::base::Size const choice_index,
		std::vector< std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > > const & connecting_node_connections_by_feature
	);

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
	add_connecting_node_choices_for_features_of_node_choices(
		masala::base::Size const absolute_node_index,
		std::vector< std::vector< std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > > > const & connecting_node_connections_by_choice_and_feature
	);

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
	add_connecting_node_choices_for_features_of_nodes_choices(
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
	);

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Given a selection of choices at variable nodes, compute the cost function.
	/// @note No mutex-locking is performed!
	masala::base::Real
	compute_cost_function(
		std::vector< masala::base::Size > const & candidate_solution
	) const override;

	/// @brief Given an old selection of choices at variable nodes and a new selection,
	/// compute the cost function difference.
	/// @note No mutex-locking is performed!
	masala::base::Real
	compute_cost_function_difference(
		std::vector< masala::base::Size > const & candidate_solution_old,
		std::vector< masala::base::Size > const & candidate_solution_new
	) const override;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Given a selection of choices at variable nodes, compute the number of unsatisfied features.
	/// @note No mutex-locking is performed, and the result is not multiplied by the weight.
	masala::base::Size
	protected_compute_cost_function_no_weight(
		std::vector< masala::base::Size > const & candidate_solution
	) const;

	/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
	/// @param[in] variable_node_indices A list of all of the absolute node indices
	/// for nodes that have more than one choice, indexed by variable node index.
	/// @details The base class function simply marks this object as finalized.  Should
	/// be overridden, and overrides should call parent class protected_finalize().
	void
	protected_finalize(
		std::vector< masala::base::Size > const & variable_node_indices
	) override;

	/// @brief Override of assign_mutex_locked().  Calls parent function.
	/// @details Throws if src is not a SumOfUnsatisfiedChoiceFeaturesCostFunction.
	void assign_mutex_locked( CostFunction const & src ) override;

	/// @brief Make this object fully independent.  Assumes mutex was already locked.
	/// Should be called by overrides.
	void
	make_independent_mutex_locked() override;

private:

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
	increment_offsets_at_node_mutex_locked(
		masala::base::Size const absolute_node_index,
		std::vector< std::vector< masala::base::Size > > const & offset_increments
	);

	/// @brief Given a node and a choice, add node/choice pairs that satisfy its features.
	/// @details The node and choice and features must already have been added, or else this throws.  This
	/// version is used internally, and assumes that the mutex has already been locked.
	///
	/// @param[in] absolute_node_index The node for which we are adding feature connections.
	/// @param[in] choice_index The choice for which we are adding feature connections.
	/// @param[in] connecting_node_connections_by_feature A vector indexed by feature index for the node and choice given by
	/// absolute_node_index and choice_index, pointing to maps indexed by other node/choice pairs, in turn pointing to
	/// the number of connections that this feature makes to those node/choice pairs.  The number of connections to those
	/// node/choice pairs will be incremented by this amount, or, if there are no connections to those node/choice pairs,
	/// will be set to this amount.
	void
	add_connecting_node_choices_for_features_of_node_choice_mutex_locked(
		masala::base::Size const absolute_node_index,
		masala::base::Size const choice_index,
		std::vector< std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > > const & connecting_node_connections_by_feature
	);

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The features, indexed by absolute node index and choice index.
	/// @details This is used only during setup, prior to finalization.  Access is mutex-controlled.
	std::unordered_map<
		std::pair< masala::base::Size, masala::base::Size >,
		std::vector< ChoiceFeatureSP >,
		masala::base::size_pair_hash
	> choice_features_by_absolute_node_and_choice_;

	/// @brief The features, indexed by variable node index and choice index.
	/// @details This is used only during the run, following finalization.  Access is read-only and
	/// not mutex-controlled.  The shared pointers continue to reside in the
	/// choice_features_by_absolute_node_and_choice_ map; this uses raw pointers.
	std::unordered_map<
		std::pair< masala::base::Size, masala::base::Size >,
		std::vector< ChoiceFeature const * >,
		masala::base::size_pair_hash
	> choice_features_by_variable_node_and_choice_;

	/// @brief The fixed features, indexed by absolute node index and choice index.
	/// @details This is used only during the run, following finalization.  Access is read-only and
	/// not mutex-controlled.  The shared pointers continue to reside in the
	/// choice_features_by_absolute_node_and_choice_ map; this uses raw pointers.
	/// @note The choice index should always be 0.
	std::unordered_map<
		std::pair< masala::base::Size, masala::base::Size >,
		std::vector< ChoiceFeature const * >,
		masala::base::size_pair_hash
	> fixed_choice_features_by_absolute_node_and_choice_;

    /// @brief A map of variable node indices indexed by absolute node index.
    std::unordered_map< masala::base::Size, masala::base::Size > variable_node_indices_by_absolute_node_index_;

}; // class SumOfUnsatisfiedChoiceFeaturesCostFunction

} // namespace feature_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimization
} // namespace numeric
} // namespace masala

#endif // Masala_src_numeric_optimization_cost_function_network_cost_function_feature_based_SumOfUnsatisfiedChoiceFeaturesCostFunction_hh