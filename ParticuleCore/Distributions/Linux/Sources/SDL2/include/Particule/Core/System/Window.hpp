#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <string>
#include <Particule/Core/Graphics/Color.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <Particule/Core/System/Input.hpp>
#include <list>

namespace Particule::Core
{
    class Window
    {
    private:
        static thread_local Window* currentWindow;
    public:
        sdl2::SDL_Window* window;
        sdl2::SDL_Renderer* renderer;
        std::list<sdl2::SDL_Event> events; // Liste des événements SDL
        std::list<sdl2::SDL_Event> eventsHeld; // Liste des événements SDL en cours de maintien
        bool IsRunning = true; // Indique si la fenêtre est en cours d'exécution
        // Constructeurs
        Window();
        Window(int width, int height, const std::string& title);
        Window(const Window& other) = delete; // Suppression du constructeur de copie
        Window& operator=(const Window& other) = delete; // Suppression de l'opérateur d'affectation
        virtual ~Window();
    
        // Méthodes principales
        inline virtual void Display() { sdl2::SDL_RenderPresent(renderer); }
        virtual void UpdateInput();
        inline Input GetKey() {
            while (true)
            {
                UpdateInput();
                sdl2::SDL_Delay(15); // Attendre un court instant pour éviter une boucle infinie
                //get first Keydown event
                for (auto& event : events)
                {
                    if (event.type == sdl2::SDL_KEYDOWN)
                        return Input(event.key.keysym.sym);
                }
            }
            return Input(); // Retourner le premier événement de la liste
         } // Remplacer par la logique d'entrée appropriée
        inline virtual void Clear()
        { 
            sdl2::SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            sdl2::SDL_RenderClear(renderer);
        }
        inline virtual void Clear(Color color)
        {
            sdl2::SDL_SetRenderDrawColor(renderer, color.R(), color.G(), color.B(), color.A());
            sdl2::SDL_RenderClear(renderer);
        }
    
        inline virtual int Width() { int w = 0; sdl2::SDL_GetWindowSize(window, &w, nullptr); return w; }
        inline virtual int Height() { int h = 0; sdl2::SDL_GetWindowSize(window, nullptr, &h); return h; }
    
        inline void SetWidth(int width)  { sdl2::SDL_SetWindowSize(window, width, Height()); }
        inline void SetHeight(int height){ sdl2::SDL_SetWindowSize(window, Width(), height); }
        inline void SetSize(int w, int h){ sdl2::SDL_SetWindowSize(window, w, h); }
        inline void SetTitle(const std::string& title) { sdl2::SDL_SetWindowTitle(window, title.c_str()); }
        inline void SetPosition(int x, int y) { sdl2::SDL_SetWindowPosition(window, x, y); }

        inline void SetFullscreen(bool fullscreen) { sdl2::SDL_SetWindowFullscreen(window, fullscreen ? sdl2::SDL_WINDOW_FULLSCREEN : 0); }
        inline void SetResizable(bool resizable) { sdl2::SDL_SetWindowResizable(window, resizable ? sdl2::SDL_TRUE : sdl2::SDL_FALSE); }
        
        constexpr void Close() { IsRunning = false; }

        virtual void OnStart(){};
        virtual void OnUpdate(){};
        virtual void OnDraw(){};
        virtual void OnClose(){};

        static inline void BindWindow(Window* window)
        {
            Window::currentWindow = window;
        }
        static inline void UnbindWindow()
        {
            Window::currentWindow = nullptr;
        }
        static inline Window* GetCurrentWindow()
        {
            return Window::currentWindow;
        }
    };
}

#endif // WINDOW_HPP