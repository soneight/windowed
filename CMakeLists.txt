cmake_minimum_required( VERSION 3.14...3.27 )
project( windowed VERSION 0.3.0 LANGUAGES CXX )
set( SON8_LIB son8__${PROJECT_NAME} )
add_subdirectory( glfw )
# Private configuration under source directory
add_subdirectory( source )
add_library( son8::${PROJECT_NAME} ALIAS ${SON8_LIB} )
target_compile_features( ${SON8_LIB} PUBLIC cxx_std_17 )
target_include_directories( ${SON8_LIB} PUBLIC include )
target_compile_options( ${SON8_LIB} INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /permissive- /Zc:__cplusplus> )

target_link_libraries( ${SON8_LIB} PUBLIC son8::overglad )

# Apache License 2.0
# NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
# SPDX-License-Identifier: Apache-2.0
# lib: `windowed` C++17 Overglad GLFW Based Window
# Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
