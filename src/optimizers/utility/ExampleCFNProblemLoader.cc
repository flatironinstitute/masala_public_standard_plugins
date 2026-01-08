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
#include <base/managers/plugin_module/MasalaPluginCreator.hh>
#include <base/api/MasalaObjectAPIDefinition.hh>
#include <base/api/constructor/MasalaObjectAPIConstructorMacros.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_ZeroInput.tmpl.hh>
#include <base/api/getter/MasalaObjectAPIGetterDefinition_OneInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_ZeroInput.tmpl.hh>
#include <base/api/work_function/MasalaObjectAPIWorkFunctionDefinition_OneInput.tmpl.hh>

// STL headers
#include <string>

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

////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Get an object describing the API for this object.
/// @details Default implementation returns nullptr.  May be overridden by
/// derived objects.
masala::base::api::MasalaObjectAPIDefinitionCWP
ExampleCFNProblemLoader::get_api_definition() {
	using namespace masala::base::api;
	using namespace masala::base::api::getter;
	using namespace masala::base::api::work_function;

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

		TODO TODO TODO;
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
// PROTECTED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// @brief Assign src to this.
void
ExampleCFNProblemLoader::protected_assign(
	ExampleCFNProblemLoader const & src
) {
	TODO TODO TODO;
}

} // namespace utility
} // namespace optimizers
} // namespace standard_masala_plugins
