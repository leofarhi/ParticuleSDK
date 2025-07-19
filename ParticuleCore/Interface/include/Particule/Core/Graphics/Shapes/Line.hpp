#ifndef DRAW_LINE_HPP
#define DRAW_LINE_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    void DrawLine(int x1, int y1, int x2, int y2, const Color& color);

    void DrawHLine(int y, const Color& color);
}

#endif // DRAW_LINE_HPP