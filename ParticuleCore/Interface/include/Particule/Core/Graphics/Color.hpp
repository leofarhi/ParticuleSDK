#ifndef COLOR_HPP
#define COLOR_HPP

namespace Particule::Core
{
    typedef unsigned long long int ColorRaw;
    
    class Color
    {
        public:
            Color();
            Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a  = 255);
            Color(ColorRaw raw);
            Color(const Color &other);
            Color &operator=(const Color &other);
            ~Color();

            void SetR(unsigned char r);
            void SetG(unsigned char g);
            void SetB(unsigned char b);
            void SetA(unsigned char a);

            unsigned char R() const;
            unsigned char G() const;
            unsigned char B() const;
            unsigned char A() const;

            ColorRaw Raw() const;
            static ColorRaw MultiplyColorRaw(ColorRaw a, ColorRaw b);
            
            void Get(unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a) const;

            bool operator==(const Color &other) const;
            bool operator!=(const Color &other) const;

            static Color FromHex(unsigned int hexValue);
            
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