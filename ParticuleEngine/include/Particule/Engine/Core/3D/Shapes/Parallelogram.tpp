#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/3D/Shapes.hpp>
#include <Particule/Engine/Components/Camera.hpp>
#include <algorithm>

template <bool TILING>
inline void DrawHorzParallelogramSimplified(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl)
{
    Window* win = Window::GetCurrentWindow();
    if (tl.iprojected.y > bl.iprojected.y) { std::swap(tl, bl); std::swap(tr, br); }
    if (tl.iprojected.x > tr.iprojected.x) { std::swap(tl, tr); std::swap(bl, br); }

    int startY = std::max(tl.iprojected.y, 0);
    int endY = std::min(br.iprojected.y, win->Height() - 1);
    int height = br.iprojected.y - tl.iprojected.y;
    if (height <= 0) return;

    fixed16_32 inv_height = fixed16_32::one() / height;
    int deltaStartY = startY - tl.iprojected.y;

    fixed16_32 x_st = fixed16_32(tl.iprojected.x) + fixed16_32(bl.iprojected.x - tl.iprojected.x) * (inv_height * deltaStartY);
    fixed16_32 x_end = fixed16_32(tr.iprojected.x) + fixed16_32(br.iprojected.x - tr.iprojected.x) * (inv_height * deltaStartY);
    fixed16_32 dx_st = fixed16_32(bl.iprojected.x - tl.iprojected.x) * inv_height;
    fixed16_32 dx_end = fixed16_32(br.iprojected.x - tr.iprojected.x) * inv_height;

    fixed16_32 dv = (br.uv.y - tl.uv.y) * inv_height;
    fixed16_32 v = tl.uv.y + dv * deltaStartY;

    fixed12_32 dz = (br.projected.z - tl.projected.z) * static_cast<fixed12_32>(inv_height);
    fixed12_32 z_line = tl.projected.z + dz * deltaStartY;

    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());
    for (int y = startY; y <= endY; ++y) {
        int x_st_int = int(x_st);
        int x_end_int = int(x_end);
        int startX = std::max(x_st_int, 0);
        int endX = std::min(x_end_int, win->Width() - 1);
        int line_width = x_end_int - x_st_int;
        if constexpr (TILING) {
            if (v >= material->end.y) v -= material->size.y;
            else if (v < material->start.y) v += material->size.y;
        }
        if (line_width > 0) {
            fixed16_32 du = (br.uv.x - tl.uv.x) / line_width;
            fixed16_32 u = tl.uv.x + du * (startX - x_st_int);

            fixed12_32 dz_inner = (br.projected.z - tl.projected.z) / line_width;
            fixed12_32 z = z_line + dz_inner * (startX - x_st_int);

            for (int x = startX; x <= endX; ++x) {
                if constexpr (TILING) {
                    if (u >= material->end.x) u -= material->size.x;
                    else if (u < material->start.x) u += material->size.x;
                }
                if (z < Camera::z_buffer[x + y * Camera::screen_width]) {
                    if (texture->PutPixel(static_cast<int>(u), static_cast<int>(v), x, y)) {
                        Camera::z_buffer[x + y * Camera::screen_width] = z;
                    }
                }
                u += du;
                z += dz_inner;
            }
        }
        x_st += dx_st;
        x_end += dx_end;
        v += dv;
        z_line += dz;
    }
}

