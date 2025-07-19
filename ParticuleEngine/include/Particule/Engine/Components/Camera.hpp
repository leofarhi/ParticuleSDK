#ifndef COMPO_CAMERA_HPP
#define COMPO_CAMERA_HPP
#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/Component.hpp>

class SceneManager;

class Mesh;

class Camera : public Component
{
private:
    Vector3<fixed12_32> cos;
    Vector3<fixed12_32> sin;
    Vector3<fixed12_32> position;
    friend class Mesh;
public:
    #ifdef CONST_SCREEN_SIZE
    static fixed12_32 z_buffer[SCREEN_WIDTH*SCREEN_HEIGHT];
    #else
    static fixed12_32* z_buffer;
    #endif
    static int screen_width;
    static int screen_height;
    static Camera *main;
    Vector2<fixed12_32> offset_projection;
    Camera(GameObject& gameObject);
    ~Camera() override;
    void Render(SceneManager* manager);

    static inline void ClearZBuffer()
    {
        std::fill(z_buffer, z_buffer + screen_width * screen_height, fixed12_32::max_value());
    };
};

#endif // COMPO_CAMERA_HPP