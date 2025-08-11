#ifndef PE_ENUM_TAG_HPP
#define PE_ENUM_TAG_HPP

namespace Particule::Engine {

    typedef enum Tag
    {
        TAG_Untagged = 0,
        TAG_Player = 1,
        TAG_Enemy = 2,
        TAG_Wall = 3,
        TAG_Ground = 4,
        TAG_Bullet = 5,
        TAG_MainCamera = 6,
        TAG_PostProcessing = 7
    } Tag;

}

#endif // PE_ENUM_TAG_HPP