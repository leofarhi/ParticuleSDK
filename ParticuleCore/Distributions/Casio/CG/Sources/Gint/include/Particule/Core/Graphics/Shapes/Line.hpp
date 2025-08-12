#ifndef DRAW_LINE_HPP
#define DRAW_LINE_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/gint.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    
    inline void DrawLine(int x1, int y1, int x2, int y2, const Color& color) {
        dline(x1, y1, x2, y2, color.Raw());
    }

    inline void DrawHLine(int y, const Color& color)
    {
        uint32_t *vram = (uint32_t*)(gint_vram + DWIDTH * y);
        uint32_t colorLong = (color.Raw() << 16) | color.Raw();
        const int length = DWIDTH / 2;
        for (int i = 0; i < length; i++)
            vram[i] = colorLong;
    }
}

#endif // DRAW_LINE_HPP