# WINDOWED
> C++17 Overglad GLFW Based Window

## Branches

- `glfw` - versions 0.0.*, only cmake wrapper interface library to find `glfw` target
- `main` - versions 0.1+, Config and Window classes to relax `glfw` window creation

## Example

```cxx
#include <son8/windowed.hxx>
#include <glad/son8.hxx>
#include <thread>

#define APP_FINETUNE 0
#define APP_2THREADS 1

namespace app {
    void draw( );
}

int main( ) {
    using namespace son8;
    using windowed::Window;
    using namespace std::chrono_literals;
    Window window;
    std::thread close( [&window]( ) {
        std::this_thread::sleep_for( 1248ms );
        window.close( );
    });
    close.detach( );
#if APP_FINETUNE
    window.init_opengl( );
    // run_void same as:
    // \ window.run< Window::Without::Poll_Events | Window::Without::Swap_Buffer >( lambda );
    window.run_void( [&window]( ) {
        // not recommending this order, but rather fine tune example
        window.swap_buffer( );
        app::draw( );
        window.poll_events( );
    });
    window.free_opengl( );
#elif APP_2THREADS
    std::thread draw( [&window]( ) {
        window.run_swap( [&window]( ) {
            app::draw( );
            // by default calls `glfwSwapBuffers` after function ends
        });
    });
    window.run_poll( [&window]( ) {
        // by default calls `glfwPollEvents` before function starts
    });
    draw.join( );
#else
    window.run( [&window]( ) {
        // by default calls `glfwPollEvents` before function starts
        app::draw( );
        // by default calls `glfwSwapBuffers` after function ends
    });
#endif
}

void app::draw( ) {
    glClearColor( .2f, .2f, .4f, 0.f );
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity( );
    glOrtho( -0.2, 1.2, -0.2, 1.2, 1.0, -1.0 );
    glBegin( GL_QUADS );
    {
        glColor3f( 1.f, 0.f, 0.f ); glVertex2i( 0, 0 );
        glColor3f( 0.f, 1.f, 0.f ); glVertex2i( 1, 0 );
        glColor3f( 0.f, 0.f, 1.f ); glVertex2i( 1, 1 );
        glColor3f( 1.f, 1.f, 1.f ); glVertex2i( 0, 1 );
    }
    glEnd( );
}
```

## [CONTRIBUTING](./CONTRIBUTING.md)
> Project Contribution Rules

## [LICENSE](./LICENSE) [Apache-2.0](./LICENSE.Apache-2.0.md) [NOTICE](./NOTICE)
> Project Copying Rules with attribution notice

###### each folder MAY contain README with additional materials
