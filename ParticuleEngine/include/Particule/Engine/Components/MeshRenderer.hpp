#pragma once
#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Core/3D/Mesh.hpp>
#include <Particule/Engine/Core/3D/RenderBuffer.hpp>
#include <Particule/Engine/Components/Camera.hpp>

class MeshRenderer : public Component
{
public:
    RenderBuffer* render_buffer;
    Mesh* mesh;
    MeshRenderer(GameObject& gameObject) : Component(gameObject),
        render_buffer(nullptr),mesh(nullptr)
    {};
    ~MeshRenderer() override = default;

    inline void SetMesh(Mesh* mesh)
    {
        this->mesh = mesh;
        if (this->render_buffer != nullptr)
            delete this->render_buffer;
        this->render_buffer = RenderBuffer::Alloc(mesh->vertex_count, mesh->face_count);
        this->mesh->CalculateTransform(&this->gameObject.transform, render_buffer);
    };

    inline void OnRenderObject(Camera* camera) override
    {
        if (this->mesh == nullptr)
            return;
        //transform->localRotation.x += fixed12_32(0.02f);
        //transform->localRotation.y += fixed12_32(0.02f);
        //this->mesh->CalculateTransform(&this->gameObject.transform, render_buffer);
        this->mesh->CalculateProjection(&this->gameObject.transform, camera, render_buffer);
        this->mesh->Render(camera, render_buffer);
    };
};