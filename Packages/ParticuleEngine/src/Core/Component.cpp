#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Core/Transform.hpp>
#include <Particule/Engine/Scene/Scene.hpp>
#include <Particule/Engine/Scene/SceneManager.hpp>
namespace Particule::Engine {

    using namespace Particule::Core;

    void Component::Destroy(Component* component) {
        if (component)
            delete component;
    }

    void Component::Destroy(GameObject* obj) {
        if (obj)
            obj->GetScene()->RemoveGameObject(obj);
    }

}