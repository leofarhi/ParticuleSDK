#include <Particule/Core/Inputs/Devices/Mouse.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/Window.hpp>
#include <iostream>
#include <algorithm>

using namespace Particule::Core;
using namespace Particule::Core::Inputs::Devices;
using namespace sdl2;



// Helpers (dans le .cpp de tes inputs)
static inline Uint8 ToSDLMouseButton(Inputs::PointerButton b)
{
    switch (b)
    {
        case Inputs::PointerButton::Left:   return SDL_BUTTON_LEFT;
        case Inputs::PointerButton::Right:  return SDL_BUTTON_RIGHT;
        case Inputs::PointerButton::Middle: return SDL_BUTTON_MIDDLE;
        case Inputs::PointerButton::X1:     return SDL_BUTTON_X1;
        case Inputs::PointerButton::X2:     return SDL_BUTTON_X2;
        default:                            return 0;
    }
}

template<typename T>
static inline T clamp01(T v)
{
    if (v < T(0)) return T(0);
    if (v > T(1)) return T(1);
    return v;
}

//------------------------------------------------------------------------------
// Mouse<Inputs::Pointer>
Inputs::Pointer Mouse<Inputs::Pointer>::get() const
{
    Inputs::Pointer out; // tout à 0 / false par défaut

    Window* window = App::GetMainWindow();
    if (!window)
        return out;

    // Taille de la fenêtre (adapte si ton Window expose autrement)
    int winW = 1, winH = 1;
    SDL_GetWindowSize(window->window, &winW, &winH);

    // --- Position absolue : on prend l’état courant de la souris
    SDL_GetMouseState(&out.pos.x, &out.pos.y);

    // --- Delta & Scroll : on additionne toutes les events de la frame
    double dx = 0.0, dy = 0.0;
    double sx = 0.0, sy = 0.0;

    for (const auto& e : window->events)
    {
        switch (e.type)
        {
            case SDL_MOUSEMOTION:
                dx += (double)e.motion.xrel / (double)winW;
                dy += (double)e.motion.yrel / (double)winH;
                break;

            case SDL_MOUSEWHEEL:
            {
                // SDL3/SDL2: si FLIPPED, inverser
                int mult = (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1;
                sx += mult * (double)e.wheel.x;
                sy += mult * (double)e.wheel.y;
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                // transitions down/up par bouton
                for (int i = 0; i < (int)Inputs::PointerButton::Count; ++i)
                {
                    if (e.button.button == ToSDLMouseButton((Inputs::PointerButton)i))
                    {
                        if (e.type == SDL_MOUSEBUTTONDOWN)
                            out.buttons[i] = Inputs::Button{ true, false, false };
                        else
                            out.buttons[i] = Inputs::Button{ false, false, true };
                    }
                }
                break;
            }

            default: break;
        }
    }

    // --- État "maintenu" (pressed) : chercher dans eventsHeld
    for (int i = 0; i < (int)Inputs::PointerButton::Count; ++i)
    {
        const Uint8 sdlBtn = ToSDLMouseButton((Inputs::PointerButton)i);

        auto itHeld = std::find_if(
            window->eventsHeld.begin(), window->eventsHeld.end(),
            [sdlBtn](const SDL_Event& ev)
            {
                return ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == sdlBtn;
            });

        if (itHeld != window->eventsHeld.end())
        {
            // si on n’a pas déjà vu un up sur ce bouton, marquer pressed
            if (!out.buttons[i].up)
                out.buttons[i] = Inputs::Button{ out.buttons[i].down, true, out.buttons[i].up };
        }
        else
        {
            // si pas held, ne change rien aux transitions déjà posées (down/up éventuels)
            if (!out.buttons[i].down && !out.buttons[i].up)
                out.buttons[i] = Inputs::Button{ false, false, false };
        }
    }

    // Appliquer delta & scroll accumulés
    out.delta.x  = (fixed12_32)dx;
    out.delta.y  = (fixed12_32)dy;
    out.scroll.x = (fixed12_32)sx;
    out.scroll.y = (fixed12_32)sy;
    return out;
}

//------------------------------------------------------------------------------
// Mouse<Inputs::Button>
Inputs::Button Mouse<Inputs::Button>::get() const
{
    Window* window = App::GetMainWindow();
    if (!window)
        return Inputs::Button{ false, false, false };

    const Uint8 sdlBtn = ToSDLMouseButton(m_button);

    // Cherche une transition sur cette frame
    auto it = std::find_if(
        window->events.begin(), window->events.end(),
        [sdlBtn](const SDL_Event& e)
        {
            if (e.type != SDL_MOUSEBUTTONDOWN && e.type != SDL_MOUSEBUTTONUP)
                return false;
            return e.button.button == sdlBtn;
        });

    if (it != window->events.end())
    {
        if (it->type == SDL_MOUSEBUTTONDOWN)
            return Inputs::Button{ true, false, false };
        else // SDL_MOUSEBUTTONUP
            return Inputs::Button{ false, false, true };
    }

    // Sinon, est-ce qu’il est maintenu ?
    auto itHeld = std::find_if(
        window->eventsHeld.begin(), window->eventsHeld.end(),
        [sdlBtn](const SDL_Event& e)
        {
            return e.type == SDL_MOUSEBUTTONDOWN && e.button.button == sdlBtn;
        });

    if (itHeld != window->eventsHeld.end())
        return Inputs::Button{ false, true, false };

    return Inputs::Button{ false, false, false };
}
