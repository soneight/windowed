#ifndef SON8_WINDOWED_HXX
#define SON8_WINDOWED_HXX

#include <son8/windowed/config.hxx>
#include <son8/windowed/window.hxx>

// TODO: remove on release?
namespace son8::windowed {
    // alias so old code would work
    using Windowed [[deprecated("son8::windowed Windowed class is an old name and subject to be removed, use Window instead")]] = Window;

}

#endif//SON8_WINDOWED_HXX

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
