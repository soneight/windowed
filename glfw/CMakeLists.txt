add_subdirectory( source )
add_library( glfw::son8 ALIAS glfw__son8 )
target_link_libraries( glfw__son8 INTERFACE glfw )
target_include_directories( glfw__son8 INTERFACE include )

# Apache License 2.0
# NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
# SPDX-License-Identifier: Apache-2.0
# lib: `windowed` C++17 Overglad GLFW Based Window
# Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
