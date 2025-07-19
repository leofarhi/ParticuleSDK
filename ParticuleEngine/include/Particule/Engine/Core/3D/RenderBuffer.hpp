#pragma once
#include <Particule/Core/ParticuleCore.hpp>
#include <span>
using namespace Particule::Core;

struct RenderBuffer
{
    Vector3<fixed12_32> offset;
    std::span<Vector3<fixed12_32>> position;
    std::span<Vector3<fixed12_32>> projected;
    std::span<Vector3<fixed12_32>> normal;
    Vector3<fixed12_32> cos_angle;
    Vector3<fixed12_32> sin_angle;

    static RenderBuffer* Alloc(size_t vertex_count, size_t normal_count)
    {
        RenderBuffer* buffer = new RenderBuffer();
        buffer->position = std::span<Vector3<fixed12_32>>(new Vector3<fixed12_32>[vertex_count], vertex_count);
        buffer->projected = std::span<Vector3<fixed12_32>>(new Vector3<fixed12_32>[vertex_count], vertex_count);
        buffer->normal = std::span<Vector3<fixed12_32>>(new Vector3<fixed12_32>[normal_count], normal_count);
        return buffer;
    }
};

#define MAX_VERTICES_BUFFER_SIZE 500
#define MAX_NORMALS_BUFFER_SIZE 500

extern RenderBuffer dynamic_render_buffer;