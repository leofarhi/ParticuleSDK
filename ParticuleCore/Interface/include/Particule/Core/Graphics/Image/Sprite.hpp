#ifndef SPRITE_HPP
#define SPRITE_HPP
#include <Particule/Core/Graphics/Image/Texture.hpp>
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/Types/Rect.hpp>
#include <Particule/Core/System/Window.hpp>
#include <Particule/Core/System/Basic.hpp>
#include <Particule/Core/System/AssetManager.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <string>

namespace Particule::Core
{
    class Sprite
    {
    private:
        Asset<Texture> texture;
        Rect rect;
    public:
        Sprite() = default;
        Sprite(Asset<Texture> asset_texture, Rect rect) : texture(std::move(asset_texture)), rect(rect) {}
        Sprite(Texture* texture, Rect rect) : texture(texture), rect(rect) {}
        Sprite(uint32_t assetID, Rect rect) : texture(assetID), rect(rect) {}
        Sprite(const Sprite& other) : texture(other.texture), rect(other.rect) {}
        Sprite& operator=(const Sprite& other)
        {
            if (this != &other)
            {
                texture = other.texture;
                rect = other.rect;
            }
            return *this;
        }
        ~Sprite() = default;

        inline void SetRect(Rect rect)
        {
            this->rect = rect;
        }
        inline void SetRect(int x, int y, int w, int h)
        {
            rect.x = x;
            rect.y = y;
            rect.w = w;
            rect.h = h;
        }

        FORCE_INLINE Texture* GetTexture()
        {
            return texture.Get();
        }

        FORCE_INLINE Asset<Texture> GetAssetTexture() const
        {
            return texture;
        }

        FORCE_INLINE Rect GetRect() const
        {
            return rect;
        }

        FORCE_INLINE void Draw(int x, int y)
        {
            //texture->DrawSub(x, y, rect);
            Rect subRect = Rect(rect.x, rect.y, rect.w > 0 ? rect.w : -rect.w, rect.h > 0 ? rect.h : -rect.h);
            texture->DrawSubSize(x, y, rect.w, rect.h, subRect);
        }

        FORCE_INLINE void DrawSize(int x, int y, int w, int h)
        {
            texture->DrawSubSize(x, y, w, h, rect);
        }

        FORCE_INLINE void DrawScaled(int x, int y, fixed12_32 scaleX, fixed12_32 scaleY)
        {
            texture->DrawSubSize(x, y, static_cast<int>(rect.w * scaleX), static_cast<int>(rect.h * scaleY), rect);
        }

        FORCE_INLINE void DrawColor(int x, int y, const Color& color)
        {
            //texture->DrawSubColor(x, y, rect, color);
            Rect subRect = Rect(rect.x, rect.y, rect.w > 0 ? rect.w : -rect.w, rect.h > 0 ? rect.h : -rect.h);
            texture->DrawSubSizeColor(x, y, rect.w, rect.h, subRect, color);
        }

        FORCE_INLINE void DrawSizeColor(int x, int y, int w, int h, const Color& color)
        {
            texture->DrawSubSizeColor(x, y, w, h, rect, color);
        }
        
        inline Sprite* CreateSubSprite(Rect rect)
        {
            Rect subRect = this->rect;
            subRect.x += rect.x;
            subRect.y += rect.y;
            subRect.w = rect.w;
            subRect.h = rect.h;
            return new Sprite(texture, subRect);
        }

        //Les Sprites sont BuiltIn mais reste des Assets et ces fonctions sont obligatoires pour les Assets
        static inline Sprite* Load(std::string path){(void)path; return nullptr;}
        static inline void Unload(Sprite* sprite){(void)sprite;}

    };
}

#endif // SPRITE_HPP