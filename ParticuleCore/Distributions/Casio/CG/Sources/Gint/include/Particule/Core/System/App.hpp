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

        App();
        App(const App& other) = delete; // Suppression du constructeur de copie
        App& operator=(const App& other) = delete; // Suppression de l'opérateur d'affectation
        ~App();
        void ResetWorkingDirectory(); // Réinitialise le répertoire de travail
        void SetWorkingDirectory(std::string& path); // Définit le répertoire de travail
        std::string GetWorkingDirectory(); // Récupère le répertoire de travail

        inline void Update()
        {
            time.Update(); // Mettre à jour le temps
            //seulement pour la fenêtre principale
            if (MainWindow == nullptr) // Vérifier si la fenêtre principale est nulle
                return;
            MainWindow->UpdateInput();
            MainWindow->OnUpdate();
            MainWindow->OnDraw();
            //for (auto window : windows) // Parcourir toutes les fenêtres
            for (size_t i = 0; i < windows.size(); ++i) // Parcourir toutes les fenêtres
            {
                if (!windows[i]->IsRunning)
                    RemoveWindow(windows[i--]); // Supprimer la fenêtre si elle n'est pas en cours d'exécution
            }
        }

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

inline Particule::Core::Window* Particule::Core::Window::GetCurrentWindow()
{
    return App::GetMainWindow();
}

int MainApp(Particule::Core::App* app, int argc, char* argv[]); // Déclaration de la fonction principale de l'application

#endif // APP_HPP