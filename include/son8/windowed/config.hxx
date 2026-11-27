#ifndef SON8_WINDOWED_CONFIG_HXX
#define SON8_WINDOWED_CONFIG_HXX

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace son8::windowed {

    using ModeVeiled = std::uint8_t;

    constexpr ModeVeiled Mode_Veiled_Full_Bit = 1u << 0u;
    constexpr ModeVeiled Mode_Veiled_Fluid_Bit = 1u << 1u;
    constexpr ModeVeiled Mode_Veiled_Square_Bit = 1u << 2u;
    // mainly applied for user area
    // \ , draw area either unrestricted (fluid), or same as user area (fixed)
    enum class Mode : ModeVeiled {
        Oblong_Fixed_Desk = 0u,
        Oblong_Fixed_Full = Mode_Veiled_Full_Bit,
        Oblong_Fluid_Desk = Mode_Veiled_Fluid_Bit,
        Oblong_Fluid_Full = Mode_Veiled_Fluid_Bit | Mode_Veiled_Full_Bit,
        Square_Fixed_Desk = Mode_Veiled_Square_Bit,
        Square_Fixed_Full = Mode_Veiled_Square_Bit | Mode_Veiled_Full_Bit,
        Square_Fluid_Desk = Mode_Veiled_Square_Bit | Mode_Veiled_Fluid_Bit,
        Square_Fluid_Full = Mode_Veiled_Square_Bit | Mode_Veiled_Fluid_Bit | Mode_Veiled_Full_Bit
    };

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
        struct Mode_ {
            windowed::Mode mode{ windowed::Mode::Oblong_Fixed_Desk };
            auto veiled( ) const noexcept { return static_cast< ModeVeiled >( mode ); }
            bool is_full( ) const noexcept { return veiled( ) & Mode_Veiled_Full_Bit; }
            bool is_fluid( ) const noexcept { return veiled( ) & Mode_Veiled_Fluid_Bit; }
            bool is_square( ) const noexcept { return veiled( ) & Mode_Veiled_Square_Bit; }
            Mode_( ) = default;
            Mode_( windowed::Mode init ) : mode{ init } { }
        } mode_;
        struct Extent_ {
            using Type_ = std::uint16_t;
            Type_ w{ };
            Type_ h{ };
        } extent_;
        struct Width_ final {
            using Type_ = typename Extent_::Type_;
            Type_ value{ };
            operator Type_( ) const noexcept { return value; }
        };
        struct Height_ final {
            using Type_ = typename Extent_::Type_;
            Type_ value{ };
            operator Type_( ) const noexcept { return value; }
        };
        struct Title_ final {
            std::string value{ };
            operator char const *( ) const noexcept { return value.c_str( ); }
        } title_;
        struct Version_ final {
            unsigned value{ static_cast< unsigned >( OpenGL::Vx0406CB )};
            Version_( ) = default;
            Version_( unsigned version ) noexcept : value{ version } { }
            Version_( OpenGL version ) noexcept : value{ static_cast< unsigned >( version )} { }
            Version_( unsigned major, unsigned minor, std::optional< bool > compat = std::nullopt ) noexcept
            : value{ ( major << 16u ) | ( minor << 8u ) | ( compat ? ( compat.value( ) ? 0xCBu : 0xCEu ) : 0x0u )}
            { }
            operator unsigned( ) const noexcept { return value; }
        } version_;
        struct Linger_ final {
            unsigned value{ 1'000u };
            operator unsigned( ) const noexcept { return value; }
        } linger_;
        struct DebugGL_ final {
            using OptBool_ = std::optional< bool >;
            OptBool_ option;
        } debugGL_;
        using Copy_ = Config const &;
        using Move_ = Config &&;
    public:
        using Mode = Mode_;
        using Extent = Extent_;
        using Width = Width_;
        using Height = Height_;
        using Title = Title_;
        using Version = Version_;
        using LingerUS = Linger_;
        using DebugGL = DebugGL_;
        // constructors
        template< typename ...Args >
        Config( Args &&...args )
        { ( set( std::forward< Args >( args )), ... ); }
        Config( Config & ) = default;
        Config( Copy_ ) = default;
        Config( Move_ ) = default;
        Config &operator=( Copy_ ) = default;
        Config &operator=( Move_ ) = default;
        // setters
        Config &set( Mode mode ) { mode_ = mode; return *this; }
        Config &set( Extent extent ) { extent_ = extent; return *this; }
        Config &set( Width width) { extent_.w = width; return *this; }
        Config &set( Height height) { extent_.h = height; return *this; }
        Config &set( Title title) { title_ = std::move( title ); return *this; }
        Config &set( Version version ) { version_ = version; return *this; }
        Config &set( LingerUS lingerUS ) { linger_ = lingerUS; return *this; }
        Config &set( DebugGL debugGL ) { debugGL_ = debugGL; return *this; }
        // getters
        auto mode( ) const { return mode_; }
        auto extent( ) const { return extent_; }
        auto width( ) const { return extent_.w; }
        auto height( ) const { return extent_.h; }
        auto title( ) const { return title_; }
        auto version( ) const { return version_; }
        auto linger_us( ) const { return linger_; }
        auto debug_gl( ) const { return debugGL_; }
    }; // class Config
} // namespace son8::windowed

#endif // INCLUDE GUARD

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
