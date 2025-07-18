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

/// @file src/optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction.cc
/// @brief Implementation for a class for FunctionOfIntegerPenaltySumCostFunctions.
/// @details FunctionOfIntegerPenaltySumCostFunctions define a penalty function which is based on the following:
/// - A signed integer penalty is assigned to each choice.
/// - The selected choices' penalties are summed, and a constant is added.
/// - An arbitrary function (I->R) is applied to the sum, and this is returned as the penalty.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/cost_function/FunctionOfIntegerPenaltySumCostFunction.hh>

// STL headers:
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// Numeric headers:
#include <numeric/optimization/cost_function_network/cost_function/CostFunctionScratchSpace.hh>

// Base headers:
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_TwoInput.tmpl.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_ThreeInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_TwoInput.tmpl.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/error/ErrorHandling.hh>
#include <base/utility/container/container_util.tmpl.hh>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {
namespace cost_function {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
FunctionOfIntegerPenaltySumCostFunction::FunctionOfIntegerPenaltySumCostFunction(
	FunctionOfIntegerPenaltySumCostFunction const & src
) :
	Parent()
{
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	FunctionOfIntegerPenaltySumCostFunction::protected_assign( src );
}

// @brief Assignment operator.
FunctionOfIntegerPenaltySumCostFunction &
FunctionOfIntegerPenaltySumCostFunction::operator=(
	FunctionOfIntegerPenaltySumCostFunction const & src
) {
	std::lock( src.data_representation_mutex(), data_representation_mutex() );
	std::lock_guard< std::mutex > lockthis( data_representation_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.data_representation_mutex(), std::adopt_lock );
	protected_assign( src );
	return *this;
}

/// @brief Make a copy of this object.
masala::numeric::optimization::cost_function_network::cost_function::CostFunctionSP
FunctionOfIntegerPenaltySumCostFunction::clone() const {
	return masala::make_shared< FunctionOfIntegerPenaltySumCostFunction >( *this );
}

/// @brief Make a copy of this object that is fully independent.
FunctionOfIntegerPenaltySumCostFunctionSP
FunctionOfIntegerPenaltySumCostFunction::deep_clone() const {
	FunctionOfIntegerPenaltySumCostFunctionSP new_object( std::static_pointer_cast< FunctionOfIntegerPenaltySumCostFunction >( this->clone() ) );
	new_object->make_independent();
	return new_object;
}

////////////////////////////////////////////////////////////////////////////////
// STATIC PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given a penalty function behaviour enum, get the corresponding string.
/*static*/
std::string
FunctionOfIntegerPenaltySumCostFunction::penalty_behaviour_string_from_enum(
	PenaltyFunctionBehaviourOutsideRange const behaviour_enum
) {
	switch( behaviour_enum ) {
		case PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR :
			return "undefined_behaviour";
		case PenaltyFunctionBehaviourOutsideRange::CONSTANT :
			return "constant";
		case PenaltyFunctionBehaviourOutsideRange::LINEAR :
			return "linear";
		case PenaltyFunctionBehaviourOutsideRange::QUADRATIC :
			return "quadratic";
	}
	MASALA_THROW( class_namespace_static() + "::" + class_name_static(),
		"penalty_behaviour_string_from_enum",
		"Invalid penalty function behaviour enum passed to this function!"
	);
	return ""; // Keep older compilers happy.
}

/// @brief Given a penalty function behaviour string, get the corresponding enum.
/// @details Returns PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR if the string is not recognized.
/*static*/
PenaltyFunctionBehaviourOutsideRange
FunctionOfIntegerPenaltySumCostFunction::penalty_behaviour_enum_from_string(
	std::string const & behaviour_string
) {
	using masala::base::Size;
	for( Size i(1); i <= static_cast<Size>(PenaltyFunctionBehaviourOutsideRange::NUM_BEHAVIOURS); ++i ) {
		if( behaviour_string == penalty_behaviour_string_from_enum( static_cast< PenaltyFunctionBehaviourOutsideRange >(i) ) ) {
			return static_cast< PenaltyFunctionBehaviourOutsideRange >(i);
		}
	}
	return PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR;
}

/// @brief Get all allowed behaviours as a comma-separated list.
/*static*/
std::string
FunctionOfIntegerPenaltySumCostFunction::list_penalty_behaviours() {
	using masala::base::Size;
	std::ostringstream ss;
	for( Size i(1); i <= static_cast<Size>(PenaltyFunctionBehaviourOutsideRange::NUM_BEHAVIOURS); ++i ) {
		if( i > 1 ) {
			ss << ", ";
			if( i > 2 && i == static_cast<Size>(PenaltyFunctionBehaviourOutsideRange::NUM_BEHAVIOURS) ) {
				ss << " and ";
			}
			ss << penalty_behaviour_string_from_enum( static_cast< PenaltyFunctionBehaviourOutsideRange >(i) );
		}
	}
	return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { { "CostFunction" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
FunctionOfIntegerPenaltySumCostFunction::get_categories() const {
	return ChoicePenaltySumBasedCostFunction::get_categories();
}

/// @brief Get the category for this MasalaDataRepresentation.
/// @returns { { "CostFunction", "ChoicePenaltySumBasedCostFunction", "IntegerPenaltySumBasedCostFunction", "FunctionOfIntegerPenaltySumCostFunction" } }.
std::vector< std::vector< std::string > >
FunctionOfIntegerPenaltySumCostFunction::get_data_representation_categories() const {
	return std::vector< std::vector< std::string > >{ { "CostFunction", "ChoicePenaltySumBasedCostFunction", "IntegerPenaltySumBasedCostFunction", "FunctionOfIntegerPenaltySumCostFunction" } };
}

/// @brief Get the keywords for this MasalaDataRepresentation.
/// @returns { "optimization_problem", "cost_function", "numeric", "choice_penalty_sum_based", "not_pairwise_decomposible", "function", "integer" }
std::vector< std::string >
FunctionOfIntegerPenaltySumCostFunction::get_data_representation_keywords() const {
	std::vector< std::string > outvec( Parent::get_data_representation_keywords() );
	outvec.push_back( "function" );
	outvec.push_back( "integer" );
	return outvec;
}

/// @brief Get the non-exhaustive list of engines with which this MasalaDataRepresentation
/// is compatible.
/// @returns {"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"}.
std::vector< std::string >
FunctionOfIntegerPenaltySumCostFunction::get_compatible_masala_engines() const {
	return std::vector< std::string >{"standard_masala_plugins::optimizers::cost_function_network::MonteCarloCostFunctionNetworkOptimizer"};
}

/// @brief Get the properties of this MasalaDataRepresentation.
/// @returns { "choice_penalty_sum_based", "cost_function", "not_pairwise_decomposible", "function", "integer" }.
std::vector< std::string >
FunctionOfIntegerPenaltySumCostFunction::get_present_data_representation_properties() const {
	return std::vector< std::string >{ "choice_penalty_sum_based", "cost_function", "not_pairwise_decomposible", "function", "integer" };
}

/// @brief Get the absent properties of this MasalaDataRepresentation.  This is of course a
/// non-exhaustive list.
/// @returns { "pairwise_decomposible" }.
std::vector< std::string >
FunctionOfIntegerPenaltySumCostFunction::get_absent_data_representation_properties() const {
	return std::vector< std::string >{ "pairwise_decomposible" };
}

/// @brief Get the keywords for this plugin class.  Default for all
/// optimization problems; may be overridden by derived classes.
/// @returns { "optimization_problem", "cost_function", "numeric", "choice_penalty_sum_based", "not_pairwise_decomposible", "quadratic", "squared" }
std::vector< std::string >
FunctionOfIntegerPenaltySumCostFunction::get_keywords() const {
	std::vector< std::string > outvec( Parent::get_keywords() );
	outvec.push_back( "function" );
	outvec.push_back( "integer" );
	return outvec;
}

/// @brief Get the name of this class ("FunctionOfIntegerPenaltySumCostFunction").
/// @details Static version.
std::string
FunctionOfIntegerPenaltySumCostFunction::class_name_static() {
	return "FunctionOfIntegerPenaltySumCostFunction";
}

/// @brief Get the name of this class ("FunctionOfIntegerPenaltySumCostFunction").
std::string
FunctionOfIntegerPenaltySumCostFunction::class_name() const {
	return class_name_static();
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizers::cost_function_network::cost_function").
/// @details Static version.
std::string
FunctionOfIntegerPenaltySumCostFunction::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network::cost_function";
}

/// @brief Get the namespace of this class ("standard_masala_plugins::optimizersn::cost_function_network::cost_function").
std::string
FunctionOfIntegerPenaltySumCostFunction::class_namespace() const {
	return class_namespace_static();
}

////////////////////////////////////////////////////////////////////////////////
// GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the penalty function behaviour below the range of values specified.
PenaltyFunctionBehaviourOutsideRange
FunctionOfIntegerPenaltySumCostFunction::get_penalty_function_behaviour_low() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return behaviour_low_;
}

/// @brief Set the penalty function behaviour below the range of values specified, by string.
std::string
FunctionOfIntegerPenaltySumCostFunction::get_penalty_function_behaviour_low_by_string() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return penalty_behaviour_string_from_enum( behaviour_low_ );
}

/// @brief Set the penalty function behaviour above the range of values specified.
PenaltyFunctionBehaviourOutsideRange
FunctionOfIntegerPenaltySumCostFunction::get_penalty_function_behaviour_high() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return behaviour_high_;
}

/// @brief Set the penalty function behaviour above the range of values specified, by string.
std::string
FunctionOfIntegerPenaltySumCostFunction::get_penalty_function_behaviour_high_by_string() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return penalty_behaviour_string_from_enum( behaviour_high_ );
}

