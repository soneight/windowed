set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_library( ${SON8PROJ} )
target_sources( ${SON8PROJ} PRIVATE
    windowed.cxx
)

if( NOT TARGET glfw )
    message( STATUS "${SON8PROJ}: `glfw` target does not exist try to find it" )
    find_package( glfw3 QUIET )
    if( NOT glfw3_FOUND )
        message( STATUS "${SON8PROJ}: `glfw3_FOUND` is not set, try to fetch it from son8fork" )
        include( FetchContent )
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/son8fork/glfw.git
            GIT_TAG        son8fork-030310
        )
        FetchContent_MakeAvailable( glfw )
    endif( )
endif( )

if( NOT TARGET son8__overglad )
    message( STATUS "${SON8PROJ}: `son8__overglad` target does not exist, fetching..." )
    include( FetchContent )
    FetchContent_Declare(
        son8__overglad
        GIT_REPOSITORY https://github.com/soneight/overglad.git
        GIT_TAG        v0.0.1
    )
    FetchContent_MakeAvailable( son8__overglad )
endif( )

target_link_libraries( ${SON8PROJ} PRIVATE glfw son8::overglad )
