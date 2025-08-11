#pragma once
#include <Particule/Core/ParticuleCore.hpp>

namespace Particule::Engine {

    struct Skybox
    {
        Particule::Core::Color top;
        Particule::Core::Color bottom;

        fixed12_32 rgb_start[3];
        fixed12_32 rgbStep[3];
        int height;
        fixed12_32 fheight;


        Skybox(): top(Particule::Core::Color::Black), bottom(Particule::Core::Color::Black) {}
        Skybox(Particule::Core::Color top, Particule::Core::Color bottom) : top(top), bottom(bottom)
        {
            height = -1;
            fheight = fixed12_32(-1);
        }
        inline void CalculateGradient(int heightInt)
        {
            height = heightInt;
            fheight = fixed12_32(heightInt);
            rgb_start[0] = fixed12_32(top.R());
            rgb_start[1] = fixed12_32(top.G());
            rgb_start[2] = fixed12_32(top.B());
            rgbStep[0] = (fixed12_32(bottom.R()) - rgb_start[0]) / heightInt;
            rgbStep[1] = (fixed12_32(bottom.G()) - rgb_start[1]) / heightInt;
            rgbStep[2] = (fixed12_32(bottom.B()) - rgb_start[2]) / heightInt;
        }
    };

}