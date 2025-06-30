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

/// @file src/file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreter.cc
/// @brief Implementations for a file interpreter for reading and writing binary cost function network
/// optimization problems.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <file_interpreters/cost_function_network/BinaryCostFunctionNetworkProblemRosettaFileInterpreter.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/base_classes/optimization/cost_function_network/PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblemCreator.hh>
#include <numeric_api/base_classes/optimization/cost_function_network/PluginCostFunctionNetworkOptimizer.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/utility/cxx_17_compatibility_util_api.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.fwd.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.fwd.hh>
#include <base/managers/engine/MasalaDataRepresentationManager.hh>
#include <base/managers/engine/MasalaDataRepresentationRequest.hh>
#include <base/managers/engine/MasalaEngineManager.hh>
#include <base/managers/engine/MasalaEngineCreator.hh>
#include <base/managers/engine/MasalaEngineRequest.hh>
#include <base/utility/string/string_manipulation.hh>
#include <base/utility/execution_policy/util.hh>

// Core API headers:
#include <core_api/utility/binary_in_ascii_util.hh>

// STL headers:
#include <vector>
#include <string>
#include <sstream>
//#include <iostream> // DELETE ME; FOR DEBUGGING ONLY.

namespace standard_masala_plugins {
namespace file_interpreters {
namespace cost_function_network {

/// @brief A structure used for storing two-body penalties temporarily.
/// @details This stores two global choice indices plus a penalty value.
template< typename INDEXTYPE, typename VALTYPE >
struct TwoBodyTuple {
public:
	INDEXTYPE choice1_ = 0;
	INDEXTYPE choice2_ = 0;
	VALTYPE penalty_ = 0.0;
};

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////


/// @brief Default constructor.
/// @details Not defaulted since the atomic bool must be set.
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::BinaryCostFunctionNetworkProblemRosettaFileInterpreter() :
	masala::base::managers::file_interpreter::MasalaFileInterpreter(),
	finalized_( false )
{}

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::BinaryCostFunctionNetworkProblemRosettaFileInterpreter(
	BinaryCostFunctionNetworkProblemRosettaFileInterpreter const & src
) :
	masala::base::managers::file_interpreter::MasalaFileInterpreter( src )
{
	std::lock_guard< std::mutex > lock( src.file_interpreter_mutex_ );
	protected_assign( src );
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

/// @brief Indicate that this interpreter reads ASCII files.
/// @note Although this file format takes advantage of the trick of encoding 3 bytes of binary information
/// in 4 bytes of ASCII text, the file is still a text file.
/// @returns True.
bool
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::filetype_is_ascii() const {
	return true;
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
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;

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
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_problem_type_to_generate, this, std::placeholders::_1 )
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
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_optimizer_type, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"filetype_is_ascii", "Does this interpreter read ASCII file types, or binary?  This override returns "
				"true, indicating that this interpreter reads ASCII files.",
				"is_ascii", "Is the filetype read by this interpreter an ASCII format?  (Yes, it is, so this "
				"is always true.)", true, false,
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::filetype_is_ascii, this )
			)
		);

