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

/// @file src/file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemFileInterpreter.hh
/// @brief Headers for a file interpreter for reading and writing binary cost function network
/// optimization problems.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_file_interpreters_cost_function_network_BinaryCostFunctionNetworkProblemFileInterpreter_hh
#define StandardMaslaPlugins_src_file_interpreters_cost_function_network_BinaryCostFunctionNetworkProblemFileInterpreter_hh

// Forward declarations:
#include <file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemFileInterpreter.fwd.hh>

// Parent header:
#include <numeric_api/base_classes/optimization/cost_function_network/CostFunctionNetworkOptimizer.hh>

// Base headers:
#include <base/types.hh>
#include <base/managers/file_interpreter/MasalaFileInterpreter.hh>
#include <base/api/MasalaObjectAPIDefinition.fwd.hh>

// STL headers:
#include <mutex>

namespace standard_masala_plugins {
namespace file_interpreters {
namespace cost_function_network {

/// @brief A file interpreter for reading and writing binary cost function network
/// optimization problems.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
class BinaryCostFunctionNetworkProblemFileInterpreter : public masala::base::managers::file_interpreter::MasalaFileInterpreter {

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	BinaryCostFunctionNetworkProblemFileInterpreter() = default;

	/// @brief Copy constructor.
	/// @details Needed since we define a mutex.
	BinaryCostFunctionNetworkProblemFileInterpreter( BinaryCostFunctionNetworkProblemFileInterpreter const & );

	/// @brief Assignment operator.
	/// @details Needed since we define a mutex.
	BinaryCostFunctionNetworkProblemFileInterpreter &
	operator=( BinaryCostFunctionNetworkProblemFileInterpreter const & );

	/// @brief Make a copy of this object that's wholly independent.
	BinaryCostFunctionNetworkProblemFileInterpreterSP
	deep_clone() const;

	/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
	void
	make_independent();

	/// @brief Destructor.
	~BinaryCostFunctionNetworkProblemFileInterpreter() override = default;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.
	/// @returns { { "FileInterpreter", "CostFunctionNetworkProblemFileInterpreter", "BinaryCostFunctionNetworkProblemFileInterpreter" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outher vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.
	/// @returns { "file_interpreter", "cost_function_network", "problem", "binary" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief All MasalaFileInterpreter subclasses must list the file types that they create.  These are
	/// provided as short descriptors (e.g. "protein_data_bank_file").
	/// @returns { "cost_function_network_problem_definition_file" }
	std::vector< std::string >
	get_file_descriptors() const override;

	/// @brief All MasalaFileInterpreter subclasses must list the file type extensions that they create.
	/// @details Extensions are provided in lower-case (e.g. "pdb").
	/// @returns { ".txt", ".pdb" }
	std::vector< std::string >
	get_file_extensions() const override;

	/// @brief Get the class name.
	/// @returns "BinaryCostFunctionNetworkProblemFileInterpreter".
	std::string class_name() const override;

	/// @brief Get the class namespace.
	/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
	std::string class_namespace() const override;

	/// @brief Get the class name (static version).
	/// @returns "BinaryCostFunctionNetworkProblemFileInterpreter".
	static std::string class_name_static();

	/// @brief Get the class namespace (static version).
	/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
	static std::string class_namespace_static();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the API definition for this object.
	masala::base::api::MasalaObjectAPIDefinitionCWP
	get_api_definition();

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC SETTERS
////////////////////////////////////////////////////////////////////////////////


public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief A mutex for threadsafe operation.
	mutable std::mutex file_interpreter_mutex_;

	/// @brief The API description.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_description_;

}; // class BinaryCostFunctionNetworkProblemFileInterpreter

} // namespace cost_function_network
} // namespace file_interpreters
} // namesapce standard_masala_plugins

#endif // StandardMaslaPlugins_src_file_interpreters_cost_function_network_BinaryCostFunctionNetworkProblemFileInterpreter_hh