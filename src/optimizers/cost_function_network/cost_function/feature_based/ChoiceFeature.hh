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

/// @file src/optimizers/cost_function_network/cost_function/feature_based/ChoiceFeature.hh
/// @brief Headers for a class for ChoiceFeatures.
/// @details ChoiceFeatures are objects attached to node choices, which can form connections across
/// choices at different nodes.  Each feature has a minimum and maximum number of connections that
/// it must make to be satisfied.
/// @note This class offers thread safety during setup only.  After finalization,
/// it is read-only.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_feature_based_ChoiceFeature_hh
#define Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_feature_based_ChoiceFeature_hh

// Forward declarations:
#include <optimizers/cost_function_network/cost_function/feature_based/ChoiceFeature.fwd.hh>

// Parent header:
#include <base/managers/plugin_module/MasalaPlugin.hh>

// Numeric headers:

// Base headers:
#include <base/types.hh>
#include <base/hash_types.hh>
#include <base/error/ErrorHandling.hh>

// STL headers:
#include <atomic>
#include <mutex>
#include <set>
#include <unordered_map>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {
namespace feature_based {

/// @brief A pure virtual base class for ChoiceFeatures.
/// @brief A class for ChoiceFeatures.
/// @details ChoiceFeatures are objects attached to node choices, which can form connections across
/// choices at different nodes.  Each feature has a minimum and maximum number of connections that
/// it must make to be satisfied.
/// @note This class offers thread safety during setup only.  After finalization,
/// it is read-only.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class ChoiceFeature : public masala::base::managers::plugin_module::MasalaPlugin {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	/// @details Should not be used.  Only for naming functions.
	ChoiceFeature();

	/// @brief Constructor with min and max connections for satisfaction, and
	/// the offset (number of connections from internal satisfaction or background).
	ChoiceFeature(
		masala::base::Size min_connections,
		masala::base::Size max_connections,
		masala::base::Size offset = 0
	);

	/// @brief Copy constructor.
	ChoiceFeature(
		ChoiceFeature const &
	);

	// @brief Assignment operator.
	ChoiceFeature &
	operator=( ChoiceFeature const & );

	/// @brief Destructor.
	~ChoiceFeature() override = default;

	/// @brief Copy this object and return a shared pointer to the copy.
	virtual ChoiceFeatureSP clone() const;

	/// @brief Copy this object and return a fully independent shared pointer to the copy.
	ChoiceFeatureSP deep_clone() const;

	/// @brief Ensure that this object is fully independent.
	virtual void make_independent();

	/// @brief Finalize this object.
	/// @param[in] variable_node_indices_by_absolute_node_index A map of all of the variable node indices
	/// for nodes that have more than one choice, indexed by absolute node index.
	void finalize(
		std::unordered_map< masala::base::Size, masala::base::Size > const & variable_node_indices_by_absolute_node_index
	);

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.  Default for all
	/// cost functions; may be overridden by derived classes.
	/// @returns { { "ChoiceFeature" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outer vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.
	/// @returns { "optimization_problem", "cost_function", "numeric", "choice_feature" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief This returns the class name ("ChoiceFeature").  Static version.
	static
	std::string class_name_static();

	/// @brief This returns the class namespace ("standard_masala_plugins::optimizers::cost_function_network::cost_function::feature_based").
	/// Static version.
	static
	std::string class_namespace_static();

	/// @brief This returns the class name ("ChoiceFeature").
	std::string class_name() const override;

	/// @brief This returns the class namespace ("standard_masala_plugins::optimizers::cost_function_network::cost_function::feature_based").
	std::string class_namespace() const override;

public:

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the minimum number of connections that this feature must have to be satisfied.
	/// @note Assumes finalized.  Throws in debug mode if not finalized.
	masala::base::Size min_connections() const;

	/// @brief Get the maximum number of connections that this feature must have to be satisfied.
	/// @note Assumes finalized.  Throws in debug mode if not finalized.
	masala::base::Size max_connections() const;

	/// @brief Get the offset in the number of connections (the number of connections that are
	/// always satisfied)
	/// @note Assumes finalized.  Throws in debug mode if not finalized.  Performs no mutex locking.
	masala::base::Size offset() const;

	/// @brief Get the number of connections that a particular variable node choice makes to this feature.
	/// @details Returns 0 by default, if the variable node and/or choice are not in the
	/// other_variable_node_choices_that_satisfy_this_ map.  Assumes finalized.  Throws in debug mode if
	/// not finalized.  Performs no mutex locking.  Inlined for speed.
	inline
	masala::base::Size
	n_connections_to_feature_from_node_and_choice(
		masala::base::Size const variable_node_index,
		masala::base::Size const choice_index
	) const {
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( finalized_.load(),
			"n_connections_to_feature_from_node_and_choice",
			"This function must be called from a finalized object only!"
		);
		DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( variable_node_index < other_variable_node_choices_that_satisfy_this_.size(),
			"n_connections_to_feature_from_node_and_choice",
			"The variable node index " + std::to_string( variable_node_index ) + " is out of range."
		);
		std::vector< masala::base::Size > const & connections_by_choice( other_variable_node_choices_that_satisfy_this_[variable_node_index] );
		if( choice_index < connections_by_choice.size() ) {
			return connections_by_choice[choice_index];
		}
		return 0;
	}

