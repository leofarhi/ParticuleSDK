#ifndef FONT_HPP
#define FONT_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/System/Window.hpp>
#include <string>
#include <cstdint>

namespace Particule::Core
{
    struct Character {
        int8_t left;
        int8_t top;
        int8_t right;
        int8_t bottom;
    
        int width() const;
        int height() const;
    
        bool get(int x, int y) const;
        int draw(int x, int y, const Color& color, fixed12_32 ratio) const;
    };

    class Font
    {
    private:
        Font();
        Font(const Font& other);
        Font& operator=(const Font& other);
    public:
        ~Font();

        int GetResolution();
        void DrawText(const std::string& text, int x, int y, const Color& color, int size);
        Vector2<int> GetTextSize(const std::string& text, int size);
        
        static Font* Load(std::string path);
        static void Unload(Font* font);
    };
}

#endif // FONT_HPP