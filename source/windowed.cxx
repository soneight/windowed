#include <son8/windowed.hxx>
// `son8`
#include <glfw/son8.hxx> // `GLFWwindow, glfw*`
#include <glad/son8/loader.h> // `gladLoadGL`
// `std`
#include <array>
#include <cstddef> // `size_t`
#include <chrono> // `duration`
#include <iostream> // `cerr`
#include <mutex> // `mutex, scoped_lock`
#include <stdexcept> // `runtime_error`
#include <thread> // `get_id( )`

namespace son8::windowed {
    // aliases
    namespace time = std::chrono;
    using Clock = time::steady_clock;
    using Lock = std::scoped_lock< std::mutex >;
    using Mutex = std::mutex;
    using Size = std::size_t;
    using Thread = std::thread;

    namespace main_thread_ {

        static Thread::id &id_ref( ) {
            static Thread::id id{ };
            return id;
        }

        struct Id {
            Id( ) {
                auto &id = id_ref( );
                if ( id == Thread::id{ } ) id = std::this_thread::get_id( );
            }
        };

        struct InitGLFW {
            InitGLFW( ) {
                if ( glfwInit( ) == GLFW_FALSE ) throw std::runtime_error( "son8::windowed: glfwInit() failed" );
                glfwSetErrorCallback( []( int errc, char const *desc ) {
                    std::cerr << "son8::windowed: glfwSetErrorCallback() code: " << errc << ", description: " << desc << '\n';
                });
            }
           ~InitGLFW( ) {
                glfwTerminate( );
            }
        };

        static Id Global;
    }

    static Size GlobalWindowCount_{ };

    class Window::Impl_ {
        GLFWwindow *window_;
        static constexpr Size Max_Count = 1u;
        static constexpr auto Max_Linger_US = 20'000u;
        static constexpr auto Error_Size = static_cast< unsigned >( Error::Size_ );
    public:
        Config const config;
        // NOTE: mutex only should be used on `(bind|free)_opengl` method pair
        Mutex /*mutable?*/ swapMutex;
        Thread::id swapThread{ };
        bool is_swap_thread_empty( ) { return swapThread == Thread::id{ }; }
        bool is_swap_thread_equal( ) { return swapThread == std::this_thread::get_id( ); }

        static constexpr std::array< char const *, Error_Size > ErrorMessages{{
            "son8::windowed: Window - Not an error",
            "son8::windowed: Window - Context is already bound",
            "son8::windowed: Window - Load glad failed",
        }};
        Impl_( Config const &configInit = { } ) : config{ configInit } {
            if ( not is_main_thread( ) ) throw std::runtime_error( "son8::windowed: Window requires create instances only on main thread" );
            { static main_thread_::InitGLFW InitGLFW; } // hide `InitGLFW` not used outside
            // NOTE: check always after `is_main_thread` check, it allow avoid using atomic
            // \ for global window counting, as window can be created only on 1 main thread
            if ( Max_Count <= GlobalWindowCount_ ) throw std::runtime_error( "son8::windowed: Window requires only one instance per process" );

            if ( config.linger_us( ) > Max_Linger_US ) throw std::runtime_error( "son8::windowed: Config requires that linger does not exceed 20 milliseconds" );

            auto version = config.version( );
            auto major = version >> 16u;
            if ( 4 < major or major < 1 ) throw std::runtime_error( "son8::windowed: Config requires valid OpenGL major version" );

            auto minor = ( version >> 8u ) & 0xFFu;
            auto skip = 0u;
            unsigned check_minor[] = { skip, 5u, 1u, 3u, 6u };
            if ( check_minor[ major ] < minor ) throw std::runtime_error( "son8::windowed: Config requires valid OpenGL minor version" );
            // NOTE: important to clear hints
            glfwDefaultWindowHints( );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, major );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minor );
            glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

            auto profile_hint = []( auto val ) { glfwWindowHint( GLFW_OPENGL_PROFILE, val ); };
            switch ( version & 0xFFu ) {
                case 0xCBu: profile_hint( GLFW_OPENGL_COMPAT_PROFILE ); break;
                case 0xCEu: profile_hint( GLFW_OPENGL_CORE_PROFILE ); break;
                case 0x00u: break; // unspecified behavior, use GLFW default
                default: throw std::runtime_error( "son8::windowed: Config requires valid OpenGL profile, one of [00,CB,CE]" );
            }

