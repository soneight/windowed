#ifndef SON8_WINDOWED_CONFIG_HXX
#define SON8_WINDOWED_CONFIG_HXX

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace son8::windowed {

    using ModeVeiled = std::uint16_t;

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
        struct Area_ {
            using Byte_ = std::uint8_t;
            static constexpr Byte_ Default_Units = 20u;
            static constexpr Byte_ Default_Extra = 0u;
            Mode mode{ };
            // IDEA: making default pixel size for 1unit as 32x18 (oblong) and 24x24 (square)
            // \ make maximum values (0=256 units_ and 0=256 extra_) to create window
            // \ with gui only 8192x4608 (oblong) and 6144x6144 (square)
            // \ satisfying maximum requirement of 8192 dimension size
            // \ and internal value 1 byte types that can never bypass max size by design
            // \ this maximum requirement applied only to user area (GUI)
            Byte_ units{ Default_Units }; // minumum window requirement on creation, 0 is 256
            Byte_ extra{ Default_Extra };
            Area_( ) = default;
            Area_( Mode mode_, Byte_ units_ = Default_Units, Byte_ extra_ = Default_Extra )
            : mode{ mode_ }, units{ units_ }, extra{ extra_ } {  }
            auto veiled( ) const noexcept { return static_cast< ModeVeiled >( mode ); }
            bool is_full( ) const noexcept { return veiled( ) & Mode_Veiled_Full_Bit; }
            bool is_fluid( ) const noexcept { return veiled( ) & Mode_Veiled_Fluid_Bit; }
            bool is_square( ) const noexcept { return veiled( ) & Mode_Veiled_Square_Bit; }
        } area_;
        static_assert( sizeof( Area_ ) == 4u );
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
            unsigned value{ static_cast< unsigned >( OpenGL::Vx0406CB )};
            Version_( ) = default;
            Version_( unsigned version ) noexcept : value{ version } { }
            Version_( OpenGL version ) noexcept : value{ static_cast< unsigned >( version )} { }
            Version_( unsigned major, unsigned minor, std::optional< bool > compat = std::nullopt ) noexcept
            : value{ ( major << 16 ) | ( minor << 8 ) | ( compat ? ( compat.value( ) ? 0xCBu : 0xCEu ) : 0x0u )}
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
        // temporary
        bool isUsedArea_{ };
        bool isUsedWH_{ };
    public:
        using Area = Area_;
        using Width [[deprecated("son8::windowed: use Config Area instead")]] = Width_;
        using Height [[deprecated("son8::windowed: user Config Area instead")]] = Height_;
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
        Config &set( Area area ) { area_ = area; isUsedArea_ = true; return *this; }
        Config &set( Width_ width) { width_ = width; isUsedWH_ = true; return *this; }
        Config &set( Height_ height) { height_ = height; isUsedWH_ = true; return *this; }
        Config &set( Title title) { title_ = std::move( title ); return *this; }
        Config &set( Version version ) { version_ = version; return *this; }
        Config &set( LingerUS lingerUS ) { linger_ = lingerUS; return *this; }
        Config &set( DebugGL debugGL ) { debugGL_ = debugGL; return *this; }
        // getters
        auto area( ) const { return area_; }
        auto width( ) const { return width_; }
        auto height( ) const { return height_; }
        auto title( ) const { return title_; }
        auto version( ) const { return version_; }
        auto linger_us( ) const { return linger_; }
        auto debug_gl( ) const { return debugGL_; }
        // -- temporary
        auto is_used_area( ) const { return isUsedArea_; }
        auto is_used_wh( ) const { return isUsedWH_; }
    }; // class Config
} // namespace son8::windowed

#endif // INCLUDE GUARD

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
