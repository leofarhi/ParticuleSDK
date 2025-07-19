#ifndef DRAW_LINE_HPP
#define DRAW_LINE_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    
    inline void DrawLine(int x1, int y1, int x2, int y2, const Color& color)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_SetRenderDrawColor(window->renderer, color.R(), color.G(), color.B(), color.A());
        sdl2::SDL_RenderDrawLine(window->renderer, x1, y1, x2, y2);
    }

    inline void DrawHLine(int y, const Color& color)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_SetRenderDrawColor(window->renderer, color.R(), color.G(), color.B(), color.A());
        sdl2::SDL_RenderDrawLine(window->renderer, 0, y, window->Width(), y);
    }
}

#endif // DRAW_LINE_HPP