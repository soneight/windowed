set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_library( ${SON8PROJ} )
target_sources( ${SON8PROJ} PRIVATE
    windowed.cxx
)

target_link_libraries( ${SON8PROJ} PRIVATE glfw::son8 )

if( NOT TARGET son8__overglad )
    include( FetchContent )
    message( STATUS "${SON8PROJ}: FetchContent `soneight/overglad`" )
    FetchContent_Declare(
        son8__overglad
        GIT_REPOSITORY https://github.com/soneight/overglad.git
        GIT_TAG        8751de77e6f5a6fb1e87e724bd842446ec48c696 # v0.0.4
    )
    FetchContent_MakeAvailable( son8__overglad )
endif( )
message( STATUS "${SON8PROJ}: found `son8__overglad` target" )
