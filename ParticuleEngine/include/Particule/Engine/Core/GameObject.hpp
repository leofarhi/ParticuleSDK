#ifndef PE_CORE_GAMEOBJECT_HPP
#define PE_CORE_GAMEOBJECT_HPP

#include <Particule/Core/ParticuleCore.hpp>
#include "Object.hpp"
#include "Transform.hpp"
#include "../Enum/Layer.hpp"
#include "../Enum/Tag.hpp"
#include <vector>
#include <string>
#include <cstdarg>

class Component;
class Scene;

class GameObject : public Object
{
private:
    bool m_activeSelf;
    Scene *scene;
    friend class Scene;
public:
    Transform transform;
    std::string name;
    std::vector<Component*> components;
    Layer layer;
    Tag tag;
    bool isStatic;

    GameObject();
    GameObject(Scene *scene);
    GameObject(Scene *scene, std::string name);
    ~GameObject() override;

    inline bool activeInHierarchy() const
    {
        return this->activeSelf() 
            && (this->transform.parent() == nullptr 
                || this->transform.parent()->gameObject.activeInHierarchy());
    };
    inline bool activeSelf() const { return m_activeSelf; }
    void SetActive(bool value);

    inline Scene *GetScene() const { return scene; }

    template <typename T_Component, typename... Args>
    T_Component *AddComponent(Args... args);
    template <typename T_Component>
    T_Component *GetComponent();
    template <typename T_Component>
    std::vector<T_Component *> GetComponents();

    template<typename Method, typename... Args>
    void CallComponent(Method method, bool includeInactive, Args&&... args);

};



#endif // PE_CORE_GAMEOBJECT_HPP