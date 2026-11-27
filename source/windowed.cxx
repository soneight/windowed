#include <son8/windowed.hxx>
// `son8`
#include <glfw/son8.hxx> // `GLFWwindow, glfw*`
// TODO: remove this header check on release `v1.0`
#if !__has_include(<glad/son8/gl/loader.h>)
#   error "missing dependency after moved glad son8 headers: update son8::overglad andor glad::son8 library version"
#endif
#include <glad/son8/gl/loader.h> // `gladLoadGL`
// `std`
#include <algorithm> // `min`
#include <array>
#include <atomic>
#include <cassert> // NOTE: use asserts only for implementation details (so no asserts on public interface)
#include <cstddef> // `size_t`
#include <chrono> // `duration`
#include <iostream> // `cerr`
#include <mutex> // `mutex, scoped_lock`
#include <optional>
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
                if ( id == Thread::id{ }) { id = std::this_thread::get_id( ); }
            }
        };

        static Id Global;
    }

    static Size GlobalWindowCount_{ };

    class Window::Impl_ {
        struct InitGLFW {
            InitGLFW( ) {
                if ( glfwInit( ) == GLFW_FALSE ) { throw std::runtime_error( "son8::windowed: glfwInit() failed" ); }
                glfwSetErrorCallback( []( int errc, char const *desc ) {
                    std::cerr << "son8::windowed: glfwSetErrorCallback() code: " << errc << ", description: " << desc << '\n';
                });
            }
           ~InitGLFW( ) {
                glfwTerminate( );
            }
        };
        std::optional< InitGLFW > initGLFW_;
        GLFWwindow *window_;
        bool isCachedGLAD_{ };
        static constexpr Size Max_Count = 1u;
        static constexpr auto Max_Linger_US = 20'000u;
        static constexpr auto Max_Viewport_Size = 8192u;
        static constexpr auto Error_Size = static_cast< unsigned >( Error::Size_ );
    public:
        std::atomic< std::uint64_t > framePacked_;
        Config const config;
        std::atomic< bool > isClosing{ };
        // NOTE: mutex only should be used on `(bind|free)_opengl` method pair
        Mutex /*mutable?*/ swapMutex;
        Thread::id swapThread{ };

        bool is_swap_thread_empty( ) { return swapThread == Thread::id{ }; }
        bool is_swap_thread_equal( ) { return swapThread == std::this_thread::get_id( ); }

        static constexpr std::array< char const *, Error_Size > ErrorMessages{{
            "son8::windowed: Window - Not an error",
            "son8::windowed: Window - Context is already bound, maybe try to `free_opengl` method before calling `run*` method family",
            "son8::windowed: Window - Load glad failed",
        }};

        static auto frame_pack( Frame frame ) noexcept {
            constexpr auto chunk = []( uint16_t value, unsigned shift ) {
                return static_cast< std::uint64_t >( value ) << shift;
            };
            return chunk( frame.draw_w( ), 48u )
                 | chunk( frame.draw_h( ), 32u )
                 | chunk( frame.user_w( ), 16u )
                 | chunk( frame.user_h( ), 0u );
        }

        void frame_update( int w, int h ) {
            if ( w <= 0 || h <= 0 ) {
                framePacked_.store( 0u, std::memory_order_relaxed );
                return;
            }

            using ExtentType = Config::Extent::Type_;

            Config::Extent extentDraw{
                static_cast< ExtentType >( w ),
                static_cast< ExtentType >( h )
            };

            auto const modeConfig = config.mode( );
            auto const extentConfig = config.extent( );
            Config::Extent extentMax = extentDraw;
            bool isSquare = modeConfig.is_square( );

            if ( not modeConfig.is_fluid( )) {
                bool isDefault = extentConfig.w == 0u || extentConfig.h == 0u;
                Config::Extent extentReq = isDefault
                    ? ( isSquare ? Config::Extent{ 480u, 480u } : Config::Extent{ 640u, 360u } )
                    : extentConfig;
                extentMax = {
                   std::min( extentDraw.w, extentReq.w ),
                   std::min( extentDraw.h, extentReq.h )
                };
            }

            Config::Extent extentUser;

            if ( isSquare ) {
                auto side = std::min( extentMax.w, extentMax.h );
                extentUser = { side, side };
            } else {
                if ( extentMax.w * 9 > extentMax.h * 16 ) {
                    extentUser = {
                        static_cast< ExtentType >( extentMax.h * 16u / 9u ),
                        extentMax.h
                    };
                } else {
                    extentUser = {
                        extentMax.w,
                        static_cast< ExtentType >( extentMax.w * 9u / 16u )
                    };
                }
            }

            if ( not extentUser.w or not extentUser.h ) { extentUser = { }; }

            Frame frame{ extentDraw.w, extentDraw.h, extentUser.w, extentUser.h };
            framePacked_.store( frame_pack( frame ), std::memory_order_relaxed );
        }

        Impl_( Config const &configInit = { }) : config{ configInit } {
            if ( not is_main_thread( )) {
                throw std::runtime_error{ "son8::windowed: Window requires create instances only on main thread" };
            }
            // NOTE: check always after `is_main_thread` check, it allow avoid using atomic
            // \ for global window counting, as window can be created only on 1 main thread
            if ( Max_Count <= GlobalWindowCount_ ) {
                throw std::runtime_error{ "son8::windowed: Window requires only one instance per process" };
            }

            if ( config.linger_us( ) > Max_Linger_US ) {
                throw std::runtime_error{ "son8::windowed: Config requires that linger does not exceed 20 milliseconds" };
            }

            auto version = config.version( );
            auto major = static_cast< int >( version >> 16u );

            if ( 4 < major or major < 1 ) {
                throw std::runtime_error{ "son8::windowed: Config requires valid OpenGL major version" };
            }

            int minor = ( version >> 8u ) & 0xFFu;
            constexpr int skip{ 0 };
            int check_minor[] = { skip, 5, 1, 3, 6 };

            if ( check_minor[ major ] < minor ) {
                throw std::runtime_error{ "son8::windowed: Config requires valid OpenGL minor version" };
            }

            int profileHint = 0;
            switch ( version & 0xFFu ) {
            case 0xCBu: { profileHint = GLFW_OPENGL_COMPAT_PROFILE; } break;
            case 0xCEu: { profileHint = GLFW_OPENGL_CORE_PROFILE; } break;
            case 0x00u: { } break; // unspecified behavior, use GLFW default
                default: {
                    throw std::runtime_error{ "son8::windowed: Config requires valid OpenGL profile, one of [00,CB,CE]" };
                }
            }

            auto mode = config.mode( );
            auto extent = config.extent( );
            bool isValidExtent = extent.w <= Max_Viewport_Size and extent.h <= Max_Viewport_Size;

            if ( not isValidExtent ) {
                throw std::runtime_error{ "son8::windowed: Config requires Extent not exceed 8192 value in any dimension" };
            }

            switch ((( extent.w == 0 ) << 1u ) | (extent.h == 0u )) {
            case 0b00u: { } break;
            case 0b01u: [[fallthrough]];
            case 0b10u: { throw std::runtime_error{ "son8::windowed: Config requires Extent width and height both be zero or non-zero" }; } break;
                default: {
                    extent = ( mode.is_square( )) ? Config::Extent{ 480u, 480u } : Config::Extent{ 640u, 360u };
                }
            }

            initGLFW_.emplace( );
            // NOTE: important to clear hints
            glfwDefaultWindowHints( );
            // OpenGL related hints
        #ifndef NDEBUG
            constexpr bool Auto_Debug_GL = true;
        #else
            constexpr bool Auto_Debug_GL = false;
        #endif // debug builds
            // `Auto_Debug_GL`:
            // \ not set auto (current behavior, for all version try add debug context hint in debug builds)
            // \ true always set debug context hint
            // \ false never set debug context hint
            // NOTE: cannot check major or minor versions as actual version could differ from requested anyway
            auto isDebugContext = config.debug_gl( ).option.value_or( Auto_Debug_GL ) ? GLFW_TRUE : GLFW_FALSE;
            glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, isDebugContext );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, major );
            glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minor );

            if ( profileHint ) { glfwWindowHint( GLFW_OPENGL_PROFILE, profileHint ); }

            glfwWindowHint( GLFW_RESIZABLE, config.mode( ).is_fluid( ) ? GLFW_TRUE : GLFW_FALSE );

            bool isFullSuccess = mode.is_full( );

            if ( isFullSuccess ) {
                auto try_full = [this] {
                    auto monitorGLFW = glfwGetPrimaryMonitor( );
                    if ( not monitorGLFW ) { return false; }

                    auto modeGLFW = glfwGetVideoMode( monitorGLFW );

                    if ( not modeGLFW ) { return false; }

                    glfwWindowHint( GLFW_RED_BITS, modeGLFW->redBits );
                    glfwWindowHint( GLFW_GREEN_BITS, modeGLFW->greenBits );
                    glfwWindowHint( GLFW_BLUE_BITS, modeGLFW->blueBits );
                    glfwWindowHint( GLFW_REFRESH_RATE, modeGLFW->refreshRate );
                    glfwWindowHint( GLFW_AUTO_ICONIFY, GLFW_FALSE ); // avoid minimizing on focus loss / alt-tab

                    window_ = glfwCreateWindow( modeGLFW->width, modeGLFW->height, config.title( ), monitorGLFW, nullptr );

                    if ( not window_ ) { throw std::runtime_error{ "son8::windowed: glfwCreateWindow() in full mode failed" }; }

                    return true;
                };

                if ( not try_full( )) { isFullSuccess = false; }
            }

            int width{ extent.w };
            int height{ extent.h };

            if ( not isFullSuccess ) {
                window_ = glfwCreateWindow( width, height, config.title( ), nullptr, nullptr );

                if ( not window_ ) { throw std::runtime_error{ "son8::windowed: glfwCreateWindow() in desk mode failed" }; }
                // NOTE: for not resizable windows resize immediately
                // \ to avoid bug on `wayland` tiling window managers
                if ( not mode.is_fluid()) { glfwSetWindowSize( window_, width, height ); }
            }

            glfwSetWindowUserPointer(window_, this);
            glfwSetFramebufferSizeCallback( window_, []( GLFWwindow *win, int w, int h ) {
                auto *self = static_cast<Impl_ *>( glfwGetWindowUserPointer(win));
                self->frame_update(w, h);
            });
            // NOTE: `GLFW` does NOT always callback on initial creation
            glfwGetFramebufferSize( window_, &width, &height );
            frame_update( width, height );
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
            if ( id == Thread::id{ }) { id = std::this_thread::get_id( ); }

            return std::this_thread::get_id( ) == id;
        }

        [[nodiscard]] Error load_opengl( ) {
            // NOTE: retry also on failed, to allow load with different settings (maybe)
            if ( isCachedGLAD_ ) { return Error::None; }
            // NOTE: requires `glfwMakeCurrentContext` to be active
            assert( glfwGetCurrentContext( ) == window_ and "son8::windowed: Window implementation logic error, not an current context" );

            if ( not gladLoadGL( glfwGetProcAddress )) { return Error::LoadGlad; }

            isCachedGLAD_ = true;

            return Error::None;
        }

        auto window( ) const { return window_; }
    }; // class `Window::Impl_`
    // window public implementation
    Window::Window( Config const &config ) : impl_{ new Impl_{ config }} { }
    Window::~Window( ) { delete impl_; }

    Window::operator GLFWwindow *( ) const {
        return impl_->window( );
    }

    auto Window::frame( ) const -> Frame {
        auto bits = impl_->framePacked_.load(std::memory_order_relaxed);
        auto chunk = [&]( unsigned shift ) {
            return static_cast< std::uint16_t >(( bits >> shift ) & 0xFFFFu );
        };
        return Frame{
            chunk( 48u ),
            chunk( 32u ),
            chunk( 16u ),
            chunk( 0u )
        };
    }

    void Window::free_opengl( ) {
        Lock contextLock{ impl_->swapMutex };

        if ( not impl_->is_swap_thread_equal( )) return;

        glfwMakeContextCurrent( nullptr );
        impl_->swapThread = Thread::id{ };
    }

    Window::Error Window::bind_opengl( ) {
        Lock contextLock{ impl_->swapMutex };

        if ( not impl_->is_swap_thread_empty( )) { return Error::AlreadyBound; }

        glfwMakeContextCurrent( impl_->window( ));

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
    // -- init_opengl is deprecated, TODO: remove on release
    Window::Error Window::init_opengl( ) {
        return bind_opengl( );
    }

    bool Window::is_closing( ) const {
        return impl_->isClosing.load( std::memory_order_relaxed );
    }

    bool Window::is_error( Error error ) noexcept {
        return error != Error::None;
    }

    void Window::close( ) const {
        impl_->isClosing.store( true, std::memory_order_relaxed );
    }

    void Window::poll_events( ) const {
        glfwPollEvents( );

        if ( glfwWindowShouldClose( impl_->window( ))) {
            impl_->isClosing.store( true, std::memory_order_relaxed );
        }
    }

    void Window::swap_buffer( ) const {
        glfwSwapBuffers( impl_->window( ));
    }
    // window private implementation
    // --
    void Window::if_Error_Throw( Error error ) const {
        assert( error < Error::Size_ and "son8::windowed: Window::if_Error_Throw implementation logic error code exceed size" );
        if ( not is_error( error )) { return; }
        throw std::runtime_error{ Impl_::ErrorMessages[static_cast< Size >( error )] };
    }
    // --
    void Window::check_Poll_Main_Thread( ) const {
        if ( impl_->is_main_thread( )) { return; }
        throw std::runtime_error{ "son8::windowed: Window requires poll events on main thread" };
    }
    // --
    long long Window::poll_Linger_Start( ) const {
        return Clock::now( ).time_since_epoch( ).count( );
    }
    // --
    void Window::poll_Linger_Until( long long startPoint ) const {
        auto duration = Clock::duration{ startPoint };
        auto timemark = Clock::time_point{ duration };
        auto deadline = timemark + time::microseconds( impl_->config.linger_us( ));
        std::this_thread::sleep_until( deadline );
    }

} // namespace son8::windowed

// Apache License 2.0
// NO WARRANTY OF ANY KIND see <http://www.apache.org/licenses/LICENSE-2.0>
// SPDX-License-Identifier: Apache-2.0
// lib: `windowed` C++17 Overglad GLFW Based Window
// Ⓒ Copyright (c) 2025-2026 Oleg'Ease'Kharchuk ᦒ
