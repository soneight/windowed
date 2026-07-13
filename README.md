# WINDOWED
> C++17 Overglad GLFW Based Window

## Branches

- `glfw` - versions 0.0.*, only CMake wrapper interface library to find `glfw` target
- `main` - versions 0.1+, Config and Window classes to relax `glfw` window creation

## Usage

### Targets
> `CMake` related

- `son8::windowed`: alias of `son8__windowed` target, this library privately link `glfw::son8`, and publicly link `son8::overglad`
- `glfw::son8`: alias of `glfw__son8` target, to use `glfw` functionality needs to be linked
- `son8::overglad`: alias of `son8__overglad` target, required for `glad` loader functionality which link interface `glad::son8`
- `glad::son8`: alias of `glad__son8` target, linked by default with `son8::windowed`

### Headers
> all `windowed son8` headers are combined inside `<son8/windowed.hxx>` header

- [`<glfw/son8.hxx>`](./glfw/include/glfw/son8.hxx): simple wrapper header for inclusion `GLFW/glfw3.h` with defined `GLFW_INCLUDE_NONE`

- [`<son8/windowed/config.hxx>`](./include/son8/windowed/config.hxx) (class `Config` and enum class `OpenGL`):
  - constructor parameters for `Config` can be passed in any order and quantity, omitted values would have default values:
    - `Version`: OpenGL version, either:
      - predefined constants from OpenGL enum class
      - hex formatted unsigned integer: 8 bit major, 8 bit minor, 8 bit profile (`CB` compatibility, `CE` core, `00` unspecified), like 0x010400, 0x020000, 0x0302CB, 0x0405CE
      - constructor with 3 parameters: major, minor and `std::optional` profile boolean: false `core`, true `compatibility`, not set `unspecified`
    - `Title`: window title, internal data is `std::string`, passed to constructor like `Title{ "your title" }`, `Title{ your_string_convertible_variable }`, default empty string
    - `Width` and `Height`: window `width/height`, internal data is unsigned, passed to constructor like `Width{ 400u }`, default `640u/360u`
    - `LingerUS`: event loop linger (sleep) when swap buffering disabled on main thread, default to `1000 microseconds`

- [`<son8/windowed/window.hxx>`](./include/son8/windowed/window.hxx) (class `Window`):
  - enforce to be created on main thread
  - enforce single `Window` per process
  - by default created with `VSync` on (not configurable yet)
  - only takes one constructor parameter `Config` which can be omitted (leads to creating window with size `640x360` and `OpenGL` version `4.6 compatibility profile`)
  - public methods:
    - `init_opengl`: initializing `OpenGL` by making window context current via `glfw` call and loading functions using `glad` loader, throw standard runtime error exception when fail to load
    - `free_opengl`: free window current context via `glfw` call
    - `swap_buffer`: calls `glfw` swap buffer function
    - `poll_events`: calls `glfw` poll events function
    - `close`: set `glfw` window should close to true
    - `is_closing`: checks is window should close via `glfw` call
  - templated `run*` blocking calling thread methods family (accepts callable function with variadic forwarding arguments until `is_closing` requested via `close` call):
    - `run`: enforce `poll_events` only on main thread, manually calls `init_opengl` before loop and `free_opengl` after loop, and support compile-time feature disabler's:
      - `Window::Without::Poll_Events`: disable run `poll_events` method
      - `Window::Without::Swap_Buffer`: disable run `swap_buffer` method
      - `Window::Without::Poll_Linger`: disable run private `poll_Linger` method, only works when buffer swapping is skipped
    - `run_poll`: calls `run` without `swap_buffer` with lingering (sleep) for `Config LingerUS` milliseconds, useful for splitting input processing and rendering logic
    - `run_swap`: calls `run` without poll events, useful for processing `OpenGL` calls on background thread
    - `run_void`: calls `run` that only call user provided function, use public methods to fine-tune a specific behavior

### Example
> below is concise example that show different use cases

