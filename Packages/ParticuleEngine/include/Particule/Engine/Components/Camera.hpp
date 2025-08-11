#ifndef COMPO_CAMERA_HPP
#define COMPO_CAMERA_HPP
#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/Component.hpp>

namespace Particule::Engine {

    class Camera : public Component
    {
    public:
        static Camera *main;
        Camera(GameObject& gameObject);
        ~Camera() override;
        void Render();
    };

}

#endif // COMPO_CAMERA_HPP