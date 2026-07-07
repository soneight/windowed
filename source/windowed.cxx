#include <son8/windowed.hxx>
// son8
#include <glfw/son8.hxx> // GLFWwindow, glfw*
#include <glad/son8/loader.h> // gladLoadGL
// std
#include <atomic> // atomic bool
#include <cstddef> // size_t
#include <stdexcept> // runtime_error
#include <thread> // get_id( )

namespace son8::windowed {

    struct MainThread_ final {
        std::thread::id id;
        MainThread_( ) : id( std::this_thread::get_id( ) ) { }
    };

    static MainThread_ GlobalMainThread_{ };
    static std::size_t GlobalWindowCount_{ };

    class Window::Impl_ {
        GLFWwindow *window_;
    public:
        std::atomic< bool > isInitOpenGL{ };
        Impl_( Config const &config = { } ) {
            if ( not is_main_thread( ) ) throw std::runtime_error( "son8::windowed: create instances only allowed on main thread" );
            if ( not GlobalWindowCount_ and not glfwInit( ) ) throw std::runtime_error( "son8::windowed: glfwInit() failed" );

            auto version = config.version( );
            auto major = version >> 16;
            if ( 1 > major or major > 4 ) throw std::runtime_error( "son8::windowed: config version OpenGL major mismatch" );
            auto minor = ( version >> 8 ) & 0xFFu;
            auto skip = 0u;
            unsigned check_minor[] = { skip, 5u, 1u, 3u, 6u };
            if ( check_minor[ major ] < minor ) throw std::runtime_error( "son8::windowed: config version OpenGL minor mismatch" );

            glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, major );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minor );
            glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

            auto profile_hint = []( auto val ) { glfwWindowHint( GLFW_OPENGL_PROFILE, val ); };
            switch ( version & 0xFFu ) {
                case 0xCBu: profile_hint( GLFW_OPENGL_COMPAT_PROFILE ); break;
                case 0xCEu: profile_hint( GLFW_OPENGL_CORE_PROFILE ); break;
                case 0x00u: break; // unspecified behavior, use GLFW default
                default: throw std::runtime_error( "son8::windowed: config version OpenGL profile mismatch" );
            }

            window_ = glfwCreateWindow( config.width( ), config.height( ), config.title( ), nullptr, nullptr );
            if ( not window_ ) {
                if ( not GlobalWindowCount_ ) glfwTerminate( );
                throw std::runtime_error( "son::windowed: glfwCreateWindow() failed" );
            }
            ++GlobalWindowCount_;
        }
        ~Impl_( ) {
            glfwDestroyWindow( window_ );
            --GlobalWindowCount_;
            if ( not GlobalWindowCount_ ) glfwTerminate( );
        }

        bool is_main_thread( ) const { return std::this_thread::get_id( ) == GlobalMainThread_.id; }

        auto window( ) const { return window_; }
    }; // class Window::Impl_
    // window public implementation
    Window::Window( Config const &config ) : impl_( std::make_unique< Impl_ >( config ) ) { }
    Window::~Window( ) = default;

    Window::operator GLFWwindow *( ) const {
        return impl_->window( );
    }

    void Window::free_opengl( ) {
        if ( not is_Init_OpenGL( ) ) return;
        impl_->isInitOpenGL.store( false );
        glfwMakeContextCurrent( nullptr );
    }

    void Window::init_opengl( ) {
        if ( is_Init_OpenGL( ) ) return;
        impl_->isInitOpenGL.store( true );
        glfwMakeContextCurrent( impl_->window( ) );
        gladLoadGL( glfwGetProcAddress );
        // TODO: add config option
        glfwSwapInterval( 1 );
    }

    bool Window::is_closing( ) const {
        return glfwWindowShouldClose( impl_->window( ) );
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
    // --
    void Window::check_Poll_Main_Thread( ) const {
        if ( not impl_->is_main_thread( ) ) throw std::runtime_error( "son8::windowed: poll events only allowed on main thread" );
    }

} // namespace son8::windowed

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
