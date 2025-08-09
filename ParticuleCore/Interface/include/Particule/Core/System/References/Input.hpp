#ifndef REFERENCE_INPUT_HPP
#define REFERENCE_INPUT_HPP

#include <Particule/Core/System/Input.hpp>
#include <Particule/Core/System/Basic.hpp>

namespace Particule::Core
{
    #ifndef GetInput
    #define GetInput(str) Input()
    #endif

    /*Exemple de redéfinition de GetInput pour le compilateur GCC
    #define GetInput(str)(\
        CONST_STR_CMP(str, "A") ? Input(gint::KEY_UP) : \
        CONST_STR_CMP(str, "B") ? Input(gint::KEY_DOWN) : \
        Input()\
        )
    */
}

#endif // REFERENCE_INPUT_HPP