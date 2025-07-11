/*
    Masala
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

/// @file src/file_interpreters/cost_function_network/ASCIICostFunctionNetworkProblemRosettaFileInterpreter.hh
/// @brief Headers for a file interpreter for reading and writing ASCII files that define cost function network optimization problems.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

#ifndef StandardMaslaPlugins_src_file_interpreters_cost_function_network_ASCIICostFunctionNetworkProblemRosettaFileInterpreter_hh
#define StandardMaslaPlugins_src_file_interpreters_cost_function_network_ASCIICostFunctionNetworkProblemRosettaFileInterpreter_hh

// Forward declarations:
#include <file_interpreters/cost_function_network/ASCIICostFunctionNetworkProblemRosettaFileInterpreter.fwd.hh>

// Parent header:
#include <base/managers/file_interpreter/MasalaFileInterpreter.hh>

// Base headers:
#include <base/types.hh>
#include <base/api/MasalaObjectAPIDefinition.fwd.hh>

// Numeric headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.fwd.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.fwd.hh>

// STL headers:
#include <mutex>
#include <string>
#include <atomic>

namespace standard_masala_plugins {
namespace file_interpreters {
namespace cost_function_network {

/// @brief A file interpreter for reading and writing ASCII cost function network
/// optimization problems.
/// @author Tristan Zaborniak (tzaborniak@flatironinstitute.org).
class ASCIICostFunctionNetworkProblemRosettaFileInterpreter : public masala::base::managers::file_interpreter::MasalaFileInterpreter {

	typedef masala::base::Size Size;
	typedef masala::base::Real Real;

public:

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

	/// @brief Default constructor.
	/// @details Not defaulted since the atomic bool must be set.
	ASCIICostFunctionNetworkProblemRosettaFileInterpreter();

	/// @brief Copy constructor.
	/// @details Needed since we define a mutex.
	ASCIICostFunctionNetworkProblemRosettaFileInterpreter( ASCIICostFunctionNetworkProblemRosettaFileInterpreter const & );

	/// @brief Assignment operator.
	/// @details Needed since we define a mutex.
	ASCIICostFunctionNetworkProblemRosettaFileInterpreter &
	operator=( ASCIICostFunctionNetworkProblemRosettaFileInterpreter const & );

	/// @brief Make a copy of this object that's wholly independent.
	ASCIICostFunctionNetworkProblemRosettaFileInterpreterSP
	deep_clone() const;

	/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
	void
	make_independent();

	/// @brief Destructor.
	~ASCIICostFunctionNetworkProblemRosettaFileInterpreter() override = default;

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Get the category or categories for this plugin class.
	/// @returns { { "FileInterpreter", "CostFunctionNetworkProblemFileInterpreter", "ASCIICostFunctionNetworkProblemRosettaFileInterpreter" } }
	/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
	/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
	/// in more than one hierarchical category (in which case there would be more than one
	/// entry in the outher vector), but must be in at least one.  The first one is used as
	/// the primary key.
	std::vector< std::vector< std::string > >
	get_categories() const override;

	/// @brief Get the keywords for this plugin class.
	/// @returns { "file_interpreter", "cost_function_network", "problem", "ascii" }
	std::vector< std::string >
	get_keywords() const override;

	/// @brief All MasalaFileInterpreter subclasses must list the file types that they create.  These are
	/// provided as short descriptors (e.g. "protein_data_bank_file").
	/// @returns { "cost_function_network_problem_definition_file" }
	std::vector< std::string >
	get_file_descriptors() const override;

	/// @brief All MasalaFileInterpreter subclasses must list the file type extensions that they create.
	/// @details Extensions are provided in lower-case (e.g. "pdb").
	/// @returns { "txt", "pdb" }
	std::vector< std::string >
	get_file_extensions() const override;

	/// @brief Get the class name.
	/// @returns "ASCIICostFunctionNetworkProblemRosettaFileInterpreter".
	std::string class_name() const override;

	/// @brief Get the class namespace.
	/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
	std::string class_namespace() const override;

	/// @brief Get the class name (static version).
	/// @returns "ASCIICostFunctionNetworkProblemRosettaFileInterpreter".
	static std::string class_name_static();

	/// @brief Get the class namespace (static version).
	/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
	static std::string class_namespace_static();

	/// @brief Indicate that this interpreter reads ASCII files.
	/// @note Although this file format takes advantage of the trick of encoding 3 bytes of binary information
	/// in 4 bytes of ASCII text, the file is still a text file.
	/// @returns True.
	bool filetype_is_ascii() const override;

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

	/// @brief Set the name of the cost function network problem description class to generate.
	/// @details This can be the short name or the full name (i.e. with or without namespace).
	/// It need only include namespace if the short name is not unique.
	/// @note If not set, then an optimizer must be specified instead.
	void
	set_cfn_problem_type_to_generate(
		std::string const & class_name_in
	);

	/// @brief Set the name of the optimizer class that will be used to solve this problem.
	/// @details This can be the short name or the full name (i.e. with or without namespace).
	/// It need only include namespace if the short name is not unique.
	/// @note If not set, then cost function network problem class must be specified instead.
	void
	set_cfn_optimizer_type(
		std::string const & class_name_in
	);

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

public:

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Read the contents of a Rosetta-format ASCII cost function network problem
	/// file, and return a cost function network problem object (as a generic MasalaObject pointer).
	/// @details This override calls cfn_problems_from_ascii_file_contents().
	masala::base::MasalaObjectAPISP
	object_from_ascii_file_contents(
		std::vector< std::string > const & filelines
	) const override;

	/// @brief Read the contents of a Rosetta-format ASCII cost function network problem
	/// file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).
	/// @details Throws if at least one problem was not successfully parsed.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
	cfn_problems_from_ascii_file_contents(
		std::vector< std::string > const & filelines
	) const;

	/// @brief Read a Rosetta-format ASCII cost function network problem file, and return a set of cost function network 
	/// problem objects (as a CostFunctionNetworkProblems pointer).
	/// @details Throws if at least one problem was not successfully parsed.
	/// @note Triggers disk i/o (through the MasalaDiskAccessManager)!
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
	cfn_problems_from_ascii_file(
		std::string const & filename
	) const;

	/// @brief Generate a Rosetta-readable CFN file from the contents of a CostFunctionNetworkSolution object.
	/// @details This override calls ascii_file_contents_from_cfn_solution().
	std::string
	ascii_file_contents_from_object(
		masala::base::MasalaObjectAPICSP const & object
	) const override;

	/// @brief Generate a Rosetta-readable CFN file from the contents of a CostFunctionNetworkSolution object.
	/// @details This override calls ascii_file_contents_from_cfn_solution().
	std::string
	ascii_file_contents_from_cfn_solution(
		masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolution_APICSP const & object
	) const;

protected:

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Assign src to this object.
	/// @details Assumes both objects' mutexes have been locked.
	void protected_assign( ASCIICostFunctionNetworkProblemRosettaFileInterpreter const & src );

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

	/// @brief Generate an empty cost function network optimization problem description of a user-specified type.
	/// @details This EITHER generates a problem of type cfn_problem_class_, OR generates a problem compatible with
	/// cfn_optimizer_class_, depending on which is specified.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
	generate_cfn_problem() const;

	/// @brief Given a set of lines starting with [BEGIN_BINARY_GRAPH_SUMMARY] and ending with [END_BINARY_GRAPH_SUMMARY],
	/// convert these to a cost function network problem definition.
	/// @param[in] lines A vector of file lines.
	/// @param[in] line_begin The [BEGIN_BINARY_GRAPH_SUMMARY] line index.
	/// @param[in] line_end The [END_BINARY_GRAPH_SUMMARY] line index.
	/// @returns A cost function network optimization problem of the given type.
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
	cfn_problem_from_ascii_file_block(
		std::vector< std::string > const & lines,
		masala::base::Size const line_begin,
		masala::base::Size const line_end
	) const;

	/// @brief Check whether the cost function network problem class is valid (i.e. whether it exists).
	/// @details Must be called from a mutex-locked or finalized context.
	void check_cfn_problem_class() const;

	/// @brief Check whether the cost function network optimizer class is valid (i.e. whether it exists).
	/// @details Must be called from a mutex-locked or finalized context.
	void check_cfn_optimizer_class() const;

private:

////////////////////////////////////////////////////////////////////////////////
// PRIVATE DATA
////////////////////////////////////////////////////////////////////////////////

	/// @brief A mutex for threadsafe operation.
	mutable std::mutex file_interpreter_mutex_;

	/// @brief Has this object been finalized?
	std::atomic_bool finalized_;

	/// @brief The API description.
	masala::base::api::MasalaObjectAPIDefinitionCSP api_description_;

	/// @brief The name of the cost function network problem description class to generate.
	/// @details This can be the short name or the full name (i.e. with or without namespace).
	/// It need only include namespace if the short name is not unique.
	/// @note If empty, then an optimizer must be specified instead.
	std::string cfn_problem_class_;

	/// @brief The name of the optimizer class that will be used to solve this problem.
	/// @details This can be the short name or the full name (i.e. with or without namespace).
	/// It need only include namespace if the short name is not unique.
	/// @note If empty, then cost function network problem class must be specified instead.
	std::string cfn_optimizer_class_;

}; // class ASCIICostFunctionNetworkProblemRosettaFileInterpreter

} // namespace cost_function_network
} // namespace file_interpreters
} // namesapce standard_masala_plugins

#endif // StandardMaslaPlugins_src_file_interpreters_cost_function_network_ASCIICostFunctionNetworkProblemRosettaFileInterpreter_hh
