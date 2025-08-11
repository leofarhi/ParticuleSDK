#ifndef COLOR_HPP
#define COLOR_HPP
#include <cstdint>

namespace Particule::Core
{
    typedef uint16_t ColorRaw;
    
    class Color
    {
    private:
        unsigned char r, g, b, a;
        ColorRaw color;
    
        static constexpr ColorRaw Pack(unsigned char r, unsigned char g, unsigned char b) {
            return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
        }
    
    public:
        // Constructeurs
        constexpr Color() : r(0), g(0), b(0), a(0), color(0) {}
    
        constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
            : r(r), g(g), b(b), a(a), color(Pack(r, g, b)) {}
    
        constexpr Color(ColorRaw raw)
            : r((raw >> 8) & 0xF8), g((raw >> 3) & 0xFC), b((raw << 3) & 0xF8), a(255), color(raw) {}
    
        constexpr Color(const Color& other)
            : r(other.r), g(other.g), b(other.b), a(other.a), color(other.color) {}
    
        // Assignation
        constexpr Color& operator=(const Color& other) {
            r = other.r; g = other.g; b = other.b; a = other.a; color = other.color;
            return *this;
        }
    
        // Accesseurs inline
        constexpr unsigned char R() const { return r; }
        constexpr unsigned char G() const { return g; }
        constexpr unsigned char B() const { return b; }
        constexpr unsigned char A() const { return a; }
        constexpr ColorRaw Raw() const { return color; }

        static inline ColorRaw MultiplyColorRaw(ColorRaw a, ColorRaw b)
        {
            const unsigned int ar = (a >> 11) & 0x1F;
            const unsigned int ag = (a >> 5) & 0x3F;
            const unsigned int ab = a & 0x1F;
        
            const unsigned int br = (b >> 11) & 0x1F;
            const unsigned int bg = (b >> 5) & 0x3F;
            const unsigned int bb = b & 0x1F;
        
            const unsigned int r = (ar * br + 15) / 31;
            const unsigned int g = (ag * bg + 31) / 63;
            const unsigned int b_ = (ab * bb + 15) / 31;
        
            return (r << 11) | (g << 5) | b_;
        }
    
        constexpr void Get(unsigned char& outR, unsigned char& outG, unsigned char& outB, unsigned char& outA) const {
            outR = r; outG = g; outB = b; outA = a;
        }
    
        // Mutateurs inline
        inline void SetR(unsigned char red)   { r = red; color = Pack(r, g, b); }
        inline void SetG(unsigned char green) { g = green; color = Pack(r, g, b); }
        inline void SetB(unsigned char blue)  { b = blue; color = Pack(r, g, b); }
        inline void SetA(unsigned char alpha) { a = alpha; }
    
        // Opérateurs de comparaison
        constexpr bool operator==(const Color& other) const {
            return color == other.color && a == other.a;
        }
    
        constexpr bool operator!=(const Color& other) const {
            return !(*this == other);
        }
    
        // Destructeur trivial
        ~Color() = default;

        static Color Black;
        static Color White;
        static Color Alpha;
        static Color Red;
        static Color Green;
        static Color Blue;
        static Color Yellow;
        static Color Cyan;
        static Color Magenta;
    };
}

namespace Particule::Core {
    inline Color Color::Black   = Color(0, 0, 0, 255);
    inline Color Color::White   = Color(255, 255, 255, 255);
    inline Color Color::Alpha   = Color(0, 0, 0, 0);
    inline Color Color::Red     = Color(255, 0, 0, 255);
    inline Color Color::Green   = Color(0, 255, 0, 255);
    inline Color Color::Blue    = Color(0, 0, 255, 255);
    inline Color Color::Yellow  = Color(255, 255, 0, 255);
    inline Color Color::Cyan    = Color(0, 255, 255, 255);
    inline Color Color::Magenta = Color(255, 0, 255, 255);
}
#endif // COLOR_HPP