/// @brief Access the penalty function.
std::vector< masala::base::Real > const &
FunctionOfIntegerPenaltySumCostFunction::get_penalty_function() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return penalty_values_;
}

/// @brief Get the start of the penalty range.
/// @details If the start of the range is S, and there are N penalty
/// values provided, then the function is defined from S to S + N - 1.
signed long int
FunctionOfIntegerPenaltySumCostFunction::get_penalty_range_start() const {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	return penalty_range_start_;
}

////////////////////////////////////////////////////////////////////////////////
// SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the penalty function behaviour below the range of values specified.
void
FunctionOfIntegerPenaltySumCostFunction::set_penalty_function_behaviour_low(
	PenaltyFunctionBehaviourOutsideRange const behaviour_low
) {
	using masala::base::Size;
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "set_penalty_function_behaviour_low", "The "
		+ class_name() + " instance has already been finalized.  This function can only be called "
		"before finalization."
	);
	CHECK_OR_THROW_FOR_CLASS(
		static_cast< Size >( behaviour_low ) > 0 &&
		static_cast<Size>( behaviour_low_ ) <= static_cast<Size>( PenaltyFunctionBehaviourOutsideRange::NUM_BEHAVIOURS ) &&
		behaviour_low_ != PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR,
		"set_penalty_function_behaviour_low",
		"The behaviour indicated is not recognized!"
	);
	behaviour_low_ = behaviour_low;
	write_to_tracer( "Set penalty function behaviour to " + penalty_behaviour_string_from_enum( behaviour_low_ )
		+ " below penalty range."
	);
}

