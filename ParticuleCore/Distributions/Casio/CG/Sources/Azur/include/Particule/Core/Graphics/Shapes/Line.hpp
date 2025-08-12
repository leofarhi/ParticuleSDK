#ifndef DRAW_LINE_HPP
#define DRAW_LINE_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/gint.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    
    inline void DrawLine(int x1, int y1, int x2, int y2, const Color& color) {
        azrp_line(x1, y1, x2, y2, color.Raw());
    }

    inline void DrawHLine(int y, const Color& color)
    {
        azrp_line(0, y, DWIDTH, y, color.Raw());
    }
}

#endif // DRAW_LINE_HPP