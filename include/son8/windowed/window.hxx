#ifndef SON8_WINDOWED_WINDOW_HXX
#define SON8_WINDOWED_WINDOW_HXX
// son8
#include <son8/windowed/config.hxx> // Config
// std
#include <memory> // unique_ptr
#include <utility> // forward

// forward declarations
struct GLFWwindow;

namespace son8::windowed {
    class Window {
        class Impl_;
        std::unique_ptr< Impl_ > impl_;
        bool is_Init_OpenGL( ) const;
        static constexpr bool is_Poll_Events( unsigned flags ) { return ~flags & Run_Bitfield_No_Poll; }
        static constexpr bool is_Swap_Buffer( unsigned flags ) { return ~flags & Run_Bitfield_No_Swap; }
    public:
        Window( Config const &config = { } );
        ~Window( );
        Window( Window && ) = delete;
        Window( Window const & ) = delete;
        Window &operator=( Window && ) = delete;
        Window &operator=( Window const & ) = delete;

        operator GLFWwindow *( ) const;

        static inline constexpr unsigned Run_Bitfield_No_Poll = 1u << 0u; // 1
        static inline constexpr unsigned Run_Bitfield_No_Swap = 1u << 1u; // 2

        bool is_closing( ) const;
        void init_opengl( );
        void poll_events( ) const;
        void swap_buffer( ) const;

        template< unsigned runBitfield = 0u, typename Callback, typename ...Args >
        void run( Callback &&callback, Args &&...args ) {
            if constexpr ( is_Swap_Buffer( runBitfield ) ) {
                if ( not is_Init_OpenGL( ) ) init_opengl( );
            }

            while ( not is_closing( ) ) {
                if constexpr ( is_Poll_Events( runBitfield ) ) poll_events( );
                std::forward< Callback >( callback )( std::forward< Args >( args )... );
                if constexpr ( is_Swap_Buffer( runBitfield ) ) swap_buffer( );
            }
        }
    }; // class Window
} // namespace son8::windowed

#endif//SON8_WINDOWED_WINDOW_HXX

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
