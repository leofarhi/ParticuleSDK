#include <Particule/Engine/Components/Camera.hpp>
#include <Particule/Engine/Scene/SceneManager.hpp>

namespace Particule::Engine {

    Camera *Camera::main = nullptr;

    Camera::Camera(GameObject& gameObject): Component(gameObject)
    {
        if (Camera::main == nullptr)
            Camera::main = this;
    }

    Camera::~Camera()
    {
        if (Camera::main == this)
            Camera::main = nullptr;
    }

    void Camera::Render()
    {
        SceneManager* manager = SceneManager::sceneManager;
        manager->activeScene()->DrawSky();
        manager->CallAllComponents(&Component::OnRenderObject, false, this);
        manager->CallAllComponents(&Component::OnRenderImage, false, this);
    }

}