		// Work functions:
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				masala::base::MasalaObjectAPISP,
				std::vector< std::string > const &
			> >(
				"object_from_ascii_file_contents", "Read the contents of a Rosetta-format binary cost "
				"function network problem file, and return a cost function network problem object (as "
				"a generic MasalaObject pointer).",
				true, false, false, false,
				"file_contents", "The contents of a Rosetta-style binary cost function network optimization problem file (a.k.a. a packing problem file), "
				"expressed as a vector of strings (one string per file line).",
				"output_object", "A shared pointer to a container of cost function network optimization problems.",
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::object_from_ascii_file_contents, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP,
				std::vector< std::string > const &
			> >(
				"cfn_problems_from_ascii_file_contents", "Parse the contents of a Rosetta-format binary cost function network problem "
				"file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).",
				true, false, false, false,
				"file_contents", "The contents of a Rosetta-style binary cost function network optimization problem file (a.k.a. a packing problem file), "
				"expressed as a vector of strings (one string per file line).",
				"problems", "A shared pointer to a container of cost function network optimization problems.",
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file_contents, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP,
				std::string const &
			> >(
				"cfn_problems_from_ascii_file", "Parse a Rosetta-format binary cost function network problem "
				"file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).  Triggers disk i/o!  "
				"Throws if the file contents cannot be parsed, or if the file does not exist.  (Uses the disk manager for disk access.)",
				true, false, false, false,
				"filename", "The name of the file to read.",
				"problems", "A shared pointer to a container of cost function network optimization problems.",
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				std::string,
				masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolution_APICSP const &
			> >(
				"ascii_file_contents_from_cfn_solution", "Generate a Rosetta-readable CFN string from the contents of a CostFunctionNetworkSolution object.",
				true, false, false, false,
				"solutions", "A shared pointer to a container of cost function network optimization solutions.",
				"filename", "The name of the file to write ascii contents to.",
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_cfn_solution, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				std::string,
				masala::base::MasalaObjectAPICSP const &
			> >(
				"ascii_file_contents_from_object", "Generate a Rosetta-readable CFN string from the contents of a CostFunctionNetworkSolution object.  Throws "
				"if the input object cannot be interpreted as a CostFunctionNetworkOptimizationSolution.",
				true, false, false, false,
				"object", "A Masala object shared pointer that can be interpreted as a shared pointer to a container of cost function network optimization solutions.  Throws if the object is of the wrong type.",
				"file_contents", "A string containing the contents of an ASCII file to write, generated from the input object.",
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_object, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_TwoInput<
				void,
				masala::base::MasalaObjectAPICSP const &,
				std::string const &
			> >(
				"object_to_file", "Call ascii_file_contents_from_object() in order to write CFN solution string to file. Uses MasalaDiskManager.",
				true, false, false, false,
				"object", "The CFN solution to be written to a file.",
				"filename", "The file that the CFN solution is to be written to.",
				"void", "The output is written to a file.",
				std::bind( &BinaryCostFunctionNetworkProblemRosettaFileInterpreter::object_to_file, this, std::placeholders::_1, std::placeholders::_2 )
			)
		);

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

/// @brief Read the contents of a Rosetta-format binary cost function network problem
/// file, and return a cost function network problem object (as a generic MasalaObject pointer).
/// @details This override calls cfn_problems_from_ascii_file_contents().
masala::base::MasalaObjectAPISP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::object_from_ascii_file_contents(
	std::vector< std::string > const & filelines
) const {
	return cfn_problems_from_ascii_file_contents( filelines );
}

/// @brief Read the contents of a Rosetta-format binary cost function network problem
/// file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).
/// @details Throws if at least one problem was not successfully parsed.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file_contents(
	std::vector< std::string > const & filelines
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	std::lock_guard< std::mutex > lock( file_interpreter_mutex_ );

	CostFunctionNetworkOptimizationProblems_APISP problems( masala::make_shared< CostFunctionNetworkOptimizationProblems_API >() );

	bool in_block(false);
	Size line_begin(0), line_end(0);
	Size counter(0);
	Size linecounter(0);

	for( std::string const & line : filelines ) {
		std::string const linestripped( masala::base::utility::string::trim( line ) );
		if( !in_block ) {
			if( linestripped == "[BEGIN_BINARY_GRAPH_SUMMARY]" ) {
				line_begin = linecounter;
				in_block = true;
			}
		} else {
			if( linestripped == "[END_BINARY_GRAPH_SUMMARY]" ) {
				in_block = false;
				line_end = linecounter;
				++counter;
				CostFunctionNetworkOptimizationProblem_APISP problem( cfn_problem_from_ascii_file_block( filelines, line_begin, line_end ) );
				if( problem != nullptr ) {
					problems->add_optimization_problem( problem );
				} else {
					write_to_tracer( "Couldn't read problem " + std::to_string(counter) + " in file.  Skipping." );
				}
			}
		}
		++linecounter;
	}

	CHECK_OR_THROW_FOR_CLASS( problems->n_problems() > 0, "cfn_problems_from_ascii_file_contents", "No problems were "
		"successfully parsed from the file contents!"
	);

	return problems;
}

/// @brief Read a Rosetta-format binary cost function network problem file, and return a set of cost function network 
/// problem objects (as a CostFunctionNetworkProblems pointer).
/// @details Throws if at least one problem was not successfully parsed.
/// @note Triggers disk i/o (through the MasalaDiskAccessManager)!
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file(
	std::string const & filename
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
#ifdef NDEBUG // Release mode -- just static-cast the object.
	return std::static_pointer_cast< CostFunctionNetworkOptimizationProblems_API >( object_from_file( filename ) );
#else
	CostFunctionNetworkOptimizationProblems_APISP returnobj( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblems_API >( object_from_file( filename ) ) );
	CHECK_OR_THROW_FOR_CLASS( returnobj != nullptr, "cfn_problems_from_ascii_file", "The return object could not be interpreted as a CostFunctionNetworkOptimizationProblems_API "
		"container!  This ought not to happen.  It is a program error.  Please consult a developer."
	);
	return returnobj;
#endif
}

/// @brief Generate a Rosetta-readable CFN string from the contents of a CostFunctionNetworkSolution object.
/// @details This override calls ascii_file_contents_from_cfn_solution().
std::string
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_object(
	masala::base::MasalaObjectAPICSP const & object
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	CostFunctionNetworkOptimizationSolution_APICSP solution( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolution_API const >( object ) );
	CHECK_OR_THROW_FOR_CLASS( solution != nullptr, "ascii_file_contents_from_object", "An object was passed to "
		"this function that could not be interpreted as a CostFunctionNetworkOptimizationSolution object."
	);
	return ascii_file_contents_from_cfn_solution( solution );
}

/// @brief Generate a Rosetta-readable CFN string from the contents of a CostFunctionNetworkSolution object.
/// @details Generates one file per solution, with (node) (choice) appearing on each line.
std::string
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_cfn_solution(
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolution_APICSP const & object
) const {
	using masala::base::Size;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	std::string solution_as_ascii_string;
	CHECK_OR_THROW_FOR_CLASS( object != nullptr, "ascii_file_contents_from_object", 
		"The returned object could not be interpreted as a CostFunctionNetworkOptimizationSolution_API!"
	);
	// Retrieve problem corresponding to solution:
	CostFunctionNetworkOptimizationProblem_APICSP solution_problem = 
		std::dynamic_pointer_cast< const CostFunctionNetworkOptimizationProblem_API >( object->problem() );
	// Retrieve node-number of choices map:
	std::map< Size, Size > node_map = solution_problem->n_choices_at_all_nodes();
	// Retrieve solution vector from solution object:
	std::vector< Size > vector = object->solution_at_all_positions();
	// Append variable node, choice to string:
	for ( auto it = node_map.begin(); it != node_map.end(); ++it ) {
		if ( it->second != 0 ) {
            solution_as_ascii_string += std::to_string( it->first ) + "\t" + std::to_string( vector[ it->first ] ) + "\n";
        }
	}
	return solution_as_ascii_string;
}

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

/// @brief Generate an empty cost function network optimization problem description of a user-specified type.
/// @details This EITHER generates a problem of type cfn_problem_class_, OR generates a problem compatible with
/// cfn_optimizer_class_, depending on which is specified.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::generate_cfn_problem() const {
	using namespace masala::base::managers::engine;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;

	CHECK_OR_THROW_FOR_CLASS( ( cfn_optimizer_class_.empty() && (!cfn_problem_class_.empty()) ) ||
		( (!cfn_optimizer_class_.empty()) && cfn_problem_class_.empty() ), "generate_cfn_problem", "Either the "
		"cost function network problem type, or the cost function network optimizer type, must be specified before "
		"this function is called.  Both cannot be specified."
	);

	if( cfn_optimizer_class_.empty() ) {
		// Creating cost function network optimization problem class subtype by name:
		std::vector< CostFunctionNetworkOptimizationProblemCreatorCSP > creators_cast;
		MasalaDataRepresentationRequest request;
		request.add_data_representation_name_requirement( cfn_problem_class_ );
		std::vector< MasalaDataRepresentationCreatorCSP > const creators(
			MasalaDataRepresentationManager::get_instance()->get_compatible_data_representation_creators( request )
		);

		for( auto const & creator : creators ) {
			CostFunctionNetworkOptimizationProblemCreatorCSP creator_cast(
				std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblemCreator const >( creator )
			);
			if( creator != nullptr ) {
				creators_cast.push_back( creator_cast );
			}
		}

		CHECK_OR_THROW_FOR_CLASS( creators_cast.size() > 0, "generate_cfn_problem", "Could not find a cost function "
			"network optimization problem class matching name \"" + cfn_problem_class_ + "\"."
		);
		if( creators_cast.size() > 1 ) {
			write_to_tracer( "Warning -- found more than one cost function network optimization problem class matching "
				"name \"" + cfn_problem_class_ + "\".  Returning first.  Include the full namespace to specify which to "
				"select."
			);
		}
		CostFunctionNetworkOptimizationProblem_APISP returnobj(
			std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API >(
				creators_cast[0]->create_data_representation()
			)
		);
		CHECK_OR_THROW_FOR_CLASS( returnobj != nullptr, "generate_cfn_problem", "Error creating cost function network "
			"problem creator.  Creator " + creators_cast[0]->class_name() + " did not return an object of expected class.  "
			"This is a program error.  Please consult a developer."
		);
		return returnobj;
	} else {
		// Creating cost function network optimization problem class subtype by compatibility with an optimizer:
		MasalaEngineRequest engine_request;
		engine_request.add_engine_name_requirement( cfn_optimizer_class_ );
		std::vector< MasalaEngineCreatorCSP > engine_creators(
			MasalaEngineManager::get_instance()->get_compatible_engine_creators( engine_request )
		);
		CHECK_OR_THROW_FOR_CLASS( engine_creators.size() > 0, "generate_cfn_problem", "Could not find a Masala "
			"engine matching name \"" + cfn_optimizer_class_ + "\"."
		);
		PluginCostFunctionNetworkOptimizer * engine(nullptr);
		for( auto const & engine_creator : engine_creators ) {
			MasalaEngineAPISP masala_engine( engine_creator->create_engine() );
			engine = dynamic_cast< PluginCostFunctionNetworkOptimizer * >( masala_engine->get_inner_engine_object().get() );
			if( engine != nullptr ) break;
		}
		CHECK_OR_THROW_FOR_CLASS( engine != nullptr, "generate_cfn_problem", "Could not find a cost function "
			"network optimizer matching name \"" + cfn_optimizer_class_ + "\"."
		);

		std::vector< CostFunctionNetworkOptimizationProblemCreatorCSP > creators_cast;
		MasalaDataRepresentationRequestResult result_type( MasalaDataRepresentationRequestResult::UNKNOWN_RESULT_TYPE );
		std::vector< MasalaDataRepresentationCreatorCSP > const creators(
			MasalaDataRepresentationManager::get_instance()->get_data_representation_creators_for_engine(
				{{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem" }},
				true, *engine, result_type
			)
		);
		CHECK_OR_THROW_FOR_CLASS( creators.size() > 0 &&
			result_type != MasalaDataRepresentationRequestResult::REQUEST_RETURNED_NO_RESULTS &&
			result_type != MasalaDataRepresentationRequestResult::UNKNOWN_RESULT_TYPE, "generate_cfn_problem",
			"Could not find a suitable cost function network optimization type compatible with the \""
			+ engine->class_name() + "\" optimizer."
		);

		for( auto const & creator : creators ) {
			CostFunctionNetworkOptimizationProblemCreatorCSP creator_cast(
				std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblemCreator const >( creator )
			);
			if( creator != nullptr ) {
				creators_cast.push_back( creator_cast );
			}
		}

		CHECK_OR_THROW_FOR_CLASS( creators_cast.size() > 0, "generate_cfn_problem", "Could not find a cost function "
			"network optimization problem class compatible with the \"" + engine->class_name() + "\" optimizer."
		);
		if( creators_cast.size() > 1 ) {
			write_to_tracer( "Warning -- found more than one cost function network optimization problem compatible with "
				"the \"" + engine->class_name() + "\" optimizer.  Returning first."
			);
		}
		CostFunctionNetworkOptimizationProblem_APISP returnobj(
			std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API >(
				creators_cast[0]->create_data_representation()
			)
		);
		CHECK_OR_THROW_FOR_CLASS( returnobj != nullptr, "generate_cfn_problem", "Error creating cost function network "
			"problem creator.  Creator " + creators_cast[0]->class_name() + " did not return an object of expected class.  "
			"This is a program error.  Please consult a developer."
		);
		return returnobj;
	}

	return nullptr;
}

/// @brief Interpret a string of binary information encoded so that 3 bytes of binary occupy 4 bytes of ASCII text
/// as a vector of integers.
/// @param[in] line The line of ASCII text to decode.
/// @param[in] vec_length The number of entries in the vector
/// @param[in] entry_bytesize The number of bytes used to represent each unsigned integer.
/// @param[out] choices_by_variable_node_expected The output vector.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::decode_choices_per_variable_node(
	std::string const & line,
	masala::base::Size const vec_length,
	masala::base::Size const entry_bytesize, 
	std::vector< masala::base::Size > & choices_by_variable_node_expected
) const {
	CHECK_OR_THROW_FOR_CLASS( entry_bytesize <= sizeof( Size ), "decode_choices_per_variable_node",
		"A maximum of " + std::to_string( sizeof( Size ) * CHAR_BIT ) + " bits can be used to represent unsigned "
		"integers on this system, yet the file indicates that choice counts are represented with "
		+ std::to_string(entry_bytesize * CHAR_BIT) + " bits!"
	);
	Size const char_bytesize( static_cast< Size >( std::ceil(static_cast<Real>(entry_bytesize*vec_length) / 3.0) ) * 4 );
	CHECK_OR_THROW_FOR_CLASS( line.size() == char_bytesize, "decode_choices_per_variable_node",
		"Expected " + std::to_string( char_bytesize ) + " bytes of ASCII data, but got " +
		std::to_string( line.size() ) + ".  Could not parse line \"" + line + "\"."
	);
	CHECK_OR_THROW_FOR_CLASS( entry_bytesize == 2 || entry_bytesize == 4 || entry_bytesize == sizeof(Size), "decode_choices_per_variable_node",
		"This function currently only supports 16-, 32-, or " + std::to_string( sizeof(Size) * CHAR_BIT ) + "-bit integers, but "
		"received an integer bit size of " + std::to_string(entry_bytesize * CHAR_BIT) + " bits."
	);

	choices_by_variable_node_expected.clear();
	choices_by_variable_node_expected.resize( vec_length, 0 );
	if( entry_bytesize == 2 ) {
		std::vector< uint16_t > two_byte_vec(vec_length, 0);
		masala::core_api::utility::decode_data_from_string( (unsigned char *)( &two_byte_vec[0] ), line, vec_length * 2 );

		// DELETE THE FOLLOWING; FOR DEBUGGING ONLY:
		// std::cout << "CHOICES PER NODE:" << std::endl;
		// for( Size i(0); i<two_byte_vec.size(); ++i ) {
		// 	std::cout << i << "\t" << two_byte_vec[i] << std::endl; 
		// }

		for( Size i(0); i<vec_length; ++i ) {
			choices_by_variable_node_expected[i] = two_byte_vec[i];
		}
	} else if( entry_bytesize == 4 ) {
		std::vector< uint32_t > four_byte_vec(vec_length, 0);
		masala::core_api::utility::decode_data_from_string( (unsigned char *)( &four_byte_vec[0] ), line, vec_length * 4 );
		
		// DELETE THE FOLLOWING; FOR DEBUGGING ONLY:
		// std::cout << "CHOICES PER NODE:" << std::endl;
		// for( Size i(0); i<four_byte_vec.size(); ++i ) {
		// 	std::cout << i << "\t" << four_byte_vec[i] << std::endl; 
		// }

		for( Size i(0); i<vec_length; ++i ) {
			choices_by_variable_node_expected[i] = four_byte_vec[i];
		}
	} else if( entry_bytesize == sizeof(Size) ) {
		masala::core_api::utility::decode_data_from_string( (unsigned char *)( &choices_by_variable_node_expected[0] ), line, vec_length * sizeof(Size) );
		
		// DELETE THE FOLLOWING; FOR DEBUGGING ONLY:
		// std::cout << "CHOICES PER NODE:" << std::endl;
		// for( Size i(0); i<choices_by_variable_node_expected.size(); ++i ) {
		// 	std::cout << i << "\t" << choices_by_variable_node_expected[i] << std::endl; 
		// }
	}
}

/// @brief Given a line encoding onebody penalties in ASCII text, set up the onebody penalties in a cost function
/// network optimization problem object.
/// @param[in] line The ASCII line we're decoding.
/// @param[in] choices_by_variable_node_expected The number of onebody penalties by variable node index.
/// @param[in] onebody_penalty_bytesize_expected The number of bytes used to encode each onebody penalty.
/// @param[in] global_node_indices The global index of each variable node.  May be an empty vector if this
/// information was not provided in the input file.
/// @param[inout] problem_api The cost function network optimization problem in which we're storing penalties.
/// @note This function will throw if the CostFunctionNetworkOptimizationProblem isn't a
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::decode_onebody_penalties(
	std::string const & line,
	std::vector< masala::base::Size > const & choices_by_variable_node_expected,
	masala::base::Size const onebody_penalty_bytesize_expected,
	std::vector< masala::base::Size > const & global_node_indices,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API & problem_api
) const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;

	CHECK_OR_THROW_FOR_CLASS( onebody_penalty_bytesize_expected <= sizeof( Real ), "decode_onebody_penalties",
		"A maximum of " + std::to_string( sizeof( Real ) * CHAR_BIT ) + " bits can be used to represent double-precision "
		"floating point numbers on this system, yet the file indicates that choice counts are represented with "
		+ std::to_string(onebody_penalty_bytesize_expected * CHAR_BIT) + " bits!"
	);
	CHECK_OR_THROW_FOR_CLASS( onebody_penalty_bytesize_expected == sizeof(Real) || onebody_penalty_bytesize_expected == sizeof(float),
		"decode_onebody_penalties", "Expected a floating-point bit size of " + std::to_string( sizeof( float ) * CHAR_BIT ) + " or "
		+ std::to_string( sizeof( Real ) * CHAR_BIT ) + " bits, but got " + std::to_string( onebody_penalty_bytesize_expected * CHAR_BIT )
		+ " bits!"
	);

	std::vector< masala::base::Size > node_indices;
	if( global_node_indices.empty() ) {
		node_indices.reserve( choices_by_variable_node_expected.size() );
		for( Size i(0); i<choices_by_variable_node_expected.size(); ++i ) {
			node_indices.push_back( i );
		}
	} else {
		node_indices = global_node_indices;
	}

	PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblemSP problem(
		std::dynamic_pointer_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem >( problem_api.get_inner_data_representation_object() )
	);
	CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "decode_onebody_penalties", "The selected problem class, " + problem_api.inner_class_name() + ", is "
		"not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.  Cannot store precomputed onebody penalties."
	);

	Size const total_choices( masala::numeric_api::utility::reduce( MASALA_UNSEQ_EXECUTION_POLICY choices_by_variable_node_expected.begin(), choices_by_variable_node_expected.end() ) );

	Size const char_bytesize( static_cast< Size >( std::ceil(static_cast<Real>(onebody_penalty_bytesize_expected*total_choices) / 3.0) ) * 4 );
	CHECK_OR_THROW_FOR_CLASS( line.size() == char_bytesize, "decode_onebody_penalties",
		"Expected " + std::to_string( char_bytesize ) + " bytes of ASCII data, but got " +
		std::to_string( line.size() ) + ".  Could not parse onebody penalties binary data."
	);

	if( onebody_penalty_bytesize_expected == sizeof( float ) ) {
		std::vector< float > onebody_floats( total_choices, 0.0 );
		masala::core_api::utility::decode_data_from_string( (unsigned char *)( &onebody_floats[0] ), line, total_choices * sizeof( float ) );

		// DELETE THE FOLLOWING; FOR DEBUGGING ONLY:
		// std::cout << "ONEBODY PENALTIES:" << std::endl;
		// for( auto const & entry : onebody_floats ) {
		// 	std::cout << entry << std::endl; 
		// }

		Size choice_counter(0), varnode_index(0);
		for( Size i(0); i<total_choices; ++i ) {
			problem->set_onebody_penalty( node_indices[varnode_index], choice_counter, onebody_floats[i] );
			++choice_counter;
			if( choice_counter >= choices_by_variable_node_expected[varnode_index] ) {
				++varnode_index;
				choice_counter = 0;
			}
		}
	} else if( onebody_penalty_bytesize_expected == sizeof( Real ) ) {
		std::vector< Real > onebody_reals( total_choices, 0.0 );
		masala::core_api::utility::decode_data_from_string( (unsigned char *)( &onebody_reals[0] ), line, total_choices * sizeof( Real ) );

		// DELETE THE FOLLOWING; FOR DEBUGGING ONLY:
		// std::cout << "ONEBODY PENALTIES:" << std::endl;
		// for( auto const & entry : onebody_reals ) {
		// 	std::cout << entry << std::endl; 
		// }

		Size choice_counter(0), varnode_index(0);
		for( Size i(0); i<total_choices; ++i ) {
			problem->set_onebody_penalty( node_indices[varnode_index], choice_counter, onebody_reals[i] );
			++choice_counter;
			if( choice_counter >= choices_by_variable_node_expected[varnode_index] ) {
				++varnode_index;
				choice_counter = 0;
			}
		}
	}
}

/// @brief Convert a binary-as-ASCII-text string to two-body penalties, and load them into the precomputed cost function network problem description.
/// @param[in] line The ASCII line we're decoding.
/// @param[in] choices_by_variable_node_expected The number of onebody penalties by variable node index.
/// @param[in] n_twobody_penalties_expected The number of pairs of twobody penalties that we expect to find.
/// @param[in] twobody_penalty_index_bytesize_expected The number of bytes that a twobody penalty index (node index or choice index) takes up.  Must be 2, 4, or sizeof(Size).
/// @param[in] twobody_penalty_bytesize_expected The number of bytes that a twobody penalty takes up.  Must be sizeof(float) or sizeof(Real).
/// @param[in] global_node_indices The global index of each variable node.  May be an empty vector if this
/// information was not provided in the input file.
/// @param[inout] problem_api The cost function network optimization problem in which we're storing penalties.
/// @note This function will throw if the CostFunctionNetworkOptimizationProblem isn't a
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::decode_twobody_penalties(
	std::string const & line,
	std::vector< masala::base::Size > const & choices_by_variable_node_expected,
	masala::base::Size const n_twobody_penalties_expected,
	masala::base::Size twobody_penalty_index_bytesize_expected,
	masala::base::Size twobody_penalty_bytesize_expected,
	std::vector< masala::base::Size > const & global_node_indices,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API & problem_api
) const {
	CHECK_OR_THROW_FOR_CLASS( twobody_penalty_bytesize_expected <= sizeof( Real ), "decode_twobody_penalties",
		"A maximum of " + std::to_string( sizeof( Real ) * CHAR_BIT ) + " bits can be used to represent double-precision "
		"floating point numbers on this system, yet the file indicates that choice counts are represented with "
		+ std::to_string(twobody_penalty_bytesize_expected * CHAR_BIT) + " bits!"
	);
	CHECK_OR_THROW_FOR_CLASS( twobody_penalty_index_bytesize_expected <= sizeof( Size ), "decode_twobody_penalties",
		"A maximum of " + std::to_string( sizeof( Size ) * CHAR_BIT ) + " bits can be used to represent unsigned "
		"integers on this system, yet the file indicates that node and choice indicess are represented with "
		+ std::to_string(twobody_penalty_index_bytesize_expected * CHAR_BIT) + " bits!"
	);

	std::vector< Size > node_indices;
	if( global_node_indices.empty() ) {
		node_indices.reserve( choices_by_variable_node_expected.size() );
		for( Size i(0); i<choices_by_variable_node_expected.size(); ++i ) {
			node_indices.push_back( i );
		}
	} else {
		node_indices = global_node_indices;
	}

	switch( twobody_penalty_bytesize_expected ) {
		case sizeof( float ) : {
			switch( twobody_penalty_index_bytesize_expected ) {
				case 2 : {
					inner_decode_twobody_penalties< uint16_t, float >( line, choices_by_variable_node_expected, n_twobody_penalties_expected, node_indices, problem_api );
					break;
				}
				case 4 : {
					inner_decode_twobody_penalties< uint32_t, float >( line, choices_by_variable_node_expected, n_twobody_penalties_expected, node_indices, problem_api );
					break;
				}
				case sizeof( Size ) : {
					inner_decode_twobody_penalties< Size, float >( line, choices_by_variable_node_expected, n_twobody_penalties_expected, node_indices, problem_api );
					break;
				}
				default : {
					MASALA_THROW( class_namespace_and_name(), "decode_twobody_penalties", "Two-body penalty node and choice indices must be 2, 4, or " + std::to_string( sizeof(Size) * CHAR_BIT )
						+ " bits, but the file indicates that these values take up " + std::to_string( twobody_penalty_index_bytesize_expected * CHAR_BIT ) + " bits!"
					);
				}
			}
			break;
		}
		case sizeof( Real ) : {
			switch( twobody_penalty_index_bytesize_expected ) {
				case 2 : {
					inner_decode_twobody_penalties< uint16_t, Real >( line, choices_by_variable_node_expected, n_twobody_penalties_expected, node_indices, problem_api );
					break;
				}
				case 4 : {
					inner_decode_twobody_penalties< uint32_t, Real >( line, choices_by_variable_node_expected, n_twobody_penalties_expected, node_indices, problem_api );
					break;
				}
				case sizeof( Size ) : {
					inner_decode_twobody_penalties< Size, Real >( line, choices_by_variable_node_expected, n_twobody_penalties_expected, node_indices, problem_api );
					break;
				}
				default : {
					MASALA_THROW( class_namespace_and_name(), "decode_twobody_penalties", "Two-body penalty node and choice indices must be 2, 4, or " + std::to_string( sizeof(Size) * CHAR_BIT )
						+ " bits, but the file indicates that these values take up " + std::to_string( twobody_penalty_index_bytesize_expected * CHAR_BIT ) + " bits!"
					);
				}
			}
			break;
		}
		default : {
			MASALA_THROW( class_namespace_and_name(), "decode_twobody_penalties", "Two-body penalties must be single- or double-precision floating-point values (i.e. "
				+ std::to_string( sizeof(float) * CHAR_BIT ) + "- or " + std::to_string( sizeof(Real) * CHAR_BIT ) + "-bit floating-point values), but the file indicates that these "
				"values take up " + std::to_string( twobody_penalty_bytesize_expected * CHAR_BIT ) + " bits!"
			);
		}
	}
}

/// @brief Template function for decoding twobody penalties from a binary-as-ASCII-text string.  This is templated for different types of indices
/// (unsigned integers) and penalties (floating point numbers).
/// @tparam INDEXTYPE The data type for twobody penalty indices.  The file format actually stores global indices, which this function internally converts
/// to variable node index and choice index.  This compilation unit will support 16, 32, or sizeof(Size)*CHAR_BIT bit indices.
/// @tparam VALTYPE The data type for twobody penalties.  This compilation unit will support sizeof(float)*CHAR_BIT or sizeof(Real)*CHAR_BIT sized values.
/// @param[in] line The ASCII line we're decoding.
/// @param[in] choices_by_variable_node_expected The number of onebody penalties by variable node index.
/// @param[in] n_twobody_penalties_expected The number of pairs of twobody penalties that we expect to find.
/// @param[in] node_indices The global indices of the variable nodes.  Must be the same size as choices_by_variable_node_expected.  If
/// global node indices were not provided in the input file, this vector should be consecutively numbered indices starting from zero.
/// @param[inout] problem_api The cost function network optimization problem in which we're storing penalties.
/// @note This function will throw if the CostFunctionNetworkOptimizationProblem isn't a
/// PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.
template< typename INDEXTYPE, typename VALTYPE >
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::inner_decode_twobody_penalties(
	std::string const & line,
	std::vector< masala::base::Size > const & choices_by_variable_node_expected,
	masala::base::Size const n_twobody_penalties_expected,
	std::vector< masala::base::Size > const & node_indices,
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_API & problem_api
) const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	using namespace masala::core_api::utility;
	PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblemSP problem(
		std::dynamic_pointer_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem >( problem_api.get_inner_data_representation_object() )
	);
	CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "inner_decode_twobody_penalties", "The selected problem class, " + problem_api.inner_class_name() + ", is "
		"not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.  Cannot store precomputed twobody penalties."
	);