template <bool TILING, bool SQUARE_UV>
void DrawHorzParallelogram(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl)
{
    Window* win = Window::GetCurrentWindow();
    if constexpr (SQUARE_UV) {
        DrawHorzParallelogramSimplified<TILING>(material, tl, tr, br, bl);
        return;
    }

    if (tl.iprojected.y > bl.iprojected.y) { std::swap(tl, bl); std::swap(tr, br); }
    if (tl.iprojected.x > tr.iprojected.x) { std::swap(tl, tr); std::swap(bl, br); }

    int startY = std::max(tl.iprojected.y, 0);
    int endY = std::min(br.iprojected.y, win->Height() - 1);
    int height = br.iprojected.y - tl.iprojected.y;
    if (height <= 0) return;

    fixed16_32 inv_height = fixed16_32::one() / height;

    fixed16_32 du_st = (bl.uv.x - tl.uv.x) * inv_height;
    fixed16_32 dv_st = (bl.uv.y - tl.uv.y) * inv_height;
    fixed16_32 du_end = (br.uv.x - tr.uv.x) * inv_height;
    fixed16_32 dv_end = (br.uv.y - tr.uv.y) * inv_height;

    fixed12_32 dz_st = (bl.projected.z - tl.projected.z) * static_cast<fixed12_32>(inv_height);
    fixed12_32 dz_end = (br.projected.z - tr.projected.z) * static_cast<fixed12_32>(inv_height);

    int deltaStartY = startY - tl.iprojected.y;

    fixed16_32 u_st = tl.uv.x + du_st * deltaStartY;
    fixed16_32 v_st = tl.uv.y + dv_st * deltaStartY;
    fixed16_32 u_end = tr.uv.x + du_end * deltaStartY;
    fixed16_32 v_end = tr.uv.y + dv_end * deltaStartY;

    fixed12_32 z_st = tl.projected.z + dz_st * deltaStartY;
    fixed12_32 z_end = tr.projected.z + dz_end * deltaStartY;

    fixed16_32 x_st = fixed16_32(tl.iprojected.x) + fixed16_32(bl.iprojected.x - tl.iprojected.x) * (inv_height * deltaStartY);
    fixed16_32 x_end = fixed16_32(tr.iprojected.x) + fixed16_32(br.iprojected.x - tr.iprojected.x) * (inv_height * deltaStartY);

    fixed16_32 dx_st = fixed16_32(bl.iprojected.x - tl.iprojected.x) * inv_height;
    fixed16_32 dx_end = fixed16_32(br.iprojected.x - tr.iprojected.x) * inv_height;

    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());

    for (int y = startY; y <= endY; ++y) {
        int x_st_int = int(x_st);
        int x_end_int = int(x_end);
        int startX = std::max(x_st_int, 0);
        int endX = std::min(x_end_int, win->Width() - 1);
        int length = x_end_int - x_st_int;

        if (length > 0) {
            fixed16_32 inv_length = fixed16_32::one() / length;

            fixed16_32 du = (u_end - u_st) * inv_length;
            fixed16_32 dv = (v_end - v_st) * inv_length;
            fixed12_32 dz = (z_end - z_st) * static_cast<fixed12_32>(inv_length);

            fixed16_32 u = u_st + du * (startX - x_st_int);
            fixed16_32 v = v_st + dv * (startX - x_st_int);
            fixed12_32 z = z_st + dz * (startX - x_st_int);

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
        }

        x_st += dx_st;
        x_end += dx_end;
        u_st += du_st;
        v_st += dv_st;
        u_end += du_end;
        v_end += dv_end;
        z_st += dz_st;
        z_end += dz_end;
    }
}


template <bool TILING>
inline void DrawVertParallelogramSimplified(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl)
{
    Window* win = Window::GetCurrentWindow();
    if (tl.iprojected.x > tr.iprojected.x) { std::swap(tl, tr); std::swap(bl, br); }
    if (tl.iprojected.y > bl.iprojected.y) { std::swap(tl, bl); std::swap(tr, br); }

    int startX = std::max(tl.iprojected.x, 0);
    int endX = std::min(tr.iprojected.x, win->Width() - 1);
    int width = tr.iprojected.x - tl.iprojected.x;
    if (width <= 0) return;

    fixed16_32 inv_width = fixed16_32::one() / width;
    fixed16_32 dy_top = fixed16_32(tr.iprojected.y - tl.iprojected.y) * inv_width;
    fixed16_32 dy_bottom = fixed16_32(br.iprojected.y - bl.iprojected.y) * inv_width;

    fixed12_32 dz = (br.projected.z - tl.projected.z) * static_cast<fixed12_32>(inv_width);
    fixed12_32 z = tl.projected.z + dz * (startX - tl.iprojected.x);

    int deltaX = startX - tl.iprojected.x;
    fixed16_32 y_top = fixed16_32(tl.iprojected.y) + dy_top * deltaX;
    fixed16_32 y_bottom = fixed16_32(bl.iprojected.y) + dy_bottom * deltaX;

    fixed16_32 du = (br.uv.x - tl.uv.x) * inv_width;
    fixed16_32 u = tl.uv.x + du * deltaX;

    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());
    for (int x = startX; x <= endX; ++x) {
        int y_top_int = int(y_top);
        int y_bottom_int = int(y_bottom);
        int startY = std::max(y_top_int, 0);
        int endY = std::min(y_bottom_int, win->Height() - 1);
        int line_height = y_bottom_int - y_top_int;
        if constexpr (TILING) {
            if (u >= material->end.x) u -= material->size.x;
            else if (u < material->start.x) u += material->size.x;
        }
        if (line_height > 0) {
            fixed16_32 dv = (br.uv.y - tl.uv.y) / line_height;
            fixed12_32 dz_line = (br.projected.z - tl.projected.z) / line_height;
            fixed16_32 v = tl.uv.y + dv * (startY - y_top_int);
            fixed12_32 zv = z + dz_line * (startY - y_top_int);

            for (int y = startY; y <= endY; ++y) {
                if constexpr (TILING) {
                    if (v >= material->end.y) v -= material->size.y;
                    else if (v < material->start.y) v += material->size.y;
                }
                if (zv < Camera::z_buffer[x + y * Camera::screen_width]) {
                    if (texture->PutPixel(static_cast<int>(u), static_cast<int>(v), x, y)) {
                        Camera::z_buffer[x + y * Camera::screen_width] = zv;
                    }
                }
                v += dv;
                zv += dz_line;
            }
        }

        y_top += dy_top;
        y_bottom += dy_bottom;
        u += du;
        z += dz;
    }
}

