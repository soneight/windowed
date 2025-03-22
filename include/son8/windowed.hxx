#ifndef SON8_WINDOWED_HXX
#define SON8_WINDOWED_HXX

#include <memory>
#include <utility>

namespace son8::windowed {
    class Windowed {
        class Impl_;
        std::unique_ptr< Impl_ > impl_;

        bool running( ) const;
        void process( ) const;
    public:
        Windowed( );
        ~Windowed( );
        Windowed( Windowed&& ) = delete;
        Windowed( Windowed const& ) = delete;
        Windowed &operator=( Windowed && ) = delete;
        Windowed &operator=( Windowed const & ) = delete;

        template< typename Callback, typename ...Args >
        void run( Callback &&callback, Args &&...args ) {
            while (running( )) {
                std::forward< Callback >( callback )( std::forward< Args >( args )... );
                process( );
            }
        }
    }; // class Windowed
} // namespace son8::windowed

#endif//SON8_WINDOWED_HXX

// Ⓒ 2025 Oleg'Ease'Kharchuk ᦒ
