#ifndef FONT_HPP
#define FONT_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/System/Window.hpp>
#include <Particule/Core/Graphics/Shapes/Pixel.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <span>
#include <cstdint>
#include <string>
#include <string_view>
#include <optional>
#include <algorithm>
#include <codecvt>
#include <locale>

namespace Particule::Core
{

    struct Character {
        int8_t left;
        int8_t top;
        int8_t right;
        int8_t bottom;
        std::span<const uint8_t> data;
    
        inline int width() const { return right - left; }
        inline int height() const { return bottom - top; }
    
        inline bool get(int x, int y) const {
            int index = y * width() + x;
            int byte = index / 8;
            int bit = 7 - (index % 8);
            if ((unsigned int)byte >= data.size()) return false;
            return (data[byte] >> bit) & 1;
        }

        int draw_scaled(int x, int y, const Color& color, fixed12_32 ratio) const
        {
            fixed12_32 scaled_width  = this->width() * ratio;
            fixed12_32 scaled_height = this->height() * ratio;
        
            // Clamp de destination
            int dst_x0 = std::max(x, 0);
            int dst_y0 = std::max(y, 0);
            int dst_x1 = std::min(x + int(scaled_width), SCREEN_WIDTH);
            int dst_y1 = std::min(y + int(scaled_height), SCREEN_HEIGHT);

            const int src_width  = width();
            const int src_height = height();
        
            if (src_width <= 0 || src_height <= 0 || dst_x1 <= dst_x0 || dst_y1 <= dst_y0)
                return int(scaled_width);
        
            // Ratio inverse : pour remonter à la source
            fixed12_32 x_step = fixed12_32(src_width)  / scaled_width;
            fixed12_32 y_step = fixed12_32(src_height) / scaled_height;
        
            // Precalculate src_y range
            fixed12_32 start_src_y_f = (dst_y0 - y) * y_step;
            fixed12_32 end_src_y_f   = (dst_y1 - y) * y_step;
        
            int start_sy = std::clamp(int(start_src_y_f), 0, src_height - 1);
            int end_sy   = std::clamp(int(end_src_y_f),   0, src_height);
        
            // Precalculate dst_y range from src_y range
            int draw_y_start = y + int(fixed12_32(start_sy) * scaled_height / src_height);
            int draw_y_end   = y + int(fixed12_32(end_sy)   * scaled_height / src_height);
            draw_y_start = std::clamp(draw_y_start, 0, SCREEN_HEIGHT);
            draw_y_end   = std::clamp(draw_y_end,   0, SCREEN_HEIGHT);
        
            for (int py = draw_y_start; py < draw_y_end; ++py) {
                fixed12_32 src_y_f = (py - y) * y_step;
                int sy = int(src_y_f); // garanti entre 0 et src_height - 1
        
                fixed12_32 src_x_f = (dst_x0 - x) * x_step;
                for (int px = dst_x0; px < dst_x1; ++px, src_x_f += x_step) {
                    int sx = int(src_x_f); // garanti entre 0 et src_width - 1
                    if (sx >= src_width) break; // simple sécurité en cas d'arrondi + overflow
        
                    if (get(sx, sy)) {
                        Particule::Core::DrawPixelUnsafe(px, py, color);
                    }
                }
            }
        
            return int(scaled_width);
        };

        int draw(int x, int y, const Color& color) const
        {
            // Clamp de destination
            int dst_x0 = std::max(x, 0);
            int dst_y0 = std::max(y, 0);
            int dst_x1 = std::min(x + width(), SCREEN_WIDTH);
            int dst_y1 = std::min(y + height(), SCREEN_HEIGHT);

            if (width() <= 0 || height() <= 0 || dst_x1 <= dst_x0 || dst_y1 <= dst_y0)
                return width();
        
            for (int py = dst_y0; py < dst_y1; ++py) {
                for (int px = dst_x0; px < dst_x1; ++px) {
                    if (get(px - x, py - y)) {
                        Particule::Core::DrawPixelUnsafe(px, py, color);
                    }
                }
            }
        
            return width();
        };

        inline int draw(int x, int y, const Color& color, fixed12_32 ratio) const {
            if (ratio == fixed12_32::one())
                return draw(x, y, color);
            else
                return draw_scaled(x, y, color, ratio);
        }
        
    };
    
    class Font
    {
    private:
        std::span<const uint8_t> data;
        int resolution;
        int count;
        int max_ascent;
        int max_descent;

        Font();
        Font(const Font& other) = delete;
        Font& operator=(const Font& other) = delete;

        std::optional<Character> get(char32_t codepoint) const;
        std::u32string to_u32(const std::string& str) const;
    public:
        Font(std::span<const uint8_t> binary);
        ~Font();

        inline int GetResolution() const { return resolution; }
        void DrawText(const std::string& text, int x, int y, const Color& color, int size);
        Vector2<int> GetTextSize(const std::string& text, int size);

        static Font* Load(std::string path);
        static void Unload(Font* font);
    };
}

#endif // FONT_HPP