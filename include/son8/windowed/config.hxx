#ifndef SON8_WINDOWED_CONFIG_HXX
#define SON8_WINDOWED_CONFIG_HXX

// #include <cstddef>
#include <optional>
#include <string>
#include <utility>

namespace son8::windowed {
    enum class OpenGL : unsigned {
        Vx010100 = 0x010100u,
        Vx010500 = 0x010500u,
        Vx020100 = 0x020100u,
        Vx0303CB = 0x0303CBu,
        Vx0303CE = 0x0303CEu,
        Vx0403CB = 0x0403CBu,
        Vx0403CE = 0x0403CEu,
        Vx0406CB = 0x0406CBu,
        Vx0406CE = 0x0406CEu,
    }; // enum class OpenGL

    class Config final {
        struct Width_ final {
            unsigned value{ 640u };
            operator unsigned( ) const noexcept { return value; }
        } width_;
        struct Height_ final {
            unsigned value{ 360u };
            operator unsigned( ) const noexcept { return value; }
        } height_;
        struct Title_ final {
            std::string value{ };
            operator char const *( ) const noexcept { return value.c_str( ); }
        } title_;
        struct Version_ final {
            unsigned value{ static_cast< unsigned >( OpenGL::Vx0406CB ) };
            Version_( ) = default;
            Version_( unsigned version ) noexcept : value{ version } { }
            Version_( OpenGL version ) noexcept : value{ static_cast< unsigned >( version ) } { }
            Version_( unsigned major, unsigned minor, std::optional< bool > compat = std::nullopt ) noexcept
            : value{ ( major << 16 ) | ( minor << 8 ) | ( compat ? ( compat.value() ? 0xCBu : 0xCEu) : 0x0u ) }
            { }
            operator unsigned( ) const noexcept { return value; }
        } version_;
        struct Linger_ final {
            unsigned value{ 1'000u };
            operator unsigned( ) const noexcept { return value; }
        } linger_;
    public:
        using Width = Width_;
        using Height = Height_;
        using Title = Title_;
        using Version = Version_;
        using LingerUS = Linger_;
        template< typename ...Args >
        Config( Args &&...args )
        { ( set( std::move( args ) ), ... ); }
        // setters
        Config &set( Width width) { width_ = width; return *this; }
        Config &set( Height height) { height_ = height; return *this; }
        Config &set( Title title) { title_ = title; return *this; }
        Config &set( Version version ) { version_ = version; return *this; }
        Config &set( LingerUS lingerUS ) { linger_ = lingerUS; return *this; }
        // getters
        auto width( ) const { return width_; }
        auto height( ) const { return height_; }
        auto title( ) const { return title_; }
        auto version( ) const { return version_; }
        auto linger_us( ) const { return linger_; }
    }; // class Config
} // namespace son8::windowed

#endif//SON8_WINDOWED_CONFIG_HXX

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
