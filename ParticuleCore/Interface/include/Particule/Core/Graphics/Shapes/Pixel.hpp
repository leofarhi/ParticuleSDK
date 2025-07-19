#ifndef DRAW_PIXEL_HPP
#define DRAW_PIXEL_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    void DrawPixel(int x, int y, const Color& color);

    //Draws a pixel without checking if the coordinates are in the screen
    void DrawPixelUnsafe(int x, int y, const Color& color);

    void DrawRawPixelUnsafe(int x, int y, const ColorRaw& rawColor);
}

#endif // DRAW_PIXEL_HPP