	Size const char_bytesize( static_cast< Size >( std::ceil(static_cast<Real>( sizeof( TwoBodyTuple< INDEXTYPE, VALTYPE > ) * n_twobody_penalties_expected ) / 3.0) ) * 4 );
	CHECK_OR_THROW_FOR_CLASS( line.size() == char_bytesize, "inner_decode_twobody_penalties",
		"Expected " + std::to_string( char_bytesize ) + " bytes of ASCII data, but got " +
		std::to_string( line.size() ) + ".  Could not parse twobody penalties binary data."
	);

	std::vector< TwoBodyTuple< INDEXTYPE, VALTYPE > > twobody_penalties_by_global_choice_indices( n_twobody_penalties_expected );
	decode_data_from_string( (unsigned char *)( &twobody_penalties_by_global_choice_indices[0] ), line, sizeof( TwoBodyTuple< INDEXTYPE, VALTYPE > ) * n_twobody_penalties_expected );

	// DELETE THE FOLLOWING; FOR DEBUGGING ONLY:
	// std::cout << "TWOBODY PENALTIES:" << std::endl;
	// for( TwoBodyTuple< INDEXTYPE, VALTYPE > const & entry : twobody_penalties_by_global_choice_indices ) {
	// 	std::cout << entry.choice1_ << "\t" << entry.choice2_ << "\t" << entry.penalty_ << std::endl; 
	// }

