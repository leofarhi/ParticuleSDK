#pragma once
#include <Particule/Core/ParticuleCore.hpp>
using namespace Particule::Core;
#include "Material.hpp"

typedef struct VertexInfo
{
    Vector3<fixed12_32> projected; //Coordonnées de l'espace 2D + z
    Vector2<int> iprojected; //Coordonnées de l'espace 2D
    Vector2<fixed16_32> uv; // Coordonnées de texture

    VertexInfo() : projected(0, 0, 0), iprojected(0, 0), uv(0, 0) {}
    VertexInfo(Vector3<fixed12_32> projected,Vector2<int> iprojected,Vector2<fixed16_32> uv) : projected(projected), iprojected(iprojected), uv(uv) {}
    VertexInfo(Vector3<fixed12_32> projected,Vector2<fixed16_32> uv) : projected(projected), iprojected((int)projected.x, (int)projected.y), uv(uv) {}

} VertexInfo;

template <bool TILING>
void DrawTriangle(Material* material, VertexInfo v0, VertexInfo v1, VertexInfo v2);
template <bool TILING, bool SQUARE_UV>
void DrawRect(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl);
template <bool TILING, bool SQUARE_UV>
void DrawHorzParallelogram(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl);
template <bool TILING, bool SQUARE_UV>
void DrawVertParallelogram(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl);

#define ConstDrawTriangle(tiling, ...) \
    do { \
        if (tiling) DrawTriangle<true>(__VA_ARGS__); \
        else      DrawTriangle<false>(__VA_ARGS__); \
    } while (0)

#define ConstDrawRect(tiling, square_uv, ...) \
    do { \
        if      (tiling && square_uv) DrawRect<true,  true>(__VA_ARGS__); \
        else if (tiling && !square_uv) DrawRect<true,  false>(__VA_ARGS__); \
        else if (!tiling && square_uv) DrawRect<false, true>(__VA_ARGS__); \
        else                          DrawRect<false, false>(__VA_ARGS__); \
    } while(0)

#define ConstDrawHorzParallelogram(tiling, square_uv, ...) \
    do { \
        if      (tiling && square_uv) DrawHorzParallelogram<true,  true>(__VA_ARGS__); \
        else if (tiling && !square_uv) DrawHorzParallelogram<true,  false>(__VA_ARGS__); \
        else if (!tiling && square_uv) DrawHorzParallelogram<false, true>(__VA_ARGS__); \
        else                          DrawHorzParallelogram<false, false>(__VA_ARGS__); \
    } while(0)

#define ConstDrawVertParallelogram(tiling, square_uv, ...) \
    do { \
        if      (tiling && square_uv) DrawVertParallelogram<true,  true>(__VA_ARGS__); \
        else if (tiling && !square_uv) DrawVertParallelogram<true,  false>(__VA_ARGS__); \
        else if (!tiling && square_uv) DrawVertParallelogram<false, true>(__VA_ARGS__); \
        else                          DrawVertParallelogram<false, false>(__VA_ARGS__); \
    } while(0)

#include "Shapes/Triangle.tpp"
#include "Shapes/Rectangle.tpp"
#include "Shapes/Parallelogram.tpp"