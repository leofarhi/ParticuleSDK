#ifndef DRAW_PIXEL_HPP
#define DRAW_PIXEL_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/gint.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    __attribute__((always_inline))
    inline void DrawPixel(int x, int y, const Color& color)
    {
        //TODO
        //dpixel(x, y, color.Raw());
        (void)x;(void)y;(void)color; // Avoid unused variable warning
    }

    //Draws a pixel without checking if the coordinates are in the screen
    __attribute__((always_inline))
    inline void DrawPixelUnsafe(int x, int y, const Color& color)
    {
        //TODO
        //gint_vram[DWIDTH * y + x] = color.Raw();
        (void)x;(void)y;(void)color; // Avoid unused variable warning
    }

    __attribute__((always_inline))
    inline void DrawRawPixelUnsafe(int x, int y, const ColorRaw& rawColor)
    {
        //TODO
        //gint_vram[DWIDTH * y + x] = rawColor;
        (void)x;(void)y;(void)rawColor; // Avoid unused variable warning
    }
}

#endif // DRAW_PIXEL_HPP