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

/// @file src/file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemFileInterpreter.cc
/// @brief Implementation for a CostFunctionNetworkOptimizer that solves a cost function network problem using Monte
/// Carlo methods.
/// @details This performs a Metropolis-Hastings Monte Carlo search of node setting space, where each move is to
/// pick a node at random and change its setting at random, compute the change in overall energy or score, and
/// accept or reject the move based on the difference in energy and the Metropolis criterion.
/// @note If the annealing schedule used ramps temperature, this does simulated annealing.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemFileInterpreter.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>

// STL headers:
#include <vector>
#include <string>

namespace standard_masala_plugins {
namespace file_interpreters {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
BinaryCostFunctionNetworkProblemFileInterpreter::BinaryCostFunctionNetworkProblemFileInterpreter(
    BinaryCostFunctionNetworkProblemFileInterpreter const & src
) :
    masala::base::managers::file_interpreter::MasalaFileInterpreter( src )
{
    std::lock_guard< std::mutex > lock( src.file_interpreter_mutex_ );
    //TODO TODO TODO
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
BinaryCostFunctionNetworkProblemFileInterpreter &
BinaryCostFunctionNetworkProblemFileInterpreter::operator=( BinaryCostFunctionNetworkProblemFileInterpreter const & src ) {
    std::lock( file_interpreter_mutex_, src.file_interpreter_mutex_ );
    std::lock_guard< std::mutex > lock1( file_interpreter_mutex_, std::adopt_lock );
    std::lock_guard< std::mutex > lock2( src.file_interpreter_mutex_, std::adopt_lock );
    // TODO TODO TODO
	masala::base::managers::file_interpreter::MasalaFileInterpreter::operator=( src );
    return *this;
}

/// @brief Make a copy of this object that's wholly independent.
BinaryCostFunctionNetworkProblemFileInterpreterSP
BinaryCostFunctionNetworkProblemFileInterpreter::deep_clone() const {
    BinaryCostFunctionNetworkProblemFileInterpreterSP new_optimizer( masala::make_shared< BinaryCostFunctionNetworkProblemFileInterpreter >(*this) );
    new_optimizer->make_independent();
    return new_optimizer;
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
BinaryCostFunctionNetworkProblemFileInterpreter::make_independent() {
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
BinaryCostFunctionNetworkProblemFileInterpreter::get_categories() const {
    using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return { { "FileInterpreter", "CostFunctionNetworkProblemFileInterpreter", "BinaryCostFunctionNetworkProblemFileInterpreter" } };
}

/// @brief Get the keywords for this plugin class.
/// @returns { "file_interpreter", "cost_function_network", "problem", "binary" }
std::vector< std::string >
BinaryCostFunctionNetworkProblemFileInterpreter::get_keywords() const {
    return { "file_interpreter", "cost_function_network", "problem", "binary" };
}

/// @brief All MasalaFileInterpreter subclasses must list the file types that they create.  These are
/// provided as short descriptors (e.g. "protein_data_bank_file").
/// @returns { "cost_function_network_problem_definition_file" }
std::vector< std::string >
BinaryCostFunctionNetworkProblemFileInterpreter::get_file_descriptors() const {
	return { "cost_function_network_problem_definition_file" };
}

/// @brief All MasalaFileInterpreter subclasses must list the file type extensions that they create.
/// @details Extensions are provided in lower-case (e.g. "pdb").
/// @returns { "txt", "pdb" }
std::vector< std::string >
BinaryCostFunctionNetworkProblemFileInterpreter::get_file_extensions() const {
	return { "txt", "pdb" };
}

/// @brief Get the class name.
/// @returns "BinaryCostFunctionNetworkProblemFileInterpreter".
std::string
BinaryCostFunctionNetworkProblemFileInterpreter::class_name() const {
    return class_name_static();
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
std::string
BinaryCostFunctionNetworkProblemFileInterpreter::class_namespace() const {
    return class_namespace_static();
}

/// @brief Get the class name (static version).
/// @returns "BinaryCostFunctionNetworkProblemFileInterpreter".
//static
std::string
BinaryCostFunctionNetworkProblemFileInterpreter::class_name_static() {
    return "BinaryCostFunctionNetworkProblemFileInterpreter";
}

/// @brief Get the class namespace (static version).
/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
//static
std::string
BinaryCostFunctionNetworkProblemFileInterpreter::class_namespace_static() {
    return "standard_masala_plugins::file_interpreters::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
BinaryCostFunctionNetworkProblemFileInterpreter::get_api_definition() {
    using namespace masala::base::api;
    using namespace masala::base::api::constructor;
    using masala::base::Size;

    std::lock_guard< std::mutex > lock( file_interpreter_mutex_ );
    if( api_description_ == nullptr ) {
        MasalaObjectAPIDefinitionSP api_description(
            masala::make_shared< MasalaObjectAPIDefinition >(
                *this,
                "TODO TODO TODO",
                false, false
            )
        );

        // Constructors:
        ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( BinaryCostFunctionNetworkProblemFileInterpreter, api_description );

        // Setters:


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

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


} // namespace cost_function_network
} // namespace file_interpreters
} // namesapce standard_masala_plugins
