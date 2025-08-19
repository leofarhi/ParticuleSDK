#include <Particule/Core/System/Window.hpp>
#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/sdl2.hpp>
#include <stdexcept>
#include <algorithm>

using namespace sdl2;

namespace Particule::Core
{
    thread_local Window* Window::currentWindow = nullptr;
    Window::Window()
    {
        window = SDL_CreateWindow("Particule", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN); // Créer une nouvelle fenêtre
        if (window == nullptr)
            throw std::runtime_error("error SDL_CreateWindow "+ SDL2_GetError());
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // Créer un nouveau rendu
        if (renderer == nullptr)
            throw std::runtime_error("error SDL_CreateRenderer "+ SDL2_GetError());
        SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND); // Définir le mode de mélange du rendu
    }

    Window::Window(int width, int height, const std::string& title)
    {
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN); // Créer une nouvelle fenêtre
        if (window == nullptr)
            throw std::runtime_error("error SDL_CreateWindow "+ SDL2_GetError());
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // Créer un nouveau rendu
        if (renderer == nullptr)
            throw std::runtime_error("error SDL_CreateRenderer "+ SDL2_GetError());
        SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND); // Définir le mode de mélange du rendu
    }

    Window::~Window()
    {
        if (renderer != nullptr) // Vérifier si le rendu n'est pas nul
            SDL_DestroyRenderer(renderer); // Détruire le rendu
        if (window != nullptr) // Vérifier si la fenêtre n'est pas nulle
            SDL_DestroyWindow(window); // Détruire la fenêtre
    }

    void Window::UpdateInput()
    {
        bool mouseCaptured = false;

        auto sameMouseAs = [&](const SDL_Event& down, const SDL_Event& up) {
            return down.type == SDL_MOUSEBUTTONDOWN &&
                up.type   == SDL_MOUSEBUTTONUP   &&
                down.button.button == up.button.button &&
                down.button.windowID == up.button.windowID;
        };

        auto purgeHeldMouse = [&](const SDL_Event& upEvt) {
            auto it = std::remove_if(eventsHeld.begin(), eventsHeld.end(),
                [&](const SDL_Event& e){ return sameMouseAs(e, upEvt); });
            eventsHeld.erase(it, eventsHeld.end());
            this->events.push_back(upEvt);
        };

        auto synthesizeMouseUp = [&](Uint32 windowID, Uint32 button, Uint32 ts){
            SDL_Event up{};
            up.type = SDL_MOUSEBUTTONUP;
            up.button.windowID = windowID;
            up.button.which    = SDL_TOUCH_MOUSEID;
            up.button.button   = static_cast<Uint8>(button);
            up.button.state    = SDL_RELEASED;
            up.button.clicks   = 1;
            up.button.timestamp= ts ? ts : SDL_GetTicks();
            purgeHeldMouse(up);
        };

        // --- dans ta boucle ---
        this->events.clear();

        // 0) si un bouton est marqué "held" mais n'est plus appuyé physiquement → on envoie un UP synthétique
        auto reconcileMouseButtons = [&](){
            int gx, gy;
            Uint32 mask = SDL_GetGlobalMouseState(&gx, &gy); // état global, même hors fenêtre
            // pour chaque bouton qu'on pourrait tenir, vérifier dans eventsHeld
            auto hasHeld = [&](Uint8 b){
                return std::any_of(eventsHeld.begin(), eventsHeld.end(), [&](const SDL_Event& e){
                    return e.type == SDL_MOUSEBUTTONDOWN &&
                        e.button.button == b &&
                        e.button.windowID == SDL_GetWindowID(this->window);
                });
            };
            struct Btn { Uint8 b; Uint32 m; };
            const Btn btns[] = { {SDL_BUTTON_LEFT,   SDL_BUTTON_LMASK},
                                {SDL_BUTTON_MIDDLE, SDL_BUTTON_MMASK},
                                {SDL_BUTTON_RIGHT,  SDL_BUTTON_RMASK},
        #if SDL_VERSION_ATLEAST(2,0,4)
                                {SDL_BUTTON_X1,     SDL_BUTTON_X1MASK},
                                {SDL_BUTTON_X2,     SDL_BUTTON_X2MASK},
        #endif
            };
            for (auto& b : btns) {
                if (hasHeld(b.b) && (mask & b.m) == 0) {
                    // Physiquement relâché mais aucun UP reçu → synthétiser
                    synthesizeMouseUp(SDL_GetWindowID(this->window), b.b, 0);
                }
            }
        };

        // 1) passer sur les événements
        for (auto& event : App::instance->events)
        {
            if (event.window.windowID != SDL_GetWindowID(this->window))
                continue;

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                    this->Close();

                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_MINIMIZED ||
                    event.window.event == SDL_WINDOWEVENT_HIDDEN) {
                    eventsHeld.clear();
                    if (mouseCaptured) {
                        SDL_CaptureMouse(SDL_FALSE);
                        mouseCaptured = false;
                    }
                    this->events.push_back(event);
                    continue;
                }

                // Lorsqu'on sort/entre, on tente une réconciliation
                if (event.window.event == SDL_WINDOWEVENT_LEAVE ||
                    event.window.event == SDL_WINDOWEVENT_ENTER) {
                    reconcileMouseButtons();
                    this->events.push_back(event);
                    continue;
                }
            }

            if (event.type == SDL_KEYDOWN) {
                if (!event.key.repeat) {
                    this->events.push_back(event);
                    this->eventsHeld.push_back(event);
                }
                continue;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Essayer de capturer : si OK, on recevra le UP même hors fenêtre
                if (SDL_CaptureMouse(SDL_TRUE) == 0) {
                    mouseCaptured = true;
                } else {
                    mouseCaptured = false; // on utilisera le fallback
                }
                this->events.push_back(event);
                this->eventsHeld.push_back(event);
                continue;
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                if (mouseCaptured) {
                    SDL_CaptureMouse(SDL_FALSE);
                    mouseCaptured = false;
                }
                purgeHeldMouse(event);
                continue;
            }

            if (event.type == SDL_KEYUP) {
                auto it = std::remove_if(eventsHeld.begin(), eventsHeld.end(),
                    [&](const SDL_Event& e){
                        return e.type == SDL_KEYDOWN &&
                            e.key.windowID == event.key.windowID &&
                            e.key.keysym.sym == event.key.keysym.sym;
                    });
                eventsHeld.erase(it, eventsHeld.end());
                this->events.push_back(event);
                continue;
            }

            this->events.push_back(event);
        }

        // 2) Juste avant de finir la frame, dernière réconciliation (utile si aucun LEAVE/ENTER n’est survenu)
        reconcileMouseButtons();

    }
}