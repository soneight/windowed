cmake_minimum_required( VERSION 3.14...3.28 )
project( windowed VERSION 0.1.0 LANGUAGES CXX )

set( SON8PROJ son8__${PROJECT_NAME} )

add_subdirectory( source )
add_library( son8::${PROJECT_NAME} ALIAS ${SON8PROJ} )
target_include_directories( ${SON8PROJ} PUBLIC include )
target_compile_features( ${SON8PROJ} PUBLIC cxx_std_17 )
target_link_libraries( ${SON8PROJ} PUBLIC son8::overglad )
