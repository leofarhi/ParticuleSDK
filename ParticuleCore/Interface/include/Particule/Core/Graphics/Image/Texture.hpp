#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/Types/Rect.hpp>
#include <Particule/Core/System/Window.hpp>
#include <string>

namespace Particule::Core
{
    class Sprite;

    class Texture
    {
    private:
        Texture();
        Texture(const Texture& other);
        Texture& operator=(const Texture& other);
    public:
        virtual ~Texture();
        int Width();
        int Height();
        bool IsWritable();

        void Draw(int x, int y);
        void DrawSub(int x, int y, Rect rect);
        void DrawSize(int x, int y, int w, int h);
        void DrawSubSize(int x, int y, int w, int h, Rect rect);
        void DrawColor(int x, int y, const Color& color);
        void DrawSubColor(int x, int y, Rect rect, const Color& color);
        void DrawSizeColor(int x, int y, int w, int h, const Color& color);
        void DrawSubSizeColor(int x, int y, int w, int h, Rect rect, const Color& color);

        // Secure and Unsecure WritePixel and ReadPixel
        //Unsecure : Don't check if x and y are in the texture : Faster
        void WritePixelRaw(int x, int y, const ColorRaw& color);
        void WritePixel(int x, int y, const Color& color);
        ColorRaw ReadPixelRaw(int x, int y);
        Color ReadPixel(int x, int y);

        //Secure : Check if x and y are in the texture : Slower
        void SetPixel(int x, int y, const Color& color);// SetPixel is Secure : Check if x and y are in the texture
        Color GetPixel(int x, int y);// GetPixel is Secure : Check if x and y are in the texture

        void UpdateTexture();//Sends the texture to the GPU, must be called after all WritePixel calls

        //Used to draw a pixel from the texture to the screen
        //Unsecure : Don't check if x and y are in the texture and the screen : Faster
        bool PutPixel(int xTexture, int yTexture, int xScreen, int yScreen);

        Sprite* CreateSprite(Rect rect);

        static Texture* Load(std::string path);
        static Texture* Create(int width, int height);
        static void Unload(Texture* texture);
    };
}

#endif // TEXTURE_HPP