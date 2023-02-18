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
#include <base/managers/threads/MasalaThreadManager.hh>

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

    Size const nthread_total( tm->hardware_threads() );
    return 0;
}