# Standard Masala Plugins
# Copyright (C) 2022 Vikram K. Mulligan
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

## This file actually does the building.

CMAKE_MINIMUM_REQUIRED( VERSION 3.10 )

# Set project attributes:
PROJECT( MasalaStandardPlugins VERSION 0.2 DESCRIPTION "Standard plugins for the Masala molecular modelling suite." )
SET( MODE debug )
SET( CMAKE_BUILD_TYPE Debug )
# SET( MODE release )
# SET( CMAKE_BUILD_TYPE Release )
SET( LINK_TYPE SHARED )
SET( LINK_EXTERNAL_LIBS stdc++ )
SET( CMAKE_MACOSX_RPATH 0 )
SET( CMAKE_SKIP_BUILD_RPATH FALSE )
SET( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
SET( THREADS_PREFER_PTHREAD_FLAG ON )
SET( Catch2_DIR ../catch2_build )
SET( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
#SET(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -pedantic-errors")
LINK_DIRECTORIES( ../lib )
FIND_PACKAGE( Threads REQUIRED )

# Set header file locations:
INCLUDE_DIRECTORIES( SYSTEM ../ ../src/ ../headers/masala/headers/ )

# Compiler settings and compilation flags:
INCLUDE( compiler.cmake )

# Generate the build lists for the selectors library and its API library:
execute_process(
    COMMAND python3 ../code_templates/generate_cmake_build.py standard_masala_plugins selectors ../src/selectors cmake_generated/selectors.cmake cmake_generated/selectors_api.cmake cmake_generated/selectors_tests.cmake
)

# Generate the build lists for the optimizers library and its API library:
execute_process(
    COMMAND python3 ../code_templates/generate_cmake_build.py standard_masala_plugins optimizers ../src/optimizers cmake_generated/optimizers.cmake cmake_generated/optimizers_api.cmake cmake_generated/optimizers_tests.cmake
)

# Generate the build lists for the file_interpreters library and its API library:
execute_process(
    COMMAND python3 ../code_templates/generate_cmake_build.py standard_masala_plugins file_interpreters ../src/file_interpreters cmake_generated/file_interpreters.cmake cmake_generated/file_interpreters_api.cmake cmake_generated/file_interpreters_tests.cmake
)

# Generate the build lists for the registration library and its API library:
execute_process(
    COMMAND python3 ../code_templates/generate_cmake_build.py standard_masala_plugins registration ../src/registration cmake_generated/registration.cmake cmake_generated/registration_api.cmake cmake_generated/registration_tests.cmake
)

# Generate the build lists for the benchmark library and its API library:
execute_process(
    COMMAND python3 ../code_templates/generate_cmake_build.py standard_masala_plugins benchmark ../src/benchmark cmake_generated/benchmark.cmake NONE NONE
)

# Build the selectors library:
INCLUDE( ../build/cmake_generated/selectors.cmake )

# Build the selectors API library:
INCLUDE( ../build/cmake_generated/selectors_api.cmake )

# Build the optimizers library:
INCLUDE( ../build/cmake_generated/optimizers.cmake )

# Build the optimizers API library:
INCLUDE( ../build/cmake_generated/optimizers_api.cmake )

# Build the file_interpreters library:
INCLUDE( ../build/cmake_generated/file_interpreters.cmake )

# Build the file_interpreters API library:
INCLUDE( ../build/cmake_generated/file_interpreters_api.cmake )

# Build the registration library:
INCLUDE( ../build/cmake_generated/registration.cmake )

# Build the registration_api library:
INCLUDE( ../build/cmake_generated/registration_api.cmake )

# Build the benchmark library:
INCLUDE( ../build/cmake_generated/benchmark.cmake )

# Build tests for the selectors library.
INCLUDE( ../build/cmake_generated/selectors_tests.cmake )

# Build tests for the optimizers library.
INCLUDE( ../build/cmake_generated/optimizers_tests.cmake )

# Build tests for the file_interpreters library.
INCLUDE( ../build/cmake_generated/file_interpreters_tests.cmake )

# Build tests for the registration library.
INCLUDE( ../build/cmake_generated/registration_tests.cmake )
