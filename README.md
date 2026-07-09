# WINDOWED
> C++17 Overglad GLFW Based Window

## Branches

- `glfw` - versions 0.0.*, only cmake wrapper interface library to find `glfw` target
- `main` - versions 0.1+, Config and Window classes to relax `glfw` window creation

## Usage
> config and window headers are combined inside windowed header

- [Config](./include/son8/windowed/config.hxx):
  - by default create window with size `640x360` with opengl version `4.6 compatibility profile`

- [Window](./include/son8/windowed/window.hxx):
  - requires to be created on main thread
  - can only create single Window per process
  - by default created with `VSync` on

### Example
> below is concise example that show different use cases

```cxx
#include <son8/windowed.hxx> // `CMake` target `son8::windowed`
#include <glad/son8.hxx> // `CMake` target `glad::son8`
#include <glfw/son8.hxx> // `CMake` target `glfw::son8`
#include <thread>

#define APP_FINETUNE 0 // require to call wrapper functions for glfw manually in user preferred way
#define APP_2THREADS 1 // 2 threads, first main thread for polling events, second background thread for opengl rendering

#define APP_TEST 0
#define APP_TEST_LOCAL 0
#define APP_TEST_LOOP 1
#define APP_TEST_GLOBAL 2
#define APP_TEST_BACKGROUND 3 // should throw because Window not created on main thread

namespace app {
    using son8::windowed::Window;
    void draw( );
}

#if APP_TEST == APP_TEST_GLOBAL
static app::Window WindowGlobal;
static app::Window *windowPtr = &WindowGlobal;
#else
static app::Window *windowPtr;
#endif

int main( ) {
    using namespace std::chrono_literals;
#if APP_TEST == APP_TEST_BACKGROUND
    std::thread background( []( ) {
        static app::Window windowBackground;
        windowPtr = &windowBackground;
    } );
    background.join( );
#elif APP_TEST == APP_TEST_LOCAL
    app::Window windowLocal;
    windowPtr = &windowLocal;
#elif APP_TEST == APP_TEST_LOOP
    unsigned loops = 3u;
    do {
        app::Window windowLoop;
        std::thread closeLoop( [&windowLoop]( ) {
            std::this_thread::sleep_for( 1052ms );
            windowLoop.close( );
        });
        windowLoop.run( []( ){ app::draw( ); } );
        closeLoop.join( );
    } while ( --loops );
    app::Window windowLoop;
    windowPtr = &windowLoop;
#endif
    app::Window &window = *windowPtr;

    glfwSetKeyCallback( window, []( GLFWwindow *w, int key, int scan, int action, int mods ){
        if ( key == GLFW_KEY_Q ) windowPtr->close( ); // or `glfwSetWindowShouldClose( w, GLFW_TRUE );`
    });

    std::thread close( []( ) {
        std::this_thread::sleep_for( 8421ms );
        windowPtr->close( );
    });
    close.detach( );

#if APP_FINETUNE
    window.init_opengl( );
    window.run_void( [&window]( ) { // not recommending this order, but rather fine tune example
        app::draw( );
        window.swap_buffer( );
        window.poll_events( );
    });
    window.free_opengl( );
#elif APP_2THREADS
    std::thread draw( []( ) {
        windowPtr->run_swap( []( ) { // by default calls `glfwSwapBuffers` after function ends
            app::draw( );
        });
    });
    window.run_poll( []( ) { } );
    draw.join( );
#else
    window.run( [&window]( ) { // by default calls `glfwPollEvents` before function starts and `glfwSwapBuffers` after function ends
        app::draw( );
    });
#endif
}

void app::draw( ) {
    glClearColor( .2f, .2f, .4f, 0.f );
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity( );
    glOrtho(-0.2, 1.2,-0.2, 1.2,-1.0, 1.0 );
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
