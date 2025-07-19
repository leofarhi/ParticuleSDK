#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/3D/Shapes.hpp>
#include <Particule/Engine/Components/Camera.hpp>
#include <algorithm>

/*__attribute__((always_inline))
static inline VertexInfo CalcEdge(const VertexInfo& a, const VertexInfo& b, int y)
{
    const int deltaY = b.iprojected.y - a.iprojected.y;
    if (deltaY == 0) return a;
    const fixed16_32 factor = fixed16_32(y - a.iprojected.y) / deltaY;
    const int x = a.iprojected.x + static_cast<int>(factor * (b.iprojected.x - a.iprojected.x));
    const fixed16_32 u = a.uv.x + ((b.uv.x - a.uv.x) * factor);
    const fixed16_32 v = a.uv.y + ((b.uv.y - a.uv.y) * factor);
    const fixed12_32 z = a.projected.z + ((b.projected.z - a.projected.z) * static_cast<fixed12_32>(factor));
    return {{fixed12_32::zero(),fixed12_32::zero(), z},{x, y}, {u, v}};
}*/

FORCE_INLINE
static VertexInfo CalcEdge(const VertexInfo& a, const VertexInfo& b, int y)
{
    const int deltaY = b.iprojected.y - a.iprojected.y;
    if (deltaY == 0) return a;

    const fixed16_32 factor = fixed16_32(y - a.iprojected.y) / deltaY;
    const int x = a.iprojected.x + static_cast<int>(factor * (b.iprojected.x - a.iprojected.x));
    const fixed12_32 z_interp = a.projected.z + (b.projected.z - a.projected.z) * static_cast<fixed12_32>(factor);

    // --- Clamping Z même s'il est < 0 ---
    const fixed16_64 epsilon = fixed16_64(0.01f);
    fixed16_64 za = static_cast<fixed16_64>(a.projected.z);
    fixed16_64 zb = static_cast<fixed16_64>(b.projected.z);

    if (za < epsilon) za = epsilon;
    if (zb < epsilon) zb = epsilon;

    const fixed16_64 inv_za = fixed16_64::one() / za;
    const fixed16_64 inv_zb = fixed16_64::one() / zb;
    const fixed16_64 inv_z = inv_za + (inv_zb - inv_za) * static_cast<fixed16_64>(factor);

    const fixed16_64 u_over_za = static_cast<fixed16_64>(a.uv.x) * inv_za;
    const fixed16_64 v_over_za = static_cast<fixed16_64>(a.uv.y) * inv_za;
    const fixed16_64 u_over_zb = static_cast<fixed16_64>(b.uv.x) * inv_zb;
    const fixed16_64 v_over_zb = static_cast<fixed16_64>(b.uv.y) * inv_zb;

    const fixed16_64 u_over_z = u_over_za + (u_over_zb - u_over_za) * static_cast<fixed16_64>(factor);
    const fixed16_64 v_over_z = v_over_za + (v_over_zb - v_over_za) * static_cast<fixed16_64>(factor);

    fixed16_64 safe_inv_z = inv_z;
    if (safe_inv_z < epsilon) safe_inv_z = epsilon;

    const fixed16_64 u64 = u_over_z / safe_inv_z;
    const fixed16_64 v64 = v_over_z / safe_inv_z;

    const fixed16_32 u = static_cast<fixed16_32>(u64);
    const fixed16_32 v = static_cast<fixed16_32>(v64);

    // Clamp final z visuel aussi, si besoin
    fixed12_32 z = z_interp;

    return {{fixed12_32::zero(), fixed12_32::zero(), z}, {x, y}, {u, v}};
}



template <bool TILING>
static inline void ProcessScanline(Window* __restrict  win, Material* __restrict material, Texture* __restrict texture, int y, const VertexInfo& A,const VertexInfo& B)
{
    const int startX = std::max(A.iprojected.x, 0);
    const int endX = std::min(B.iprojected.x, win->Width() - 1);
    if (startX >= endX) return;

    const fixed16_32 invLen = fixed16_32::one() / (B.iprojected.x - A.iprojected.x);
    const fixed16_32 du = (B.uv.x - A.uv.x) * invLen;
    const fixed16_32 dv = (B.uv.y - A.uv.y) * invLen;
    const fixed12_32 dz = (B.projected.z - A.projected.z) * static_cast<fixed12_32>(invLen);

    const int f = startX - A.iprojected.x;
    fixed16_32 u = A.uv.x + du * f;
    fixed16_32 v = A.uv.y + dv * f;
    fixed12_32 z = A.projected.z + dz * f;

    for (int x = startX; x <= endX; ++x) {
        if constexpr (TILING)
        {
            if (u >= material->end.x) u -= material->size.x;
            else if (u < material->start.x) u += material->size.x;
            if (v >= material->end.y) v -= material->size.y;
            else if (v < material->start.y) v += material->size.y;
        }
        if (z < Camera::z_buffer[x + y * Camera::screen_width])
        {
            //if (Texture::PutPixelImpl(texture, static_cast<int>(u), static_cast<int>(v), x, y))
            if (texture->PutPixel(static_cast<int>(u), static_cast<int>(v), x, y))
                Camera::z_buffer[x + y * Camera::screen_width] = z;
        }
        u += du;
        v += dv;
        z += dz;
    }
};

template <bool TILING>
void DrawTriangle(Material* material, VertexInfo v0, VertexInfo v1, VertexInfo v2)
{
    Window* win = Window::GetCurrentWindow();
    if (v1.iprojected.y < v0.iprojected.y) std::swap(v0, v1);
    if (v2.iprojected.y < v0.iprojected.y) std::swap(v0, v2);
    if (v2.iprojected.y < v1.iprojected.y) std::swap(v1, v2);

    if (v2.iprojected.y == v0.iprojected.y) return;

    int startY = std::max(v0.iprojected.y, 0);
    int endY = std::min(v2.iprojected.y, win->Height() - 1);

    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());
    for (int y = startY; y <= endY; ++y) {
        const bool second_half = y > v1.iprojected.y || v1.iprojected.y == v0.iprojected.y;
        VertexInfo A = CalcEdge(v0, v2, y);
        VertexInfo B = second_half ? CalcEdge(v1, v2, y) : CalcEdge(v0, v1, y);
        if (A.iprojected.x > B.iprojected.x)
            std::swap(A, B);
        ProcessScanline<TILING>(win, material, texture, y, A, B);
    }

};