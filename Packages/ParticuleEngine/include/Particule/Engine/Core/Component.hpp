#ifndef PE_CORE_COMPONENT_HPP
#define PE_CORE_COMPONENT_HPP

#include <Particule/Engine/Core/Object.hpp>
#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Transform.hpp>
#include <Particule/Engine/Core/Coroutine/CoroutineManager.hpp>
#include <stdio.h>
#include <cstdarg>

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
    T_Component *GameObject::AddComponent(Args... args)
    {
        static_assert(std::is_base_of<Component, T_Component>::value,
                  "T_Component must derive from Component");
        T_Component *component = new T_Component(*this, args...);
        components.push_back(component);
        return component;
    }

    template <typename T_Component>
    T_Component *GameObject::GetComponent()
    {
        for (Component *component : components)
        {
            T_Component *tComponent = dynamic_cast<T_Component *>(component);
            if (tComponent != nullptr)
                return tComponent;
        }
        return nullptr;
    }

    template <typename T_Component>
    std::vector<T_Component *> GameObject::GetComponents()
    {
        std::vector<T_Component *> list;
        for (Component *component : components)
        {
            T_Component *tComponent = dynamic_cast<T_Component *>(component);
            if (tComponent != nullptr)
                list.push_back(tComponent);
        }
        return list;
    }

    template<typename Method, typename... Args>
    void GameObject::CallComponent(Method method, bool includeInactive, Args&&... args)
    {
        for (Component *component : this->components)
        {
            if (includeInactive || component->enabled())
                (component->*method)(std::forward<Args>(args)...);
        }
    };

}

#endif // PE_CORE_COMPONENT_HPP