#pragma once
#include <Particule/Core/ParticuleCore.hpp>
using namespace Particule::Core;

typedef struct Material
{
    Vector2<fixed16_32> size;
    Vector2<fixed16_32> start;
    Vector2<fixed16_32> end;
    Sprite* sprite;
    Material() : size(0, 0), start(0, 0), end(0, 0), sprite(nullptr) {}
    Material(Sprite* sprite) : size(sprite->GetRect().w, sprite->GetRect().h),
                                        start(sprite->GetRect().x, sprite->GetRect().y),
                                        end(sprite->GetRect().x + sprite->GetRect().w, sprite->GetRect().y + sprite->GetRect().h),
                                        sprite(sprite) {}
} Material;