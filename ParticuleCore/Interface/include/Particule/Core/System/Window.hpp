#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <string>
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/Input.hpp>

namespace Particule::Core
{
    class Window
    {
    public:
        Window();
        Window(int width, int height, const std::string& title);
        Window(const Window& other) = delete; // Suppression du constructeur de copie
        Window& operator=(const Window& other) = delete; // Suppression de l'opérateur d'affectation
        virtual ~Window();

        void Display();
        void UpdateInput();
        Input GetKey();
        void Clear();
        void Clear(Color color);
        int Width();
        int Height();

        void SetWidth(int width);
        void SetHeight(int height);
        void SetSize(int width, int height);
        void SetTitle(const std::string& title);
        void SetPosition(int x, int y);
        void SetFullscreen(bool fullscreen);
        void SetResizable(bool resizable);
        void Close();

        virtual void OnStart(){};
        virtual void OnUpdate(){};
        virtual void OnDraw(){};
        virtual void OnClose(){};

        static void BindWindow(Window* window);
        static void UnbindWindow();
        static Window* GetCurrentWindow();
    };
}

#endif // WINDOW_HPP