	for( TwoBodyTuple< INDEXTYPE, VALTYPE > const & entry : twobody_penalties_by_global_choice_indices ) {
		std::pair< Size, Size > const indices1( node_and_choice_from_global_index( entry.choice1_, choices_by_variable_node_expected ) );
		std::pair< Size, Size > const indices2( node_and_choice_from_global_index( entry.choice2_, choices_by_variable_node_expected ) );
		CHECK_OR_THROW_FOR_CLASS( indices1.first < node_indices.size(), "inner_decode_twobody_penalties", "Variable node index " + std::to_string( indices1.first )
			+ " is out of range.  Expected " + std::to_string(node_indices.size()) + " variable nodes."
		);
		CHECK_OR_THROW_FOR_CLASS( indices2.first < node_indices.size(), "inner_decode_twobody_penalties", "Variable node index " + std::to_string( indices2.first )
			+ " is out of range.  Expected " + std::to_string(node_indices.size()) + " variable nodes."
		);
		problem->set_twobody_penalty( std::make_pair( node_indices[indices1.first], node_indices[indices2.first] ), std::make_pair( indices1.second, indices2.second ), entry.penalty_ );
	}
}

/// @brief Given a global choice index and the number of choices per variable node, get the index of the variable node and local choice index.
/// @param[in] global_index The global choice index.  All choices are numbered consecutively from 0.
/// @param[in] n_choices_by_variable_node The number of choices at each variable index.
/// @return A pair of local node index and local choice index.
std::pair< masala::base::Size, masala::base::Size >
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::node_and_choice_from_global_index(
	masala::base::Size const global_index,
	std::vector< masala::base::Size > const & n_choices_by_variable_node
) const {
	Size accumulator(0), var_index(0);
	while( accumulator + n_choices_by_variable_node[var_index] <= global_index ) {
		accumulator += n_choices_by_variable_node[var_index];
		++var_index;
	}
	return std::make_pair( var_index, global_index - accumulator );
}

