#ifndef SON8_WINDOWED_WINDOW_HXX
#define SON8_WINDOWED_WINDOW_HXX
// son8
#include <son8/windowed/config.hxx> // Config
// std
#include <memory> // unique_ptr
#include <utility> // forward

// forward declarations
struct GLFWwindow;

namespace son8::windowed {
    class Window {
        class Impl_;
        std::unique_ptr< Impl_ > impl_;

        bool running( ) const;
        void process( ) const;
    public:
        Window( Config const &config = { } );
        ~Window( );
        Window( Window && ) = delete;
        Window( Window const & ) = delete;
        Window &operator=( Window && ) = delete;
        Window &operator=( Window const & ) = delete;

        operator GLFWwindow *( ) const;

        template< typename Callback, typename ...Args >
        void run( Callback &&callback, Args &&...args ) {
            while ( running( ) ) {
                std::forward< Callback >( callback )( std::forward< Args >( args )... );
                process( );
            }
        }
    }; // class Window
} // namespace son8::windowed

#endif//SON8_WINDOWED_WINDOW_HXX

// Ⓒ 2025 Oleg'Ease'Kharchuk ᦒ
