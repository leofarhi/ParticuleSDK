#include <Particule/Core/Graphics/Image/Texture.hpp>
#include <Particule/Core/Graphics/Image/Sprite.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/Types/Vector2.hpp>
#include <algorithm>

namespace Particule::Core
{

    Texture::Texture() : texture(nullptr), surface(nullptr), isWritable(false) {}

    Texture::Texture(const Texture& other) : texture(other.texture), surface(other.surface), isWritable(other.isWritable) {}

    Texture& Texture::operator=(const Texture& other)
    {
        if (this != &other)
        {
            texture = other.texture;
            surface = other.surface;
            isWritable = other.isWritable;
        }
        return *this;
    }

    Texture::~Texture() {
        if (texture != nullptr)
        {
            sdl2::SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        if (surface != nullptr)
        {
            sdl2::SDL_FreeSurface(surface);
            surface = nullptr;
        }
    }

    void Texture::Draw(int x, int y)
    {
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_Rect rect = {x, y, surface->w, surface->h};
        sdl2::SDL_RenderCopy(window->renderer, texture, nullptr, &rect);
    }

    void Texture::DrawSub(int x, int y, Rect rect)
    {
        Window* window = Window::GetCurrentWindow();
        //change w and h if the rect is too big
        if (rect.x + rect.w > surface->w) rect.w = surface->w - rect.x;
        if (rect.y + rect.h > surface->h) rect.h = surface->h - rect.y;
        sdl2::SDL_Rect dstRect = {x, y, rect.w, rect.h};
        sdl2::SDL_Rect srcRect = {rect.x, rect.y, rect.w, rect.h};
        sdl2::SDL_RenderCopy(window->renderer, texture, &srcRect, &dstRect);
    }

    void Texture::DrawSubSize(int x, int y, int w, int h, Rect rect)
    {
        if (rect.w == w && rect.h == h)
        {
            DrawSub(x, y, rect);
            return;
        }
        sdl2::SDL_RendererFlip flip = sdl2::SDL_FLIP_NONE;
        if (w < 0)
            flip = static_cast<sdl2::SDL_RendererFlip>(flip | sdl2::SDL_FLIP_HORIZONTAL);
        if (h < 0)
            flip = static_cast<sdl2::SDL_RendererFlip>(flip | sdl2::SDL_FLIP_VERTICAL);
        w = abs(w);
        h = abs(h);
        //change w and h if the rect is too big
        if (rect.x + rect.w > surface->w) rect.w = surface->w - rect.x;
        if (rect.y + rect.h > surface->h) rect.h = surface->h - rect.y;
        sdl2::SDL_Rect dstRect = {x, y, w, h};
        sdl2::SDL_Rect srcRect = {rect.x, rect.y, rect.w, rect.h};
        Window* window = Window::GetCurrentWindow();
        sdl2::SDL_RenderCopyEx(window->renderer, texture, &srcRect, &dstRect, 0, nullptr, flip);
    }

    void Texture::DrawSubSizeColor(int x, int y, int w, int h, Rect rect, const Color& color)
    {
        // Sauvegarde l’état courant de la texture
        uint8_t oldR, oldG, oldB, oldA;
        sdl2::SDL_GetTextureColorMod(texture, &oldR, &oldG, &oldB);
        sdl2::SDL_GetTextureAlphaMod(texture, &oldA);
        // Applique la nouvelle couleur
        sdl2::SDL_SetTextureColorMod(texture, color.R(), color.G(), color.B());
        sdl2::SDL_SetTextureAlphaMod(texture, color.A());
        // Dessine la texture modifiée
        DrawSubSize(x, y, w, h, rect);
        // Restaure les valeurs précédentes de la texture
        sdl2::SDL_SetTextureColorMod(texture, oldR, oldG, oldB);
        sdl2::SDL_SetTextureAlphaMod(texture, oldA);
    }


    Sprite* Texture::CreateSprite(Rect rect)
    {
        return new Sprite(this, rect);
    }

    Texture* Texture::Load(std::string path)
    {
        Texture* texture = new Texture();
        auto surface = sdl2::IMG_Load(path.c_str());
        texture->surface = sdl2::SDL_ConvertSurfaceFormat(surface, sdl2::SDL_PIXELFORMAT_RGBA8888, 0);
        sdl2::SDL_FreeSurface(surface);
        if (texture->surface == nullptr)
        {
            delete texture;
            return nullptr;
        }
        const Window* window = App::GetMainWindow();
        texture->texture = sdl2::SDL_CreateTextureFromSurface(window->renderer, texture->surface);
        if (texture->texture == nullptr)
        {
            delete texture;
            return nullptr;
        }
        texture->isWritable = false; // Set to true if the texture is writable
        return texture;
    }

    Texture* Texture::Create(int width, int height)
    {
        Texture* texture = new Texture();
        texture->surface = sdl2::SDL_CreateRGBSurface(0, width, height, 32,
                                            0x000000FF,  // Rmask
                                            0x0000FF00,  // Gmask
                                            0x00FF0000,  // Bmask
                                            0xFF000000); // Amask
        if (texture->surface == nullptr)
        {
            delete texture;
            return nullptr;
        }
        texture->texture = sdl2::SDL_CreateTexture(App::GetMainWindow()->renderer, sdl2::SDL_PIXELFORMAT_RGBA8888, sdl2::SDL_TEXTUREACCESS_STREAMING, width, height);
        if (texture->texture == nullptr)
        {
            delete texture;
            return nullptr;
        }
        texture->isWritable = true; // Set to true if the texture is writable
        return texture;
    }

    void Texture::Unload(Texture* texture)
    {
        if (texture != nullptr)
            delete texture;
    }

}