            window_ = glfwCreateWindow( config.width( ), config.height( ), config.title( ), nullptr, nullptr );
            if ( not window_ ) throw std::runtime_error( "son::windowed: glfwCreateWindow() failed" );
            // NOTE: for not resizable windows resize immediately
            // \ to avoid bug on `wayland` tiling window managers
            glfwSetWindowSize( window_, config.width( ), config.height( ) );
            // NOTE: should be LAST to avoid not correct behavior
            // \ when adding more stuff to the constructor method
            ++GlobalWindowCount_;
        }
        ~Impl_( ) {
            glfwDestroyWindow( window_ );
            --GlobalWindowCount_;
        }

        bool is_main_thread( ) const {
            auto &id = main_thread_::id_ref( );
            // NOTE: when window created globally without this check due
            // \ to possible static (globals) objects out-order creation
            // \ main thread Global id could be not initialized properly
            // \ in case of background thread this check is always false
            if ( id == Thread::id{ } ) id = std::this_thread::get_id( );

            return std::this_thread::get_id( ) == id;
        }

        Error load_opengl( ) const {
            // NOTE: requires `glfwMakeCurrentContext` to be active
            static Error_ loadError = []( ) {
                if ( gladLoadGL( glfwGetProcAddress ) ) return Error::None;

                return Error::LoadGlad;
            }( );

            return loadError;
        }

        auto window( ) const { return window_; }
    }; // class `Window::Impl_`
    // window public implementation
    Window::Window( Config const &config ) : impl_{ new Impl_{ config } } { }
    Window::~Window( ) { delete impl_; }

    Window::operator GLFWwindow *( ) const {
        return impl_->window( );
    }

    void Window::free_opengl( ) {
        Lock contextLock{ impl_->swapMutex };

        if ( not impl_->is_swap_thread_equal( ) ) return;

        glfwMakeContextCurrent( nullptr );
        impl_->swapThread = Thread::id{ };
    }

    Window::Error Window::bind_opengl( ) {
        Lock contextLock{ impl_->swapMutex };

        if ( not impl_->is_swap_thread_empty( ) ) return Error::AlreadyBound;

        glfwMakeContextCurrent( impl_->window( ) );

        auto loadError = impl_->load_opengl( );
        if ( loadError != Error::None ) {
            glfwMakeContextCurrent( nullptr );
            return loadError;
        }

        impl_->swapThread = std::this_thread::get_id( );

        // TODO: add config option
        glfwSwapInterval( 1 );

        return Error::None;
    }
    // -- init_opengl is deprecated
    Window::Error Window::init_opengl( ) {
        return bind_opengl( );
    }

    bool Window::is_closing( ) const {
        return glfwWindowShouldClose( impl_->window( ) );
    }

    bool Window::is_error( Error error ) noexcept {
        return error != Error::None;
    }

    void Window::close( ) const {
        glfwSetWindowShouldClose( impl_->window( ), GLFW_TRUE );
    }

    void Window::poll_events( ) const {
        glfwPollEvents( );
    }

    void Window::swap_buffer( ) const {
        glfwSwapBuffers( impl_->window( ) );
    }
    // window private implementation
    // --
    void Window::if_Error_Throw( Error error ) const {
        if ( not is_error( error ) ) return;
        throw std::runtime_error{ Impl_::ErrorMessages[static_cast< Size >( error )] };
    }
    // --
    void Window::check_Poll_Main_Thread( ) const {
        if ( not impl_->is_main_thread( ) ) throw std::runtime_error( "son8::windowed: Window requires poll events on main thread" );
    }
    // --
    long long Window::poll_Linger_Start( ) const {
        return Clock::now( ).time_since_epoch( ).count( );
    }
    // --
    void Window::poll_Linger_Until( long long startPoint ) const {
        auto duration = Clock::duration{ startPoint };
        auto timemark = Clock::time_point{ duration };
        auto deadline = timemark + time::microseconds( impl_->config.linger_us( ) );
        std::this_thread::sleep_until( deadline );
    }

} // namespace son8::windowed

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
