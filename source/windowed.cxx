#include <son8/windowed.hxx>
// son8
#include <glfw/son8.hxx> // GLFWwindow, glfw*
#include <glad/son8/loader.h> // gladLoadGL
// std
#include <stdexcept> // runtime_error

namespace son8::windowed {

    class Window::Impl_ {
        GLFWwindow *window_;
    public:
        Impl_( Config const &config = { } ) {
            if ( !glfwInit( ) ) { throw std::runtime_error( "son8::windowed: glfwInit() failed" ); }

            auto version = config.version( );
            auto major = version >> 16;
            if ( 1 > major || major > 4 ) throw std::runtime_error( "son8::windowed: config version OpenGL major mismatch" );
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
            if ( !window_ ) {
                glfwTerminate( );
                throw std::runtime_error( "son::windowed: glfwCreateWindow() failed" );
            }
            glfwMakeContextCurrent( window_ );
            gladLoadGL( glfwGetProcAddress );
            // TODO: add config option
            glfwSwapInterval( 1 );
        }
        ~Impl_( ) {
            glfwDestroyWindow( window_ );
            glfwTerminate( );
        }

        auto window( ) const { return window_; }
    }; // class Window::Impl_

    Window::Window( Config const &config ) : impl_( std::make_unique< Impl_ >( config ) ) { }
    Window::~Window( ) = default;

    Window::operator GLFWwindow *( ) const {
        return impl_->window( );
    }

    bool Window::running( ) const {
        return !glfwWindowShouldClose( impl_->window( ) );
    }

    void Window::process( ) const {
        glfwPollEvents( );
        glfwSwapBuffers( impl_->window( ) );
    }
} // namespace son8::windowed

// Ⓒ 2025 Oleg'Ease'Kharchuk ᦒ
