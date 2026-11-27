#ifndef SON8_WINDOWED_WINDOW_HXX
#define SON8_WINDOWED_WINDOW_HXX
// son8
#include <son8/windowed/config.hxx> // Config
// std
#include <cstdint>
#include <utility> // forward

// forward declarations
struct GLFWwindow;

namespace son8::windowed {

    class Window final {
        class Impl_;
        Impl_ *impl_;
        enum class Error_ : unsigned {
            None,
            AlreadyBound, ReinitOpenGL [[deprecated]] = AlreadyBound, // TODO: remove deprecated on release `v1.0`
            LoadGlad,
            Size_
        };
        class Frame_ final {
            using Unt1 = std::uint16_t;
            Unt1 dw_{ };
            Unt1 dh_{ };
            Unt1 uw_{ };
            Unt1 uh_{ };
        public:
            Frame_( ) = default;
            Frame_( Unt1 drawW, Unt1 drawH, Unt1 userW, Unt1 userH )
            : dw_{ drawW }, dh_{ drawH }, uw_{ userW }, uh_{ userH } { }
            auto draw_w( ) const noexcept { return dw_; }
            auto draw_h( ) const noexcept { return dh_; }
            auto draw_extent( ) const noexcept { return Config::Extent{ dw_, dh_ }; }
            auto user_w( ) const noexcept { return uw_; }
            auto user_h( ) const noexcept { return uh_; }
            auto user_extent( ) const noexcept { return Config::Extent{ uw_, uh_ }; }
            auto user_x( ) const noexcept { return static_cast< Unt1 >(( draw_w( ) - user_w( )) / 2u ); }
            auto user_y( ) const noexcept { return static_cast< Unt1 >(( draw_h( ) - user_h( )) / 2u ); }
            explicit operator bool( ) const noexcept { return draw_w( ) and draw_h( ); }
            bool operator==( Frame_ other ) const noexcept {
                return dw_ == other.dw_
                   and dh_ == other.dh_
                   and uw_ == other.uw_
                   and uh_ == other.uh_;
            }
            bool operator!=( Frame_ other ) const noexcept { return not ( *this == other ); }
        };
        static_assert( sizeof( Frame_ ) == 8u
            , "son8::windowed: Frame size required to be 8 to pack it into 64-bit atomic integer" );
        void check_Poll_Main_Thread( ) const;
        long long poll_Linger_Start( ) const;
        void poll_Linger_Until( long long startTime ) const;
        void if_Error_Throw( Error_ error ) const;
        static constexpr bool is_Poll_Events( unsigned flags ) { return ~flags & Without::Poll_Events; }
        static constexpr bool is_Swap_Buffer( unsigned flags ) { return ~flags & Without::Swap_Buffer; }
        static constexpr bool is_Poll_Linger( unsigned flags ) { return ~flags & Without::Poll_Linger; }
    public:
        using Error = Error_;
        using Frame = Frame_;
        Window( Config const &config = { });
        ~Window( );
        Window( Window && ) = delete;
        Window( Window const & ) = delete;
        Window &operator=( Window && ) = delete;
        Window &operator=( Window const & ) = delete;

        operator GLFWwindow *( ) const;

        struct Without final {
            static inline constexpr unsigned Poll_Events = 1u << 0u; // 1
            static inline constexpr unsigned Swap_Buffer = 1u << 1u; // 2
            static inline constexpr unsigned Poll_Linger = 1u << 2u; // 4
        };

        void free_opengl( );
        [[nodiscard]] Error bind_opengl( );
        // TODO: remove on version major 1
        [[nodiscard]] [[deprecated("son8::windowed: Window, use bind_opengl instead, this function would be removed!")]]
        Error init_opengl( );
        static bool is_error( Error error ) noexcept;
        auto frame( ) const -> Frame;
        bool is_closing( ) const;
        void poll_events( ) const;
        void swap_buffer( ) const;
        void close( ) const;
        // generic run function, by default do all work useful in single threaded applications
        template< unsigned without = 0u, typename Callback, typename ...Args >
        void run( Callback &&callback, Args &&...args ) {
            if constexpr ( is_Poll_Events( without )) { check_Poll_Main_Thread( ); }

            if constexpr ( is_Swap_Buffer( without )) { if_Error_Throw( bind_opengl( )); }
            // NOTE: lingering make only sense when there is none
            // \ buffer swapping involved and after user callback
            static constexpr bool Is_Linger_Active = not is_Swap_Buffer( without ) and is_Poll_Events( without ) and is_Poll_Linger( without );
            // NOTE: intentionally do NOT `std::forward` `callback` andor `args` here
            // \ because of the loop calls
            while ( not is_closing( )) {
                if constexpr ( Is_Linger_Active ) {
                    auto startTime = poll_Linger_Start( );
                    poll_events( );
                    callback( args... );
                    poll_Linger_Until( startTime );
                } else {
                    if constexpr ( is_Poll_Events( without )) { poll_events( ); }
                    callback( args... );
                    if constexpr ( is_Swap_Buffer( without )) { swap_buffer( ); }
                }
            }

            if constexpr ( is_Swap_Buffer( without )) { free_opengl( ); }
        }
        // run without `OpenGL` initialization, useful for hidden or event only windows
        template< typename Callback, typename ...Args >
        void run_poll( Callback &&callback, Args &&...args ) {
            run< Without::Swap_Buffer >
            ( std::forward< Callback >( callback ), std::forward< Args >( args )... );
        }
        // run without polling events, useful to run on secondary thread for rendering in multi-threaded applications
        template< typename Callback, typename ...Args >
        void run_swap( Callback &&callback, Args &&...args ) {
            run< Without::Poll_Events | Without::Poll_Linger >
            ( std::forward< Callback >( callback ), std::forward< Args >( args )... );
        }
        // run only callback, useful to fine tune behavior, but requires using all window public functions correctly
        template< typename Callback, typename ...Args >
        void run_void( Callback &&callback, Args &&...args ) {
            run< Without::Poll_Events | Without::Swap_Buffer | Without::Poll_Linger >
            ( std::forward< Callback >( callback ), std::forward< Args >( args )... );
        }

    }; // class Window

} // namespace son8::windowed

#endif // INCLUDE GUARD

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
