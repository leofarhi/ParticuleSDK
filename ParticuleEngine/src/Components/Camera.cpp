#include <Particule/Engine/Components/Camera.hpp>
#include <Particule/Engine/Scene/SceneManager.hpp>
using namespace Particule::Core;

Camera *Camera::main = nullptr;
#ifdef CONST_SCREEN_SIZE
fixed12_32 Camera::z_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
#else
fixed12_32* Camera::z_buffer = nullptr;
#endif
int Camera::screen_width = -1;
int Camera::screen_height = -1;

Camera::Camera(GameObject& gameObject): Component(gameObject)
{
    if (Camera::main == nullptr)
        Camera::main = this;
    #ifndef CONST_SCREEN_SIZE
    this->screen_width = -1;
    this->screen_height = -1;
    Camera::z_buffer = nullptr;
    #endif
    this->offset_projection = Vector2<fixed12_32>(0, 0);
}

Camera::~Camera()
{
    if (Camera::main == this)
        Camera::main = nullptr;
}

void Camera::Render(SceneManager* manager)
{
    Window* window = Window::GetCurrentWindow();
    Vector3<fixed12_32> rotation = this->gameObject.transform.rotation();
    this->cos.x = fixed12_32::cos(rotation.x);
    this->cos.y = fixed12_32::cos(rotation.y);
    this->cos.z = fixed12_32::cos(rotation.z);
    this->sin.x = fixed12_32::sin(rotation.x);
    this->sin.y = fixed12_32::sin(rotation.y);
    this->sin.z = fixed12_32::sin(rotation.z);
    this->position = this->gameObject.transform.position();
    #ifndef CONST_SCREEN_SIZE
    if (this->screen_width != window->Width() || this->screen_height != window->Height())
    {
        if (Camera::z_buffer != nullptr)
            delete[] Camera::z_buffer;
        Camera::z_buffer = new fixed12_32[window->Width() * window->Height()];
    }
    #endif
    this->screen_width = window->Width();
    this->screen_height = window->Height();
    Camera::ClearZBuffer();
    manager->activeScene()->DrawSky();
    manager->CallAllComponents(&Component::OnRenderObject, false, this);
    //Draw depth map
    /*for (int y = 0; y < window->Height(); y++)
    {
        for (int x = 0; x < window->Width(); x++)
        {
            int depth = (Camera::z_buffer[x + y * window->Width()]*1500)/255;
            if (depth == fixed12_32::max_value())
                continue;
            Color color = Color(depth, depth, depth, 255);
            DrawPixelUnsafe(window, x, y, color);
        }
    }*/
    manager->CallAllComponents(&Component::OnRenderImage, false, this);
}