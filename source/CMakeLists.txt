set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_library( ${SON8PROJ} )
target_sources( ${SON8PROJ} PRIVATE
    windowed.cxx
)

if( NOT TARGET glfw )
    message( STATUS "${SON8PROJ}: FindPackage `glfw3`" )
    find_package( glfw3 QUIET )
    if( NOT glfw3_FOUND )
        include( FetchContent )
        message( STATUS "${SON8PROJ}: failed `glfw3_FOUND`, FetchContent `son8fork/glfw`" )
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/son8fork/glfw.git
            GIT_TAG        son8fork-030310
        )
        FetchContent_MakeAvailable( glfw )
    endif( )
endif( )
message( STATUS "${SON8PROJ}: found `glfw` target" )

if( NOT TARGET son8__overglad )
    include( FetchContent )
    message( STATUS "${SON8PROJ}: FetchContent `soneight/overglad`" )
    FetchContent_Declare(
        son8__overglad
        GIT_REPOSITORY https://github.com/soneight/overglad.git
        GIT_TAG        v0.0.1
    )
    FetchContent_MakeAvailable( son8__overglad )
endif( )
message( STATUS "${SON8PROJ}: found `son8__overglad` target" )

target_link_libraries( ${SON8PROJ} PRIVATE glfw )
