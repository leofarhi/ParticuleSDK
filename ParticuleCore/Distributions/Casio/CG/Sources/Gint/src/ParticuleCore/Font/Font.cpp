#include <Particule/Core/Font/Font.hpp>
#include <stdexcept>

namespace Particule::Core
{
    static inline bool is_ascii(const std::string& str) {
        return std::all_of(str.begin(), str.end(), [](unsigned char c) { return c < 128; });
    }

    Font::Font(std::span<const uint8_t> binary) : data(binary), max_ascent(0), max_descent(0)
    {
        if (data.size() < 3) return;
        resolution = data[0];
        count = (data[1] << 8) | data[2];

        size_t off = 3;
        for (int i = 0; i < count && off + 10 <= data.size(); ++i) {
            uint16_t size = (data[off + 8] << 8) | data[off + 9];
            int8_t top = static_cast<int8_t>(data[off + 5]);
            int8_t bottom = static_cast<int8_t>(data[off + 7]);
            max_ascent = std::max(max_ascent, -(int)top);
            max_descent = std::max(max_descent, (int)bottom);
            off += 10 + size;
        }
    };

    Font::~Font(){};

    std::optional<Character> Font::get(char32_t codepoint) const {
        size_t off = 3;
        for (int i = 0; i < count && off + 10 <= data.size(); ++i) {
            uint32_t cp = (data[off] << 24) | (data[off + 1] << 16) |
                          (data[off + 2] << 8) | data[off + 3];
            if (cp > codepoint) 
                return std::nullopt;
            uint16_t size = (data[off + 8] << 8) | data[off + 9];
            if (cp == codepoint) {
                int8_t left = static_cast<int8_t>(data[off + 4]);
                int8_t top = static_cast<int8_t>(data[off + 5]);
                int8_t right = static_cast<int8_t>(data[off + 6]);
                int8_t bottom = static_cast<int8_t>(data[off + 7]);
                off += 10;
                return Character{
                    left, top, right, bottom,
                    std::span<const uint8_t>(&data[off], size)
                };
            }

            off += 10 + size;
        }
        return std::nullopt;
    };

    std::u32string Font::to_u32(const std::string& str) const {
        if (is_ascii(str))
            return std::u32string(str.begin(), str.end());
        else {
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
            return conv.from_bytes(str);
        }
    }


    void Font::DrawText(const std::string& text, int x, int y, const Color& color, int size)
    {
        fixed12_32 ratio = fixed12_32(size) / resolution;
        std::u32string u32text = to_u32(text);
        int draw_x = x;

        for (char32_t c : u32text) {
            auto ch_opt = get(c);
            if (!ch_opt)
                continue;

            const Character& ch = *ch_opt;
            int draw_y = y + int((max_ascent + ch.top) * ratio);
            draw_x += int(ch.left * ratio);
            draw_x += ch.draw(draw_x, draw_y, color, ratio);
            draw_x += int(1/ratio);
        }
    }


    Vector2<int> Font::GetTextSize(const std::string& text, int size)
    {
        std::u32string u32text = to_u32(text);
        int width = 0;
        for (char32_t c : u32text) {
            auto ch = get(c);
            if (ch)
            {
                width += ch->width();
                width += 1;
            }            
        }
        return Vector2<int>{ (width*size)/resolution, ((max_ascent + max_descent)*size)/resolution };
    }

    Font* Font::Load(std::string path)
    {
        (void)path;
        return nullptr;
    }

    void Font::Unload(Font* font)
    {
        (void)font;
    }
}