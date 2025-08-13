#ifndef PE_CORE_COMPONENT_HPP
#define PE_CORE_COMPONENT_HPP

#include <Particule/Engine/Core/Object.hpp>
#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Transform.hpp>
#include <Particule/Engine/Core/Coroutine/CoroutineManager.hpp>
#include <stdio.h>
#include <cstdarg>
#include <memory>

namespace Particule::Engine {

    using namespace Particule::Core;

    class Camera;

    class Component : public Object
    {
    private:
        bool m_enabled;
    public:
        GameObject& gameObject;
        Component(GameObject& gameObject) : m_enabled(true), gameObject(gameObject) {}
        virtual ~Component() = default;

        inline bool enabled() const { return m_enabled; }
        void inline SetActive(bool value)
        {
            if (value)
                this->OnEnable();
            else
                this->OnDisable();
            this->m_enabled = value;
        };

        virtual void Awake() {};
        virtual void Start() {};
        virtual void Update() {};
        virtual void FixedUpdate() {};
        virtual void LateUpdate() {};

        virtual void OnEnable() {};
        virtual void OnDisable() {};
        virtual void OnDestroy() {};

        virtual void OnRenderObject(Camera* camera) {(void)camera;};
        virtual void OnRenderImage(Camera* camera) {(void)camera;};

        Coroutine* StartCoroutine(Coroutine&& co) {
            return CoroutineManager::instance().start(std::move(co));
        }

        void StopCoroutine(Coroutine* co) {
            if (co)
                co->stop();
        }

    };

    template <typename T_Component, typename... Args>
    T_Component* GameObject::AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T_Component>,
                    "T_Component must derive from Component");
        auto up = std::make_unique<T_Component>(*this, std::forward<Args>(args)...);
        T_Component* raw = up.get();
        components.emplace_back(std::move(up));
        return raw;
    }

    // GetComponent (non-const)
    template <typename T_Component>
    [[nodiscard]] T_Component* GameObject::GetComponent()
    {
        for (auto& up : components)
            if (auto* p = dynamic_cast<T_Component*>(up.get()))
                return p;
        return nullptr;
    }

    // GetComponent (const)
    template <typename T_Component>
    [[nodiscard]] const T_Component* GameObject::GetComponent() const
    {
        for (auto const& up : components)
            if (auto* p = dynamic_cast<const T_Component*>(up.get()))
                return p;
        return nullptr;
    }

    // GetComponents (non-const)
    template <typename T_Component>
    [[nodiscard]] std::vector<T_Component*> GameObject::GetComponents()
    {
        std::vector<T_Component*> list;
        list.reserve(components.size());
        for (auto& up : components)
            if (auto* p = dynamic_cast<T_Component*>(up.get()))
                list.push_back(p);
        return list;
    }

    // GetComponents (const)
    template <typename T_Component>
    [[nodiscard]] std::vector<const T_Component*> GameObject::GetComponents() const
    {
        std::vector<const T_Component*> list;
        list.reserve(components.size());
        for (auto const& up : components)
            if (auto* p = dynamic_cast<const T_Component*>(up.get()))
                list.push_back(p);
        return list;
    }

    template<typename Method, typename... Args>
    void GameObject::CallComponent(Method method, bool includeInactive, Args&&... args)
    {
        for (auto& up : components)
        {
            Component* c = up.get();
            if (includeInactive || c->enabled())
                (c->*method)(std::forward<Args>(args)...);
        }
    }

}

#endif // PE_CORE_COMPONENT_HPP