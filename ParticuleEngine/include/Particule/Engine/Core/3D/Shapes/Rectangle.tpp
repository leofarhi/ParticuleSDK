#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/3D/Shapes.hpp>
#include <Particule/Engine/Components/Camera.hpp>
#include <algorithm>

template <bool TILING>
static inline void DrawRectSimplified(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl)
{
    Window* win = Window::GetCurrentWindow();
    if (tl.iprojected.x > tr.iprojected.x) {
        std::swap(tl, tr);
        std::swap(bl, br);
    }
    if (tl.iprojected.y > bl.iprojected.y) {
        std::swap(tl, bl);
        std::swap(tr, br);
    }

    const int startY = std::max(tl.iprojected.y, 0);
    const int endY = std::min(br.iprojected.y, win->Height() - 1);
    const int startX = std::max(tl.iprojected.x, 0);
    const int endX = std::min(br.iprojected.x, win->Width() - 1);

    const int height = br.iprojected.y - tl.iprojected.y;
    const int width = br.iprojected.x - tl.iprojected.x;

    if (width <= 0 || height <= 0) return;

    const fixed16_32 inv_height = fixed16_32::one() / height;
    const fixed16_32 inv_length = fixed16_32::one() / width;

    const fixed16_32 du = (br.uv.x - tl.uv.x) * inv_length;
    const fixed16_32 dv = (br.uv.y - tl.uv.y) * inv_height;
    const fixed12_32 dz = (br.projected.z - tl.projected.z) * static_cast<fixed12_32>(inv_height * inv_length);

    fixed16_32 v = tl.uv.y + dv * (startY - tl.iprojected.y);
    fixed12_32 z_start = tl.projected.z + dz * ((startY - tl.iprojected.y) * width);
    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());

    for (int y = startY; y <= endY; ++y) {
        fixed16_32 u = tl.uv.x + du * (startX - tl.iprojected.x);
        fixed16_32 v_curr = v;
        fixed12_32 z = z_start + dz * (startX - tl.iprojected.x);
        if constexpr (TILING) {
            if (v >= material->end.y) v -= material->size.y;
            else if (v < material->start.y) v += material->size.y;
        }
        for (int x = startX; x <= endX; ++x) {
            if constexpr (TILING) {
                if (u >= material->end.x) u -= material->size.x;
                else if (u < material->start.x) u += material->size.x;
            }
            if (z < Camera::z_buffer[x + y * Camera::screen_width]) {
                if (texture->PutPixel(static_cast<int>(u), static_cast<int>(v_curr), x, y)) {
                    Camera::z_buffer[x + y * Camera::screen_width] = z;
                }
            }
            u += du;
            z += dz;
        }
        v += dv;
        z_start += dz * width;
    }
}

template <bool TILING, bool SQUARE_UV>
void DrawRect(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl)
{
    Window* win = Window::GetCurrentWindow();
    if constexpr (SQUARE_UV) {
        DrawRectSimplified<TILING>(material, tl, tr, br, bl);
        return;
    }

    if (tl.iprojected.x > tr.iprojected.x) {
        std::swap(tl, tr);
        std::swap(bl, br);
    }
    if (tl.iprojected.y > bl.iprojected.y) {
        std::swap(tl, bl);
        std::swap(tr, br);
    }

    const int startY = std::max(tl.iprojected.y, 0);
    const int endY = std::min(br.iprojected.y, win->Height() - 1);
    const int startX = std::max(tl.iprojected.x, 0);
    const int endX = std::min(br.iprojected.x, win->Width() - 1);

    const int height = br.iprojected.y - tl.iprojected.y;
    const int width = br.iprojected.x - tl.iprojected.x;

    if (width <= 0 || height <= 0) return;

    const fixed16_32 inv_height = fixed16_32::one() / height;
    const fixed16_32 inv_length = fixed16_32::one() / width;

    const fixed16_32 du_st = (bl.uv.x - tl.uv.x) * inv_height;
    const fixed16_32 dv_st = (bl.uv.y - tl.uv.y) * inv_height;
    const fixed16_32 du_end = (br.uv.x - tr.uv.x) * inv_height;
    const fixed16_32 dv_end = (br.uv.y - tr.uv.y) * inv_height;

    const fixed12_32 dz_st = (bl.projected.z - tl.projected.z) * static_cast<fixed12_32>(inv_height);
    const fixed12_32 dz_end = (br.projected.z - tr.projected.z) * static_cast<fixed12_32>(inv_height);

    const int deltaStartY = startY - tl.iprojected.y;
    fixed16_32 u_st = tl.uv.x + du_st * deltaStartY;
    fixed16_32 v_st = tl.uv.y + dv_st * deltaStartY;
    fixed16_32 u_end = tr.uv.x + du_end * deltaStartY;
    fixed16_32 v_end = tr.uv.y + dv_end * deltaStartY;

    fixed12_32 z_st = tl.projected.z + dz_st * deltaStartY;
    fixed12_32 z_end = tr.projected.z + dz_end * deltaStartY;

    const int deltaX = startX - tl.iprojected.x;
    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());

    for (int y = startY; y <= endY; ++y) {
        const fixed16_32 du = (u_end - u_st) * inv_length;
        const fixed16_32 dv = (v_end - v_st) * inv_length;
        const fixed12_32 dz = (z_end - z_st) * static_cast<fixed12_32>(inv_length);

        fixed16_32 u = u_st + du * deltaX;
        fixed16_32 v = v_st + dv * deltaX;
        fixed12_32 z = z_st + dz * deltaX;

        for (int x = startX; x <= endX; ++x) {
            if constexpr (TILING) {
                if (u >= material->end.x) u -= material->size.x;
                else if (u < material->start.x) u += material->size.x;
                if (v >= material->end.y) v -= material->size.y;
                else if (v < material->start.y) v += material->size.y;
            }
            if (z < Camera::z_buffer[x + y * Camera::screen_width]) {
                if (texture->PutPixel(static_cast<int>(u), static_cast<int>(v), x, y)) {
                    Camera::z_buffer[x + y * Camera::screen_width] = z;
                }
            }
            u += du;
            v += dv;
            z += dz;
        }

        u_st += du_st;
        v_st += dv_st;
        z_st += dz_st;
        u_end += du_end;
        v_end += dv_end;
        z_end += dz_end;
    }
}
