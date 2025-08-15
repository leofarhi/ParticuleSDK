#ifndef REFERENCE_INPUT_HPP
#define REFERENCE_INPUT_HPP

#include <Particule/Core/Inputs/Input.hpp>
#include <Particule/Core/Inputs/Devices.hpp>
#include <Particule/Core/System/Basic.hpp>

namespace Particule::Core
{
    #ifndef GetInput
    #define GetInput(TYPE, str)(static_cast<const Inputs::Input<TYPE>*>(nullptr))
    #endif
}

#endif // REFERENCE_INPUT_HPP