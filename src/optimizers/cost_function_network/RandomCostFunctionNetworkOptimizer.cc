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

/// @file src/optimizers/cost_function_network/RandomCostFunctionNetworkOptimizer.cc
/// @brief Implementations for a CostFunctionNetworkOptimizer that solves a cost function network problem by picking a
/// solution at random.
/// @details This is intended as a control during methods development, to compare a new solver to just choosing
/// a solution at random.  This is NOT intended to be a useful production solver.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit header:
#include <optimizers/cost_function_network/RandomCostFunctionNetworkOptimizer.hh>

// Optimizers headers:
#include <optimizers/cost_function_network/PairwisePrecomputedCostFunctionNetworkOptimizationProblem.hh>


// Numeric API headers:
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolution_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/cost_function_network/CostFunctionNetworkOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/annealing/AnnealingScheduleBase_API.hh>
#include <numeric/optimization/cost_function_network/CFNProblemScratchSpace.hh>

// Base headers:
#include <base/error/ErrorHandling.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/setter/MasalaObjectAPISetterDefinition_OneInput.tmpl.hh>
#include <base/api/setter/setter_annotation/OwnedSingleObjectSetterAnnotation.hh>
#include <base/api/setter/setter_annotation/PreferredTemplateDataRepresentationSetterAnnotation.hh>
#include <base/api/setter/setter_annotation/DeprecatedSetterAnnotation.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/threads/MasalaThreadedWorkRequest.hh>
#include <base/managers/threads/MasalaThreadedWorkExecutionSummary.hh>
#include <base/managers/random/MasalaRandomNumberGenerator.hh>
#include <base/managers/plugin_module/MasalaPluginModuleManager.hh>
#include <base/managers/engine/MasalaDataRepresentationManager.hh>
#include <base/managers/engine/MasalaDataRepresentationCreator.hh>
#include <base/managers/engine/data_representation_request/MasalaDataRepresentationNameRequirementCriterion.hh>
#include <base/managers/engine/MasalaDataRepresentationRequest.hh>
#include <base/utility/container/container_util.tmpl.hh>

// STL headers:
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <iostream>

