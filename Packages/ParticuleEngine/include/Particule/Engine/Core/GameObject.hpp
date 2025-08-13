#ifndef PE_CORE_GAMEOBJECT_HPP
#define PE_CORE_GAMEOBJECT_HPP

#include <Particule/Core/ParticuleCore.hpp>
#include "Object.hpp"
#include "Transform.hpp"
#include <Particule/Engine/Enum/Layer.hpp>
#include <Particule/Engine/Enum/Tag.hpp>
#include <vector>
#include <string>
#include <cstdarg>
#include <memory>

namespace Particule::Engine {

    class Component;
    class Scene;

    class GameObject : public Object
    {
    private:
        bool m_activeSelf;
        Scene *scene;
        std::vector<std::unique_ptr<Component>> components;
        friend class Scene;

        GameObject() = delete;
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = delete;            // ou implémente prudemment
        GameObject& operator=(GameObject&&) = delete; // idem
    public:
        Transform transform;
        std::string name;
        Layer layer;
        Tag tag;
        bool isStatic;

        GameObject(Scene *scene);
        GameObject(Scene *scene, std::string name);
        ~GameObject() override;

        inline bool activeInHierarchy() const
        {
            return this->activeSelf() 
                && (this->transform.parent() == nullptr 
                    || this->transform.parent()->gameObject.activeInHierarchy());
        };
        inline bool activeSelf() const noexcept { return m_activeSelf; }
        void SetActive(bool value);

        inline Scene *GetScene() const noexcept { return scene; }

        template <typename T_Component, typename... Args>
        T_Component* AddComponent(Args&&... args);

        template <typename T_Component>
        T_Component *GetComponent();

        template <typename T_Component>
        [[nodiscard]] const T_Component* GetComponent() const;

        template <typename T_Component>
        std::vector<T_Component *> GetComponents();

        template <typename T_Component>
        [[nodiscard]] std::vector<const T_Component*> GetComponents() const;

        template<typename Method, typename... Args>
        void CallComponent(Method method, bool includeInactive, Args&&... args);

    };

}

#endif // PE_CORE_GAMEOBJECT_HPP