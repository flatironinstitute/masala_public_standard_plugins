/*
    Masala
    Copyright (C) 2026 Vikram K. Mulligan

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

/// @file src/optimizers/utility/ExampleCFNProblemLoader.cc
/// @brief Implementations for a utility class that loads 100 example CFN problems, which are small enough to solve
/// using the Toulbar2 deterministic solver.
/// @details These problems are intended for unit-testing new CFN solvers.  Pretty much any CFN solver
/// should be able to find the optimal solution for these.  The solutions are also produced by the loader.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Class headers:
#include <optimizers/utility/ExampleCFNProblemLoader.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/managers/environment/MasalaEnvironmentManager.hh>
#include <base/managers/disk/MasalaDiskManager.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>

// Numeric API headers:
#include <numeric_api/base_classes/optimization/cost_function_network/PluginCostFunctionNetworkOptimizer.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/OptimizationProblem_API.hh>

// Numeric headers:
#include <numeric/optimization/cost_function_network/CFNProblemScratchSpace.hh>

// STL headers
#include <string>
#include <sstream>

namespace standard_masala_plugins {
namespace optimizers {
namespace utility {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION, DESTRUCTION, ETC.
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.  Must be explicit due to mutex.
ExampleCFNProblemLoader::ExampleCFNProblemLoader(
	ExampleCFNProblemLoader const & src
) :
	Parent(src)
{
	std::lock< std::mutex >( mutex_, src.mutex_ );
	std::lock_guard< std::mutex > lockthis( mutex_, std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.mutex_, std::adopt_lock );
	protected_assign(src);
}

/// @brief Assignment operator.  Must be explicit due to mutex.
ExampleCFNProblemLoader &
ExampleCFNProblemLoader::operator=(
	ExampleCFNProblemLoader const & src
) {
	std::lock< std::mutex >( mutex_, src.mutex_ );
	std::lock_guard< std::mutex > lockthis( mutex_, std::adopt_lock );
	std::lock_guard< std::mutex > lockthat( src.mutex_, std::adopt_lock );
	protected_assign(src);
	return *this;
}

/// @brief Every class can name itself.
/// @returns "ExampleCFNProblemLoader".
std::string
ExampleCFNProblemLoader::class_name() const {
	return "ExampleCFNProblemLoader";
}

/// @brief Every class can provide its own namespace.
/// @returns "standard_masala_plugins::optimizers::utility".
std::string
ExampleCFNProblemLoader::class_namespace() const {
	return "standard_masala_plugins::optimizers::utility";
}

/// @brief Make a copy of this object.
ExampleCFNProblemLoaderSP
ExampleCFNProblemLoader::clone() const {
	return masala::make_shared< ExampleCFNProblemLoader >( *this );
}

/// @brief Make a fully independent copy of this object.
ExampleCFNProblemLoaderSP
ExampleCFNProblemLoader::deep_clone() const {
	ExampleCFNProblemLoaderSP object_copy( masala::make_shared< ExampleCFNProblemLoader >( *this ) );
	object_copy->make_independent();
	return object_copy;
}

/// @brief Ensure that this object doesn't share any data with anything else, by
/// deep-cloning all of its internal data.
void
ExampleCFNProblemLoader::make_independent() {
	std::lock_guard< std::mutex > lock( mutex_ );
	protected_make_independent();
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get an object describing the API for this object.
/// @details Default implementation returns nullptr.  May be overridden by
/// derived objects.
masala::base::api::MasalaObjectAPIDefinitionCWP
ExampleCFNProblemLoader::get_api_definition() {
	using masala::base::Size;
	using namespace masala::base::api;
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	std::lock_guard< std::mutex > lock( mutex_ );
	if( api_definition_ == nullptr ) {
		MasalaObjectAPIDefinitionSP apidef(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"A utility class class that loads 100 example CFN problems, which are small enough to solve "
				"using the Toulbar2 deterministic solver.  These problems are intended for unit-testing new CFN "
				"solvers.  Pretty much any CFN solver should be able to find the optimal solution for these.  "
				"The solutions are also produced by the loader.",
				false, false
			)
		);
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( ExampleCFNProblemLoader, apidef );

		// Work functions:
		apidef->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< void, std::string const & > >(
				"initialize_from_optimizer_type_name",
				"Load problems and solutions from disk, and cache them in this object in a format compatible with a given optimizer.  Throws if already initialized.  "
				"This version picks the preferred data representation given the name of a type of optimizer.",
				false, false, false, false,
				"optimizer_name", "The name of a type of optimizer type that will be accepting the problems, used to determine the problem data representation to generate.",
				"void", "This function returns nothing.",
				std::bind( &ExampleCFNProblemLoader::initialize_from_optimizer_type_name, this, std::placeholders::_1 )
			)
		);
		apidef->add_work_function(
			masala::make_shared< MasalaObjectAPIWorkFunctionDefinition_OneInput< void, std::string const & > >(
				"initialize_from_problem_type_name",
				"Load problems and solutions from disk, and cache them in this object in a format compatible with a given optimizer.  Throws if already initialized.  "
				"This version accepts the name of a CFN problem data representation to generate.",
				false, false, false, false,
				"problem_type_name", "The name of a type of CFN problem data representation to generate.",
				"void", "This function returns nothing.",
				std::bind( &ExampleCFNProblemLoader::initialize_from_problem_type_name, this, std::placeholders::_1 )
			)
		);

		// Getters:
		apidef->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< std::vector< std::string > const & > >(
				"get_problem_names",
				"Get a vector of problem names.  Throws if not initialized.",
				"problem_names", "A set of 400 problem names.",
				false, false,
				std::bind( &ExampleCFNProblemLoader::get_problem_names, this )
			)
		);	
		apidef->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< CostFunctionNetworkOptimizationProblems_APISP > >(
				"get_problems",
				"Returns a container of 400 problems.  Throws if problems and solutions have not already been "
				"loaded and cached.  These problems are not finalized.",
				"problems", "A set of 400 CFN problems, in an appropriate format.  Not finalized.",
				false, false,
				std::bind( static_cast<CostFunctionNetworkOptimizationProblems_APISP(ExampleCFNProblemLoader::*)() const>( &ExampleCFNProblemLoader::get_problems ), this )
			)
		);
		apidef->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_OneInput< CostFunctionNetworkOptimizationProblems_APISP, Size const > >(
				"get_problems",
				"Returns a container of n problems (where 0 < n <= 400).  Throws if problems and solutions "
				"have not already been loaded and cached.  These problems are not finalized.",
				"n_problems", "The number of problems to return.  Must be in the range [1, 400].  If smaller "
				"than 400, then the first n problems are returned.",
				"problems", "A set of n CFN problems, in an appropriate format.  Not finalized.",
				false, false,
				std::bind( static_cast<CostFunctionNetworkOptimizationProblems_APISP(ExampleCFNProblemLoader::*)( masala::base::Size const ) const>( &ExampleCFNProblemLoader::get_problems ), this, std::placeholders::_1 )
			)
		);
		apidef->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< std::vector< CostFunctionNetworkOptimizationSolutions_APISP > > >(
				"get_solutions",
				"Returns a container of 400 solutions.  Throws if problems and solutions have not already been "
				"loaded and cached.  These solutions are not finalized.",
				"solutions", "A set of 400 CFN solutions.  Not finalized.",
				false, false,
				std::bind( static_cast<std::vector< CostFunctionNetworkOptimizationSolutions_APISP>(ExampleCFNProblemLoader::*)() const>( &ExampleCFNProblemLoader::get_solutions ), this )
			)
		);
		apidef->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_OneInput< std::vector< CostFunctionNetworkOptimizationSolutions_APISP >, Size const > >(
				"get_solutions",
				"Returns a container of n solutions (where 0 < n <= 400).  Throws if problems and solutions "
				"have not already been loaded and cached.  These solutions are not finalized.",
				"n_solutions", "The number of solutions to return.  Must be in the range [1, 400].  If smaller "
				"than 400, then the first n solutions are returned.",
				"solutions", "A set of n CFN solutions.  Not finalized.",
				false, false,
				std::bind( static_cast<std::vector< CostFunctionNetworkOptimizationSolutions_APISP >(ExampleCFNProblemLoader::*)( masala::base::Size const ) const>( &ExampleCFNProblemLoader::get_solutions ), this, std::placeholders::_1 )
			)
		);

		// Nonconst to const:
		api_definition_ = apidef;
	}
	return api_definition_;
}

/// @brief Get the category or categories for this plugin class.
/// @details Pure virtual; must be implemented by derived classes.
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outer vector), but must be in at least one.  The first one is used as
/// the primary key.
/// @returns This override returns {{"Utility", "CFN_Utility", "ExampleCFNProblemLoader"}}.
std::vector< std::vector< std::string > >
ExampleCFNProblemLoader::get_categories() const {
	return {{"Utility", "CFN_Utility", "ExampleCFNProblemLoader"}};
}

/// @brief Get the keywords for this plugin class.
/// @details Pure virtual; must be implemented by derived classes.
/// @returns This override returns { "utility", "cost_function_network", "example_problem", "example_problem_loader" }.
std::vector< std::string >
ExampleCFNProblemLoader::get_keywords() const {
	return { "utility", "cost_function_network", "example_problem", "example_problem_loader" };
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get a vector of problem names.
/// @details Throws if not initialized.
std::vector< std::string > const &
ExampleCFNProblemLoader::get_problem_names() {
	std::lock_guard< std::mutex > lock( mutex_ );
	CHECK_OR_THROW_FOR_CLASS( !problem_names_.empty(), "get_problem_names", "This object must be initialized before this function is called." );
	return problem_names_;
}

/// @brief Returns a container of 400 problems.  Throws if problems and solutions  have not already been
/// loaded and cached.
/// @note These problems are not finalized.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
ExampleCFNProblemLoader::get_problems() const {
	std::lock_guard< std::mutex > lock( mutex_ );
	return protected_get_problems();
}

/// @brief Returns a container of n problems (where 0 < n <= 400).  Throws if problems and solutions 
/// have not already been loaded and cached.
/// @param[in] n_problems The number of problems to return.  Must be in the range [1, 400].  If smaller than
/// 400, then the first n problems are returned.
/// @note These problems are not finalized.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
ExampleCFNProblemLoader::get_problems( masala::base::Size const n_problems ) const {
	std::lock_guard< std::mutex > lock( mutex_ );
	return protected_get_problems( n_problems );
}

/// @brief Returns a container of 400 solutions.  Throws if problems and solutions have not already been
/// loaded and cached.
/// @note These solutions are not finalized.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP >
ExampleCFNProblemLoader::get_solutions() const {
	std::lock_guard< std::mutex > lock( mutex_ );
	return protected_get_solutions();
}

/// @brief Returns a container of n solutions (where 0 < n <= 400).  Throws if problems and solutions 
/// have not already been loaded and cached.
/// @param[in] n_solutions The number of solutions to return.  Must be in the range [1, 400].  If smaller than
/// 400, then the first n solutions are returned.
/// @note These solutions are not finalized.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP >
ExampleCFNProblemLoader::get_solutions( masala::base::Size const n_solutions ) const {
	std::lock_guard< std::mutex > lock( mutex_ );
	return protected_get_solutions( n_solutions );
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Load problems and solutions from disk, and cache them in this object in a format compatible with a given type of optimizer.
/// @details Throws if already initialized.
void
ExampleCFNProblemLoader::initialize_from_optimizer_type_name(
	std::string const & optimizer_name
) {
	std::lock_guard< std::mutex > lock( mutex_ );
	protected_initialize( optimizer_name, "" );
}

/// @brief Load problems and solutions from disk, and cache them in this object in a given CFN problem format.
/// @details Throws if already initialized.
void
ExampleCFNProblemLoader::initialize_from_problem_type_name(
	std::string const & problem_type_name
) {
	std::lock_guard< std::mutex > lock( mutex_ );
	protected_initialize( "", problem_type_name );
}

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Initialize this object from either an optimizer type name or a problem type name.
/// @details Performs no mutex-locking.  Throws if already initialized.
void
ExampleCFNProblemLoader::protected_initialize(
	std::string const & optimizer_name,
	std::string const & problem_type_name
) {
	using masala::base::Size;
	using masala::base::Real;
	using namespace masala::base::managers::plugin_module;
	using namespace masala::base::managers::environment;
	using namespace masala::base::managers::disk;
	using namespace masala::base::api;
	using namespace masala::base::api::work_function;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	CHECK_OR_THROW_FOR_CLASS( problems_ == nullptr && solutions_.empty() && problem_names_.empty(), "protected_initialize", "This object has already been initialized." );
	CHECK_OR_THROW_FOR_CLASS(
		(optimizer_name.empty() && (!problem_type_name.empty())) ||
		((!optimizer_name.empty()) && problem_type_name.empty()),
		"protected_initialize", "Only one of optimizer_name or problem_type_name should be provided to this function.  The other should be an empty string."
	)

	MasalaPluginModuleManagerHandle plugman( MasalaPluginModuleManager::get_instance() );
	MasalaPluginAPISP fileloader(
		plugman->create_plugin_object_instance_by_short_name(
			{"FileInterpreter", "CostFunctionNetworkProblemFileInterpreter"},
			"ASCIICostFunctionNetworkProblemRosettaFileInterpreter",
			true
		)
	);
	CHECK_OR_THROW_FOR_CLASS( fileloader != nullptr, "protected_initialize", "Could not create an instance "
		"of an ASCIICostFunctionNetworkProblemRosettaFileInterpreter class.  Has the Standard Masala Plugins "
		"library been loaded?"
	);

	// Get environment variables:
	std::string std_plugin_path;
	CHECK_OR_THROW_FOR_CLASS(
		MasalaEnvironmentManager::get_instance()->get_environment_variable("MASALA_STANDARD_PLUGINS", std_plugin_path ),
		"protected_initialize",
		"The MASALA_STANDARD_PLUGINS environment variable must be set to point at the Masala Standard Plugins directory."
	);

	// Loading the problems in:
	{
		// Get work functions:
		MasalaObjectAPIDefinitionCSP fileloader_apidef( fileloader->get_api_definition_for_inner_class().lock() );
		CHECK_OR_THROW_FOR_CLASS( fileloader_apidef != nullptr, "protected_initialize", "Could not get an API definition for the " + fileloader->inner_class_name() + " class." );
		MasalaObjectAPIWorkFunctionDefinition_OneInputCSP< CostFunctionNetworkOptimizationProblems_APISP, std::string const & > fileloader_load_fxn(
			fileloader_apidef->get_oneinput_work_function< CostFunctionNetworkOptimizationProblems_APISP, std::string const & >( "cfn_problems_from_ascii_file" ).lock()
		);
		CHECK_OR_THROW_FOR_CLASS( fileloader_load_fxn != nullptr, "protected_initialize", "The " + fileloader->inner_class_name() + " class does not appear to have "
			"a cfn_problems_from_ascii_file() function."
		);

		// Set the optimizer type:
		if( !optimizer_name.empty() ) {
			MasalaObjectAPIWorkFunctionDefinition_OneInputCSP< void, std::string const & > fileloader_setoptimizer_fxn(
				fileloader_apidef->get_oneinput_work_function< void, std::string const & >( "set_cfn_optimizer_type" ).lock()
			);
			CHECK_OR_THROW_FOR_CLASS( fileloader_setoptimizer_fxn != nullptr, "protected_initialize", "The " + fileloader->inner_class_name()
				+ " class does not appear to have a set_cfn_optimizer_type() function."
			);
			fileloader_setoptimizer_fxn->function( optimizer_name );
		} else {
			MasalaObjectAPIWorkFunctionDefinition_OneInputCSP< void, std::string const & > fileloader_setprobtype_fxn(
				fileloader_apidef->get_oneinput_work_function< void, std::string const & >( "set_cfn_problem_type_to_generate" ).lock()
			);
			CHECK_OR_THROW_FOR_CLASS( fileloader_setprobtype_fxn != nullptr, "protected_initialize", "The " + fileloader->inner_class_name()
				+ " class does not appear to have a set_cfn_problem_type_to_generate() function."
			);
			fileloader_setprobtype_fxn->function( problem_type_name );
		}

		// Load the problems:
		std::string const problem_path( std_plugin_path + "/database/small_cfn_problems/problems_concatenated.txt" );
		problems_ = fileloader_load_fxn->function( problem_path );
		CHECK_OR_THROW_FOR_CLASS( problems_ != nullptr, "protected_initialize", "Unable to load CFN problems from file " + problem_path + "." );
		CHECK_OR_THROW_FOR_CLASS( problems_->n_problems() == 400, "protected_initialize", "Expected 400 problems in container, but got " + std::to_string( problems_->n_problems() ) + "." );
		write_to_tracer( "Loaded " + std::to_string( problems_->n_problems() ) + " problems from file " + problem_path + "." );
	}

	// Load the solutions:
	{
		std::string const solution_path( std_plugin_path + "/database/small_cfn_problems/optimal_scores_and_solutions_zerobased.txt" );
		std::vector< std::string > const solutionlines( MasalaDiskManager::get_instance()->read_ascii_file_to_string_vector( solution_path ) );
		CHECK_OR_THROW_FOR_CLASS( solutionlines.size() == 400, "protected_initialize", "Expected 400 lines in file " + solution_path + ", but got " + std::to_string( solutionlines.size() ) + "." );
		Size counter(0);
		for( std::string const & line : solutionlines ) {
			std::istringstream ss(line);
			std::string probname;
			Real solutionscore;
			ss >> probname >> solutionscore;
			CHECK_OR_THROW_FOR_CLASS( !( ss.bad() || ss.fail() || ss.eof() ), "protected_initialize", "Failed to parse line \"" + line + "\" from file " + solution_path + "." );
			std::vector< Size > solnvec;
			while( !ss.eof() ) {
				Size val;
				ss >> val;
				CHECK_OR_THROW_FOR_CLASS( !(ss.bad() || ss.fail()), "protected_initialize", "Failure to parse solution from line \"" + line + "\" from file " + solution_path + "." );
				solnvec.push_back(val);
			}

			// Copy the current problem.
			CostFunctionNetworkOptimizationProblem_APISP curprob( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API >( problems_->problem(counter)->clone() ) );
			curprob->finalize();
			masala::numeric::optimization::cost_function_network::CFNProblemScratchSpaceSP curscratch( curprob->generate_cfn_problem_scratch_space() );
			CostFunctionNetworkOptimizationSolutions_APISP cursoln( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >( curprob->create_solutions_container() ) );
			CHECK_OR_THROW_FOR_CLASS( cursoln != nullptr, "protected_initialize", "Unable to create solutions container for problem " + std::to_string(counter) + "." );
			cursoln->merge_in_lowest_scoring_solutions(
				std::vector< std::tuple< std::vector< Size >, Real, Size > >{ std::make_tuple( solnvec, solutionscore, 1 ) },
				1,
				curprob,
				curscratch.get()
			);
			problem_names_.push_back(probname);
			solutions_.push_back(cursoln);

			++counter;
		}
		CHECK_OR_THROW_FOR_CLASS( problem_names_.size() == 400 && solutions_.size() == 400, "protected_initialize", "Expected 400 solutions, but got "
			+ std::to_string( problem_names_.size() ) + " problem names and " + std::to_string( solutions_.size() ) + " solutions."
		);
		write_to_tracer( "Loaded " + std::to_string( solutions_.size() ) + " solutions." );
	}
}

/// @brief Deep-clone all the internal data in this object.  Should be implemented by
/// derived classes, and the derived classes' implementations should call their parent classes'
/// implementations.
void
ExampleCFNProblemLoader::protected_make_independent() {
	using masala::base::Size;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	if( problems_ != nullptr ) {
		CostFunctionNetworkOptimizationProblems_APISP problems_copy( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblems_API >( problems_->clone() ) );
		CHECK_OR_THROW_FOR_CLASS( problems_copy != nullptr, "protected_make_independent", "Could not properly clone a " + problems_->inner_class_name() + " object." );
		problems_copy->make_independent();
		problems_ = problems_copy;
	}
	if( !solutions_.empty() ) {
		for( Size i(0); i<solutions_.size(); ++i ) {
			CostFunctionNetworkOptimizationSolutions_APISP solution_copy( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >( solutions_[i]->clone() ) );
			CHECK_OR_THROW_FOR_CLASS( solution_copy != nullptr, "protected_make_independent", "Could not properly clone solutions object " + std::to_string(i) + "." );
			solution_copy->make_independent();
			solutions_[i] = solution_copy;
		}
	}
}

/// @brief Assign src to this.  Performs no mutex locking.  Derived classes should
/// override this, and the overrides should call the parent class protected_assign().
void
ExampleCFNProblemLoader::protected_assign(
	ExampleCFNProblemLoader const & src
) {
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	if( src.problems_ != nullptr ) {
		problems_ = std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblems_API >( src.problems_->clone() );
		CHECK_OR_THROW_FOR_CLASS( problems_ != nullptr, "protected_assign", "Unable to clone problems from source." );
		problems_->make_independent();
	} else {
		problems_ = nullptr;
	}
	solutions_.clear();

	if( src.solutions_.size() > 0 ) {
		solutions_.reserve(src.solutions_.size());
		for( auto const & solution : src.solutions_ ) {
#ifdef NDEBUG
			CostFunctionNetworkOptimizationSolutions_APISP solution_copy( std::static_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >( solution->clone() ) );
#else
			CostFunctionNetworkOptimizationSolutions_APISP solution_copy( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >( solution->clone() ) );
			DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( solution_copy != nullptr, "protected_assign", "Could not clone a solutions object of type " + solution->class_name() + "." );
#endif
			solution_copy->make_independent();
			solutions_.push_back( solution_copy );
		}
	}

	problem_names_ = src.problem_names_;
}

/// @brief Clone the first N of the cached problems and package the clones into a problems container.
/// @details Throws if problems and solutions have not yet been loaded.
masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_APISP
ExampleCFNProblemLoader::protected_get_problems( masala::base::Size const n_problems /*= 400*/ ) const {
	using masala::base::Size;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	CHECK_OR_THROW_FOR_CLASS( n_problems > 0 && n_problems <= 400, "protected_get_problems", "Expected n_problems to be in the range [1,400], but got " + std::to_string(n_problems) + "." );
	CHECK_OR_THROW_FOR_CLASS( problems_ != nullptr, "protected_get_problems", "This object must be initialized before this function is called." );

	CostFunctionNetworkOptimizationProblems_APISP problems_copy( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblems_API >( problems_->clone() ) );
	CHECK_OR_THROW_FOR_CLASS( problems_copy != nullptr, "protected_get_problems", "Unable to clone problems." );

	if( n_problems < 400 ) {
		problems_copy->reset();
		problems_copy->make_independent();
		for( Size i(0); i<n_problems; ++i ) {
			CostFunctionNetworkOptimizationProblem_APISP problem_copy( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API >( problems_->problem(i)->clone() ) );
			CHECK_OR_THROW_FOR_CLASS( problems_copy != nullptr, "protected_get_problems", "Unable to clone problem " + std::to_string(i) + "." );
			problem_copy->make_independent();
			problems_copy->add_optimization_problem( problem_copy );
		}
	} else {
		problems_copy->make_independent();
	}

	return problems_copy;
}

