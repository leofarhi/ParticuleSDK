#ifndef REDEFINE_HPP
#define REDEFINE_HPP
#include <Particule/Core/System/Input.hpp>


namespace Particule::Core
{
    #ifndef GetInput
    #define GetInput(str) Input()
    #endif

    #ifndef GetResource
    #define GetResource(str) Resource(str)
    #endif
}

#endif // REDEFINE_HPP