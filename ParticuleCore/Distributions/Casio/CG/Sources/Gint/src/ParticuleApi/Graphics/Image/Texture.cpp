#include <Particule/Core/Graphics/Image/Texture.hpp>
#include <Particule/Core/Graphics/Image/Sprite.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/System/gint.hpp>
#include <Particule/Core/System/File.hpp>
#include <algorithm>

namespace Particule::Core
{
    struct Sampler2D {
        Vector2<int> iteration;         // Boucle for (x, y)
        Vector2<int> screenOffset;      // Coordonnées de départ écran (x, y)
        Vector2<int> textureOffset;     // Coordonnées de départ texture (sx, sy)
        Vector2<fixed12_32> TexIncr;       // Incrément en fixed pour la texture (xinc, yinc)
    
        Sampler2D(int x, int y, int w, int h, Rect rect, int screenW, int screenH, int texW, int texH)
        {
            if (w == 0 || h == 0 || rect.w == 0 || rect.h == 0)
                return;
            Vector2<fixed12_32> uv_start = { fixed12_32(std::clamp(rect.x, 0, texW - 1)), fixed12_32(std::clamp(rect.y, 0, texH - 1)) };
            Vector2<fixed12_32> uv_end = { fixed12_32(std::clamp(rect.x + rect.w - 1, 0, texW - 1)), fixed12_32(std::clamp(rect.y + rect.h - 1, 0, texH - 1)) };
            if (w < 0) {
                std::swap(uv_start.x, uv_end.x);
                w = -w;
            }
            if (h < 0) {
                std::swap(uv_start.y, uv_end.y);
                h = -h;
            }
            screenOffset.x = std::max(0, x);
            screenOffset.y = std::max(0, y);
            int endX = std::min(screenW - 1, x + w - 1);
            int endY = std::min(screenH - 1, y + h - 1);
            iteration.x = endX - screenOffset.x + 1;
            iteration.y = endY - screenOffset.y + 1;
            if (iteration.x <= 0 || iteration.y <= 0)
                return;
            int drawW = w - 1;
            int drawH = h - 1;
            TexIncr.x = (drawW > 0) ? ((uv_end.x - uv_start.x) / drawW) : fixed12_32(0);
            TexIncr.y = (drawH > 0) ? ((uv_end.y - uv_start.y) / drawH) : fixed12_32(0);
            textureOffset.x = uv_start.x + TexIncr.x * (screenOffset.x - x);
            textureOffset.y = uv_start.y + TexIncr.y * (screenOffset.y - y);
        }

    
        inline int GetScreenX(int col) const {
            return screenOffset.x + col;
        }
    
        inline int GetScreenY(int row) const {
            return screenOffset.y + row;
        }
    
        inline int GetTexX(fixed12_32 x2) const {
            return textureOffset.x + x2;
        }
    
        inline int GetTexY(fixed12_32 y2) const {
            return textureOffset.y + y2;
        }
    };

    Texture::Texture() : img(nullptr), _alphaValue(0), isAllocated(false) {}

    Texture::Texture(const Texture& other) : img(other.img), _alphaValue(other._alphaValue), isAllocated(other.isAllocated) {}

    Texture& Texture::operator=(const Texture& other)
    {
        if (this != &other)
        {
            img = other.img;
            _alphaValue = other._alphaValue;
            isAllocated = other.isAllocated;
        }
        return *this;
    }

    Texture::~Texture() {
        if (isAllocated && img != nullptr)
        {
            image_free(img);
            img = nullptr;
        }
    }

