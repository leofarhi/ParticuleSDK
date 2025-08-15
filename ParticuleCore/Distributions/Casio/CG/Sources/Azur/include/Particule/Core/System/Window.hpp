#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <string>
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/Inputs/Input.hpp>
#include <Particule/Core/Inputs/Devices.hpp>

#include <Particule/Core/System/gint.hpp>

namespace Particule::Core
{
    class Window
    {
    private:
        static constexpr int DefaultWidth = 396;
        static constexpr int DefaultHeight = 224;
    public:
        bool IsRunning = true; // Indique si la fenêtre est en cours d'exécution
        // Constructeurs
        Window(){};
        Window(int width, int height, const std::string& title){(void)width; (void)height; (void)title;};
        Window(const Window& other) = delete; // Suppression du constructeur de copie
        Window& operator=(const Window& other) = delete; // Suppression de l'opérateur d'affectation
        virtual ~Window(){};
    
        // Méthodes principales
        inline void Display() { azrp_update(); }
        inline void UpdateInput() { cleareventflips();clearevents(); }
        inline void Clear() { azrp_clear(C_BLACK); }
        inline void Clear(Color color) { azrp_clear(color.Raw()); }
    
        constexpr int Width() const { return DefaultWidth; }
        constexpr int Height() const { return DefaultHeight; }
    
        constexpr void SetWidth(int width)  { (void)width; }
        constexpr void SetHeight(int height){ (void)height; }
        constexpr void SetSize(int w, int h){ (void)w; (void)h; }
        constexpr void SetTitle(const std::string& title) { (void)title; }
        constexpr void SetPosition(int x, int y) { (void)x; (void)y; }

        constexpr void SetFullscreen(bool fullscreen) { (void)fullscreen; }
        constexpr void SetResizable(bool resizable) { (void)resizable; }
        
        constexpr void Close() { IsRunning = false; }

        virtual void OnStart(){};
        virtual void OnUpdate(){};
        virtual void OnDraw(){};
        virtual void OnClose(){};

        static inline void BindWindow(Window* window) {(void)window; };
        static inline void UnbindWindow(){};
        static Window* GetCurrentWindow();
    };
}

#define CONST_SCREEN_SIZE
#define SCREEN_WIDTH 396
#define SCREEN_HEIGHT 224

#endif // WINDOW_HPP