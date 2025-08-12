#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/AssetManager.hpp>
#include <Particule/Core/System/gint.hpp>
#include <Particule/Core/System/memtrack.hpp>
#include <Particule/Core/System/Time.hpp>
#include <algorithm>

namespace Particule::Core
{
    Time App::time; // Instance statique de la classe Time
    App* App::instance = nullptr; // Initialisation de l'instance statique de l'application
    Window* App::MainWindow = nullptr; // Initialisation du pointeur statique vers la fenêtre principale

    App::App()
    {
        time = Time(); // Initialiser l'instance de temps
        instance = this; // Assigner l'instance courante à l'instance statique
    }

    App::~App()
    {
        for (auto window : windows) // Libérer toutes les fenêtres créées
        {
            if (window != nullptr) // Vérifier si la fenêtre n'est pas nulle
            {
                window->OnClose(); // Appeler la méthode de fermeture de la fenêtre
                delete window; // Libérer la mémoire allouée pour la fenêtre
            }
        }
        windows.clear(); // Vider le vecteur de fenêtres
    }

    void App::ResetWorkingDirectory(){}
    void App::SetWorkingDirectory(std::string& path){(void)path;} // Ne rien faire pour le répertoire de travail
    std::string App::GetWorkingDirectory(){ return ""; } // Retourner une chaîne vide pour le répertoire de travail

    void App::AddWindow(Window* window)
    {
        if (window != nullptr) // Vérifier si la fenêtre n'est pas nulle
        {
            windows.push_back(window); // Ajouter la fenêtre au vecteur de fenêtres
            if (MainWindow == nullptr) // Vérifier si la fenêtre principale est nulle
                MainWindow = window; // Définir la fenêtre principale
            window->OnStart(); // Appeler la méthode de démarrage de la fenêtre
        }
    }

    void App::RemoveWindow(Window* window)
    {
        if (window != nullptr) // Vérifier si la fenêtre n'est pas nulle
        {
            auto it = std::find(windows.begin(), windows.end(), window); // Trouver la fenêtre dans le vecteur
            if (it != windows.end()) // Vérifier si la fenêtre a été trouvée
            {
                windows.erase(it); // Supprimer la fenêtre du vecteur
                if (window == MainWindow) // Vérifier si la fenêtre est la fenêtre principale
                    MainWindow = nullptr; // Réinitialiser la fenêtre principale
                window->OnClose(); // Appeler la méthode de fermeture de la fenêtre
                delete window; // Libérer la mémoire allouée pour la fenêtre
            }
        }
    }

    std::vector<Window*> App::GetWindows()
    {
        return windows; // Retourner le vecteur de fenêtres
    }

    Window* App::GetWindow(size_t index)
    {
        if (index < windows.size()) // Vérifier si l'index est valide
            return windows[index]; // Retourner la fenêtre à l'index spécifié
        return nullptr; // Retourner un pointeur nul si l'index est invalide
    }

    void App::SetIcon(std::string path)
    {
        // Ne rien faire pour l'icône de l'application
        (void)path;
    }

    int App::FPS()
    {
        return static_cast<int>(1000000 / (time.DeltaTime() + 1)); // Calculer le nombre d'images par seconde
    }
}

int main()
{
    prof_init(); // Initialiser le système de minuterie
    Particule::Core::Time::m_startTime = prof_make(); // Initialiser le temps de départ
    prof_enter_norec(Particule::Core::Time::m_startTime); // Entrer dans le système de minuterie
    #if defined(MEMTRACK_ENABLED)
        memtrack_init(); // Initialiser le suivi de la mémoire
    #endif
    Particule::Core::App app; // Créer une instance de l'application
    Particule::Core::AssetManager::InitAssetManager(); // Initialiser le gestionnaire d'assets
    int value = MainApp(&app, 0, nullptr); // Appeler la fonction principale de l'application
    if (value == EXIT_SUCCESS)
    {
        while (app.GetMainWindow() != nullptr) // Boucle tant que la fenêtre principale n'est pas nulle
        {
            app.Update(); // Mettre à jour l'application
            if (keydown(KEY_ACON) && app.GetMainWindow() != nullptr)
                app.GetMainWindow()->Close();
        }
    }
    Particule::Core::AssetManager::UnloadAll(); // Décharger toutes les ressources
    prof_leave_norec(Particule::Core::Time::m_startTime); // Quitter le système de minuterie
    return value; // Retourner le code de sortie de l'application
}