/// @brief Given a line consisting of a series of integer values, convert this to a vector of unsigned integers.
/// @returns A vector of integers, or, if the line could not be parsed as such, an empty vector.
std::vector< masala::base::Size >
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::parse_global_node_indices(
	std::string const & line
) const {
	if( line == "[END_BINARY_GRAPH_SUMMARY]" ) {
		return std::vector< Size >{};
	}
	std::vector< Size > outvec;
	std::istringstream ss(line);
	Size buffer;
	do {
		ss >> buffer;
		if( !(ss.bad() || ss.fail()) ) {
			outvec.push_back(buffer);
		} else {
			return std::vector< Size >{};
		}
	} while( !ss.eof() );
	return outvec;
}

/// @brief Given a set of lines starting with [BEGIN_BINARY_GRAPH_SUMMARY] and ending with [END_BINARY_GRAPH_SUMMARY],
/// convert these to a cost function network problem definition.
/// @param[in] lines A vector of file lines.
/// @param[in] line_begin The [BEGIN_BINARY_GRAPH_SUMMARY] line index.
/// @param[in] line_end The [END_BINARY_GRAPH_SUMMARY] line index.
/// @returns A cost function network optimization problem of the given type.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::cfn_problem_from_ascii_file_block(
	std::vector< std::string > const & lines,
	masala::base::Size const line_begin,
	masala::base::Size const line_end
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	using namespace masala::base::utility::string;

	CostFunctionNetworkOptimizationProblem_APISP problem( generate_cfn_problem() );
	CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "cfn_problem_from_ascii_file_block", "Unable to generate cost "
		"function network optimization problem.  This is a program error.  Please consult a developer."
	);

	Size read_step(0); // Which step are we on in reading the record?
	Size n_variable_nodes_expected(0), choicecount_bytesize_expected(0),
		n_onebody_penalties_expected(0), onebody_penalty_bytesize_expected(0),
		n_twobody_penalties_expected(0), twobody_penalty_index_bytesize_expected(0),
		twobody_penalty_bytesize_expected(0);
	std::vector< Size > choices_by_variable_node_expected;
	Size additional_ignored_line_count(0);

	// Get the global node indices, if available.
	std::vector< Size > global_node_indices;
	if( lines.size() > line_begin + 7 && line_end >= line_begin + 7 ) {
		global_node_indices = parse_global_node_indices( trim(lines[line_begin + 7]) );
	}

	for( Size i(line_begin); i<=line_end; ++i ) {
		std::string const linestripped( trim( lines[i] ) );
		switch( read_step ) {
			case 0 : {
				// Read BEGIN_BINARY_GRAPH_SUMMARY line.
				CHECK_OR_THROW_FOR_CLASS( linestripped == "[BEGIN_BINARY_GRAPH_SUMMARY]", "cfn_problem_from_ascii_file_block",
					"Expected the cost function network problem description to begin with \"[BEGIN_BINARY_GRAPH_SUMMARY]\"!"
				);
				++read_step;
				break;
			}
			case 1 : {
				// Read headers that tell us how many variable nodes there are and how many binary bytes are being
				// used to represent choice counts.
				std::istringstream ss(linestripped);
				ss >> n_variable_nodes_expected >> choicecount_bytesize_expected;
				CHECK_OR_THROW_FOR_CLASS( ss.eof() && !( ss.bad() || ss.fail() ), "cfn_problem_from_ascii_file_block", "Error parsing "
					"line \"" + linestripped + "\".  Expected two unsigned integer entries."
				);
				++read_step;
				break;
			}
			case 2 : {
				// Read the choice counts per variable node.
				CHECK_OR_THROW_FOR_CLASS( choicecount_bytesize_expected != 0, "cfn_problem_from_ascii_file_block", "Error "
					"reading cost function network problem description: got an integer bytesize of 0!"
				);
				decode_choices_per_variable_node( linestripped, n_variable_nodes_expected, choicecount_bytesize_expected, choices_by_variable_node_expected );
				if( !global_node_indices.empty() ) {
					CHECK_OR_THROW_FOR_CLASS( global_node_indices.size() == n_variable_nodes_expected, "cfn_problem_from_ascii_file_block",
						"Incorrect number of global node indices found.  Expected " + std::to_string( n_variable_nodes_expected ) + ", but got "
						+ std::to_string( global_node_indices.size() )
					);
				}
				++read_step;
				break;
			}
			case 3 : {
				// Read onebody penalty headers that tell us (a) how many onebody penalties (i.e. how many total choices)
				// we have, and (b) how many bytes are being used to represent onebody peanlties.
				std::istringstream ss(linestripped);
				ss >> n_onebody_penalties_expected >> onebody_penalty_bytesize_expected;
				CHECK_OR_THROW_FOR_CLASS( ss.eof() && !( ss.bad() || ss.fail() ), "cfn_problem_from_ascii_file_block", "Error parsing "
					"line \"" + linestripped + "\".  Expected two unsigned integer entries."
				);
				++read_step;
				break;
			}
			case 4 : {
				// Read onebody penalties list.
				decode_onebody_penalties( linestripped, choices_by_variable_node_expected,
					onebody_penalty_bytesize_expected, global_node_indices, *problem
				);
				++read_step;
				break;
			}
			case 5 : {
				// Read twobody penalty headers that tell us (a) how many twobody penalties we have, (b) how many
				// bytes are being used to represent node and choice indices, and (c) how many bytes are being used
				// to represent twobody penalties.
				std::istringstream ss(linestripped);
				ss >> n_twobody_penalties_expected >> twobody_penalty_index_bytesize_expected >> twobody_penalty_bytesize_expected;
				CHECK_OR_THROW_FOR_CLASS( ss.eof() && !( ss.bad() || ss.fail() ), "cfn_problem_from_ascii_file_block", "Error parsing "
					"line \"" + linestripped + "\".  Expected three unsigned integer entries."
				);
				++read_step;
				break;
			}
			case 6 : {
				// Read twobody penalties.
				decode_twobody_penalties( linestripped, choices_by_variable_node_expected,
					n_twobody_penalties_expected, twobody_penalty_index_bytesize_expected,
					twobody_penalty_bytesize_expected, global_node_indices, *problem
				);
				++read_step;
				break;
			}
			default : {
				// Ignore additional lines for now.
				if( linestripped != "[END_BINARY_GRAPH_SUMMARY]" ) {
					++additional_ignored_line_count;
				}
			}
		} // switch
	} // for


	if( !global_node_indices.empty() && additional_ignored_line_count > 0 ) { --additional_ignored_line_count; }

	if( additional_ignored_line_count > 0 ) {
		write_to_tracer( "Ignored " + std::to_string( additional_ignored_line_count ) + " extra lines at end of graph summary block." );
	}

	return problem;
}

