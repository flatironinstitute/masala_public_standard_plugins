/*
    Masala
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

/// @file src/file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreter.cc
/// @brief Implementation for a CostFunctionNetworkOptimizer that solves a cost function network problem using Monte
/// Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreter.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.fwd.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace file_interpreters {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////


/// @brief Default constructor.
/// @details Not defaulted since the atomic bool must be set.
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::BinaryCostFunctionNetworkProblemRosettaFileInterpreter() :
	masala::base::file_interpreters::MasalaFileInterpreter(),
	finalized_( false )


/// @brief Copy constructor.
/// @details Needed since we define a mutex.
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::BinaryCostFunctionNetworkProblemRosettaFileInterpreter(
    BinaryCostFunctionNetworkProblemRosettaFileInterpreter const & src
) :
    masala::base::managers::file_interpreter::MasalaFileInterpreter( src )
{
    std::lock_guard< std::mutex > lock( src.file_interpreter_mutex_ );
	protected_assign( src );
    //TODO TODO TODO
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
BinaryCostFunctionNetworkProblemRosettaFileInterpreter &
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::operator=( BinaryCostFunctionNetworkProblemRosettaFileInterpreter const & src ) {
    std::lock( file_interpreter_mutex_, src.file_interpreter_mutex_ );
    std::lock_guard< std::mutex > lock1( file_interpreter_mutex_, std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.file_interpreter_mutex_, std::adopt_lock );
	protected_assign(src);
    return *this;
}

/// @brief Make a copy of this object that's wholly independent.
BinaryCostFunctionNetworkProblemRosettaFileInterpreterSP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::deep_clone() const {
    BinaryCostFunctionNetworkProblemRosettaFileInterpreterSP new_optimizer( masala::make_shared< BinaryCostFunctionNetworkProblemRosettaFileInterpreter >(*this) );
    new_optimizer->make_independent();
    return new_optimizer;
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::make_independent() {
	std::lock_guard< std::mutex > lock( file_interpreter_mutex_ );
	//TODO TODO TODO
}


////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outher vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::get_categories() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return { { "FileInterpreter", "CostFunctionNetworkProblemFileInterpreter", "BinaryCostFunctionNetworkProblemRosettaFileInterpreter" } };
}

/// @brief Get the keywords for this plugin class.
/// @returns { "file_interpreter", "cost_function_network", "problem", "binary" }
std::vector< std::string >
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::get_keywords() const {
    return { "file_interpreter", "cost_function_network", "problem", "binary" };
}

/// @brief All MasalaFileInterpreter subclasses must list the file types that they create.  These are
/// provided as short descriptors (e.g. "protein_data_bank_file").
/// @returns { "cost_function_network_problem_definition_file" }
std::vector< std::string >
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::get_file_descriptors() const {
	return { "cost_function_network_problem_definition_file" };
}

/// @brief All MasalaFileInterpreter subclasses must list the file type extensions that they create.
/// @details Extensions are provided in lower-case (e.g. "pdb").
/// @returns { "txt", "pdb" }
std::vector< std::string >
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::get_file_extensions() const {
	return { "txt", "pdb" };
}

/// @brief Get the class name.
/// @returns "BinaryCostFunctionNetworkProblemRosettaFileInterpreter".
std::string
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::class_name() const {
    return class_name_static();
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
std::string
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::class_namespace() const {
    return class_namespace_static();
}

/// @brief Get the class name (static version).
/// @returns "BinaryCostFunctionNetworkProblemRosettaFileInterpreter".
//static
std::string
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::class_name_static() {
    return "BinaryCostFunctionNetworkProblemRosettaFileInterpreter";
}

/// @brief Get the class namespace (static version).
/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
//static
std::string
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::class_namespace_static() {
    return "standard_masala_plugins::file_interpreters::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using namespace masala::base::api::setter;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( file_interpreter_mutex_ );
    if( api_description_ == nullptr ) {
        MasalaObjectAPIDefinitionSP api_description(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this,
                "This class reads binary cost function network optimization problem files (or packing problem files) written by Rosetta, "
				"and generates a user-selected type of cost function network optimization problem description.",
                false, false
            )
        );

        // Constructors:
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( BinaryCostFunctionNetworkProblemRosettaFileInterpreter, api_description );

        // Setters:
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_cfn_problem_type_to_generate", "Set the name of the cost function network problem description class "
				"to generate.  This can be the short name or the full name (i.e. with or without namespace).  It need only "
				"include namespace if the short name is not unique.  If not set, then an optimizer must be specified instead.",
				"problem_type_name_in", "The name of the cost function network optimization problem subclass to generate.  Namespace is "
				"optional unless the name is not unique.", false, false,
				std::bind( &set_cfn_problem_type_to_generate, this, std::placeholders::_1 )
			)
		);
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_cfn_optimizer_type", "Set the name of the optimizer class that will be used to solve this problem.  This can "
				"be the short name or the full name (i.e. with or without namespace).  It need only include namespace if the short "
				"name is not unique.  If not set, then cost function network problem class must be specified instead.",
				"cfn_optimizer_name_in", "The name of the cost function network optimizer that will be used to solve the problem.  The "
				"problem type generated will be chosen for its suitability for this optimizer.  Namespace is optional unless the name is "
				"not unique.", false, false,
				std::bind( &set_cfn_optimizer_type, this, std::placeholders::_1 )
			)
		);


		// Getters:


        // Work functions:

        // Convert nonconst to const:
        api_description_ = api_description;
    }
    return api_description_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Set the name of the cost function network problem description class to generate.
/// @details This can be the short name or the full name (i.e. with or without namespace).
/// It need only include namespace if the short name is not unique.
/// @note If not set, then an optimizer must be specified instead.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_problem_type_to_generate(
	std::string const & class_name_in
) {
	std::lock_guard< std::mutex > lock( file_interpreter_mutex_ );
	CHECK_OR_THROW_FOR_CLASS( !(finalized_.load()), "set_cfn_problem_type_to_generate", "The problem type to generate must be "
		"set prior to finalization.  This object has already been finalized."
	);
	cfn_problem_class_ = class_name_in;
	if( !cfn_problem_class_.empty() ) {
		check_cfn_problem_class();
	}
}

/// @brief Set the name of the optimizer class that will be used to solve this problem.
/// @details This can be the short name or the full name (i.e. with or without namespace).
/// It need only include namespace if the short name is not unique.
/// @note If not set, then cost function network problem class must be specified instead.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_optimizer_type(
	std::string const & class_name_in
) {
	std::lock_guard< std::mutex > lock( file_interpreter_mutex_ );
	CHECK_OR_THROW_FOR_CLASS( !(finalized_.load()), "set_cfn_optimizer_type", "The optimizer type must be "
		"set prior to finalization.  This object has already been finalized."
	);
	cfn_optimizer_class_ = class_name_in;
	if( !cfn_optimizer_class_.empty() ) {
		check_cfn_optimizer_class();
	}
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assign src to this object.
/// @details Assumes both objects' mutexes have been locked.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::protected_assign(
	BinaryCostFunctionNetworkProblemRosettaFileInterpreter const & src
) {
	finalized_.store( src.finalized_.load() );
	// Deliberately do not copy api description.
	cfn_problem_class_ = src.cfn_problem_class_;
	cfn_optimizer_class_ = src.cfn_optimizer_class_;
}


////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


} // namespace cost_function_network
} // namespace file_interpreters
} // namesapce standard_masala_plugins
