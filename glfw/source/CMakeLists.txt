add_library( glfw__son8 INTERFACE )

if( NOT TARGET glfw )
    message( STATUS "${SON8_LIB}: FindPackage `glfw3`" )
    find_package( glfw3 QUIET )
    if( NOT glfw3_FOUND )
        include( FetchContent )
        message( STATUS "${SON8_LIB}: failed `glfw3_FOUND`, FetchContent `glfw/glfw`" )
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG        dc46d3f8129712e42856c20e99a604a3b08ad581 # 3.3.10
        )
        set( GLFW_BUILD_DOCS OFF )
        set( GLFW_BUILD_EXAMPLES OFF )
        set( GLFW_BUILD_TESTS OFF )
        set( GLFW_INSTALL OFF )
        FetchContent_MakeAvailable( glfw )
    endif( )
endif( )
message( STATUS "${SON8_LIB}: found `glfw` target" )

# Apache License 2.0
# NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
# SPDX-License-Identifier: Apache-2.0
# lib: `windowed` C++17 Overglad GLFW Based Window
# Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
