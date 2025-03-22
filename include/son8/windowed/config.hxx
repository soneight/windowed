#ifndef SON8_WINDOWED_CONFIG_HXX
#define SON8_WINDOWED_CONFIG_HXX

#include <cstddef>
#include <utility>
#include <string>

namespace son8::windowed {
    class Config final {
        struct Width_ {
            int value{ 640 };
            operator int( ) const { return value; }
        } width_;
        struct Height_ {
            int value{ 360 };
            operator int( ) const { return value; }
        } height_;
        struct Title_ {
            std::string value{ };
            operator char const *( ) const { return value.c_str( ); }
        } title_;
    public:
        using Width = Width_;
        using Height = Height_;
        using Title = Title_;
        template< typename ...Args >
        Config( Args &&...args )
        { ( set( std::move( args ) ), ... ); }
        // setters
        Config &set( Width width) { width_ = width; return *this; }
        Config &set( Height height) { height_ = height; return *this; }
        Config &set( Title title) { title_ = title; return *this; }
        // getters
        auto width( ) const { return width_; }
        auto height( ) const { return height_; }
        auto title( ) const { return title_; }
    }; // class Config
} // namespace son8::windowed

#endif//SON8_WINDOWED_CONFIG_HXX
