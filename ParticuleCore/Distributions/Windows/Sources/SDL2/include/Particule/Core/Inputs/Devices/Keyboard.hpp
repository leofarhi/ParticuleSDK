#pragma once

#include <Particule/Core/Inputs/Input.hpp>
#include <Particule/Core/System/sdl2.hpp>

namespace Particule::Core::Inputs::Devices
{
    template<typename T>
    class Keyboard // déclaration
    { 
    public:
        static inline Keyboard<Inputs::Button> GetKey();
    };

    // ---- Keyboard<Button> ----
    template<>
    class Keyboard<Inputs::Button> : public Input<Inputs::Button> {
    public:
        explicit Keyboard(sdl2::SDL_Keycode keycode) : m_key(keycode) {}
        std::string GetKeyName() const override;
    protected:
        Inputs::Button get() const override;

    private:
        sdl2::SDL_Keycode m_key;
    };

    // ---- Keyboard<Axis1D> (optionnel) ----
    template<>
    class Keyboard<Inputs::Axis1D> : public Input<Inputs::Axis1D> {
    public:
        explicit Keyboard(sdl2::SDL_Keycode pkeycode, sdl2::SDL_Keycode nkeycode, fixed12_32 onValue = 1.f)
            : m_pkey(pkeycode), m_nkey(nkeycode), m_on(onValue) {}

    protected:
        Inputs::Axis1D get() const override { return Inputs::Axis1D{}; }

    private:
        sdl2::SDL_Keycode m_pkey;
        sdl2::SDL_Keycode m_nkey;
        fixed12_32 m_on;
    };

    // ---- Keyboard<Axis2D> (optionnel) ----
    template<>
    class Keyboard<Inputs::Axis2D> : public Input<Inputs::Axis2D> {
    public:
        explicit Keyboard(
            sdl2::SDL_Keycode pkeycodeX, sdl2::SDL_Keycode nkeycodeX,
            sdl2::SDL_Keycode pkeycodeY, sdl2::SDL_Keycode nkeycodeY)
            : m_pkeyX(pkeycodeX), m_nkeyX(nkeycodeX), m_pkeyY(pkeycodeY), m_nkeyY(nkeycodeY) {}

    protected:
        Inputs::Axis2D get() const override { return Inputs::Axis2D{}; }

    private:
        sdl2::SDL_Keycode m_pkeyX;
        sdl2::SDL_Keycode m_nkeyX;
        sdl2::SDL_Keycode m_pkeyY;
        sdl2::SDL_Keycode m_nkeyY;
    };

    // ---- Keyboard<Pointer> (optionnel) ----
    template<>
    class Keyboard<Inputs::Pointer> : public Input<Inputs::Pointer> {
    public:
        explicit Keyboard(
            sdl2::SDL_Keycode pkeycodeX, sdl2::SDL_Keycode nkeycodeX,
            sdl2::SDL_Keycode pkeycodeY, sdl2::SDL_Keycode nkeycodeY)
            : m_pkeyX(pkeycodeX), m_nkeyX(nkeycodeX), m_pkeyY(pkeycodeY), m_nkeyY(nkeycodeY) {}

    protected:
        Inputs::Pointer get() const override { return m_pointer; }

    private:
        sdl2::SDL_Keycode m_pkeyX;
        sdl2::SDL_Keycode m_nkeyX;
        sdl2::SDL_Keycode m_pkeyY;
        sdl2::SDL_Keycode m_nkeyY;
        Inputs::Pointer m_pointer;
    };
}
