#ifndef SON8_WINDOWED_WINDOW_HXX
#define SON8_WINDOWED_WINDOW_HXX
// son8
#include <son8/windowed/config.hxx> // Config
// std
#include <memory> // `unique_ptr`
#include <utility> // forward

// forward declarations
struct GLFWwindow;

namespace son8::windowed {

    class Window {
        class Impl_;
        std::unique_ptr< Impl_ > impl_;
        enum class Error_ : unsigned {
            None, ReinitOpenGL, LoadGlad, Size_
        };
        bool is_Init_OpenGL( ) const;
        void check_Poll_Main_Thread( ) const;
        void poll_Linger( ) const;
        void throw_Error( Error_ error ) const;
        static constexpr auto error_Size( ) { return static_cast< unsigned >( Error_::Size_ ); }
        static constexpr bool is_Poll_Events( unsigned flags ) { return ~flags & Without::Poll_Events; }
        static constexpr bool is_Swap_Buffer( unsigned flags ) { return ~flags & Without::Swap_Buffer; }
        static constexpr bool is_Poll_Linger( unsigned flags ) { return ~flags & Without::Poll_Linger; }
    public:
        using Error = Error_;
        Window( Config const &config = { } );
        ~Window( );
        Window( Window && ) = delete;
        Window( Window const & ) = delete;
        Window &operator=( Window && ) = delete;
        Window &operator=( Window const & ) = delete;

        operator GLFWwindow *( ) const;

        struct Without {
            static inline constexpr unsigned Poll_Events = 1u << 0u; // 1
            static inline constexpr unsigned Swap_Buffer = 1u << 1u; // 2
            static inline constexpr unsigned Poll_Linger = 1u << 2u; // 4
        };

        void free_opengl( );
        [[nodiscard]] Error init_opengl( );
        static bool is_error( Error error ) noexcept;
        bool is_closing( ) const;
        void poll_events( ) const;
        void swap_buffer( ) const;
        void close( ) const;
        // generic run function, by default do all work useful in single threaded applications
        template< unsigned without = 0u, typename Callback, typename ...Args >
        void run( Callback &&callback, Args &&...args ) {
            if constexpr ( is_Poll_Events( without ) ) check_Poll_Main_Thread( );

            if constexpr ( is_Swap_Buffer( without ) ) throw_Error( init_opengl( ) );

            while ( not is_closing( ) ) {
                if constexpr ( is_Poll_Events( without ) ) poll_events( );
                std::forward< Callback >( callback )( std::forward< Args >( args )... );
                // NOTE: lingering make only sense when there is none
                // \ buffer swapping involved and after user callback
                if constexpr ( is_Swap_Buffer( without ) ) swap_buffer( );
                else if constexpr ( is_Poll_Events( without ) and is_Poll_Linger( without ) ) poll_Linger( );
            }

            if constexpr ( is_Swap_Buffer( without ) ) free_opengl( );
        }
        // run without `OpenGL` initialization, useful for hidden or event only windows
        template< typename Callback, typename ...Args >
        void run_poll( Callback &&callback, Args &&...args ) {
            run< Without::Swap_Buffer >( std::forward< Callback >( callback ), std::forward< Args >( args )... );
        }
        // run without polling events, useful to run on secondary thread for rendering in multi-threaded applications
        template< typename Callback, typename ...Args >
        void run_swap( Callback &&callback, Args &&...args ) {
            run< Without::Poll_Events | Without::Poll_Linger >( std::forward< Callback >( callback ), std::forward< Args >( args )... );
        }
        // run only callback, useful to fine tune behavior, but requires using all window public functions correctly
        template< typename Callback, typename ...Args >
        void run_void( Callback &&callback, Args &&...args ) {
            run< Without::Poll_Events | Without::Swap_Buffer | Without::Poll_Linger >( std::forward< Callback >( callback ), std::forward< Args >( args )... );
        }

    }; // class Window

} // namespace son8::windowed

#endif//SON8_WINDOWED_WINDOW_HXX

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
