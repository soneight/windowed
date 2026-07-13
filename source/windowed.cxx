#include <son8/windowed.hxx>
// `son8`
#include <glfw/son8.hxx> // `GLFWwindow, glfw*`
#include <glad/son8/loader.h> // `gladLoadGL`
// `std`
#include <array>
#include <atomic> // `atomic bool`
#include <cstddef> // `size_t`
#include <chrono> // `duration`
#include <iostream> // `cerr`
#include <stdexcept> // `runtime_error`
#include <thread> // `get_id( )`

namespace son8::windowed {

    using Size = std::size_t;

    namespace main_thread_ {

        static std::thread::id &id_ref( ) {
            static std::thread::id id{ };
            return id;
        }

        struct Id {
            Id( ) {
                auto &id = id_ref( );
                if ( id == std::thread::id{ } ) id = std::this_thread::get_id( );
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
        static constexpr Size Count_Max = 1u;
    public:
        Config const config;
        std::chrono::steady_clock::time_point lingerTimePoint;
        std::atomic< bool > isInitOpenGL{ };
        static constexpr std::array< char const *, error_Size( ) > ErrorMessages{{
            "son8::windowed: Window - Not an error",
            "son8::windowed: Window - OpenGL already initialized",
            "son8::windowed: Window - Load glad failed"
        }};
        Impl_( Config const &configInit = { } ) : config{ configInit } {
            if ( not is_main_thread( ) ) throw std::runtime_error( "son8::windowed: Window requires create instances only on main thread" );
            static main_thread_::InitGLFW InitGLFW;
            if ( Count_Max <= GlobalWindowCount_ ) throw std::runtime_error( "son8::windowed: Window requires only one instance per process" );

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
            if ( id == std::thread::id{ } ) id = std::this_thread::get_id( );

            return std::this_thread::get_id( ) == id;
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
        if ( not is_Init_OpenGL( ) ) return;
        impl_->isInitOpenGL.store( false );
        glfwMakeContextCurrent( nullptr );
    }

    Window::Error Window::init_opengl( ) {
        if ( is_Init_OpenGL( ) ) return Error::ReinitOpenGL;
        impl_->isInitOpenGL.store( true );
        glfwMakeContextCurrent( impl_->window( ) );
        if ( not gladLoadGL( glfwGetProcAddress ) ) return Error::LoadGlad;
        // TODO: add config option
        glfwSwapInterval( 1 );
        return Error::None;
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
    bool Window::is_Init_OpenGL( ) const {
        return impl_->isInitOpenGL.load( );
    }
    void Window::throw_Error( Error error ) const {
        if ( error == Error::None ) return;
        throw std::runtime_error{ Impl_::ErrorMessages[static_cast< Size >( error )] };
    }
    // --
    void Window::check_Poll_Main_Thread( ) const {
        if ( not impl_->is_main_thread( ) ) throw std::runtime_error( "son8::windowed: Window requires poll events on main thread" );
    }
    // --
    void Window::poll_Linger_Play( ) {
        impl_->lingerTimePoint = std::chrono::steady_clock::now( );
    }
    // --
    void Window::poll_Linger_Stop( ) {
        auto deadline = impl_->lingerTimePoint + std::chrono::microseconds( impl_->config.linger_us( ) );
        std::this_thread::sleep_until( deadline );
    }

} // namespace son8::windowed

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
