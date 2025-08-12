#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/Input.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/Window.hpp>
#include <algorithm>

namespace Particule::Core
{
    Input::Input() : m_key(sdl2::SDLK_UNKNOWN) {}
    Input::Input(sdl2::SDL_Keycode key) : m_key(key) {}
    Input::Input(const Input& other) : m_key(other.m_key) {}
    Input& Input::operator=(const Input& other) { m_key = other.m_key; return *this; }
    Input::~Input() {}

    bool Input::IsKeyPressed()
    {
        const Window* window = App::GetMainWindow();
        return std::find_if(window->eventsHeld.begin(), window->eventsHeld.end(),
            [this](const sdl2::SDL_Event& event) {
                return event.type == sdl2::SDL_KEYDOWN && event.key.keysym.sym == this->m_key;
            }) != window->eventsHeld.end();
    };

    bool Input::IsKeyDown()
    {
        const Window* window = App::GetMainWindow();
        return std::find_if(window->events.begin(), window->events.end(),
            [this](const sdl2::SDL_Event& event) {
                return event.type == sdl2::SDL_KEYDOWN && event.key.keysym.sym == this->m_key;
            }) != window->events.end();
    };

    bool Input::IsKeyUp()
    {
        const Window* window = App::GetMainWindow();
        return std::find_if(window->events.begin(), window->events.end(),
            [this](const sdl2::SDL_Event& event) {
                return event.type == sdl2::SDL_KEYUP && event.key.keysym.sym == this->m_key;
            }) != window->events.end();
    };

    std::string Input::GetKeyName()
    {
        return std::string(sdl2::SDL_GetKeyName(m_key));
    }

    fixed12_32 Input::Get1DAxis() {return 0;};
    Vector2<fixed12_32> Input::Get2DAxis() {return Vector2<fixed12_32>(0, 0);};
}