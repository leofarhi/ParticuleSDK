#ifndef INPUT_TYPES_HPP
#define INPUT_TYPES_HPP

#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/Types/Fixed.hpp>

namespace Particule::Core::Inputs
{

    // Valeurs retournées par get()
    struct Button {
        bool down = false;     // vient d'être pressé cette frame
        bool pressed = false;  // maintenu
        bool up = false;       // vient d'être relâché cette frame
    };

    struct Axis1D { fixed12_32 v = 0; };
    struct Axis2D { fixed12_32 x = 0, y = 0; };

    // Boutons du pointer
    enum class PointerButton { Left=0, Right=1, Middle=2, X1=3, X2=4, Count };

    // État complet d'un pointer
    struct Pointer {
        Vector2<int> pos {0,0};     // position absolue (pixels)
        Vector2<fixed12_32> delta {0,0};   // delta dans le même espace que pos
        Vector2<fixed12_32> scroll {0,0};  // vertical/horizontal
        Button buttons[(int)PointerButton::Count]{}; // L/R/M/X1/X2

        const Button& button(PointerButton b) const {
            return buttons[(int)b];
        }
    };
}

#endif // INPUT_TYPES_HPP