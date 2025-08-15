#pragma once

#include <Particule/Core/Inputs/Input.hpp>
#include <Particule/Core/System/gint.hpp>
#include <string>
#include <algorithm>

namespace Particule::Core::Inputs::Devices
{
    
    template<typename T>
    class Keyboard // déclaration
    {
    public:
        static Keyboard<Inputs::Button> GetKey();
    };

    // ---- Keyboard<Button> ----
    template<>
    class Keyboard<Inputs::Button> : public Input<Inputs::Button> {
    public:
        explicit Keyboard(int keycode) : m_key(keycode) {}
        std::string GetKeyName() const override;
    protected:
        Inputs::Button get() const override { 
            return Inputs::Button{
                static_cast<bool>(keypressed(m_key)),
                static_cast<bool>(keydown(m_key)),
                static_cast<bool>(keyreleased(m_key))
            }; }

    private:
        int m_key;
    };

    // ---- Keyboard<Axis1D> (optionnel) ----
    template<>
    class Keyboard<Inputs::Axis1D> : public Input<Inputs::Axis1D> {
    public:
        explicit Keyboard(int pkeycode, int nkeycode, fixed12_32 onValue = 1.f)
            : m_pkey(pkeycode), m_nkey(nkeycode), m_on(onValue) {}

    protected:
        Inputs::Axis1D get() const override { return Inputs::Axis1D{}; }

    private:
        int m_pkey;
        int m_nkey;
        fixed12_32 m_on;
    };

    // ---- Keyboard<Axis2D> (optionnel) ----
    template<>
    class Keyboard<Inputs::Axis2D> : public Input<Inputs::Axis2D> {
    public:
        explicit Keyboard(int pkeycodeX, int nkeycodeX, int pkeycodeY, int nkeycodeY)
            : m_pkeyX(pkeycodeX), m_nkeyX(nkeycodeX), m_pkeyY(pkeycodeY), m_nkeyY(nkeycodeY) {}

    protected:
        Inputs::Axis2D get() const override { return Inputs::Axis2D{}; }

    private:
        int m_pkeyX;
        int m_nkeyX;
        int m_pkeyY;
        int m_nkeyY;
    };

    // ---- Keyboard<Pointer> (optionnel) ----
    template<>
    class Keyboard<Inputs::Pointer> : public Input<Inputs::Pointer> {
    public:
        explicit Keyboard(int pkeycodeX, int nkeycodeX, int pkeycodeY, int nkeycodeY)
            : m_pkeyX(pkeycodeX), m_nkeyX(nkeycodeX), m_pkeyY(pkeycodeY), m_nkeyY(nkeycodeY) {}

    protected:
        Inputs::Pointer get() const override { return m_pointer; }

    private:
        int m_pkeyX;
        int m_nkeyX;
        int m_pkeyY;
        int m_nkeyY;
        Inputs::Pointer m_pointer;
    };
}
