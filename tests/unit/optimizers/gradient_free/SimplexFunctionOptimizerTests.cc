/*
    Standard Masala Plugins
    Copyright (C) 2024 Vikram K. Mulligan

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

/// @file tests/unit/optimizers/gradient_free/SimplexOptimizerTests.cc
/// @brief Unit tests for the SimplexFunctionOptimizer
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Unit testing library (Catch2) headers:
#include <external/catch2/single_include/catch2/catch.hpp>

// Unit headers:
#include <optimizers_api/auto_generated_api/gradient_free/SimplexFunctionOptimizer_API.hh>

// Registraton headers:
#include <registration_api/register_library.hh>

// Masala core headers:
#include <core_api/auto_generated_api/registration/register_core.hh>

// Masala base headers:
#include <base/types.hh>
#include <base/error/ErrorHandling.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/managers/tracer/MasalaTracerManager.hh>

// Masala numeric API headers:
#include <numeric_api/auto_generated_api/registration/register_numeric.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblems_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationProblem_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolutions_API.hh>
#include <numeric_api/auto_generated_api/optimization/real_valued_local/RealValuedFunctionLocalOptimizationSolution_API.hh>

// External headers:
#include <external/eigen/Eigen/Core>

// STL headers
#include <cmath>
#include <functional>

namespace standard_masala_plugins {
namespace tests {
namespace unit {
namespace optimizers {
namespace gradient_free {

masala::base::Real
test_gaussian(
	masala::base::Real const x,
	masala::base::Real const coeff,
	masala::base::Real const offset,
	masala::base::Real const stddev
) {
	return coeff * std::exp( -1.0 * std::pow( ( x - offset ) / stddev , 2.0 ) );
}

masala::base::Real
d_test_gaussian_dx(
	masala::base::Real const x,
	masala::base::Real const coeff,
	masala::base::Real const offset,
	masala::base::Real const stddev
) {
	return -2.0 * ( x - offset ) * coeff / ( stddev * stddev ) * std::exp( -1.0 * std::pow( ( x - offset ) / stddev , 2.0 ) );
}

/// @brief See util_test_fxn_1.png to see this plotted.
/// @details Has minima at (2.002, -2.018), (2.995, -0.999), and (3.397,-0.475).
/// Has maxima at (2.440, -0.376) and (3.266,-0.266).  Can be multiplied by -1 by
/// setting invert true.
masala::base::Real
test_function_1(
	masala::base::Real const x,
	bool invert
) {
	// For plotting using Desmos:
	// -1.0\cdot\exp\left(-\left(\frac{\left(x-3\right)}{0.5}\right)^{2}\right)-2.0\cdot\exp\left(-\left(\frac{\left(x-2\right)}{0.25}\right)^{2}\right)+0.5\cdot\exp\left(-\left(\frac{\left(x-3.25\right)}{0.1}\right)^{2}\right)
	return ( invert ? -1.0 : 1.0 ) * ( test_gaussian( x, -1.0, 3.0, 0.5 ) + test_gaussian( x, -2.0, 2.0, 0.25 ) + test_gaussian( x, 0.5, 3.25, 0.1 ) );
}

/// @brief See util_test_fxn_1.png to see this plotted.  This is an Eigen vector version of the above function.
/// @details Has minima at (2.002, -2.018), (2.995, -0.999), and (3.397,-0.475).
/// Has maxima at (2.440, -0.376) and (3.266,-0.266).  Can be multiplied by -1 by
/// setting invert true.
masala::base::Real
test_function_1b(
	Eigen::VectorXd const & x,
	bool invert
) {
	// For plotting using Desmos:
	// -1.0\cdot\exp\left(-\left(\frac{\left(x-3\right)}{0.5}\right)^{2}\right)-2.0\cdot\exp\left(-\left(\frac{\left(x-2\right)}{0.25}\right)^{2}\right)+0.5\cdot\exp\left(-\left(\frac{\left(x-3.25\right)}{0.1}\right)^{2}\right)
	masala::base::Real const xval( x[0] );
	return ( invert ? -1.0 : 1.0 ) * ( test_gaussian( xval, -1.0, 3.0, 0.5 ) + test_gaussian( xval, -2.0, 2.0, 0.25 ) + test_gaussian( xval, 0.5, 3.25, 0.1 ) );
}

/// @brief A function of two variables for testing.
/// @details Has minima somewhere near (-1, 0), (1, 0), and (0, 2), with values
/// somewhere around -1.0, -2.0, and -2.5, respectively.
masala::base::Real
test_function_2(
	Eigen::VectorXd const & x,
	bool invert
) {
	// For plotting using Desmos:
	// -1.0\cdot\exp\left(\frac{-\left(x-1\right)^{2}}{.25}\right)\exp\left(\frac{-y^{2}}{1}\right)-2.0\cdot\exp\left(\frac{-\left(x+1\right)^{2}}{1}\right)\exp\left(\frac{-y^{2}}{0.25}\right)-2.5\cdot\exp\left(\frac{-x^{2}}{1}\right)\exp\left(\frac{-\left(y-2\right)^{2}}{1.5}\right)
	CHECK_OR_THROW( x.size() == 2, "standard_masala_plugins::tests::unit::optimizers::gradient_free", "test_function_2", "Expected a function of two variables." );
	masala::base::Real const xval(x[0]);
	masala::base::Real const yval(x[1]);
	return ( invert ? -1.0 : 1.0 ) * (
		test_gaussian( xval, -1.0, 1.0, 0.5 ) * test_gaussian( yval, 1.0, 0, 1.0 ) +
		test_gaussian( xval, -2.0, -1.0, 1.0 ) * test_gaussian( yval, 1.0, 0, 0.5 ) +
		test_gaussian( xval, -2.5, 0.0, 1.0 ) * test_gaussian( yval, 1.0, 2.0, std::sqrt(1.5) )
	);
}

/// @brief A function of two variables for testing.
/// @details Has minima somewhere near (-1, 0), (1, 0), and (0, 2), with values
/// somewhere around -1.0, -2.0, and -2.5, respectively.
masala::base::Real
grad_test_function_2(
	Eigen::VectorXd const & x,
	Eigen::VectorXd & grad_f_at_x,
	bool invert
) {
	// For plotting using Desmos:
	// -1.0\cdot\exp\left(\frac{-\left(x-1\right)^{2}}{.25}\right)\exp\left(\frac{-y^{2}}{1}\right)-2.0\cdot\exp\left(\frac{-\left(x+1\right)^{2}}{1}\right)\exp\left(\frac{-y^{2}}{0.25}\right)-2.5\cdot\exp\left(\frac{-x^{2}}{1}\right)\exp\left(\frac{-\left(y-2\right)^{2}}{1.5}\right)
	CHECK_OR_THROW( x.size() == 2, "standard_masala_plugins::tests::unit::optimizers::gradient_free", "grad_test_function_2", "Expected a function of two variables." );
	masala::base::Real const xval(x[0]);
	masala::base::Real const yval(x[1]);

	grad_f_at_x.resize( 2 );
	grad_f_at_x[0] = ( invert ? -1.0 : 1.0 ) * (
		d_test_gaussian_dx( xval, -1.0, 1.0, 0.5 ) * test_gaussian( yval, 1.0, 0, 1.0 ) +
		d_test_gaussian_dx( xval, -2.0, -1.0, 1.0 ) * test_gaussian( yval, 1.0, 0, 0.5 ) +
		d_test_gaussian_dx( xval, -2.5, 0.0, 1.0 ) * test_gaussian( yval, 1.0, 2.0, std::sqrt(1.5) )
	);
	grad_f_at_x[1] = ( invert ? -1.0 : 1.0 ) * (
		test_gaussian( xval, -1.0, 1.0, 0.5 ) * d_test_gaussian_dx( yval, 1.0, 0, 1.0 ) +
		test_gaussian( xval, -2.0, -1.0, 1.0 ) * d_test_gaussian_dx( yval, 1.0, 0, 0.5 ) +
		test_gaussian( xval, -2.5, 0.0, 1.0 ) * d_test_gaussian_dx( yval, 1.0, 2.0, std::sqrt(1.5) )
	);

	return ( invert ? -1.0 : 1.0 ) * (
		test_gaussian( xval, -1.0, 1.0, 0.5 ) * test_gaussian( yval, 1.0, 0, 1.0 ) +
		test_gaussian( xval, -2.0, -1.0, 1.0 ) * test_gaussian( yval, 1.0, 0, 0.5 ) +
		test_gaussian( xval, -2.5, 0.0, 1.0 ) * test_gaussian( yval, 1.0, 2.0, std::sqrt(1.5) )
	);
}

TEST_CASE( "Find the local minimum of a two-dimensional function using the SimplexFunctionOptimizer.", "[standard_masala_plugins::optimizers::gradient_free::SimplexFunctionOptimizer][local_minimization][gradient_free][brent_algorithm][line_optimizer]" ) {
	using masala::base::Real;
	using namespace standard_masala_plugins::optimizers_api::auto_generated_api::gradient_free;
	using masala::base::managers::tracer::MasalaTracerManager;
	using masala::base::managers::tracer::MasalaTracerManagerHandle;
	using masala::base::managers::threads::MasalaThreadManager;
	using masala::base::managers::threads::MasalaThreadManagerHandle;
	using namespace masala::numeric_api::auto_generated_api::optimization::real_valued_local;

	masala::core_api::auto_generated_api::registration::register_core();
	masala::numeric_api::auto_generated_api::registration::register_numeric();
	standard_masala_plugins::registration_api::register_library();

	MasalaTracerManagerHandle tm( MasalaTracerManager::get_instance() );
	MasalaThreadManagerHandle threadman( MasalaThreadManager::get_instance() );
	threadman->set_total_threads(3);

	std::function< Real( Eigen::VectorXd const & ) > const fxn2( std::bind( &test_function_2, std::placeholders::_1, false ) );
	std::function< Real( Eigen::VectorXd const &, Eigen::VectorXd & ) > const fxn2_grad( std::bind( &grad_test_function_2, std::placeholders::_1, std::placeholders::_2, false ) );
	std::vector< std::vector< Real > > const initial_points {
		{ -1.5, 0.1 },
		{ -0.9, -0.1 },
		{ 0.9, -0.1 },
		{ 2.0, 0.1 },
		{ 0.1, 3.0 },
		{ -0.1, 1.8 },
	};

	RealValuedFunctionLocalOptimizationProblems_APISP curproblems( masala::make_shared< RealValuedFunctionLocalOptimizationProblems_API >() );

	REQUIRE_NOTHROW([&](){
		for( std::vector< Real > const & entry : initial_points ) {
			Eigen::VectorXd x0;
			x0.resize(2);
			x0[0] = entry[0];
			x0[1] = entry[1];

			RealValuedFunctionLocalOptimizationProblem_APISP curproblem( masala::make_shared< RealValuedFunctionLocalOptimizationProblem_API >() );
			curproblem->add_starting_point( x0 );
			curproblem->set_objective_function( fxn2 );
			curproblem->set_objective_function_gradient( fxn2_grad );
			curproblem->finalize();
			curproblems->add_optimization_problem( curproblem );
		}

		SimplexFunctionOptimizer_APISP simplexopt( masala::make_shared<SimplexFunctionOptimizer_API>() );
		simplexopt->set_throw_if_iterations_exceeded(true);
		simplexopt->set_threads_to_request(3);
		std::vector< RealValuedFunctionLocalOptimizationSolutions_APICSP > const cursolutions_vec( simplexopt->run_real_valued_local_optimizer( *curproblems ) );

		CHECK( cursolutions_vec.size() == initial_points.size() );

		for( masala::base::Size i(0); i<cursolutions_vec.size(); ++i ) {
			RealValuedFunctionLocalOptimizationSolutions_API const & cursolutions( *cursolutions_vec[i] );
			CHECK( cursolutions.n_solutions() == 1 );
			RealValuedFunctionLocalOptimizationSolution_APICSP cursolution( std::dynamic_pointer_cast< RealValuedFunctionLocalOptimizationSolution_API const >( cursolutions.solution(0) ) );
			CHECK( cursolution != nullptr );
			Eigen::VectorXd const solpt( cursolution->solution_point() );
			CHECK( solpt.size() == 2 );

			tm->write_to_tracer( "standard_masala_plugins::tests::unit::optimizers::gradient_free::UtilityFunctionTests", "Attempt " + std::to_string(i)
				+ ":\tinitial_point = [" + std::to_string(initial_points[i][0]) + "," + std::to_string(initial_points[i][1])
				+ "]\tsoln_point = [" + std::to_string(solpt[0]) + "," + std::to_string(solpt[1])
				+ "]\tf(x) = " + std::to_string(cursolution->solution_score()) );

			if( i < 2 ) {
				CHECK( std::abs(solpt[0] + 0.9659) < 1.0e-3 );
				CHECK( std::abs(solpt[1] - 0.0116) < 1.0e-3 );
				CHECK( std::abs(cursolution->solution_score() + 2.067076) < 2.0e-5 );
			} else if( i < 4 ) {
				CHECK( std::abs(solpt[0] - 0.9573) < 1.0e-3 );
				CHECK( std::abs(solpt[1] - 0.0989) < 1.0e-3 );
				CHECK( std::abs(cursolution->solution_score() + 1.114634) < 2.0e-5 );
			} else {
				CHECK( std::abs(solpt[0] - 0.0005) < 1.0e-3 );
				CHECK( std::abs(solpt[1] - 1.9996) < 1.0e-3 );
				CHECK( std::abs(cursolution->solution_score() + 2.5) < 2.0e-2 );
			}
		}
	}() );

	threadman->set_total_threads(1);

	standard_masala_plugins::registration_api::unregister_library();
	masala::numeric_api::auto_generated_api::registration::unregister_numeric();
	masala::core_api::auto_generated_api::registration::unregister_core();
}

} // namespace gradient_free
} // namespace optimizers
} // namespace unit
} // namespace tests
} // namespace standard_masala_plugins
