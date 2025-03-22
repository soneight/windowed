#include <son8/windowed.hxx>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace son8::windowed {

    class Windowed::Impl_ {
        GLFWwindow *window_;
    public:
        Impl_( ) {
            if (!glfwInit( )) { throw std::runtime_error( "glfwInit() failed" ); }

            window_ = glfwCreateWindow( 640, 360, "", nullptr, nullptr );
            if ( !window_ ) {
                glfwTerminate( );
                throw std::runtime_error( "glfwCreateWindow() failed" );
            }
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
    }
} // namespace son8::windowed

// Ⓒ 2025 Oleg'Ease'Kharchuk ᦒ
