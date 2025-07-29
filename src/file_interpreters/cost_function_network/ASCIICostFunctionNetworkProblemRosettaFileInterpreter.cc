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

/// @file src/file_interpreters/cost_function_network/ASCIICostFunctionNetworkProblemRosettaFileInterpreter.cc
/// @brief Implementation for a file interpreter for reading and writing ASCII files that define cost function network optimization problems.
/// @author Tristan Zaborniak (tzaborniak@flatironinstitute.org).

// Unit header:
#include <file_interpreters/cost_function_network/ASCIICostFunctionNetworkProblemRosettaFileInterpreter.hh>

// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/base_classes/optimization/cost_function_network/PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblemCreator.hh>
#include <numeric_api/base_classes/optimization/cost_function_network/PluginCostFunctionNetworkOptimizer.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>

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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::ASCIICostFunctionNetworkProblemRosettaFileInterpreter() :
	masala::base::managers::file_interpreter::MasalaFileInterpreter(),
	finalized_( false )
{}

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::ASCIICostFunctionNetworkProblemRosettaFileInterpreter(
	ASCIICostFunctionNetworkProblemRosettaFileInterpreter const & src
) :
	masala::base::managers::file_interpreter::MasalaFileInterpreter( src )
{
	std::lock_guard< std::mutex > lock( src.file_interpreter_mutex_ );
	protected_assign( src );
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
ASCIICostFunctionNetworkProblemRosettaFileInterpreter &
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::operator=( ASCIICostFunctionNetworkProblemRosettaFileInterpreter const & src ) {
	std::lock( file_interpreter_mutex_, src.file_interpreter_mutex_ );
	std::lock_guard< std::mutex > lock1( file_interpreter_mutex_, std::adopt_lock );
	std::lock_guard< std::mutex > lock2( src.file_interpreter_mutex_, std::adopt_lock );
	protected_assign(src);
	return *this;
}

/// @brief Make a copy of this object that's wholly independent.
ASCIICostFunctionNetworkProblemRosettaFileInterpreterSP
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::deep_clone() const {
	ASCIICostFunctionNetworkProblemRosettaFileInterpreterSP new_optimizer( masala::make_shared< ASCIICostFunctionNetworkProblemRosettaFileInterpreter >(*this) );
	new_optimizer->make_independent();
	return new_optimizer;
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::make_independent() {
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::get_categories() const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return { { "FileInterpreter", "CostFunctionNetworkProblemFileInterpreter", "ASCIICostFunctionNetworkProblemRosettaFileInterpreter" } };
}

/// @brief Get the keywords for this plugin class.
/// @returns { "file_interpreter", "cost_function_network", "problem", "ascii" }
std::vector< std::string >
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::get_keywords() const {
	return { "file_interpreter", "cost_function_network", "problem", "ascii" };
}

/// @brief All MasalaFileInterpreter subclasses must list the file types that they create.  These are
/// provided as short descriptors (e.g. "protein_data_bank_file").
/// @returns { "cost_function_network_problem_definition_file" }
std::vector< std::string >
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::get_file_descriptors() const {
	return { "cost_function_network_problem_definition_file" };
}

/// @brief All MasalaFileInterpreter subclasses must list the file type extensions that they create.
/// @details Extensions are provided in lower-case (e.g. "pdb").
/// @returns { "txt", "pdb" }
std::vector< std::string >
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::get_file_extensions() const {
	return { "txt", "pdb" };
}

/// @brief Get the class name.
/// @returns "ASCIICostFunctionNetworkProblemRosettaFileInterpreter".
std::string
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::class_name() const {
	return class_name_static();
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
std::string
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::class_namespace() const {
	return class_namespace_static();
}

/// @brief Get the class name (static version).
/// @returns "ASCIICostFunctionNetworkProblemRosettaFileInterpreter".
//static
std::string
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::class_name_static() {
	return "ASCIICostFunctionNetworkProblemRosettaFileInterpreter";
}

/// @brief Get the class namespace (static version).
/// @returns "standard_masala_plugins::file_interpreters::cost_function_network".
//static
std::string
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::class_namespace_static() {
	return "standard_masala_plugins::file_interpreters::cost_function_network";
}

/// @brief Indicate that this interpreter reads ASCII files.
/// @returns True.
bool
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::filetype_is_ascii() const {
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::get_api_definition() {
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
				"This class reads ascii cost function network optimization problem files (or packing problem files) written by Rosetta, "
				"and generates a user-selected type of cost function network optimization problem description.",
				false, false
			)
		);

		// Constructors:
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( ASCIICostFunctionNetworkProblemRosettaFileInterpreter, api_description );

		// Setters:
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< std::string const & > >(
				"set_cfn_problem_type_to_generate", "Set the name of the cost function network problem description class "
				"to generate.  This can be the short name or the full name (i.e. with or without namespace).  It need only "
				"include namespace if the short name is not unique.  If not set, then an optimizer must be specified instead.",
				"problem_type_name_in", "The name of the cost function network optimization problem subclass to generate.  Namespace is "
				"optional unless the name is not unique.", false, false,
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_problem_type_to_generate, this, std::placeholders::_1 )
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
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_optimizer_type, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< bool > >(
				"filetype_is_ascii", "Does this interpreter read ASCII file types, or binary?  This override returns "
				"true, indicating that this interpreter reads ASCII files.",
				"is_ascii", "Is the filetype read by this interpreter an ASCII format?  (Yes, it is, so this "
				"is always true.)", true, false,
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::filetype_is_ascii, this )
			)
		);

		// Work functions:
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				masala::base::MasalaObjectAPISP,
				std::vector< std::string > const &
			> >(
				"object_from_ascii_file_contents", "Read the contents of a Rosetta-format ascii cost "
				"function network problem file, and return a cost function network problem object (as "
				"a generic MasalaObject pointer).",
				true, false, false, false,
				"file_contents", "The contents of a Rosetta-style ascii cost function network optimization problem file (a.k.a. a packing problem file), "
				"expressed as a vector of strings (one string per file line).",
				"output_object", "A shared pointer to a container of cost function network optimization problems.",
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::object_from_ascii_file_contents, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP,
				std::vector< std::string > const &
			> >(
				"cfn_problems_from_ascii_file_contents", "Parse the contents of a Rosetta-format ascii cost function network problem "
				"file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).",
				true, false, false, false,
				"file_contents", "The contents of a Rosetta-style ascii cost function network optimization problem file (a.k.a. a packing problem file), "
				"expressed as a vector of strings (one string per file line).",
				"problems", "A shared pointer to a container of cost function network optimization problems.",
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file_contents, this, std::placeholders::_1 )
			)
		);
		api_description->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput<
				masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP,
				std::string const &
			> >(
				"cfn_problems_from_ascii_file", "Parse a Rosetta-format ascii cost function network problem "
				"file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).  Triggers disk i/o!  "
				"Throws if the file contents cannot be parsed, or if the file does not exist.  (Uses the disk manager for disk access.)",
				true, false, false, false,
				"filename", "The name of the file to read.",
				"problems", "A shared pointer to a container of cost function network optimization problems.",
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file, this, std::placeholders::_1 )
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
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_cfn_solution, this, std::placeholders::_1 )
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
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_object, this, std::placeholders::_1 )
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
				std::bind( &ASCIICostFunctionNetworkProblemRosettaFileInterpreter::object_to_file, this, std::placeholders::_1, std::placeholders::_2 )
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_problem_type_to_generate(
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::set_cfn_optimizer_type(
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

/// @brief Read the contents of a Rosetta-format ascii cost function network problem
/// file, and return a cost function network problem object (as a generic MasalaObject pointer).
/// @details This override calls cfn_problems_from_ascii_file_contents().
masala::base::MasalaObjectAPISP
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::object_from_ascii_file_contents(
	std::vector< std::string > const & filelines
) const {
	return cfn_problems_from_ascii_file_contents( filelines );
}

/// @brief Read the contents of a Rosetta-format ascii cost function network problem
/// file, and return a set of cost function network problem objects (as a CostFunctionNetworkProblems pointer).
/// @details Throws if at least one problem was not successfully parsed.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file_contents(
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
			if( linestripped == "[BEGIN ONEBODY SEQPOS/ROTINDEX/ENERGY]" ) {
				line_begin = linecounter;
				in_block = true;
			}
		} else {
			if( linestripped == "[END TWOBODY SEQPOS1/ROTINDEX1/SEQPOS2/ROTINDEX2/ENERGY]" ) {
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

/// @brief Read a Rosetta-format ascii cost function network problem file, and return a set of cost function network 
/// problem objects (as a CostFunctionNetworkProblems pointer).
/// @details Throws if at least one problem was not successfully parsed.
/// @note Triggers disk i/o (through the MasalaDiskAccessManager)!
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::cfn_problems_from_ascii_file(
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_object(
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::ascii_file_contents_from_cfn_solution(
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::protected_assign(
	ASCIICostFunctionNetworkProblemRosettaFileInterpreter const & src
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::generate_cfn_problem() const {
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

/// @brief Given a set of lines starting with [BEGIN ONEBODY SEQPOS/ROTINDEX/ENERGY] and ending with [END TWOBODY SEQPOS1/ROTINDEX1/SEQPOS2/ROTINDEX2/ENERGY],
/// convert these to a cost function network problem definition.
/// @param[in] lines A vector of file lines.
/// @param[in] line_begin The [BEGIN ONEBODY SEQPOS/ROTINDEX/ENERGY] line index.
/// @param[in] line_end The [END TWOBODY SEQPOS1/ROTINDEX1/SEQPOS2/ROTINDEX2/ENERGY] line index.
/// @returns A cost function network optimization problem of the given type.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblem_APISP
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::cfn_problem_from_ascii_file_block(
	std::vector< std::string > const & lines,
	masala::base::Size const line_begin,
	masala::base::Size const line_end
) const {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
   	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	using namespace masala::base::utility::string;

	CostFunctionNetworkOptimizationProblem_APISP problem_api( generate_cfn_problem() );
	CHECK_OR_THROW_FOR_CLASS( problem_api != nullptr, "cfn_problem_from_ascii_file_block", "Unable to generate cost "
		"function network optimization problem.  This is a program error.  Please consult a developer."
	);

    PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblemSP problem(
		std::dynamic_pointer_cast< PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem >( problem_api->get_inner_data_representation_object() )
	);
	CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "decode_onebody_penalties", "The selected problem class, " + problem_api->inner_class_name() + ", is "
		"not a PluginPairwisePrecomputedCostFunctionNetworkOptimizationProblem.  Cannot store precomputed onebody penalties."
	);

    bool onebody_line( false );
    bool twobody_line( false );

	for( Size i(line_begin); i<=line_end; ++i ) {
		std::string const linestripped( trim( lines[i] ) );
	
        if(        linestripped == "[BEGIN ONEBODY SEQPOS/ROTINDEX/ENERGY]" ) {
            onebody_line = true;
            continue;
        } else if( linestripped == "[END ONEBODY SEQPOS/ROTINDEX/ENERGY]" ){
            onebody_line = false;
            continue;
        } else if( linestripped == "[BEGIN TWOBODY SEQPOS1/ROTINDEX1/SEQPOS2/ROTINDEX2/ENERGY]" ){
            twobody_line = true;
            continue;
        } else if( linestripped == "[END TWOBODY SEQPOS1/ROTINDEX1/SEQPOS2/ROTINDEX2/ENERGY]" ) {
            break;
        } else if( onebody_line == true && linestripped != "[END ONEBODY SEQPOS/ROTINDEX/ENERGY]" ) {
                std::stringstream ss( linestripped );
                std::string item;
                std::vector<std::string> parts;
                while (std::getline(ss, item, '\t')) {
                    parts.push_back(item);
                }
                problem->set_onebody_penalty( std::stoi( parts[0] ), std::stoi( parts[1] ) - 1, std::stof( parts[2] ) );
        } else if( twobody_line == true && linestripped != "[END TWOBODY SEQPOS1/ROTINDEX1/SEQPOS2/ROTINDEX2/ENERGY]" ) {
                std::stringstream ss( linestripped );
                std::string item;
                std::vector<std::string> parts;
                while (std::getline(ss, item, '\t')) {
                    parts.push_back(item);
                }
                problem->set_twobody_penalty(
                    std::pair( std::stoul( parts[0] ), std::stoul( parts[2] ) ), 
                    std::pair( std::stoul( parts[1] ) - 1, std::stoul( parts[3] ) - 1 ), 
                    std::stof( parts[4] ) 
                );
        }
    }

	return problem_api;
}

/// @brief Check whether the cost function network problem class is valid (i.e. whether it exists).
/// @details Must be called from a mutex-locked or finalized context.
void
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::check_cfn_problem_class() const {
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
ASCIICostFunctionNetworkProblemRosettaFileInterpreter::check_cfn_optimizer_class() const {
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
