#include <Particule/Engine/Scene/Scene.hpp>
#include <algorithm>

namespace Particule::Engine {

    Scene::~Scene() noexcept
    {
        // unique_ptr cleans up automatically
        toRemove_.clear();
        gameObjects_.clear();
    }

    void Scene::DrawSky() noexcept
    {
        Window* window = Window::GetCurrentWindow();
        if (skybox.top.A() == 0 && skybox.bottom.A() == 0)
            return;
        if (skybox.top == skybox.bottom)
            window->Clear(skybox.top);
        else
        {
            int heightInt = window->Height();
            if (skybox.height != heightInt)
                skybox.CalculateGradient(heightInt);

            fixed12_32 rgb[3] = {skybox.rgb_start[0], skybox.rgb_start[1], skybox.rgb_start[2]};
            for (int y = 0; y < heightInt; y++)
            {
                Color color(static_cast<int>(rgb[0]), static_cast<int>(rgb[1]), static_cast<int>(rgb[2]), 255);
                DrawHLine(y, color);
                rgb[0] += skybox.rgbStep[0];
                rgb[1] += skybox.rgbStep[1];
                rgb[2] += skybox.rgbStep[2];
            }
        }
    }

    GameObject& Scene::AddGameObject(std::unique_ptr<GameObject> go)
    {
        if (!go) throw std::invalid_argument("AddGameObject: null unique_ptr");
        GameObject* raw = go.get();
        // Ensure the back-reference is correct
        raw->scene = this;
        // Only push if not already owned by this scene
        if (find_uptr_(raw) == gameObjects_.end())
            gameObjects_.push_back(std::move(go));
        return *raw;
    }

    GameObject& Scene::AddGameObject(GameObject* go_raw)
    {
        if (!go_raw) throw std::invalid_argument("AddGameObject: null raw pointer");
        // Ensure the back-reference is correct
        if (go_raw->scene != this) go_raw->scene = this;
        // Adopt ownership only if not already present in this scene
        if (find_uptr_(go_raw) == gameObjects_.end())
            gameObjects_.emplace_back(go_raw);
        return *go_raw;
    }

    void Scene::RemoveGameObject(GameObject* go) noexcept
    {
        if (!go || go->scene != this) return;
        toRemove_.push_back(go);
    }

    GameObject* Scene::FindGameObject(std::string_view nm) const noexcept
    {
        for (auto const& up : gameObjects_) {
            GameObject* go = up.get();
            if (go && go->name == nm) return go;
        }
        return nullptr;
    }

    bool Scene::MoveGameObjectTo(Scene& dst, GameObject* go) noexcept
    {
        if (!go || go->scene != this) return false;
        auto it = find_uptr_(go);
        if (it == gameObjects_.end()) return false; // not found (shouldn’t happen)

        // Keep raw pointer stable while transferring unique_ptr
        std::unique_ptr<GameObject> owned = std::move(*it);
        gameObjects_.erase(it);

        go->scene = &dst;
        dst.AddGameObject(std::move(owned));
        return true;
    }

    void Scene::EndMainLoop()
    {
        if (toRemove_.empty()) return;

        std::unordered_set<GameObject*> doomed(toRemove_.begin(), toRemove_.end());

        // Destroy owned objects marked for removal
        // (unique_ptr handles delete when removed from vector)
        gameObjects_.erase(
            std::remove_if(gameObjects_.begin(), gameObjects_.end(),
                [&](const std::unique_ptr<GameObject>& up){
                    if (!up) return true; // shouldn't happen, but be safe
                    if (doomed.count(up.get()) == 0) return false;
                    up->scene = nullptr;
                    return true; // erase => destroy
                }),
            gameObjects_.end());

        toRemove_.clear();
    }
}