#ifndef PE_SCENE_MANAGER_HPP
#define PE_SCENE_MANAGER_HPP
#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Scene/Scene.hpp>
#include <vector>
#include <unordered_set>
#include <string>
#include <functional>
#include <memory>

namespace Particule::Engine {

    class SceneManager
    {
        struct SceneLoader
        {
            std::string name;
            // Legacy-compatible: returns raw Scene* allocated with new
            Scene* (*loadScene)(void) { nullptr };

            SceneLoader() = default;
            SceneLoader(std::string name_, Scene* (*fn)(void))
                : name(std::move(name_)), loadScene(fn) {}
        };

    private:
        std::vector<SceneLoader> availableScenes;
        std::vector<std::unique_ptr<Scene>> loadedScenes;   // ownership here
        std::unordered_set<int> to_load;
        std::unordered_set<Scene*> to_unload;               // non-owning markers
        bool loading { false };

    public:
        static SceneManager* sceneManager;

        SceneManager() noexcept;
        ~SceneManager() noexcept;

        bool isRunning() const noexcept;

        void AddScene(std::string name, Scene* (*loadScene)(void));

        void LoadScene(int index);
        void LoadScene(const std::string& name);
        void ChangeScene(int index);
        void ChangeScene(const std::string& name);

        void UnloadScene(Scene* scene) noexcept;
        void UnloadScene(const std::string& name) noexcept;

        Scene* GetScene(const std::string& name) const noexcept;
        Scene* activeScene() const noexcept;

        template<typename Method, typename... Args>
        void CallAllComponents(Method method, bool includeInactive, Args&&... args)
        {
            for (auto& up : loadedScenes) {
                Scene* scene = up.get();
                if (scene->enabled)
                    scene->CallAllComponents(method, includeInactive, std::forward<Args>(args)...);
            }
        }

        void MainLoop();
        void Draw();
    };

}

#endif // PE_SCENE_MANAGER_HPP