/// @brief Set the penalty function behaviour below the range of values specified, by string.
void
FunctionOfIntegerPenaltySumCostFunction::set_penalty_function_behaviour_low_by_string(
	std::string const & behaviour_low_string
) {
	PenaltyFunctionBehaviourOutsideRange const behaviour( penalty_behaviour_enum_from_string(behaviour_low_string) );
	CHECK_OR_THROW_FOR_CLASS( behaviour != PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR,
		"set_penalty_function_behaviour_low_by_string",
		"Could not interpret \"" + behaviour_low_string + "\" as an allowed behaviour.  Allowed behaviours "
		"are " + list_penalty_behaviours() + "."
	);
	set_penalty_function_behaviour_low( behaviour );
}

/// @brief Set the penalty function behaviour above the range of values specified.
void
FunctionOfIntegerPenaltySumCostFunction::set_penalty_function_behaviour_high(
	PenaltyFunctionBehaviourOutsideRange const behaviour_high
) {
	using masala::base::Size;
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "set_penalty_function_behaviour_high", "The "
		+ class_name() + " instance has already been finalized.  This function can only be called "
		"before finalization."
	);
	CHECK_OR_THROW_FOR_CLASS(
		static_cast< Size >( behaviour_high ) > 0 &&
		static_cast<Size>( behaviour_high_ ) <= static_cast<Size>( PenaltyFunctionBehaviourOutsideRange::NUM_BEHAVIOURS ) &&
		behaviour_high_ != PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR,
		"set_penalty_function_behaviour_high",
		"The behaviour indicated is not recognized!"
	);
	behaviour_high_ = behaviour_high;
	write_to_tracer( "Set penalty function behaviour to " + penalty_behaviour_string_from_enum( behaviour_high_ )
		+ " above penalty range."
	);
}

/// @brief Set the penalty function behaviour above the range of values specified, by string.
void
FunctionOfIntegerPenaltySumCostFunction::set_penalty_function_behaviour_high_by_string(
	std::string const & behaviour_high_string
) {
	PenaltyFunctionBehaviourOutsideRange const behaviour( penalty_behaviour_enum_from_string(behaviour_high_string) );
	CHECK_OR_THROW_FOR_CLASS( behaviour != PenaltyFunctionBehaviourOutsideRange::UNDEFINED_BEHAVIOUR,
		"set_penalty_function_behaviour_high_by_string",
		"Could not interpret \"" + behaviour_high_string + "\" as an allowed behaviour.  Allowed behaviours "
		"are " + list_penalty_behaviours() + "."
	);
	set_penalty_function_behaviour_high( behaviour );
}

/// @brief Set penalty function in a given range of values.
void
FunctionOfIntegerPenaltySumCostFunction::set_penalty_function(
	std::vector< masala::base::Real > const & penalty_function_in
) {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "set_penalty_function", "The "
		+ class_name() + " instance has already been finalized.  This function can only be called "
		"before finalization."
	);
	CHECK_OR_THROW_FOR_CLASS( !penalty_function_in.empty(), "set_penalty_function", "The "
		"input penalty function was an empty vector.  At least one entry is required!"
	);
	penalty_values_ = penalty_function_in;
}

/// @brief Set the value at which the penalty range starts.
/// @details If the start of the range is S, and there are N penalty
/// values provided, then the function is defined from S to S + N - 1.
void
FunctionOfIntegerPenaltySumCostFunction::set_penalty_range_start(
	signed long int const range_start
) {
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	CHECK_OR_THROW_FOR_CLASS( !protected_finalized(), "set_penalty_range_start", "The "
		+ class_name() + " instance has already been finalized.  This function can only be called "
		"before finalization."
	);
	penalty_range_start_ = range_start;
	write_to_tracer( "Set penalty range start to " + std::to_string(penalty_range_start_) + "." );
}