```cxx
#include <son8/windowed.hxx> // `CMake` target `son8::windowed`
#include <glad/son8.hxx> // `CMake` target `glad::son8`
#include <glfw/son8.hxx> // `CMake` target `glfw::son8`
#include <array>
#include <thread>

#define APP_FINETUNE 0 // require to call wrapper functions for glfw manually in user preferred way
#define APP_2THREADS 1 // 2 threads, first main thread for polling events, second background thread for opengl rendering

#define APP_TEST 0
#define APP_TEST_LOCAL 0
#define APP_TEST_LOOP 1
#define APP_TEST_GLOBAL 2
#define APP_TEST_BACKGROUND 3 // should throw because Window not created on main thread

using son8::windowed::Window;
using Cfg = son8::windowed::Config;
static constexpr std::array< unsigned, 8 > VersionsGlobal{{ 0x01'0500, 0x02'0100, 0x03'03CB, 0x03'03CE, 0x04'03CB, 0x04'03CE, 0x04'06CB, 0x0406CE }};
static Cfg ConfigGlobal{ 0x01'0100u }; // setting `OpenGL` version to `1.1` unspecified

namespace app { void draw( ); }

#if APP_TEST == APP_TEST_GLOBAL
static Window WindowGlobal{ ConfigGlobal };
static Window *windowPtr = &WindowGlobal;
#else
static Window *windowPtr;
#endif

int main( ) {
    using namespace std::chrono_literals;
#if APP_TEST == APP_TEST_BACKGROUND
    std::thread background( []( ) {
        static Window windowBackground;
        windowPtr = &windowBackground;
    } );
    background.join( );
#elif APP_TEST == APP_TEST_LOCAL
    Window windowLocal;
    windowPtr = &windowLocal;
#elif APP_TEST == APP_TEST_LOOP
    unsigned loops = 0u;
    while ( loops < VersionsGlobal.size( ) ) {
        Window windowLoop{ ConfigGlobal.set( VersionsGlobal[loops++] ).set( Cfg::Height{ ConfigGlobal.height( ) + 90 } ) };
        std::thread closeLoop( [&windowLoop]( ) {
            std::this_thread::sleep_for( 1248ms );
            windowLoop.close( );
        });
        windowLoop.run( []( ){ app::draw( ); } );
        closeLoop.join( );
    }
    Window windowLoop1{ ConfigGlobal.set( son8::windowed::OpenGL::Vx010100 ).set( Cfg::Height{ 360u} ) };
    windowPtr = &windowLoop1;
#endif
    Window &window = *windowPtr;

    glfwSetKeyCallback( window, []( GLFWwindow *w, int key, int scan, int action, int mods ){
        if ( key == GLFW_KEY_Q ) windowPtr->close( ); // or `glfwSetWindowShouldClose( w, GLFW_TRUE );`
    });

    std::thread close( []( ) {
        std::this_thread::sleep_for( 8421ms );
        windowPtr->close( );
    });
    close.detach( );
#if APP_FINETUNE
    if ( Window::is_error( window.init_opengl( ) ) ) return -1;
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
    window.run( []( ) { // by default calls `glfwPollEvents` before function starts and `glfwSwapBuffers` after function ends
        app::draw( );
    });
#endif
}

void app::draw( ) {
    glClearColor( .2f, .2f, .4f, 0.f );
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity( );
    glOrtho(-0.2, 1.2,-0.2, 1.2,-1.0, 1.0 );
    glBegin( GL_QUADS ); {
        glColor3f( 1.f, 0.f, 0.f ); glVertex2i( 0, 0 );
        glColor3f( 0.f, 1.f, 0.f ); glVertex2i( 1, 0 );
        glColor3f( 0.f, 0.f, 1.f ); glVertex2i( 1, 1 );
        glColor3f( 1.f, 1.f, 1.f ); glVertex2i( 0, 1 );
    } glEnd( );
}
```

## [CONTRIBUTING](./CONTRIBUTING.md)
> Project Contribution Rules

## [LICENSE](./LICENSE) [Apache-2.0](./LICENSE.Apache-2.0.md) [NOTICE](./NOTICE)
> Project Copying Rules with attribution notice

###### each folder MAY contain README with additional materials
