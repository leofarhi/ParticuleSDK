#ifndef INPUT_HPP
#define INPUT_HPP
#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <string>

namespace Particule::Core
{
    class Input
    {
    private:
        sdl2::SDL_Keycode m_key;
    public:
        Input();
        Input(sdl2::SDL_Keycode key);
        Input(const Input& other);
        Input& operator=(const Input& other);
        ~Input();
        bool IsKeyPressed();
        bool IsKeyDown();
        bool IsKeyUp();
        std::string GetKeyName();
        fixed12_32 Get1DAxis();
        Vector2<fixed12_32> Get2DAxis();
    };
}

#endif // INPUT_HPP