    void Texture::DrawSubSize(int x, int y, int w, int h, Rect rect)
    {
        if (rect.w == w && rect.h == h) {
            DrawSub(x, y, rect);
            return;
        }
        if (w == 0 || h == 0 || rect.w == 0 || rect.h == 0) return;
        Sampler2D sampler(x, y, w, h, rect, DWIDTH, DHEIGHT, img->width, img->height);
        fixed12_32 y2 = 0;
        for (int row = 0; row < sampler.iteration.y; ++row)
        {
            const int screenY = sampler.GetScreenY(row);
            const int texY = sampler.GetTexY(y2);

            fixed12_32 x2 = 0;
            for (int col = 0; col < sampler.iteration.x; ++col)
            {
                PutPixel(sampler.GetTexX(x2), texY, sampler.GetScreenX(col), screenY);
                x2 += sampler.TexIncr.x;
            }
            y2 += sampler.TexIncr.y;
        }
    }

    void Texture::DrawSubSizeColor(int x, int y, int w, int h, Rect rect, const Color& color)
    {
        if (color.A() < 128) return;
        if (w == 0 || h == 0 || rect.w == 0 || rect.h == 0) return;
        Sampler2D sampler(x, y, w, h, rect, DWIDTH, DHEIGHT, img->width, img->height);
        fixed12_32 y2 = 0;
        for (int row = 0; row < sampler.iteration.y; ++row)
        {
            const int screenY = sampler.GetScreenY(row);
            const int texY = sampler.GetTexY(y2);

            fixed12_32 x2 = 0;
            for (int col = 0; col < sampler.iteration.x; ++col)
            {
                const int i = _getPixel(sampler.GetTexX(x2), texY);
                if (i != _alphaValue)
                    gint::gint_vram[DWIDTH * (screenY) + (sampler.GetScreenX(col))] = Color::MultiplyColorRaw(_decodePixel(i), color.Raw());
                x2 += sampler.TexIncr.x;
            }
            y2 += sampler.TexIncr.y;
        }
    }

    Sprite* Texture::CreateSprite(Rect rect)
    {
        return new Sprite(this, rect);
    }

    Texture* Texture::Load(std::string path)
    {
        File* imgFile = File::Open(path, FileMode::Read, Endian::BigEndian);
        if (imgFile == nullptr) return nullptr;
        uint8_t format;
        int16_t color_count;
        int16_t width, height;
        int stride;
        imgFile->Read<uint8_t>(format);
        imgFile->Read<int16_t>(color_count);
        imgFile->Read<int16_t>(width);
        imgFile->Read<int16_t>(height);
        imgFile->Read<int>(stride);
        gint::image_t* img = image_alloc(width, height, format);
        if (img == nullptr) {
            File::Close(imgFile);
            return nullptr;
        }
        img->stride = stride;
        img->color_count = color_count;
        img->palette = nullptr;
        unsigned long long SizeOfData = 0;
        imgFile->Read<unsigned long long>(SizeOfData);
        for (unsigned long long i = 0; i < SizeOfData; i++)
            imgFile->Read<uint8_t>(((uint8_t*)img->data)[i]);
        unsigned int SizeOfPalette = 0;
        imgFile->Read<unsigned int>(SizeOfPalette);
        if (SizeOfPalette != 0)
        {
            if (!gint::image_alloc_palette(img, color_count)) {
                gint::image_free(img);
                File::Close(imgFile);
                return nullptr;
            }
            imgFile->Read(img->palette, SizeOfPalette);
        }
        File::Close(imgFile);
        Texture* texture = nullptr;
        if (IMAGE_IS_RGB16(img->format))
            texture = new Texture(img, true);
        else if (IMAGE_IS_P4(img->format))
            texture = new TextureP4(img, true);
        else if (IMAGE_IS_P8(img->format))
            texture = new TextureP8(img, true);
        if (texture == nullptr) {
            image_free(img);
            return nullptr;
        }
        return texture;
    }

    Texture* Texture::Create(int width, int height)
    {
        gint::image_t* img = image_alloc(width,height,gint::IMAGE_RGB565A);
        if (img == nullptr) return nullptr;
        Texture* texture = new Texture(img, true);
        if (texture == nullptr) {
            image_free(img);
            return nullptr;
        }
        return texture;
    }

    void Texture::Unload(Texture* texture)
    {
        if (texture != nullptr && texture->isAllocated && texture->img != nullptr)
            delete texture;
    }

}