	/// @brief Given a particular count of connections to a feature, return true if this feature is satisfied
	/// and false if it is under- or over-satisfied.
	/// @note Assumes finalized.  Throws in debug mode if not finalized.  Performs no mutex locking.
	bool
	is_satisfied(
		masala::base::Size const connection_count
	) const;

public:

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Indicate that a particular choice at another node satisfies this feature.
	/// @details This feature must not be finalized yet.  Threadsafe.
	/// @param other_node_absolute_index The other node index (absolute index, not variable index).
	/// @param other_choice_index The other choice index.
	/// @param n_connections The number of connections that are made from the features of the
	/// other node choice to this feature.
	void
	add_other_node_and_choice_that_satisfies_this(
		masala::base::Size const other_node_absolute_index,
		masala::base::Size const other_choice_index,
    	masala::base::Size const n_connections
	);

	/// @brief Indicate that a particular choice at another node satisfies this feature.  If connections
	/// have already been added for that node and choice, increment them by n_connections.
	/// @details This feature must not be finalized yet.  Threadsafe.
	/// @param other_node_absolute_index The other node index (absolute index, not variable index).
	/// @param other_choice_index The other choice index.
	/// @param n_connections The number of connections that are made from the features of the
	/// other node choice to this feature.  If the other node and choice already have connections,
	/// then this is the amount by which the connection count is incremented.
	void
	increment_other_node_and_choice_that_satisfies_this(
		masala::base::Size const other_node_absolute_index,
		masala::base::Size const other_choice_index,
    	masala::base::Size const n_connections
	);

	/// @brief Increase the offset for this choice.
	/// @param[in] increment The amount by which to increase the offset.  Must be positive.
	void
	increment_offset(
		masala::base::Size const increment
	);

public:

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get an object describing the API for this object.
	/// @details Default implementation returns nullptr.  May be overridden by
	/// derived objects.
	/// @note This is a weak pointer rather than a shared pointer since the
	/// original object is expected to hold on to its API definition (which includes
	/// funciton pointers to the functions of the instance).  Querying whether the
	/// weak pointer can be converted to a shared pointer serves on a check as to
	/// whether it is safe to use the function pointers.  Not ideal, but better than
	/// nothing.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition() override;

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Assign this object based on src.  Assumes that both mutexes have been locked.
	virtual
	void
	protected_assign(
		ChoiceFeature const & src
	);

	/// @brief Finalize this object.  Assumes that mutex has been locked.
	/// @param[in] variable_node_indices_by_absolute_node_index A map of all of the variable node indices
	/// for nodes that have more than one choice, indexed by absolute node index.
	virtual
	void
	protected_finalize(
		std::unordered_map< masala::base::Size, masala::base::Size > const & variable_node_indices_by_absolute_node_index
	);

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
////////////////////////////////////////////////////////////////////////////////

	/// @brief The mutex for this object.
	mutable std::mutex mutex_;

	/// @brief Has this object been finalized?
	std::atomic_bool finalized_;

	/// @brief The API definition for this object.  Nullptr if not yet accessed.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_definition_;

	/// @brief The minimum number of connections.
	masala::base::Size min_connections_ = 0;

	/// @brief The maximum number of connections.
	masala::base::Size max_connections_ = 0;

	/// @brief The offset.  (e.g. The number of hydrogen bonds satisfied to background or
	/// by internal connections.)
	masala::base::Size offset_ = 0;

	/// @brief Choices at other nodes that satisfy this feature, indexed
	/// by absolute node index, mapped to the number of connections that they
	/// make to this feature.
	/// @details Used during setup, then deleted at finalization time.
	std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > other_absolute_node_choices_that_satisfy_this_;

	/// @brief Choices at other nodes that satisfy this feature, indexed
	/// by variable node index and choice index, mapped to the number of connections that they
	/// make to this feature.
	/// @details Used during run, after being produced by finalization step.
	std::vector< std::vector< masala::base::Size > > other_variable_node_choices_that_satisfy_this_;
	//std::unordered_map< std::pair< masala::base::Size, masala::base::Size >, masala::base::Size, masala::base::size_pair_hash > other_variable_node_choices_that_satisfy_this_;

}; // class ChoiceFeature

} // namespace feature_based
} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins

#endif // Standard_Masala_Plugins_src_optimizers_cost_function_network_cost_function_feature_based_ChoiceFeature_hh