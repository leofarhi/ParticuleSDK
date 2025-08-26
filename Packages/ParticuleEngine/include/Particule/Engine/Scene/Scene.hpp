#ifndef PE_SCENE_HPP
#define PE_SCENE_HPP

#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Core/Skybox.hpp>
#include <Particule/Engine/Core/GameObject.hpp>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <string_view>

namespace Particule::Engine {
    using namespace Particule::Core;

    class Scene
    {
    private:
        // Ownership: Scene exclusively owns its GameObjects
        std::vector<std::unique_ptr<GameObject>> gameObjects_;
        // Non-owning list scheduled for removal at EndMainLoop
        std::vector<GameObject*> toRemove_;

        // Deleted copy/move to avoid accidental duplication of ownership
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        // Helpers
        auto find_uptr_(GameObject* ptr) noexcept {
            return std::find_if(gameObjects_.begin(), gameObjects_.end(),
                [&](const std::unique_ptr<GameObject>& up){ return up.get() == ptr; });
        }

        // Called once per frame by SceneManager
        void EndMainLoop();
        void ToInitialize(GameObject* go) noexcept;

        bool isLoaded;
        friend class SceneManager;
    public:
        std::string name;
        bool enabled;
        Skybox skybox;

        explicit Scene(std::string name) : gameObjects_(0), toRemove_(0), isLoaded(false), name(std::move(name)), enabled(true), skybox(Color::Blue, Color::Cyan) {}
        ~Scene() noexcept;

        void DrawSky() noexcept;

        // --- Add / Remove ---
        // Take ownership from unique_ptr
        GameObject& AddGameObject(std::unique_ptr<GameObject> go);
        // Take ownership from raw ptr (for legacy code: new GameObject(scene, ...))
        GameObject& AddGameObject(GameObject* go_raw);

        // Schedule removal at end of frame (safe)
        void RemoveGameObject(GameObject* go) noexcept;

        // Find by name (non-owning pointer)
        GameObject* FindGameObject(std::string_view name) const noexcept;

        // Transfer ownership of a GameObject to another scene (keeps the same pointer value for external refs)
        bool MoveGameObjectTo(Scene& dst, GameObject* go) noexcept;

        // Iterate components of all objects
        template<typename Method, typename... Args>
        void CallAllComponents(Method method, bool includeInactive, Args&&... args)
        {
            for (size_t i = 0; i < gameObjects_.size(); i++)
            {
                GameObject* go = gameObjects_[i].get();
                if (includeInactive || go->activeInHierarchy())
                    go->CallComponents(method, includeInactive, std::forward<Args>(args)...);
            }
        }

        // Iteration utility (read-only)
        const std::vector<std::unique_ptr<GameObject>>& objects() const noexcept { return gameObjects_; }

        template<class TGO = GameObject, class... Args>
        TGO* EmplaceGameObject(Args&&... args)
        {
            auto ptr = std::make_unique<TGO>(this, std::forward<Args>(args)...);
            TGO* raw = ptr.get();
            AddGameObject(std::move(ptr)); // adoption ownership et association à la scène
            if (isLoaded)
                ToInitialize(raw);
            return raw;
        }
        
    };

}

#endif // PE_SCENE_HPP