////////////////////////////////////////////////////////////////////////////////
// WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Given a selection of choices at variable nodes, compute the cost function.
/// @details This version computes the sum of the selected choices plus a constant,
/// then squares the result.
/// @note No mutex-locking is performed!  The scratch_space pointer should be null.
masala::base::Real
FunctionOfIntegerPenaltySumCostFunction::compute_cost_function(
	std::vector< masala::base::Size > const & candidate_solution,
#ifndef NDEBUG
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * scratch_space
#else
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * /*scratch_space*/
#endif
) const {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( scratch_space == nullptr, "compute_cost_function", "Expected a null pointer for the scratch space, but got a pointer to a " + scratch_space->class_name() + " object." );
	signed long const sum( Parent::protected_compute_cost_function_no_weight( candidate_solution ) );
	// write_to_tracer( "***** [" + masala::base::utility::container::container_to_string( candidate_solution, "," ) + "] " + std::to_string(sum) + " "  + std::to_string( protected_weight() * function_of_sum( sum ) ) + " *****" ); // DELETE ME
	return protected_weight() * function_of_sum( sum );
}

/// @brief Given an old selection of choices at variable nodes and a new selection,
/// compute the cost function difference.
/// @details This version computes the sum of the old selected choices plus a constant,
/// then squares the result.  It repeats this for the new selected choices, then returns
/// the difference.
/// @note No mutex-locking is performed!  The scratch_space pointer should be null.
masala::base::Real
FunctionOfIntegerPenaltySumCostFunction::compute_cost_function_difference(
	std::vector< masala::base::Size > const & candidate_solution_old,
	std::vector< masala::base::Size > const & candidate_solution_new,
#ifndef NDEBUG
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * scratch_space
#else
	masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * /*scratch_space*/
#endif
) const {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( scratch_space == nullptr, "compute_cost_function_difference", "Expected a null pointer for the scratch space, but got a pointer to a " + scratch_space->class_name() + " object." );
	signed long const oldsum( Parent::protected_compute_cost_function_no_weight( candidate_solution_old ) );
	signed long const newsum( Parent::protected_compute_cost_function_no_weight( candidate_solution_new ) );
	return protected_weight() * ( function_of_sum( newsum ) - function_of_sum( oldsum ) );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC INTERFACE DEFINITION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get a description of the API of this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
FunctionOfIntegerPenaltySumCostFunction::get_api_definition() {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::base::api;
	std::lock_guard< std::mutex > lock( data_representation_mutex() );
	if( api_definition_mutex_locked() == nullptr ) {
		
		MasalaObjectAPIDefinitionSP api_def(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this, "A cost function which sums the penalties of the individual choices that were selected for "
				"a given solution, then applies a nonlinear function to the result.  The individual choices' penalties "
				"are signed integers, and the final function maps integers to real numbers.", false, false
			)
		);

		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( FunctionOfIntegerPenaltySumCostFunction, api_def );

		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput<bool> >(
				"finalized", "Has the data-entry phase ended and the object been locked?",
				"finalized", "True if we have finalized this object, false otherwise.",
				false, false, std::bind( &FunctionOfIntegerPenaltySumCostFunction::finalized, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< std::string > >(
				"get_penalty_function_behaviour_low_by_string", "Get the penalty function behaviour below the range of values provided.",
				"behaviour", "The penalty function behaviour below the range of penalties proivded.  Available outputs are: "
				+ list_penalty_behaviours() + ".", false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::get_penalty_function_behaviour_low_by_string, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< std::string > >(
				"get_penalty_function_behaviour_high_by_string", "Get the penalty function behaviour above the range of values provided.",
				"behaviour", "The penalty function behaviour above the range of penalties proivded.  Available outputs are: "
				+ list_penalty_behaviours() + ".", false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::get_penalty_function_behaviour_high_by_string, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< std::vector< Real > const & > >(
				"get_penalty_function", "Get the values of the penalty function in the range provided.",
				"penalty_function_values", "The values of the penalty function in the range proivded.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::get_penalty_function, this )
			)
		);
		api_def->add_getter(
			masala::make_shared< getter::MasalaObjectAPIGetterDefinition_ZeroInput< signed long int > >(
				"get_penalty_range_start", "Get the start of the range over which penalty values are defined.  "
				"(If the start of the range is S, and there are N penalty values provided, then the function "
				"is defined from S to S + N - 1.)",
				"penalty_range_start", "The start of the range over which penalty values are defined.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::get_penalty_range_start, this )
			)
		);


		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_TwoInput< Size, std::vector< signed long int > const & > >(
				"set_penalties_for_all_choices_at_node", "Set the penalties for all of the choices at one node.",
				"absolute_node_index", "The absolute index of the node for which we're setting penalties.",
				"penalties_by_choice_index", "The penalties for all of the choices, indexed by choice index.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_penalties_for_all_choices_at_node, this, std::placeholders::_1, std::placeholders::_2 )
			)
		);
		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_ThreeInput< Size, Size, signed long int > >(
				"set_penalty_for_choice_at_node", "Set the penalty for a given choice at one node.",
				"absolute_node_index", "The absolute index of the node for which we're setting a penalty.",
				"choice_index", "The index of the choice at this node for which we're setting a penalty.",
				"penalty_value", "The penalty value for this choice at this node.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_penalty_for_choice_at_node, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
			)
		);
		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_OneInput< Real > >(
				"set_weight", "Set a multiplier for this cost function.",
				"weight_in", "The multiplier, a factor by which the computed cost function is always multiplied.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_weight, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_penalty_function_behaviour_low_by_string", "Set the penalty function behaviour below the range "
				"of penalty values specified.", "behaviour_in", "The penalty function behaviour below the range of "
				"penalties specified.  Available options are: " + list_penalty_behaviours() + ".",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_penalty_function_behaviour_low_by_string, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_penalty_function_behaviour_high_by_string", "Set the penalty function behaviour above the range "
				"of penalty values specified.", "behaviour_in", "The penalty function behaviour above the range of "
				"penalties specified.  Available options are: " + list_penalty_behaviours() + ".",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_penalty_function_behaviour_high_by_string, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_OneInput< std::vector< Real > const & > >(
				"set_penalty_function", "Set the penalty function values from the starting value up to a user-desired "
				"number of values.", "penalty_values_in", "The penalty function values.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_penalty_function, this, std::placeholders::_1 )
			)
		);
		api_def->add_setter(
			masala::make_shared< setter::MasalaObjectAPISetterDefinition_OneInput< signed long int > >(
				"set_penalty_range_start", "Set the start of the range over which penalty values are defined.  "
				"(If the start of the range is S, and there are N penalty values provided, then the function "
				"is defined from S to S + N - 1.)",
				"penalty_range_start", "The start of the range over which penalty values are defined.",
				false, false,
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::set_penalty_range_start, this, std::placeholders::_1 )
			)
		);

		api_def->add_work_function(
			masala::make_shared< work_function::MasalaObjectAPIWorkFunctionDefinition_OneInput< void, std::vector< Size > const & > >(
				"finalize", "Indicate that all data input is complete.", false, false, false, false,
				"variable_node_indices", "A list of all of the absolute node indices for nodes that have more than "
				"one choice, indexed by variable node index.", "void", "No output.",
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::finalize, this, std::placeholders::_1 )
			)
		);

		work_function::MasalaObjectAPIWorkFunctionDefinitionSP compute_fxn(
			masala::make_shared< work_function::MasalaObjectAPIWorkFunctionDefinition_TwoInput< Real, std::vector< Size > const &, masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * > >(
				"compute_cost_function", "Given a selection of choices at variable nodes, compute the cost function.  This version "
				"computes some function of the sum of integer penalties for selected choices.", true, false, false, false,
				"candidate_solution", "A candidate solution, as a vector of choices for each variable position (i.e. position with "
				"more than one choice).",
				"scratch_space", "A pointer to scratch space for accelerating this calculation, or nullptr.  Should be nullptr for this class.",
				"score", "The output score: the sum of the penalties for the selected choices, plus a constant "
				"offset, all squared.", std::bind( &FunctionOfIntegerPenaltySumCostFunction::compute_cost_function, this, std::placeholders::_1, std::placeholders::_2 )
			)
		);
		compute_fxn->set_triggers_no_mutex_lock();
		api_def->add_work_function( compute_fxn );

		work_function::MasalaObjectAPIWorkFunctionDefinitionSP compute_diff_fxn(
			masala::make_shared< work_function::MasalaObjectAPIWorkFunctionDefinition_ThreeInput< Real, std::vector< Size > const &, std::vector< Size > const &, masala::numeric::optimization::cost_function_network::cost_function::CostFunctionScratchSpace * > >(
				"compute_cost_function_difference", "Given an old selection of choices at variable nodes and a new selection, compute "
				"the cost function difference.  This version computes some function of the sum of integer penalties for selected "
				"choices.  It repeats this for the new selected choices, then returns the difference.",
				true, false, false, false,
				"candidate_solution_old", "The old candidate solution, as a vector of choices for each variable position.",
				"candidate_solution_new", "The new candidate solution, as a vector of choices for each variable position.",
				"scratch_space", "A pointer to scratch space for accelerating this calculation, or nullptr.  Should be nullptr for this class.",
				"score", "The output score: the difference of the function of the sum of the integer penalties for the "
				"selected choices.",
				std::bind( &FunctionOfIntegerPenaltySumCostFunction::compute_cost_function_difference, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
			)
		);
		compute_diff_fxn->set_triggers_no_mutex_lock();
		api_def->add_work_function( compute_diff_fxn );
	
		api_definition_mutex_locked() = api_def;
	}
	return api_definition_mutex_locked();
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Indicate that all data input is complete.  Performs no mutex-locking.
/// @param[in] variable_node_indices A list of all of the absolute node indices
/// for nodes that have more than one choice, indexed by variable node index.
/// @details The base class function simply marks this object as finalized.  Should
/// be overridden, and overrides should call parent class protected_finalize().
void
FunctionOfIntegerPenaltySumCostFunction::protected_finalize(
	std::vector< masala::base::Size > const & variable_node_indices
) {
	using masala::base::Size;
	using masala::base::Real;

	if( behaviour_high_ == PenaltyFunctionBehaviourOutsideRange::QUADRATIC ||
		behaviour_low_ == PenaltyFunctionBehaviourOutsideRange::QUADRATIC
	) {
		CHECK_OR_THROW_FOR_CLASS( penalty_values_.size() >= 2, "protected_finalize", "If a quadratic function "
			"is used before or after the defined penalty function value range, then at least two penalty "
			"values must be defined.  (A parabola is fitted to have its vertex on the second-to-terminal point "
			"and to pass through the terminal point.)"
		);
	} else if ( behaviour_high_ == PenaltyFunctionBehaviourOutsideRange::LINEAR ||
		behaviour_low_ == PenaltyFunctionBehaviourOutsideRange::LINEAR
	) {
		CHECK_OR_THROW_FOR_CLASS( penalty_values_.size() >= 2, "protected_finalize", "If a linear function "
			"is used before or after the defined penalty function value range, then at least two penalty "
			"values must be defined.  (A line is fitted to two terminal points to continuously extend them.)"
		);
	} else {
		CHECK_OR_THROW_FOR_CLASS( penalty_values_.size() >= 1, "protected_finalize", "At least one penalty value "
			"must be provided."
		);
	}

	// Determine the parameters for the tail functions.
	fit_tail_functions_mutex_locked();

	Parent::protected_finalize( variable_node_indices );

	// DELETE THE FOLLOWING: FOR DEBUGGING ONLY.
	// std::stringstream ss;
	// ss << "COPIED:\n"
	// 	<< "a_high=" << a_high_ << "\n"
	// 	<< "b_high=" << b_high_ << "\n"
	// 	<< "c_high=" << c_high_ << "\n"
	// 	<< "a_low=" << a_low_ << "\n"
	// 	<< "b_low=" << b_low_ << "\n"
	// 	<< "c_low=" << c_low_ << "\n"
	// 	<< "penalty_range_start=" << penalty_range_start_ << "\n"
	// 	<< "penalty_values=[ " << masala::base::utility::container::container_to_string( penalty_values_, ", " ) << " ]\n"
	// 	<< "behaviour_low=" << penalty_behaviour_string_from_enum( behaviour_low_ ) << "\n"
	// 	<< "behaviour_high=" << penalty_behaviour_string_from_enum( behaviour_high_ );
	// write_to_tracer( ss.str() );

}

/// @brief Override of assign_protected_assignmutex_locked().  Calls parent function.
/// @details Throws if src is not a FunctionOfIntegerPenaltySumCostFunction.
void
FunctionOfIntegerPenaltySumCostFunction::protected_assign(
	masala::base::managers::engine::MasalaDataRepresentation const & src
) {
	FunctionOfIntegerPenaltySumCostFunction const * const src_cast_ptr( dynamic_cast< FunctionOfIntegerPenaltySumCostFunction const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Cannot assign a FunctionOfIntegerPenaltySumCostFunction given an input " + src.class_name() + " object!  Object types do not match." );

	penalty_range_start_ = src_cast_ptr->penalty_range_start_;
	penalty_values_ = src_cast_ptr->penalty_values_;
	behaviour_low_ = src_cast_ptr->behaviour_low_;
	behaviour_high_ = src_cast_ptr->behaviour_high_;
	c_high_ = src_cast_ptr->c_high_;
	b_high_ = src_cast_ptr->b_high_;
	a_high_ = src_cast_ptr->a_high_;
	c_low_ = src_cast_ptr->c_low_;
	b_low_ = src_cast_ptr->b_low_;
	a_low_ = src_cast_ptr->a_low_;

	Parent::protected_assign( src );
}

/// @brief Make this object fully independent.  Assumes mutex was already locked.
/// Should be called by overrides.
void
FunctionOfIntegerPenaltySumCostFunction::protected_make_independent() {
	// GNDN
	Parent::protected_make_independent();
}

/// @brief Is this data representation empty?
/// @details Must be implemented by derived classes.  Should return its value && the parent class protected_empty().  Performs no mutex-locking.
/// @returns True if no data have been loaded into this data representation, false otherwise.
/// @note This does not report on whether the data representation has been configured; only whether it has been loaded with data.
bool
FunctionOfIntegerPenaltySumCostFunction::protected_empty() const {
	return c_high_ == 0.0 &&
		c_low_ == 0.0 &&
		b_high_ == 0.0 &&
		b_low_ == 0.0 &&
		a_high_ == 0.0 &&
		a_low_ == 0.0 &&
		Parent::protected_empty();
}

/// @brief Remove the data loaded in this object.  Note that this does not result in the configuration being discarded.
/// @details Must be implemented by derived classes, and should call parent class protected_clear().  Performs no mutex-locking.
void
FunctionOfIntegerPenaltySumCostFunction::protected_clear() {
	c_high_ = 0.0;
	c_low_ = 0.0;
	b_high_ = 0.0;
	b_low_ = 0.0;
	a_high_ = 0.0;
	a_low_ = 0.0;
	Parent::protected_clear();
}

/// @brief Remove the data loaded in this object AND reset its configuration to defaults.
/// @details Must be implemented by derived classes, and should call parent class protected_reset().  Performs no mutex-locking.
void
FunctionOfIntegerPenaltySumCostFunction::protected_reset() {
	protected_clear();
	Parent::protected_reset();
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Compute the tail function outside of the range of penalty values specified.
/// @details This is a static function.
/// @param[in] behaviour The behaviour (constant, linear, or quadratic).
/// @param[in] x The value of the input function.
/// @param[in] a The constant offset.
/// @param[in] b The slope.
/// @param[in] c The quadratic term coefficient.
/// @details The overall equation is cx^2 + bx + a for quadratic, bx+a for linear, a for constant.
/*static*/
masala::base::Real
FunctionOfIntegerPenaltySumCostFunction::compute_outside_range_function(
	PenaltyFunctionBehaviourOutsideRange const behaviour,
	signed long int x,
	masala::base::Real a,
	masala::base::Real b,
	masala::base::Real c
) {
	switch( behaviour ) {
		case PenaltyFunctionBehaviourOutsideRange::CONSTANT :
			return a;
		case PenaltyFunctionBehaviourOutsideRange::LINEAR :
			return b*static_cast< masala::base::Real >(x) + a;
		case PenaltyFunctionBehaviourOutsideRange::QUADRATIC :
			return c*static_cast< masala::base::Real >(x*x) + b*static_cast< masala::base::Real >(x) + a;
		default :
			MASALA_THROW( class_namespace_static() + "::" + class_name_static(),
				"compute_outside_range_function",
				"Undefined penalty value behaviour was specified!"
			);
	}
	return 0.0; // Keep older compilers happy.
}

/// @brief Compute the function that maps I->R.
/// @note Performs no mutex-locking.  Expects a finalized context.
masala::base::Real
FunctionOfIntegerPenaltySumCostFunction::function_of_sum(
	signed long int x
) const {
	DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( protected_finalized(), "function_of_sum", "This function must be called from a finalized context!" );
	if( x < penalty_range_start_ ) {
		return compute_outside_range_function( behaviour_low_, x, a_low_, b_low_, c_low_ );
	} else if ( x >= penalty_range_start_ + static_cast<signed long int>(penalty_values_.size()) ) {
		return compute_outside_range_function( behaviour_high_, x, a_high_, b_high_, c_high_ );
	} else {
		return penalty_values_[ x - penalty_range_start_ ];
	}
	return 0.0; // For older compilers.
}

/// @brief Fit a single tail function.
/// @details This is a static function.
/// @param[in] high If true, we're doing the high end of the range; if false, we're doing the low.
/// @param[in] behaviour The penalty function behaviour (constant, linear, or quadratic).
/// @param[in] start_x The value of x at the start of the range.
/// @param[in] penalty_values The penalty values within the range.  Up to two at the low or high
/// end will be fitted to determine the parameters.
/// @param[out] a The constant offset, fitted by this function (for all behaviours).
/// @param[out] b The slope, fitted by this function (for linear or quadratic).  Will be 0 for constant.
/// @param[out] c The quadratic term coefficient, fitted by this function (for quadratic).  Will be 0 for
/// constant or linear.
/*static*/
void
FunctionOfIntegerPenaltySumCostFunction::fit_tail_function(
	bool const high,
	PenaltyFunctionBehaviourOutsideRange const behaviour,
	signed long int start_x,
	std::vector< masala::base::Real > const & penalty_values,
	masala::base::Real & a,
	masala::base::Real & b,
	masala::base::Real & c
) {
	using masala::base::Size;
	using masala::base::Real;

	switch( behaviour ) {
		case PenaltyFunctionBehaviourOutsideRange::CONSTANT :
		{
			DEBUG_MODE_CHECK_OR_THROW(
				penalty_values.size() > 0,
				class_namespace_static() + "::" + class_name_static(),
				"fit_tail_function",
				"Expected at least one penalty value to be defined before this function is called "
				"for constant fit!"
			);
			a = ( high ? penalty_values[penalty_values.size() - 1] : penalty_values[0] );
			b = 0;
			c = 0;

			CHECK_OR_THROW_FOR_CLASS( !std::isinf(a), "fit_tail_function", "Error!  Variable a is infinity!  (Trying to fit constant penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );
			CHECK_OR_THROW_FOR_CLASS( !std::isinf(b), "fit_tail_function", "Error!  Variable b is infinity!  (Trying to fit constant penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );
			CHECK_OR_THROW_FOR_CLASS( !std::isinf(c), "fit_tail_function", "Error!  Variable c is infinity!  (Trying to fit constant penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );

			break;
		}
		case PenaltyFunctionBehaviourOutsideRange::LINEAR :
		{
			// y = b x + a, passing through (x1, y1) and (x2, y2)
			// y1 = b x1 + a; y2 = b x2 + a
			// y1 - y2 = b( x1 - x2 ) --> b = (y1 - y2)/(x1 - x2)
			// a = y1 - b x1
			DEBUG_MODE_CHECK_OR_THROW(
				penalty_values.size() > 1,
				class_namespace_static() + "::" + class_name_static(),
				"fit_tail_function",
				"Expected at least two penalty values to be defined before this function is called "
				"for linear fit!"
			);
			Size const x1_index( high ? penalty_values.size() - 1 : 0 );
			Size const x2_index( high ? x1_index - 1 : x1_index + 1 );
			signed long int const x1( high ? start_x + static_cast< signed long int >(x1_index) : start_x );
			signed long int const x2( high ? x1 - 1 : x1 + 1 );
			c = 0;
			b = (penalty_values[x1_index] - penalty_values[x2_index])/static_cast<Real>(x1 - x2);
			a = penalty_values[x1_index] - ( b * static_cast<Real>(x1) );

			CHECK_OR_THROW_FOR_CLASS( !std::isinf(a), "fit_tail_function", "Error!  Variable a is infinity!  (Trying to fit linear penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );
			CHECK_OR_THROW_FOR_CLASS( !std::isinf(b), "fit_tail_function", "Error!  Variable b is infinity!  (Trying to fit linear penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );
			CHECK_OR_THROW_FOR_CLASS( !std::isinf(c), "fit_tail_function", "Error!  Variable c is infinity!  (Trying to fit linear penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );

			break;
		}
		case PenaltyFunctionBehaviourOutsideRange::QUADRATIC :
		{
			// We want a parabola passing through (x1,y1), (x2,y2), and (x3,y3).
			// y1 = a + b x1 + c x1^2
			// y2 = a + b x2 + c x2^2
			// y3 = a + b x3 + c x3^2
			// y1 - y2 = ( b + c ( x1 + x2 ) ) ( x1 - x2 )
			// ( y1 - y2 ) / ( x1 - x2 ) - c ( x1 + x2 ) = b
			// Similarly ( y1 - y3 ) / ( x1 - x3 ) - c ( x1 + x3 ) = b
			// Therefore, ( y1 - y2 ) / ( x1 - x2 ) - c ( x1 + x2 ) = ( y1 - y3 ) / ( x1 - x3 ) - c ( x1 + x3 )
			// c ( x3 - x2 ) = ( y1 - y3 ) / ( x1 - x3 ) - ( y1 - y2 ) / ( x1 - x2 )
			// c = ( ( y1 - y3 ) / ( x1 - x3 ) - ( y1 - y2 ) / ( x1 - x2 ) ) / ( x3 - x2 ) ****
			// b = ( y1 - y2 ) / ( x1 - x2 ) - c ( x1 + x2 ) ****
			// a = y1 - b x1 - c x1^2 ****

			DEBUG_MODE_CHECK_OR_THROW(
				penalty_values.size() > 2,
				class_namespace_static() + "::" + class_name_static(),
				"fit_tail_function",
				"Expected at least three penalty values to be defined before this function is called "
				"for quadratic fit!"
			);
			Size const x1_index( high ? penalty_values.size() - 1 : 0 );
			Size const x2_index( high ? x1_index - 1 : x1_index + 1 );
			Size const x3_index( high ? x2_index - 1 : x2_index + 1 );
			Real const x1( static_cast<Real>( high ? start_x + static_cast< signed long int >(x1_index) : start_x ) );
			Real const x2( static_cast<Real>( high ? x1 - 1 : x1 + 1 ) );
			Real const x3( static_cast<Real>( high ? x2 - 1 : x2 + 1 ) );
			Real const y1( penalty_values[x1_index] );
			Real const y2( penalty_values[x2_index] );
			Real const y3( penalty_values[x3_index] );

			c = ( ( y1 - y3 ) / ( x1 - x3 ) - ( y1 - y2 ) / ( x1 - x2 ) ) / ( x3 - x2 );
			b = ( y1 - y2 ) / ( x1 - x2 ) - c * ( x1 + x2 );
			a = y1 - b * x1 - c * x1 * x1;

			CHECK_OR_THROW_FOR_CLASS( !std::isinf(a), "fit_tail_function", "Error!  Variable a is infinity!  (Trying to fit quadratic penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );
			CHECK_OR_THROW_FOR_CLASS( !std::isinf(b), "fit_tail_function", "Error!  Variable b is infinity!  (Trying to fit quadratic penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );
			CHECK_OR_THROW_FOR_CLASS( !std::isinf(c), "fit_tail_function", "Error!  Variable c is infinity!  (Trying to fit quadratic penalty " + ( high ? std::string("above" ) : std::string("below") ) + " user-specified range.)" );

			break;
		}
		default :
		{
			MASALA_THROW( class_namespace_static() + "::" + class_name_static(),
				"fit_tail_function",
				"Undefined penalty value behaviour was specified!"
			);
		}
	}
}

/// @brief Determine the parameters of the tail functions.
/// @note Performs no mutex-locking.  Called from protected_finalize().
void
FunctionOfIntegerPenaltySumCostFunction::fit_tail_functions_mutex_locked() {
	fit_tail_function( false, behaviour_low_, penalty_range_start_, penalty_values_, a_low_, b_low_, c_low_ );
	fit_tail_function( true, behaviour_high_, penalty_range_start_, penalty_values_, a_high_, b_high_, c_high_ );
}

} // namespace cost_function
} // namespace cost_function_network
} // namespace optimizers
} // namespace standard_masala_plugins
