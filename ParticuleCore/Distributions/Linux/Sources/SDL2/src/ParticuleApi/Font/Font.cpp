#include <Particule/Core/Font/Font.hpp>
#include <Particule/Core/System/File.hpp>
#include <stdexcept>

namespace Particule::Core
{
    Font::Font(int count, int resolution) : resolution(resolution), max_ascent(0), max_descent(0), characters()
    {
        if (count <= 0) throw std::invalid_argument("Font count must be positive");
        if (resolution <= 0) throw std::invalid_argument("Font resolution must be positive");
    };

    Font::~Font()
    {
        characters.clear();
    };

    inline std::u32string Font::to_u32(const std::string& str) const {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.from_bytes(str);
    }

    void Font::DrawText(const std::string& text, int x, int y, const Color& color, int size)
    {
        fixed12_32 ratio = fixed12_32(size) / resolution;
        std::u32string u32text = to_u32(text);
        int draw_x = x;

        for (char32_t c : u32text) {
            Character* ch = get(c);
            if (!ch)
                continue;
            int draw_y = y + int((max_ascent + ch->top) * ratio);
            draw_x += int(ch->left * ratio);
            draw_x += ch->draw(draw_x, draw_y, color, ratio);
            draw_x += int(1/ratio);
        }
    }


    Vector2<int> Font::GetTextSize(const std::string& text, int size)
    {
        std::u32string u32text = to_u32(text);
        int width = 0;
        for (char32_t c : u32text) {
            Character* ch = get(c);
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
        File* file = File::Open(path, FileMode::Read, Endian::BigEndian);
        if (file == nullptr) return nullptr;
        uint8_t resolution;
        file->Read<uint8_t>(resolution);
        uint16_t count;
        file->Read<uint16_t>(count);
        Font* font = new Font(count, resolution);
        for (int i = 0; i < count; ++i) {
            uint32_t codepoint;
            file->Read<uint32_t>(codepoint);
            int8_t left, top, right, bottom;
            file->Read<int8_t>(left);
            file->Read<int8_t>(top);
            file->Read<int8_t>(right);
            file->Read<int8_t>(bottom);
            uint16_t size;
            file->Read<uint16_t>(size);
            std::vector<uint8_t> data(size);
            file->Read(data.data(), size);
            font->characters[codepoint] = Character{left, top, right, bottom, data};

            font->max_ascent = std::max(font->max_ascent, -(int)top);
            font->max_descent = std::max(font->max_descent, (int)bottom);
        }
        File::Close(file);
        return font;
    }

    void Font::Unload(Font* font)
    {
        if (font != nullptr)
            delete font;
    }
}