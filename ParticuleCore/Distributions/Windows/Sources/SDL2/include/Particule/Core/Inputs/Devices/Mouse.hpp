#pragma once

#include <Particule/Core/Inputs/Input.hpp>

namespace Particule::Core::Inputs::Devices
{
    template<typename T>
    class Mouse; // déclaration

    // ---- Mouse<Pointer> ----
    template<>
    class Mouse<Inputs::Pointer> : public Input<Inputs::Pointer> {
    public:
        explicit Mouse(){}

    protected:
        Inputs::Pointer get() const override;
    };

    // ---- Mouse<Button> (optionnel) ----
    template<>
    class Mouse<Inputs::Button> : public Input<Inputs::Button> {
    public:
        explicit Mouse(Inputs::PointerButton button) : m_button(button) {}

    protected:
        Inputs::Button get() const override;

    private:
        Inputs::PointerButton m_button;
    };

    //TODO
    // ---- Mouse<Axis1D> (optionnel) ----
    /*template<>
    class Mouse<Inputs::Axis1D> : public Input<Inputs::Axis1D> {
    public:
        explicit Mouse(int axis, fixed12_32 onValue = 1.f)
            : m_axis(axis), m_on(onValue) {}

    protected:
        Inputs::Axis1D get() const override { return Inputs::Axis1D{}; }

    private:
        int m_axis;
        fixed12_32 m_on;
    };

    // ---- Mouse<Axis2D> (optionnel) ----
    template<>
    class Mouse<Inputs::Axis2D> : public Input<Inputs::Axis2D> {
    public:
        explicit Mouse(int axisX, int axisY)
            : m_axisX(axisX), m_axisY(axisY) {}

    protected:
        Inputs::Axis2D get() const override { return Inputs::Axis2D{}; }

    private:
        int m_axisX;
        int m_axisY;
    };*/
}
