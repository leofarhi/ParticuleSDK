#ifndef PE_ENUM_LAYER_HPP
#define PE_ENUM_LAYER_HPP

typedef enum Layer
{
    LAYER_Default = 0,
    LAYER_TransparentFX = 1,
    LAYER_IgnoreRaycast = 2,
    LAYER_Water = 4,
    LAYER_UI = 5,
    LAYER_PostProcessing = 8
} Layer;

#endif // PE_ENUM_LAYER_HPP