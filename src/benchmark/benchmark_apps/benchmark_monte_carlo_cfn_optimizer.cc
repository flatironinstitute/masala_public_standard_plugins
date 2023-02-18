/*
    Standard Masala Plugins
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

/// @file src/benchmark/benchmark_apps/benchmark_monte_carlo_cfn_optimizer.hh
/// @brief An application used for benchmarking the MonteCarloCostFunctionNetworkOptimizer.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).

// Core headers
#include <base/types.hh>

// Optimizers API headers
#include <optimizers_api/auto_generated_api/registration/register_optimizers.hh>

// Base headers
#include <base/managers/tracer/MasalaTracerManager.hh>
#include <base/managers/threads/MasalaThreadManager.hh>
#include <base/error/ErrorHandling.hh>

// External headers

// STL headers

// Program entry point:
int
main(
    // int argc,
    // char * argv[]
    int, char**
) {
    using masala::base::Size;
     masala::base::managers::threads::MasalaThreadManagerHandle tm( masala::base::managers::threads::MasalaThreadManager::get_instance() );
    masala::base::managers::tracer::MasalaTracerManagerHandle tr( masala::base::managers::tracer::MasalaTracerManager::get_instance() );

    try{
        Size const nthread_total( tm->hardware_threads() );
        if( nthread_total == 0 ) {
            MASALA_THROW( "benchmark_monte_carlo_cfn_optimizer application", "main", "Could not auto-detect hardware threads!" );
        }
        tr->write_to_tracer( "benchmark_monte_carlo_cfn_optimizer application", "Detected " + std::to_string(nthread_total) + " hardware threads." );
    } catch( masala::base::error::MasalaException const e ) {
        tr->write_to_tracer( "benchmark_monte_carlo_cfn_optimizer application", "Caught Masala exception: " + e. message() );
        return 1;
    }

    return 0;
}