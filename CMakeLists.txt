cmake_minimum_required( VERSION 3.14...3.28 )
project( windowed VERSION 0.0.0 LANGUAGES CXX )

set( SON8PROJ son8__${PROJECT_NAME} )

add_subdirectory( source )
add_library( son8::${PROJECT_NAME} ALIAS ${SON8PROJ} )

add_subdirectory( temp )
