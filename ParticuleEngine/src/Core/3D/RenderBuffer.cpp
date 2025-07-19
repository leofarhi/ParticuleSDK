#include <Particule/Engine/Core/3D/RenderBuffer.hpp>

static Vector3<fixed12_32> position_buffer[MAX_VERTICES_BUFFER_SIZE];
static Vector3<fixed12_32> projected_buffer[MAX_VERTICES_BUFFER_SIZE];
static Vector3<fixed12_32> normal_buffer[MAX_NORMALS_BUFFER_SIZE];

RenderBuffer dynamic_render_buffer = {
    Vector3<fixed12_32>(0, 0, 0),
    std::span<Vector3<fixed12_32>>(position_buffer, MAX_VERTICES_BUFFER_SIZE),
    std::span<Vector3<fixed12_32>>(projected_buffer, MAX_VERTICES_BUFFER_SIZE),
    std::span<Vector3<fixed12_32>>(normal_buffer, MAX_NORMALS_BUFFER_SIZE),
    Vector3<fixed12_32>(0, 0, 0), Vector3<fixed12_32>(0, 0, 0)
};