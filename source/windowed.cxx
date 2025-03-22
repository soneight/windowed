#include <son8/windowed.hxx>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/son8/loader.h>

#include <stdexcept>

namespace son8::windowed {

    class Windowed::Impl_ {
        GLFWwindow *window_;
    public:
        Impl_( ) {
            if (!glfwInit( ) ) { throw std::runtime_error( "glfwInit() failed" ); }

            glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
            glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );

            window_ = glfwCreateWindow( 640, 360, "", nullptr, nullptr );
            if ( !window_ ) {
                glfwTerminate( );
                throw std::runtime_error( "glfwCreateWindow() failed" );
            }
            glfwMakeContextCurrent( window_ );
            gladLoadGL( glfwGetProcAddress );
        }
        ~Impl_( ) {
            glfwDestroyWindow( window_ );
            glfwTerminate( );
        }

        auto window( ) const { return window_; }
    }; // class Windowed::Impl_

    Windowed::Windowed( ) : impl_( std::make_unique< Impl_ >( ) ) { }
    Windowed::~Windowed( ) = default;

    bool Windowed::running( ) const {
        return !glfwWindowShouldClose( impl_->window( ) );
    }

    void Windowed::process( ) const {
        glfwPollEvents( );
        glfwSwapBuffers( impl_->window( ) );
    }
} // namespace son8::windowed

// Ⓒ 2025 Oleg'Ease'Kharchuk ᦒ
