set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_library( ${SON8PROJ} INTERFACE )

if( NOT TARGET son8__overglad )
    include( FetchContent )
    message( STATUS "${SON8PROJ}: FetchContent `soneight/overglad`" )
    FetchContent_Declare(
        son8__overglad
        GIT_REPOSITORY https://github.com/soneight/overglad.git
        GIT_TAG        b3679065419d626099062101416249ee5b8ebac2 # v0.0.5
    )
    FetchContent_MakeAvailable( son8__overglad )
endif( )
message( STATUS "${SON8PROJ}: found `son8__overglad` target" )

