#include <Particule/Core/System/App.hpp>
#include <Particule/Core/System/AssetManager.hpp>
#include <algorithm>
#include <stdexcept>
#include <windows.h>
#include <Particule/Core/System/sdl2.hpp>

namespace Particule::Core
{
    Time App::time; // Instance statique de la classe Time
    App* App::instance = nullptr; // Initialisation de l'instance statique de l'application
    Window* App::MainWindow = nullptr; // Initialisation du pointeur statique vers la fenêtre principale

    App::App()
    {
        time = Time(); // Initialisation de l'instance de temps
        instance = this; // Assigner l'instance courante à l'instance statique
        // Initializes the SDL.
        if (sdl2::SDL_Init(SDL_INIT_VIDEO) != 0)
            throw std::runtime_error("error Init " + sdl2::SDL2_GetError());
        ResetWorkingDirectory(); // Réinitialiser le répertoire de travail
        // Initializes the image library.
        if (sdl2::IMG_Init(sdl2::IMG_INIT_PNG) == 0)
            throw std::runtime_error("error IMG_Init " + sdl2::SDL2_GetError());
        // Initializes the font library.
        if (sdl2::TTF_Init() == -1)
            throw std::runtime_error("error TTF_Init " + sdl2::SDL2_GetError());
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

    void App::ResetWorkingDirectory(){
        // Obtenez le chemin absolu du programme
        char *path = sdl2::SDL_GetBasePath();
        if (path == NULL)
            throw std::runtime_error("error SDL_GetBasePath "+ sdl2::SDL2_GetError());
        // Changez le répertoire de travail
        if (!SetCurrentDirectory(path))
            throw std::runtime_error("error chdir "+ sdl2::SDL2_GetError());
        sdl2::SDL_free(path);
    }
    void App::SetWorkingDirectory(std::string& path){
        // Changez le répertoire de travail
        if (!SetCurrentDirectory(path.c_str()))
            throw std::runtime_error("error chdir "+ sdl2::SDL2_GetError());
    }
    std::string App::GetWorkingDirectory(){ 
        char path[MAX_PATH];
        if (GetCurrentDirectoryA(MAX_PATH, path) == 0)
            throw std::runtime_error("error GetCurrentDirectory "+ sdl2::SDL2_GetError());
        return std::string(path);
    }

    void App::Update()
    {
        time.Update(); // Mettre à jour le temps
        uint32_t startTime = sdl2::SDL_GetTicks(); // Obtenir le temps de début
        this->events.clear();
        sdl2::SDL_Event event;
        while (sdl2::SDL_PollEvent(&event))
            this->events.push_back(event);
        for (size_t i = 0; i < windows.size(); ++i) // Parcourir toutes les fenêtres
        {
            if (windows[i] != nullptr) // Vérifier si la fenêtre n'est pas nulle
            {
                Window::BindWindow(windows[i]); // Lier la fenêtre courante
                windows[i]->UpdateInput();
                windows[i]->OnUpdate(); // Appeler la méthode de mise à jour de la fenêtre
                windows[i]->OnDraw();
                Window::UnbindWindow(); // Délier la fenêtre courante
                if (!windows[i]->IsRunning)
                    RemoveWindow(windows[i--]); // Supprimer la fenêtre si elle n'est pas en cours d'exécution
            }
        }
        //limit a 120 fps
        uint32_t endTime = sdl2::SDL_GetTicks(); // Obtenir le temps de fin
        uint32_t deltaTime = endTime - startTime; // Calculer le temps écoulé
        if (deltaTime < 1000 / 120) // Vérifier si le temps écoulé est inférieur à la limite de 120 FPS
        {
            sdl2::SDL_Delay(1000 / 120 - deltaTime); // Attendre pour respecter la limite de FPS
        }
    }

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
        (void)path; // Ignorer le paramètre de chemin
        //if (MainWindow != nullptr) // Vérifier si la fenêtre principale n'est pas nulle
        //    sdl2::SetWindowIcon(MainWindow->window, path); // Définir l'icône de la fenêtre principale
    }

    int App::FPS()
    {
        return static_cast<int>(1000000 / (time.DeltaTime() + 1)); // Calculer et retourner le nombre d'images par seconde
    }
}

int main(int argc, char* argv[])
{
    Particule::Core::App app; // Créer une instance de l'application
    Particule::Core::AssetManager::InitAssetManager(); // Initialiser le gestionnaire d'assets
    int value = MainApp(&app, argc, argv); // Appeler la fonction principale de l'application
    if (value == EXIT_SUCCESS)
    {
        while (app.GetMainWindow() != nullptr) // Boucle tant que la fenêtre principale n'est pas nulle
            app.Update(); // Mettre à jour l'application
    }
    Particule::Core::AssetManager::UnloadAll(); // Décharger toutes les ressources
    return value; // Retourner le code de sortie de l'application
}

#ifdef _WIN32
// Ajoute ce WinMain pour satisfaire le linker en mode WINDOWS subsystem
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif