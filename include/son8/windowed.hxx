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

// Ⓒ 2025 Oleg'Ease'Kharchuk ᦒ