/// @brief Check whether the cost function network problem class is valid (i.e. whether it exists).
/// @details Must be called from a mutex-locked or finalized context.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::check_cfn_problem_class() const {
	using namespace masala::base::managers::engine;
	if( !cfn_problem_class_.empty() ) {
		MasalaDataRepresentationRequest request;
		request.add_data_representation_category_requirement( {{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem" }}, true );
		request.add_data_representation_name_requirement( cfn_problem_class_ );
		std::vector<MasalaDataRepresentationCreatorCSP> const vec(
			MasalaDataRepresentationManager::get_instance()->get_compatible_data_representation_creators( request )
		);
		CHECK_OR_THROW_FOR_CLASS( !vec.empty(), "check_cfn_problem_class", "No cost function network optimization problem data representaiton "
			"subclass has been registered with name \"" + cfn_problem_class_ + "\"."
		);
		CHECK_OR_THROW_FOR_CLASS( vec.size() < 2, "check_cfn_problem_class", "More than one cost function network optimization problem data "
			"representation subclass has been registered with name \"" + cfn_problem_class_ + "\".  Use full class namespace and name, "
			"separated by double colons, to disambiguate.  For example, \""
			"my_masala_library::my_domain_application::MySpecializedCFNDataRepresentation\"."
		);
	}
}

/// @brief Check whether the cost function network optimizer class is valid (i.e. whether it exists).
/// @details Must be called from a mutex-locked or finalized context.
void
BinaryCostFunctionNetworkProblemRosettaFileInterpreter::check_cfn_optimizer_class() const {
	using namespace masala::base::managers::engine;
	if( !cfn_optimizer_class_.empty() ) {
		MasalaEngineRequest request;
		request.add_engine_category_requirement( {{ "Optimizer", "CostFunctionNetworkOptimizer" }}, true );
		request.add_engine_name_requirement( cfn_optimizer_class_ );
		std::vector<MasalaEngineCreatorCSP> const vec(
			MasalaEngineManager::get_instance()->get_compatible_engine_creators( request )
		);
		CHECK_OR_THROW_FOR_CLASS( !vec.empty(), "check_cfn_optimizer_class", "No cost function network optimizer engine "
			"subclass has been registered with name \"" + cfn_problem_class_ + "\"."
		);
		CHECK_OR_THROW_FOR_CLASS( vec.size() < 2, "check_cfn_optimizer_class", "More than one cost function network optimizer "
			"engine subclass has been registered with name \"" + cfn_problem_class_ + "\".  Use full class namespace and name, "
			"separated by double colons, to disambiguate.  For example, \""
			"my_masala_library::my_domain_application::MySpecializedCFNOptimizer\"."
		);
	}
}

} // namespace cost_function_network
} // namespace file_interpreters
} // namesapce standard_masala_plugins
