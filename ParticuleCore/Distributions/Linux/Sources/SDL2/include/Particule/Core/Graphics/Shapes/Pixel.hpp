#ifndef DRAW_PIXEL_HPP
#define DRAW_PIXEL_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/Window.hpp>

namespace Particule::Core
{
    inline void DrawPixel(int x, int y, const Color& color)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_SetRenderDrawColor(window->renderer, color.R(), color.G(), color.B(), color.A());
        sdl2::SDL_RenderDrawPoint(window->renderer, x, y);
    }

    //Draws a pixel without checking if the coordinates are in the screen
    inline void DrawPixelUnsafe(int x, int y, const Color& color)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_SetRenderDrawColor(window->renderer, color.R(), color.G(), color.B(), color.A());
        sdl2::SDL_RenderDrawPoint(window->renderer, x, y);
    }

    inline void DrawRawPixelUnsafe(int x, int y, const ColorRaw& rawColor)
    {
        Color color(rawColor);
        DrawPixelUnsafe(x, y, color);
    }
}

#endif // DRAW_PIXEL_HPP