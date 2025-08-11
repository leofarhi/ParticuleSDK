#ifndef PE_SCENE_MANAGER_HPP
#define PE_SCENE_MANAGER_HPP
#include <Particule/Core/ParticuleCore.hpp>
#include <vector>
#include <unordered_set>
#include <string>
#include <Particule/Engine/Scene/Scene.hpp>

namespace Particule::Engine {

    class SceneManager
    {
        struct SceneLoader
        {
            std::string name;
            Scene* (*loadScene)(void);
            SceneLoader(std::string name, Scene* (*loadScene)(void)) : name(name), loadScene(loadScene) {}
            SceneLoader() : name(""), loadScene(nullptr) {}
            SceneLoader(const SceneLoader& other) : name(other.name), loadScene(other.loadScene) {}
            SceneLoader(SceneLoader&& other) noexcept : name(std::move(other.name)), loadScene(other.loadScene) {}
            SceneLoader& operator=(const SceneLoader& other)
            {
                if (this != &other)
                {
                    name = other.name;
                    loadScene = other.loadScene;
                }
                return *this;
            }
            SceneLoader& operator=(SceneLoader&& other) noexcept
            {
                if (this != &other)
                {
                    name = std::move(other.name);
                    loadScene = other.loadScene;
                }
                return *this;
            }
            bool operator==(const SceneLoader& other) const
            {
                return name == other.name && loadScene == other.loadScene;
            }
            bool operator!=(const SceneLoader& other) const
            {
                return !(*this == other);
            }
        };

    private:
        std::vector<SceneLoader> availableScenes;
        std::vector<Scene*> loadedScenes;
        std::unordered_set<int> to_load;
        std::unordered_set<Scene*> to_unload;
        bool loading;
    public:
        static SceneManager *sceneManager;

        SceneManager();
        ~SceneManager();
        bool isRunning();
        void AddScene(std::string name, Scene* (*loadScene)(void));

        void LoadScene(int index);
        void LoadScene(std::string name);
        void ChangeScene(int index);
        void ChangeScene(std::string name);

        void UnloadScene(Scene *scene);
        void UnloadScene(std::string name);

        Scene *GetScene(std::string name);
        Scene *activeScene();

        template<typename Method, typename... Args>
        void CallAllComponents(Method method, bool includeInactive, Args&&... args)
        {
            for (Scene* scene : loadedScenes)
            {
                if (scene->enabled)
                    scene->CallAllComponents(method, includeInactive, std::forward<Args>(args)...);
            }
        }

        void MainLoop();
        void Draw();
    };

}

#endif // PE_SCENE_MANAGER_HPP