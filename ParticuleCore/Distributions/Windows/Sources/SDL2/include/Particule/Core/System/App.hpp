#ifndef APP_HPP
#define APP_HPP
#include <Particule/Core/System/Window.hpp>
#include <Particule/Core/System/Time.hpp>
#include <vector>
#include <string>

namespace Particule::Core
{
    class App
    {
    private:
        static Window* MainWindow;
        std::vector<Window*> windows;
         // Indique si le monde est actif ou non
    public:
        static Time time;
        static App* instance; // Instance unique de l'application

        std::list<sdl2::SDL_Event> events; // Liste des événements SDL

        App();
        App(const App& other) = delete; // Suppression du constructeur de copie
        App& operator=(const App& other) = delete; // Suppression de l'opérateur d'affectation
        ~App();
        void ResetWorkingDirectory(); // Réinitialise le répertoire de travail
        void SetWorkingDirectory(std::string& path); // Définit le répertoire de travail
        std::string GetWorkingDirectory(); // Récupère le répertoire de travail

        void Update(); // Met à jour l'application

        void AddWindow(Window* window); // Ajoute une fenêtre à l'application
        void RemoveWindow(Window* window); // Supprime une fenêtre de l'application

        static inline void SetMainWindow(Window* window) { MainWindow = window; } // Définit la fenêtre principale de l'application
        static inline Window* GetMainWindow() { return MainWindow; } // Récupère la fenêtre principale de l'application

        std::vector<Window*> GetWindows(); // Récupère toutes les fenêtres de l'application
        Window* GetWindow(size_t index); // Récupère une fenêtre par son index

        void SetIcon(std::string path); // Définit l'icône de l'application

        int FPS(); // Récupère le nombre d'images par seconde
    };
}

int MainApp(Particule::Core::App* app, int argc, char* argv[]); // Déclaration de la fonction principale de l'application

#endif // APP_HPP