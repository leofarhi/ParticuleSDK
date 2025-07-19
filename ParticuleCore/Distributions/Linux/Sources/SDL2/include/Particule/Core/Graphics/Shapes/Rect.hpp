#ifndef DRAW_RECT_HPP
#define DRAW_RECT_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{

    inline void DrawRectOutline(int x, int y, int w, int h, const Color& color)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_SetRenderDrawColor(window->renderer, color.R(), color.G(), color.B(), color.A());
        sdl2::SDL_Rect rect = { x, y, w, h };
        sdl2::SDL_RenderDrawRect(window->renderer, &rect);
    }

    inline void DrawRectFilled(int x, int y, int w, int h, const Color& color)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_SetRenderDrawColor(window->renderer, color.R(), color.G(), color.B(), color.A());
        sdl2::SDL_Rect rect = { x, y, w, h };
        sdl2::SDL_RenderFillRect(window->renderer, &rect);
    }
}

#endif // DRAW_RECT_HPP