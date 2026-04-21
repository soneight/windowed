set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_library( ${SON8_LIB} INTERFACE )

if( NOT TARGET son8__overglad )
    include( FetchContent )
    message( STATUS "${SON8_LIB}: FetchContent `soneight/overglad`" )
    FetchContent_Declare(
        son8__overglad
        GIT_REPOSITORY https://github.com/soneight/overglad.git
        GIT_TAG        50fb5537ef5612ae059cd0b9482b26bef532a98f # v0.0.6
    )
    FetchContent_MakeAvailable( son8__overglad )
endif( )
message( STATUS "${SON8_LIB}: found `son8__overglad` target" )

# Apache License 2.0
# NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
# SPDX-License-Identifier: Apache-2.0
# lib: `windowed` C++17 Overglad GLFW Based Window
# Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
