#ifndef REDEFINE_HPP
#define REDEFINE_HPP
#include <Particule/Core/System/Input.hpp>
#include <Particule/Core/System/Basic.hpp>

namespace Particule::Core::Resources
{
    inline void* __builtInAssetsRaw[] = {nullptr};
}

namespace Particule::Core
{
    #ifndef GetInput
    #define GetInput(str) Input()
    #endif

    #ifndef GetResourceID
    #define GetResourceID(str) -1
    #endif

    #define EXTERNAL_ASSET_COUNT 0 // Nombre d'assets externes, à ajuster selon les besoins
    #define EXTERNAL_ASSET_PATH "assets" // Chemin des assets externes

    /*Exemple de redéfinition de GetInput pour le compilateur GCC
    #define GetInput(str)(\
        CONST_STR_CMP(str, "A") ? Input(gint::KEY_UP) : \
        CONST_STR_CMP(str, "B") ? Input(gint::KEY_DOWN) : \
        Input()\
        )
    */
}

#endif // REDEFINE_HPP