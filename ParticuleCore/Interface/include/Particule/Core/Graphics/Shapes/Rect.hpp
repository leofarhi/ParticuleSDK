#ifndef DRAW_RECT_HPP
#define DRAW_RECT_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    void DrawRectOutline(int x, int y, int w, int h, const Color& color);

    void DrawRectFilled(int x, int y, int w, int h, const Color& color);
}

#endif // DRAW_RECT_HPP