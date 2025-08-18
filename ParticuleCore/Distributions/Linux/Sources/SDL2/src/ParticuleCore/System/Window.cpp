#include <Particule/Core/System/Window.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <stdexcept>
#include <algorithm>
//#include <windows.h>

namespace Particule::Core
{
    thread_local Window* Window::currentWindow = nullptr;
    Window::Window()
    {
        window = sdl2::SDL_CreateWindow("Particule", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, sdl2::SDL_WINDOW_SHOWN); // Créer une nouvelle fenêtre
        if (window == nullptr)
            throw std::runtime_error("error SDL_CreateWindow "+ sdl2::SDL2_GetError());
        renderer = sdl2::SDL_CreateRenderer(window, -1, sdl2::SDL_RENDERER_ACCELERATED); // Créer un nouveau rendu
        if (renderer == nullptr)
            throw std::runtime_error("error SDL_CreateRenderer "+ sdl2::SDL2_GetError());
        sdl2::SDL_SetRenderDrawBlendMode(this->renderer, sdl2::SDL_BLENDMODE_BLEND); // Définir le mode de mélange du rendu
    }

    Window::Window(int width, int height, const std::string& title)
    {
        window = sdl2::SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, sdl2::SDL_WINDOW_SHOWN); // Créer une nouvelle fenêtre
        if (window == nullptr)
            throw std::runtime_error("error SDL_CreateWindow "+ sdl2::SDL2_GetError());
        renderer = sdl2::SDL_CreateRenderer(window, -1, sdl2::SDL_RENDERER_ACCELERATED); // Créer un nouveau rendu
        if (renderer == nullptr)
            throw std::runtime_error("error SDL_CreateRenderer "+ sdl2::SDL2_GetError());
        sdl2::SDL_SetRenderDrawBlendMode(this->renderer, sdl2::SDL_BLENDMODE_BLEND); // Définir le mode de mélange du rendu
    }

    Window::~Window()
    {
        if (renderer != nullptr) // Vérifier si le rendu n'est pas nul
            sdl2::SDL_DestroyRenderer(renderer); // Détruire le rendu
        if (window != nullptr) // Vérifier si la fenêtre n'est pas nulle
            sdl2::SDL_DestroyWindow(window); // Détruire la fenêtre
    }

    void Window::UpdateInput()
    {
        this->events.clear(); // Vider la liste des événements
        for (auto& event : App::instance->events) // Parcourir tous les événements de l'application
        {
            if (event.window.windowID != sdl2::SDL_GetWindowID(this->window))
                continue;
            if (event.type == sdl2::SDL_WINDOWEVENT && event.window.event == sdl2::SDL_WINDOWEVENT_CLOSE) // Vérifier si l'événement est une demande de fermeture
                this->Close();
            if (event.key.repeat)
                continue;
            if (event.type == sdl2::SDL_KEYDOWN || event.type == sdl2::SDL_MOUSEBUTTONDOWN) // Vérifier si l'événement est une touche enfoncée
            {
                this->events.push_back(event); // Ajouter l'événement à la liste des événements
                this->eventsHeld.push_back(event); // Ajouter l'événement à la liste des événements maintenus
            }
            else if (event.type == sdl2::SDL_KEYUP || event.type == sdl2::SDL_MOUSEBUTTONUP) // Vérifier si l'événement est une touche relâchée
            {
                //Trouver toute les touches maintenues et les supprimer de la liste des événements maintenus
                auto it = std::remove_if(eventsHeld.begin(), eventsHeld.end(), [&](const sdl2::SDL_Event& e) {
                    return e.key.keysym.sym == event.key.keysym.sym;
                });
                eventsHeld.erase(it, eventsHeld.end()); // Supprimer les événements maintenus de la liste
                this->events.push_back(event); // Ajouter l'événement à la liste des événements
            }
            else
            {
                this->events.push_back(event);
            }
        }
    }
}