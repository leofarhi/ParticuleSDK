#ifndef COLOR_HPP
#define COLOR_HPP
#include <cstddef>
#include <cstdint>

namespace Particule::Core
{
    typedef uint32_t ColorRaw;
    
    class Color
    {
    private:
        unsigned char r, g, b, a;
        ColorRaw color;
    
        static constexpr ColorRaw Pack(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
            return (static_cast<ColorRaw>(r) << 24) |
                (static_cast<ColorRaw>(g) << 16) |
                (static_cast<ColorRaw>(b) << 8)  |
                static_cast<ColorRaw>(a);
        }

        static inline uint8_t Mul8bit(uint8_t a, uint8_t b)
        {
            return (uint8_t)(((uint16_t)a * b + 128) * 257 >> 16);
        }
    
    public:
        // Constructeurs
        constexpr Color() : r(0), g(0), b(0), a(0), color(0) {}
    
        constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
            : r(r), g(g), b(b), a(a), color(Pack(r, g, b, a)) {}
    
        constexpr Color(ColorRaw raw)
            : r((raw >> 24) & 0xFF),
            g((raw >> 16) & 0xFF),
            b((raw >> 8)  & 0xFF),
            a(raw & 0xFF),
            color(raw) {}
    
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


        static inline ColorRaw MultiplyColorRaw(ColorRaw c1, ColorRaw c2)
        {
            const uint8_t a = Mul8bit(c1 & 0xFF, c2 & 0xFF);
            const uint8_t b = Mul8bit((c1 >> 8) & 0xFF, (c2 >> 8) & 0xFF);
            const uint8_t g = Mul8bit((c1 >> 16) & 0xFF, (c2 >> 16) & 0xFF);
            const uint8_t r = Mul8bit((c1 >> 24) & 0xFF, (c2 >> 24) & 0xFF);

            return Pack(r, g, b, a);
        }
    
        constexpr void Get(unsigned char& outR, unsigned char& outG, unsigned char& outB, unsigned char& outA) const {
            outR = r; outG = g; outB = b; outA = a;
        }
    
        // Mutateurs inline
        inline void SetR(unsigned char red)   { r = red; color = Pack(r, g, b, a); }
        inline void SetG(unsigned char green) { g = green; color = Pack(r, g, b, a); }
        inline void SetB(unsigned char blue)  { b = blue; color = Pack(r, g, b, a); }
        inline void SetA(unsigned char alpha) { a = alpha; color = Pack(r, g, b, a); }
    
        // Opérateurs de comparaison
        constexpr bool operator==(const Color& other) const { return color == other.color; }

        constexpr bool operator!=(const Color& other) const { return !(*this == other); }

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