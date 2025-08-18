#include <Particule/Core/Inputs/Devices/Keyboard.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/Window.hpp>
#include <algorithm>

using namespace Particule::Core;
using namespace Particule::Core::Inputs::Devices;

template<typename T>
Keyboard<Inputs::Button> Keyboard<T>::GetKey() {
    while (true)
    {
        Window* window = App::GetMainWindow();
        window->UpdateInput();
        sdl2::SDL_Delay(15); // Attendre un court instant pour éviter une boucle infinie
        //get first Keydown event
        for (auto& event : window->events)
        {
            if (event.type == sdl2::SDL_KEYDOWN)
                return Keyboard<Inputs::Button>(event.key.keysym.sym);
        }
    }
    return Keyboard<Inputs::Button>(sdl2::SDLK_UNKNOWN);
}

std::string Keyboard<Inputs::Button>::GetKeyName() const {
    return std::string(sdl2::SDL_GetKeyName(m_key));
}


Inputs::Button Keyboard<Inputs::Button>::get() const {
    Window* window = App::GetMainWindow();
    if (window)
    {
        auto it = std::find_if(window->events.begin(), window->events.end(),
            [this](const sdl2::SDL_Event& event) {
                return event.key.keysym.sym == this->m_key;
            });
        if (it != window->events.end())
        {
            if (it->type == sdl2::SDL_KEYDOWN)
                return Inputs::Button{true, false, false};
            else if (it->type == sdl2::SDL_KEYUP)
                return Inputs::Button{false, false, true};
        }
        it = std::find_if(window->eventsHeld.begin(), window->eventsHeld.end(),
            [this](const sdl2::SDL_Event& event) {
                return event.type == sdl2::SDL_KEYDOWN && event.key.keysym.sym == this->m_key;
            });
        if (it != window->eventsHeld.end())
            return Inputs::Button{false, true, false};
    }
    return Inputs::Button{false, false, false};
}