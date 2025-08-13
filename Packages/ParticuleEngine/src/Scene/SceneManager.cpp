#include <Particule/Engine/Scene/SceneManager.hpp>
#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Components/Camera.hpp>
#include <algorithm>

namespace Particule::Engine {

    SceneManager* SceneManager::sceneManager = nullptr;

    SceneManager::SceneManager() noexcept
    {
        SceneManager::sceneManager = this;
    }

    SceneManager::~SceneManager() noexcept
    {
        to_unload.clear();
        to_load.clear();
        loadedScenes.clear();
        AssetManager::UnloadUnused();
    }

    bool SceneManager::isRunning() const noexcept
    {
        return !loadedScenes.empty() || !to_load.empty() || !to_unload.empty();
    }

    void SceneManager::AddScene(std::string name, void (*loadScene)(Scene&))
    {
        if (name.empty()) throw std::invalid_argument("AddScene: empty name");
        if (!loadScene) throw std::invalid_argument("AddScene: null loader");
        // Enforce unique names
        for (auto const& sl : availableScenes)
            if (sl.name == name)
                throw std::invalid_argument("AddScene: duplicate scene name: " + name);
        availableScenes.emplace_back(std::move(name), loadScene);
    }

    void SceneManager::LoadScene(int index)
    {
        if (index < 0 || index >= static_cast<int>(availableScenes.size()))
            throw std::out_of_range("Scene index out of range (" + std::to_string(index) + ")");
        to_load.insert(index);
        loading = true;
    }

    void SceneManager::LoadScene(const std::string& name)
    {
        for (int i = 0; i < static_cast<int>(availableScenes.size()); ++i)
            if (availableScenes[i].name == name) { LoadScene(i); return; }
        throw std::out_of_range("Scene name not found (" + name + ")");
    }

    void SceneManager::ChangeScene(int index)
    {
        if (index < 0 || index >= static_cast<int>(availableScenes.size()))
            throw std::out_of_range("Scene index out of range (" + std::to_string(index) + ")");
        for (auto& up : loadedScenes)
            UnloadScene(up.get());
        to_load.insert(index);
        loading = true;
    }

    void SceneManager::ChangeScene(const std::string& name)
    {
        for (int i = 0; i < static_cast<int>(availableScenes.size()); ++i)
            if (availableScenes[i].name == name) { ChangeScene(i); return; }
        throw std::out_of_range("Scene name not found (" + name + ")");
    }

    void SceneManager::UnloadScene(Scene* scene) noexcept
    {
        if (!scene) return;
        to_unload.insert(scene);
        loading = true;
    }

    void SceneManager::UnloadScene(const std::string& name) noexcept
    {
        for (auto& up : loadedScenes)
            if (up && up->name == name) UnloadScene(up.get());
    }

    Scene* SceneManager::GetScene(const std::string& name) const noexcept
    {
        for (auto const& up : loadedScenes)
            if (up && up->name == name) return up.get();
        return nullptr;
    }

    Scene* SceneManager::activeScene() const noexcept
    {
        if (loadedScenes.empty()) return nullptr;
        return loadedScenes.front().get();
    }

    void SceneManager::MainLoop()
    {
        if (loading) {
            // Unload requested
            for (Scene* scene : to_unload) {
                if (!scene) continue;
                scene->CallAllComponents(&Component::OnDisable, false);
                scene->CallAllComponents(&Component::OnDestroy, true);
            }
            loadedScenes.erase(
                std::remove_if(loadedScenes.begin(), loadedScenes.end(),
                    [&](const std::unique_ptr<Scene>& up){ return to_unload.count(up.get()) > 0; }),
                loadedScenes.end());
            to_unload.clear();

            // Load requested
            for (int index : to_load) {
                auto& loader = availableScenes[index];
                // 1) Alloue la scène une seule fois avec le NOM source de vérité
                auto owned = std::make_unique<Scene>(loader.name);
                Scene* s = owned.get();
                // 2) Laisse le loadScene remplir la scène (sans new Scene à l'intérieur)
                loader.loadScene(*s);
                // 3) Adopte ownership et lance le cycle de vie
                loadedScenes.push_back(std::move(owned));
                s->CallAllComponents(&Component::Awake, true);
                s->CallAllComponents(&Component::OnEnable, false);
                s->CallAllComponents(&Component::Start, false);
            }
            to_load.clear();
            AssetManager::UnloadUnused();
            AssetManager::LoadUsed();
            loading = false;
        }

        CallAllComponents(&Component::FixedUpdate, false);
        CallAllComponents(&Component::Update, false);
        CoroutineManager::instance().update();
        CallAllComponents(&Component::LateUpdate, false);

        for (auto& up : loadedScenes)
            up->EndMainLoop();
    }

    void SceneManager::Draw()
    {
        if (loadedScenes.empty() || Camera::main == nullptr)
            return;
        Camera::main->Render();
    }
}
