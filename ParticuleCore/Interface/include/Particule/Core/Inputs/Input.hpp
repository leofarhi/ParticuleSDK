#ifndef INPUT_HPP
#define INPUT_HPP
#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/Inputs/Types.hpp>
#include <string>

namespace Particule::Core::Inputs
{
    // ================== Base Input<T> ==================
    // Déclarations générales (on spécialise par T)
    template<typename T> class Input;

    // ---- Spécialisation: Button ----
    template<>
    class Input<Inputs::Button> {
    public:
        virtual ~Input() = default;

        bool    IsDown()                    const   { return get().down; }
        bool    IsPressed()                 const   { return get().pressed; }
        bool    IsUp()                      const   { return get().up; }
        virtual std::string GetKeyName()    const   { return "UNKNOWN"; }

    protected:
        // À implémenter par le device backend (Keyboard<Button>, etc.)
        virtual Inputs::Button get() const { return Inputs::Button{}; }
    };

    // ---- Spécialisation: Axis1D ----
    template<>
    class Input<Inputs::Axis1D> {
    public:
        virtual ~Input() = default;

        fixed12_32 Value() const { return get().v; }

    protected:
        virtual Inputs::Axis1D get() const { return Inputs::Axis1D{}; }
    };

    // ---- Spécialisation: Axis2D ----
    template<>
    class Input<Inputs::Axis2D> {
    public:
        virtual ~Input() = default;

        Vector2<fixed12_32> Value() const { auto v = get(); return {v.x, v.y}; }

    protected:
        virtual Inputs::Axis2D get() const { return Inputs::Axis2D{}; }
    };

    // ---- Spécialisation: Pointer ----
    template<>
    class Input<Inputs::Pointer> {
    public:
        virtual ~Input() = default;

        Vector2<fixed12_32> Position() const { return get().pos; }
        Vector2<fixed12_32> Delta() const { return get().delta; }
        Vector2<fixed12_32> Scroll() const { return get().scroll; }
        const Inputs::Button& Button(Inputs::PointerButton b) const { return get().button(b); }

    protected:
        virtual Inputs::Pointer get() const { return Inputs::Pointer{}; };
    };
}

namespace Inputs = Particule::Core::Inputs;

#endif // INPUT_HPP