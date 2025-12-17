add_subdirectory( source )
add_library( glfw::son8 ALIAS glfw__son8 )
target_link_libraries( glfw__son8 INTERFACE glfw )
target_include_directories( glfw__son8 INTERFACE include )