template <bool TILING, bool SQUARE_UV>
void DrawVertParallelogram(Material* material, VertexInfo tl, VertexInfo tr, VertexInfo br, VertexInfo bl)
{
    Window* win = Window::GetCurrentWindow();
    if constexpr (SQUARE_UV) {
        DrawVertParallelogramSimplified<TILING>(material, tl, tr, br, bl);
        return;
    }

    if (tl.iprojected.x > tr.iprojected.x) { std::swap(tl, tr); std::swap(bl, br); }
    if (tl.iprojected.y > bl.iprojected.y) { std::swap(tl, bl); std::swap(tr, br); }

    int startX = std::max(tl.iprojected.x, 0);
    int endX = std::min(tr.iprojected.x, win->Width() - 1);
    int width = tr.iprojected.x - tl.iprojected.x;
    if (width <= 0) return;

    fixed16_32 inv_width = fixed16_32::one() / width;

    fixed16_32 du_top = (tr.uv.x - tl.uv.x) * inv_width;
    fixed16_32 dv_top = (tr.uv.y - tl.uv.y) * inv_width;
    fixed16_32 du_bottom = (br.uv.x - bl.uv.x) * inv_width;
    fixed16_32 dv_bottom = (br.uv.y - bl.uv.y) * inv_width;

    fixed12_32 dz_top = (tr.projected.z - tl.projected.z) * static_cast<fixed12_32>(inv_width);
    fixed12_32 dz_bottom = (br.projected.z - bl.projected.z) * static_cast<fixed12_32>(inv_width);

    fixed16_32 dy_top = fixed16_32(tr.iprojected.y - tl.iprojected.y) * inv_width;
    fixed16_32 dy_bottom = fixed16_32(br.iprojected.y - bl.iprojected.y) * inv_width;

    int deltaX = startX - tl.iprojected.x;

    fixed16_32 u_top = tl.uv.x + du_top * deltaX;
    fixed16_32 v_top = tl.uv.y + dv_top * deltaX;
    fixed16_32 u_bottom = bl.uv.x + du_bottom * deltaX;
    fixed16_32 v_bottom = bl.uv.y + dv_bottom * deltaX;

    fixed12_32 z_top = tl.projected.z + dz_top * deltaX;
    fixed12_32 z_bottom = bl.projected.z + dz_bottom * deltaX;

    fixed16_32 y_top = fixed16_32(tl.iprojected.y) + dy_top * deltaX;
    fixed16_32 y_bottom = fixed16_32(bl.iprojected.y) + dy_bottom * deltaX;

    Texture* texture = static_cast<Texture*>(material->sprite->GetTexture());
    for (int x = startX; x <= endX; ++x) {
        int y_top_int = int(y_top);
        int y_bottom_int = int(y_bottom);
        int startY = std::max(y_top_int, 0);
        int endY = std::min(y_bottom_int, win->Height() - 1);
        int length = y_bottom_int - y_top_int;

        if (length > 0) {
            fixed16_32 inv_length = fixed16_32::one() / length;

            fixed16_32 du = (u_bottom - u_top) * inv_length;
            fixed16_32 dv = (v_bottom - v_top) * inv_length;
            fixed12_32 dz = (z_bottom - z_top) * static_cast<fixed12_32>(inv_length);

            fixed16_32 u = u_top + du * (startY - y_top_int);
            fixed16_32 v = v_top + dv * (startY - y_top_int);
            fixed12_32 z = z_top + dz * (startY - y_top_int);

            for (int y = startY; y <= endY; ++y) {
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
        }

        y_top += dy_top;
        y_bottom += dy_bottom;
        u_top += du_top;
        v_top += dv_top;
        u_bottom += du_bottom;
        v_bottom += dv_bottom;
        z_top += dz_top;
        z_bottom += dz_bottom;
    }
}