/// @brief Clone the first N of the cached solutions and package the clones into a problems container.
/// @details Throws if problems and solutions have not yet been loaded.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APISP >
ExampleCFNProblemLoader::protected_get_solutions( masala::base::Size const n_solutions /*= 400*/ ) const {
	using masala::base::Size;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;

	CHECK_OR_THROW_FOR_CLASS( n_solutions > 0 && n_solutions <= 400, "protected_get_solutions", "Expected n_solutions to be in the range [1,400], but got " + std::to_string(n_solutions) + "." );
	CHECK_OR_THROW_FOR_CLASS( solutions_.size() == 400, "protected_get_solutions", "This object must be initialized before this function is called." );
	std::vector< CostFunctionNetworkOptimizationSolutions_APISP > solutions_copy;
	solutions_copy.reserve( n_solutions );
	for( Size i(0); i<n_solutions; ++i ) {
		CostFunctionNetworkOptimizationSolutions_APISP solution_copy( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >( solutions_[i]->clone() ) );
		CHECK_OR_THROW_FOR_CLASS( solution_copy != nullptr, "protected_get_solutions", "Unable to clone solution for problem " + std::to_string(i) + "." );
		solution_copy->make_independent();
		solutions_copy.push_back(solution_copy);
	}

	return solutions_copy;
}

} // namespace utility
} // namespace optimizers
} // namespace standard_masala_plugins