namespace standard_masala_plugins {
namespace optimizers {
namespace cost_function_network {

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Copy constructor.
/// @details Needed since we define a mutex.
RandomCostFunctionNetworkOptimizer::RandomCostFunctionNetworkOptimizer(
	RandomCostFunctionNetworkOptimizer const & src
) :
	masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer( src ) // Calls protected_assign(), but only for the base class, since this is a constructor.
{
	std::lock( src.cfn_solver_mutex(), cfn_solver_mutex() );
	std::lock_guard< std::mutex > lock( src.cfn_solver_mutex(), std::adopt_lock );
	std::lock_guard< std::mutex > lock2( cfn_solver_mutex(), std::adopt_lock );
	RandomCostFunctionNetworkOptimizer::protected_assign(src); // Repeats call to parent class protected_assign(), but that's okay.  Needed since virtual function calls aren't possible in constructors.
}

/// @brief Assignment operator.
/// @details Needed since we define a mutex.
RandomCostFunctionNetworkOptimizer &
RandomCostFunctionNetworkOptimizer::operator=( RandomCostFunctionNetworkOptimizer const & src ) {
	masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer::operator=( src ); // Calls protected_assign().
	return *this;
}

/// @brief Make a copy of this object that's wholly independent.
RandomCostFunctionNetworkOptimizerSP
RandomCostFunctionNetworkOptimizer::deep_clone() const {
	RandomCostFunctionNetworkOptimizerSP new_optimizer( masala::make_shared< RandomCostFunctionNetworkOptimizer >(*this) );
	new_optimizer->make_independent();
	return new_optimizer;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the category or categories for this plugin class.  Default for all optimizers;
/// may be overridden by derived classes.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }
/// @note Categories are hierarchical (e.g. Selector->AtomSelector->AnnotatedRegionSelector,
/// stored as { {"Selector", "AtomSelector", "AnnotatedRegionSelector"} }). A plugin can be
/// in more than one hierarchical category (in which case there would be more than one
/// entry in the outher vector), but must be in at least one.  The first one is used as
/// the primary key.
std::vector< std::vector< std::string > >
RandomCostFunctionNetworkOptimizer::get_categories() const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return PluginCostFunctionNetworkOptimizer::get_categories();
}

/// @brief Get the keywords for this plugin class.  Default for all optimizers; may be overridden
/// by derived classes.
/// @returns { "optimizer", "cost_function_network", "numeric", "random", "stochastic" }
std::vector< std::string >
RandomCostFunctionNetworkOptimizer::get_keywords() const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	std::vector< std::string > keywords( PluginCostFunctionNetworkOptimizer::get_keywords() );
	keywords.push_back( "random" );
	keywords.push_back( "stochastic" );
	return keywords;
}

/// @brief Get the category that this MasalaEngine fits into.
/// @returns { { "Optimizer", "CostFunctionNetworkOptimizer" } }.
std::vector< std::vector< std::string > >
RandomCostFunctionNetworkOptimizer::get_engine_categories() const {
	using namespace masala::numeric_api::base_classes::optimization::cost_function_network;
	return PluginCostFunctionNetworkOptimizer::get_engine_categories();
}

/// @brief Get the keywords that this MasalaEngine has.
/// @returns { "optimizer", "cost_function_network", "numeric", "random", "stochastic" }
std::vector< std::string >
RandomCostFunctionNetworkOptimizer::get_engine_keywords() const {
	return get_keywords();
}

/// @brief Get the class name.
/// @returns "RandomCostFunctionNetworkOptimizer".
std::string
RandomCostFunctionNetworkOptimizer::class_name() const {
	return class_name_static();
}

/// @brief Get the class namespace.
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
std::string
RandomCostFunctionNetworkOptimizer::class_namespace() const {
	return class_namespace_static();
}

/// @brief Get the class name (static version).
/// @returns "RandomCostFunctionNetworkOptimizer".
//static
std::string
RandomCostFunctionNetworkOptimizer::class_name_static() {
	return "RandomCostFunctionNetworkOptimizer";
}

/// @brief Get the class namespace (static version).
/// @returns "standard_masala_plugins::optimizers::cost_function_network".
//static
std::string
RandomCostFunctionNetworkOptimizer::class_namespace_static() {
	return "standard_masala_plugins::optimizers::cost_function_network";
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC API FUNCTION
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the API definition for this object.
masala::base::api::MasalaObjectAPIDefinitionCWP
RandomCostFunctionNetworkOptimizer::get_api_definition() {
	using namespace masala::base::managers::plugin_module;
	using namespace masala::base::api;
	using namespace masala::base::api::constructor;
	using namespace masala::base::api::setter;
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;
	using namespace masala::numeric_api::auto_generated_api::optimization::annealing;
	using masala::base::Size;

	std::lock_guard< std::mutex > lock( cfn_solver_mutex() );
	if( api_description_ == nullptr ) {
		MasalaObjectAPIDefinitionSP api_description(
			masala::make_shared< MasalaObjectAPIDefinition >(
				*this,
				"a CostFunctionNetworkOptimizer that solves a cost function network problem by picking a "
				"solution at random.  This is intended as a control during methods development, to compare "
				"a new solver to just choosing a solution at random.  This is NOT intended to be a useful "
				"production solver.",
				false, false
			)
		);

		// Constructors:
		ADD_PUBLIC_CONSTRUCTOR_DEFINITIONS( RandomCostFunctionNetworkOptimizer, api_description );

		// Setters:
		{
			MasalaObjectAPISetterDefinition_OneInputSP< masala::base::managers::engine::MasalaDataRepresentationAPICSP const & > template_setter(
				masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< masala::base::managers::engine::MasalaDataRepresentationAPICSP const & > >(
					"set_template_preferred_cfn_data_representation", "Set a template cost function network optimization problem data representation, "
					"configured by the user but with no data entered.  This can optionally be passed in, in which case the get_template_preferred_cfn_data_representation() "
					"function can be used to retrieve a deep clone.  This allows the solver to cache its preferred data representation with its setup.",
					"representation_in", "A fully configured but otherwise empty data representation object, to be cached.  Deep clones will be retrievable with the "
					"get_template_preferred_cfn_data_representation() function when calling code wants to start populating a data representation with data.",
					true, false,
					std::bind( &RandomCostFunctionNetworkOptimizer::set_template_preferred_cfn_data_representation, this, std::placeholders::_1 )
				)
			);
			setter_annotation::PreferredTemplateDataRepresentationSetterAnnotationSP annotation(
				masala::make_shared< setter_annotation::PreferredTemplateDataRepresentationSetterAnnotation >()
			);
			annotation->set_data_representation_manager_info(
				std::vector< std::string >{ "OptimizationProblem", "CostFunctionNetworkOptimizationProblem" },
				std::vector< std::string >{ "cpu" },
				*template_setter,
				true
			);
			template_setter->add_setter_annotation( annotation );
			api_description->add_setter( template_setter );
		}
		api_description->add_setter(
			masala::make_shared< MasalaObjectAPISetterDefinition_OneInput< Size > > (
				"set_attempts_per_problem", "Sets the number of times to generate a random solution for each problem.",
				"attempts_in", "The number of times to try each problem.  Minimum 1.", false, false,
				std::bind( &RandomCostFunctionNetworkOptimizer::set_attempts_per_problem, this, std::placeholders::_1 )
			)
		);

		// Getters:
		api_description->add_getter(
			masala::make_shared< MasalaObjectAPIGetterDefinition_ZeroInput< Size > > (
				"attempts_per_problem", "Gets the number of times to try each problem.",
				"attempts_per_problem", "The number of times to try each problem.  Minimum 1.", false, false,
				std::bind( &RandomCostFunctionNetworkOptimizer::attempts_per_problem, this )
			)
		);

		// Work functions:
		api_description->add_work_function(
			masala::make_shared<
				MasalaObjectAPIWorkFunctionDefinition_OneInput<
					std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >,
					masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const &
				>
			>(
				"run_cost_function_network_optimizer", "Run the optimizer on a set of cost function network optimization problems, and produce a set solutions for each problem.",
				true, false, true, false,
				"problems", "A set of problems to run.",
				"solutions", "A vector of solution sets.  Each CostFunctionNetworkOptimizationSolutions object contains the set of solutions for the problem "
				"in the input vector with the corresponding index.  There may be multiple solutions, depending on settings.",
				std::bind( &RandomCostFunctionNetworkOptimizer::run_cost_function_network_optimizer, this, std::placeholders::_1 )
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

/// @brief Set the number of times to try each problem.
/// @details Minimum is 1.
void
RandomCostFunctionNetworkOptimizer::set_attempts_per_problem(
	masala::base::Size const attempts_in
) {
	CHECK_OR_THROW_FOR_CLASS( attempts_in > 0, "set_attempts_per_problem", "The number of attempts per problem must be greater than zero." );
	std::lock_guard< std::mutex > lock( cfn_solver_mutex() );
	attempts_per_problem_ = attempts_in;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC GETTERS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get the number of times to try each problem.
/// @details Minimum is 1.
masala::base::Size
RandomCostFunctionNetworkOptimizer::attempts_per_problem() const {
	std::lock_guard< std::mutex > lock( cfn_solver_mutex() );
	return attempts_per_problem_;
}

////////////////////////////////////////////////////////////////////////////////
// PUBLIC WORK FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Run the optimizer on a cost function network optimization problem, and produce a solution.
/// @details Must be implemented by derived classes.  Each solutions set in the vector of solutions corresponds to
/// the problem with the same index.
std::vector< masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationSolutions_APICSP >
RandomCostFunctionNetworkOptimizer::run_cost_function_network_optimizer(
	masala::numeric_api::auto_generated_api::optimization::cost_function_network::CostFunctionNetworkOptimizationProblems_API const & problems
) const { 
	using namespace masala::base::managers::random;
	using namespace masala::numeric_api::auto_generated_api::optimization::cost_function_network;
	using masala::base::Size;
	using masala::base::Real;

	std::lock_guard< std::mutex > lock( cfn_solver_mutex() );

	// Get the handle of the random generator:
	MasalaRandomNumberGeneratorHandle rg( MasalaRandomNumberGenerator::get_instance() );

	// Create storange for solutions and generate random solutions:
	Size const nproblems( problems.n_problems() );
	std::vector< CostFunctionNetworkOptimizationSolutions_APICSP > solutions_by_problem;
	solutions_by_problem.reserve( nproblems );
	for( Size i(0); i<nproblems; ++i ) {
		CostFunctionNetworkOptimizationProblem_APICSP problem( std::dynamic_pointer_cast< CostFunctionNetworkOptimizationProblem_API const >( problems.problem(i) ) );
		CHECK_OR_THROW_FOR_CLASS( problem != nullptr, "run_cost_function_network_optimizer", "Problem " + std::to_string(i) + " was of type "
			+ problems.problem(i)->inner_class_name() + ", which could not be interpreted as a CostFunctionNetworkOptimizationProblem."
		);

		masala::numeric::optimization::cost_function_network::CFNProblemScratchSpaceSP scratchspace(
			problem->generate_cfn_problem_scratch_space() // May or may not be nullptr.
		);

		masala::numeric_api::auto_generated_api::optimization::OptimizationSolutions_APISP new_solutions_container_uncast(
			problem->create_solutions_container()
		);
		CostFunctionNetworkOptimizationSolutions_APISP new_solutions_container(
			std::dynamic_pointer_cast< CostFunctionNetworkOptimizationSolutions_API >(
				new_solutions_container_uncast
			)
		);
		CHECK_OR_THROW_FOR_CLASS( new_solutions_container != nullptr, "run_cost_function_network_optimizer", "Problem "
			+ std::to_string(i) + " created a " + new_solutions_container_uncast->inner_class_name() + " container, but this function "
			"only works with CostFunctionNetworkOptimizationSolutions containers.  Program error.  Please consult a developer, as "
			"this ought not to happen."
		);

		std::vector< std::pair< Size, Size > > const nchoices_at_varnodes( problem->n_choices_at_variable_nodes() );
		Size const nvarnodes( nchoices_at_varnodes.size() );
		for( Size iattempt(0); iattempt < attempts_per_problem_; ++iattempt ) {
			std::vector< Size > soln_vec( nvarnodes );
			for( Size inode(0); inode < nvarnodes; ++inode ) {
				DEBUG_MODE_CHECK_OR_THROW_FOR_CLASS( nchoices_at_varnodes[inode].second > 0, "run_cost_function_network_optimizer",
					"Expected a nonzero number of choices for variable node " + std::to_string( inode )
					+ ", but got 0!"
				);
				soln_vec[inode] = rg->uniform_size_distribution( 0, nchoices_at_varnodes[inode].second - 1 );
			}
			new_solutions_container->merge_in_lowest_scoring_solutions(
				std::vector< std::tuple< std::vector< Size >, Real, Size > >{ std::make_tuple( soln_vec, problem->compute_absolute_score( soln_vec, scratchspace.get() ), 1 ) },
				attempts_per_problem_,
				problem,
				scratchspace.get()
			);
		}

		solutions_by_problem.push_back( new_solutions_container );
	}
	solutions_by_problem.shrink_to_fit();

	return solutions_by_problem;
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assign src to this object.  Must be implemented by derived classes.  Performs no mutex-locking.  Derived classes should call their parent's protected_assign().
void
RandomCostFunctionNetworkOptimizer::protected_assign(
	PluginCostFunctionNetworkOptimizer const & src
) {
	RandomCostFunctionNetworkOptimizer const * src_cast_ptr( dynamic_cast< RandomCostFunctionNetworkOptimizer const * >( &src ) );
	CHECK_OR_THROW_FOR_CLASS( src_cast_ptr != nullptr, "protected_assign", "Could not interpret source object of type " + src.class_name() + " as a RandomCostFunctionNetworkOptimizer object." );

	attempts_per_problem_ = src_cast_ptr->attempts_per_problem_;
	
	masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer::protected_assign( src );
}

/// @brief Make this object independent of any of its copies (i.e. deep-clone all of its internal data).
void
RandomCostFunctionNetworkOptimizer::protected_make_independent() {
	api_description_ = nullptr;
	masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer::make_independent();
}

/// @brief Set a template cost function network optimization problem data representation, configured by the user but with no data entered.
/// @details This can optionally be passed in, in which case the get_template_preferred_cfn_data_representation() function can be
/// used to retrieve a deep clone.  This allows the solver to cache its preferred data representation with its setup.
/// @note This version performs no mutex-locking, and is called by set_template_preferred_cfn_data_representation(), which does lock the mutex.
/// This version is virtual to allow derived classes to override it, to add checks of their own.  If overridden, the override should call the
/// base class to set the variable internally.
void
RandomCostFunctionNetworkOptimizer::protected_set_template_preferred_cfn_data_representation(
	masala::base::managers::engine::MasalaDataRepresentationAPICSP const & representation_in
) {
	masala::numeric_api::base_classes::optimization::cost_function_network::PluginCostFunctionNetworkOptimizer::protected_set_template_preferred_cfn_data_representation( representation_in );
}

/// @brief If the template preferred CFN data representation has not been set, return a default CFN data representation.
/// @details This version returns a PairwisePrecomputedCostFunctionNetworkOptimizationProblem, with default configuration.  Performs no mutex-locking.
masala::base::managers::engine::MasalaDataRepresentationAPISP
RandomCostFunctionNetworkOptimizer::protected_get_default_template_preferred_cfn_data_representation() const {
	using namespace masala::base::managers::engine;
	using namespace masala::base::managers::engine::data_representation_request;
	using masala::base::Size;

	MasalaDataRepresentationNameRequirementCriterionSP name_criterion( masala::make_shared< MasalaDataRepresentationNameRequirementCriterion >( "PairwisePrecomputedCostFunctionNetworkOptimizationProblem" ) );
	MasalaDataRepresentationRequest request;
	request.add_data_representation_criterion( name_criterion );
	std::vector< MasalaDataRepresentationCreatorCSP > creators( MasalaDataRepresentationManager::get_instance()->get_compatible_data_representation_creators( request ) );
	CHECK_OR_THROW_FOR_CLASS( creators.size() >= 1, "protected_get_default_template_preferred_cfn_data_representation", "Could not find the PairwisePrecomputedCostFunctionNetworkOptimizationProblem class."
		"  Was it registered with the Masala data representation manager?  (Note that the register_library() function of each plugin library must be called before the library is used.)"
	);
	bool found(false);
	Size found_index(0);
	for( auto const & creator : creators ) {
		if( creator->get_plugin_object_namespace_and_name() == "standard_masala_plugins::optimizers::cost_function_network::PairwisePrecomputedCostFunctionNetworkOptimizationProblem" ) {
			found = true;
			break;
		}
		++found_index;
	}
	CHECK_OR_THROW_FOR_CLASS( found, "protected_get_default_template_preferred_cfn_data_representation", "Could not find the PairwisePrecomputedCostFunctionNetworkOptimizationProblem class."
		"  Was it registered with the Masala data representation manager?  (Note that the register_library() function of each plugin library must be called before the library is used.)"
	);

	MasalaDataRepresentationAPISP pairwise_cfn_api( creators[found_index]->create_data_representation() );
	PairwisePrecomputedCostFunctionNetworkOptimizationProblemSP pairwise_cfn( std::dynamic_pointer_cast< PairwisePrecomputedCostFunctionNetworkOptimizationProblem >( pairwise_cfn_api->get_inner_data_representation_object() ) );
	CHECK_OR_THROW_FOR_CLASS( pairwise_cfn != nullptr, "protected_get_default_template_preferred_cfn_data_representation", "The created object "
		"could not be interpreted as a PairwisePrecomputedCostFunctionNetworkOptimizationProblem.  This is a program error.  Please consult a developer."
	);

	// Could configure the pairwise_cfn object here if needed.
	return pairwise_cfn_api;
}

} // namespace cost_function_network
} // namespace optimizers
} // namesapce standard_masala_plugins