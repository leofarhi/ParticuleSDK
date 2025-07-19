#include <Particule/Engine/Core/3D/Mesh.hpp>
#include <Particule/Engine/Core/3D/Shapes.hpp>
#include <Particule/Core/System/File.hpp>
using namespace Particule::Core;

Mesh::Mesh(unsigned int vertex_count, unsigned int uv_count, unsigned int face_count)
    : vertex_count(vertex_count), uv_count(uv_count), face_count(face_count)
{
    vertices = new Vertex[vertex_count];
    uvs = new Vector2<fixed16_32>[uv_count];
    faces = new Face[face_count];
    normals = new Vector3<fixed12_32>[face_count];
    for (unsigned int i = 0; i < face_count; i++)
    {
        faces[i].length = 0;
        faces[i].vertices = nullptr;
        faces[i].uvs = nullptr;
        faces[i].material = 255;
    }
}

Mesh::~Mesh()
{
    delete[] vertices;
    delete[] uvs;
    for (unsigned int i = 0; i < face_count; i++)
    {
        if (faces[i].vertices != nullptr)
            delete[] faces[i].vertices;
        if (faces[i].uvs != nullptr)
            delete[] faces[i].uvs;
    }
    delete[] faces;
    delete[] normals;
}

inline static void ApplyRotationX(fixed12_32 *y, fixed12_32 *z, fixed12_32 cos_theta, fixed12_32 sin_theta)
{
    const fixed12_32 new_y = (*y * cos_theta) - (*z * sin_theta);
    const fixed12_32 new_z = (*y * sin_theta) + (*z * cos_theta);
    *y = new_y;
    *z = new_z;
}

inline static void ApplyRotationY(fixed12_32 *x, fixed12_32 *z, fixed12_32 cos_theta, fixed12_32 sin_theta)
{
    const fixed12_32 new_x = (*x * cos_theta) + (*z * sin_theta);
    const fixed12_32 new_z = (-*x * sin_theta) + (*z * cos_theta);
    *x = new_x;
    *z = new_z;
}

inline static void ApplyRotationZ(fixed12_32 *x, fixed12_32 *y, fixed12_32 cos_theta, fixed12_32 sin_theta)
{
    const fixed12_32 new_x = (*x * cos_theta) - (*y * sin_theta);
    const fixed12_32 new_y = (*x * sin_theta) + (*y * cos_theta);
    *x = new_x;
    *y = new_y;
}

void Mesh::CalculateTransform(Transform* transform, RenderBuffer* render_buffer)
{
    const Vector3<fixed12_32> mesh_scale = transform->scale();
    const Vector3<fixed12_32> mesh_rotation = transform->rotation();
    render_buffer->cos_angle.x = fixed12_32::cos(mesh_rotation.x);
    render_buffer->cos_angle.y = fixed12_32::cos(mesh_rotation.y);
    render_buffer->cos_angle.z = fixed12_32::cos(mesh_rotation.z);
    render_buffer->sin_angle.x = fixed12_32::sin(mesh_rotation.x);
    render_buffer->sin_angle.y = fixed12_32::sin(mesh_rotation.y);
    render_buffer->sin_angle.z = fixed12_32::sin(mesh_rotation.z);
    Vector3<fixed12_32>& cos_mesh = render_buffer->cos_angle;
    Vector3<fixed12_32>& sin_mesh = render_buffer->sin_angle;
    for (unsigned int i = 0; i < this->vertex_count; i++)
    {
        Vector3<fixed12_32> vertice_position = this->vertices[i];
        // Apply local scale
        vertice_position.x *= mesh_scale.x;
        vertice_position.y *= mesh_scale.y;
        vertice_position.z *= mesh_scale.z;
        // Apply local rotation
        ApplyRotationX(&vertice_position.y, &vertice_position.z, cos_mesh.x, sin_mesh.x);
        ApplyRotationY(&vertice_position.x, &vertice_position.z, cos_mesh.y, sin_mesh.y);
        ApplyRotationZ(&vertice_position.x, &vertice_position.y, cos_mesh.z, sin_mesh.z);
        render_buffer->position[i] = vertice_position;
    }
    for (unsigned int i = 0; i < this->face_count; i++)
    {
        render_buffer->normal[i] = this->normals[i];
        Vector3<fixed12_32>& n =  render_buffer->normal[i];
        n.x *= mesh_scale.x.raw() < 0 ? -1 : 1;
        n.y *= mesh_scale.y.raw() < 0 ? -1 : 1;
        n.z *= mesh_scale.z.raw() < 0 ? -1 : 1;
        // Appliquer rotation dans l’ordre : X, Y, Z
        ApplyRotationX(&n.y, &n.z, cos_mesh.x, sin_mesh.x);
        ApplyRotationY(&n.x, &n.z, cos_mesh.y, sin_mesh.y);
        ApplyRotationZ(&n.x, &n.y, cos_mesh.z, sin_mesh.z);
    }
}

#ifdef CONST_SCREEN_SIZE
static const fixed12_32 half_screen_width = fixed12_32(SCREEN_WIDTH / 2);
static const fixed12_32 half_screen_height = fixed12_32(SCREEN_HEIGHT / 2);
#endif

void Mesh::CalculateProjection(Transform* transform, Camera* camera, RenderBuffer* render_buffer)
{
    Window* window = Window::GetCurrentWindow();
    const Vector3<fixed12_32> mesh_position = transform->position();
    render_buffer->offset = mesh_position;
    #ifndef CONST_SCREEN_SIZE
    const fixed12_32 half_screen_width = fixed12_32(window->Width() / 2);
    const fixed12_32 half_screen_height = fixed12_32(window->Height() / 2);
    #endif
    for (unsigned int i = 0; i < this->vertex_count; i++)
    {
        Vector3<fixed12_32> vertice_position = render_buffer->position[i];
        // Application de la translation (incluant l'inverse de la translation de la caméra)
        vertice_position.x += mesh_position.x - camera->position.x;
        vertice_position.y += mesh_position.y - camera->position.y;
        vertice_position.z += mesh_position.z - camera->position.z;
        // Application de la rotation de la caméra
        const Vector3<fixed12_32> cos_camera = camera->cos;
        const Vector3<fixed12_32> sin_camera = camera->sin;
        ApplyRotationX(&vertice_position.y, &vertice_position.z, cos_camera.x, sin_camera.x);
        ApplyRotationY(&vertice_position.x, &vertice_position.z, cos_camera.y, sin_camera.y);
        ApplyRotationZ(&vertice_position.x, &vertice_position.y, cos_camera.z, sin_camera.z);
        // Projection perspective
        //const fixed12_32 m_z = (vertice_position.z < fixed12_32(0.1)) ? fixed12_32(0.1) : vertice_position.z;
        const fixed12_32 m_z = (vertice_position.z <= fixed12_32(0.1)) ? fixed12_32(0.1) : vertice_position.z;
        const fixed12_32 f = (window->Width() / m_z);
        // Calcul final de la position projetée
        render_buffer->projected[i].x = (vertice_position.x * f) + half_screen_width;
        render_buffer->projected[i].y = (-vertice_position.y * f) + half_screen_height;
        render_buffer->projected[i].z = vertice_position.z;

        // Projection orthographique
        /*fixed12_32 orthographic_zoom = fixed12_32(camera->position.z/5); // 1.0 par défaut (pas de zoom)
        render_buffer->projected[i].x = (vertice_position.x * orthographic_zoom) + half_screen_width;
        render_buffer->projected[i].y = (-vertice_position.y * orthographic_zoom) + half_screen_height;
        render_buffer->projected[i].z = vertice_position.z; // Peut rester si vous voulez trier par profondeur*/


        render_buffer->projected[i].x += camera->offset_projection.x;
        render_buffer->projected[i].y += camera->offset_projection.y;
    }
}

bool Mesh::IsBackface(Camera* camera, RenderBuffer* render_buffer, unsigned int face_index)
{
    Face& face = this->faces[face_index];
    Vector3<fixed12_32> sum =  render_buffer->position[face.vertices[0]] +
                            render_buffer->position[face.vertices[1]] +
                            render_buffer->position[face.vertices[2]];
    if (face.length == 4)
        sum +=  render_buffer->position[face.vertices[3]];
    // Produit scalaire avec vecteur vue = (somme - camPos * n) · normal
    sum -= camera->position * face.length;
    sum += render_buffer->offset * 4;
    return render_buffer->normal[face_index].dot(sum) > 0;
}

static inline fixed12_32 GetDepth(RenderBuffer* render_buffer, Face* face)
{
    fixed12_32 depth = std::max(
        render_buffer->projected[face->vertices[0]].z,
        std::max(
            render_buffer->projected[face->vertices[1]].z,
            render_buffer->projected[face->vertices[2]].z
        )
    );
    if (face->length == 4)
        depth = std::max(depth, render_buffer->projected[face->vertices[3]].z);
    return depth;
}

#ifdef CONST_SCREEN_SIZE
static const fixed12_32 fscreen_width = fixed12_32(SCREEN_WIDTH);
static const fixed12_32 fscreen_height = fixed12_32(SCREEN_HEIGHT);
#endif
static inline bool IsVisible(Camera* camera, RenderBuffer* render_buffer, Face* face)
{
    (void)camera;
    fixed12_32 depth = GetDepth(render_buffer, face);
    if (depth <= 0 || depth > fixed12_32(100))
        return false;
    fixed12_32 minX = fixed12_32::max_value();
    fixed12_32 minY = fixed12_32::max_value();
    fixed12_32 maxX = fixed12_32::min_value();
    fixed12_32 maxY = fixed12_32::min_value();

    for (int i = 0; i < face->length; ++i)
    {
        fixed12_32 x = render_buffer->projected[face->vertices[i]].x;
        fixed12_32 y = render_buffer->projected[face->vertices[i]].y;

        if (x < minX) minX = x;
        if (y < minY) minY = y;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
    }

    // Écran AABB
    #ifndef CONST_SCREEN_SIZE
    const fixed12_32 fscreen_width = fixed12_32(window->Width());
    const fixed12_32 fscreen_height = fixed12_32(window->Height());
    #endif
    // Test d’intersection AABB: si bbox hors de l’écran → non visible
    if (maxX < 0 || maxY < 0 || minX >= fscreen_width || minY >= fscreen_height)
        return false;
    return true;
}

inline static void RenderQuad(RenderBuffer* render_buffer, Mesh* mesh, Face* face)
{
    VertexInfo verts[4] = {
        {render_buffer->projected[face->vertices[0]], mesh->uvs[face->uvs[0]]},
        {render_buffer->projected[face->vertices[1]], mesh->uvs[face->uvs[1]]},
        {render_buffer->projected[face->vertices[2]], mesh->uvs[face->uvs[2]]},
        {render_buffer->projected[face->vertices[3]], mesh->uvs[face->uvs[3]]}
    };
    for (int i0 = 1; i0 < 4; ++i0) {
        VertexInfo TL = verts[i0];
        VertexInfo TR = verts[(i0 + 1) % 4];
        VertexInfo BR = verts[(i0 + 2) % 4];
        VertexInfo BL = verts[(i0 + 3) % 4];

        // 2. Tester les segments pour déterminer la forme
        bool top_horizontal    = (TL.iprojected.y == TR.iprojected.y);
        bool bottom_horizontal = (BL.iprojected.y == BR.iprojected.y);
        bool left_vertical     = (TL.iprojected.x == BL.iprojected.x);
        bool right_vertical    = (TR.iprojected.x == BR.iprojected.x);

        if (top_horizontal && bottom_horizontal && left_vertical && right_vertical) {
            ConstDrawRect(face->tilling, face->square_uv,&mesh->materials[face->material], verts[0], verts[1], verts[2], verts[3]);
            return;
        }
        else if (top_horizontal && bottom_horizontal) {
            ConstDrawHorzParallelogram(face->tilling, face->square_uv,&mesh->materials[face->material], TL, TR, BR, BL);
            return;
        }
        else if (left_vertical && right_vertical) {
            ConstDrawVertParallelogram(face->tilling, face->square_uv,&mesh->materials[face->material], TL, TR, BR, BL);
            return;
        }
    }
    ConstDrawTriangle(face->tilling, &mesh->materials[face->material], verts[0], verts[1], verts[2]);
    ConstDrawTriangle(face->tilling, &mesh->materials[face->material], verts[0], verts[2], verts[3]);
}

inline void RenderWireframe(RenderBuffer* render_buffer, Face* face)
{
    for (int j = 0; j < face->length; ++j)
    {
        unsigned int v1 = face->vertices[j];
        unsigned int v2 = face->vertices[(j + 1) % face->length];
        DrawLine(
            (int)render_buffer->projected[v1].x,
            (int)render_buffer->projected[v1].y,
            (int)render_buffer->projected[v2].x,
            (int)render_buffer->projected[v2].y,
            COLOR_RED
        );
    }
}

void Mesh::Render(Camera* camera, RenderBuffer* render_buffer)
{
    for (unsigned int i = 0; i < this->face_count; i++)
    {
        Face* face = &this->faces[i];
        if (face->material != 255 && IsVisible(camera, render_buffer, face) && !IsBackface(camera, render_buffer, i))
        {
            if (face->length == 4)
                RenderQuad(render_buffer, this, face);
            else
            {
                ConstDrawTriangle(face->tilling,
                    &this->materials[face->material],
                    {render_buffer->projected[face->vertices[0]], this->uvs[face->uvs[0]]},
                    {render_buffer->projected[face->vertices[1]], this->uvs[face->uvs[1]]},
                    {render_buffer->projected[face->vertices[2]], this->uvs[face->uvs[2]]}
                );
            }
            RenderWireframe(render_buffer, face);
        }
    }
}

void Mesh::CalculateNormals()
{
    for (unsigned int i = 0; i < this->face_count; i++)
        this->normals[i] = this->CalculateNormalFace(i);
}

void Mesh::BakeUV()
{
    bool* uv_baked = new bool[this->uv_count];
    for (unsigned int i = 0; i < this->uv_count; i++)
        uv_baked[i] = false;
    for (unsigned int i = 0; i < this->face_count; i++)
    {
        Face* face = &this->faces[i];
        Material* material = &this->materials[face->material];
        if (material->sprite == nullptr)
            continue;
        for (int j = 0; j < face->length; j++)
        {
            if (!uv_baked[face->uvs[j]])
            {
                uv_baked[face->uvs[j]] = true;
                this->uvs[face->uvs[j]].x = material->sprite->GetRect().x + this->uvs[face->uvs[j]].x * (material->sprite->GetRect().w-1);
                this->uvs[face->uvs[j]].y = material->sprite->GetRect().y + this->uvs[face->uvs[j]].y * (material->sprite->GetRect().h-1);
            }
        }
    }
    delete[] uv_baked;
}

void Mesh::UnbakeUV()
{
    bool* uv_unbaked = new bool[this->uv_count];
    for (unsigned int i = 0; i < this->uv_count; i++)
        uv_unbaked[i] = false;
    for (unsigned int i = 0; i < this->face_count; i++)
    {
        Face* face = &this->faces[i];
        Material* material = &this->materials[face->material];
        if (material->sprite == nullptr)
            continue;
        for (int j = 0; j < face->length; j++)
        {
            if (!uv_unbaked[face->uvs[j]])
            {
                uv_unbaked[face->uvs[j]] = true;
                this->uvs[face->uvs[j]].x = (this->uvs[face->uvs[j]].x - material->sprite->GetRect().x) / (material->sprite->GetRect().w-1);
                this->uvs[face->uvs[j]].y = (this->uvs[face->uvs[j]].y - material->sprite->GetRect().y) / (material->sprite->GetRect().h-1);
            }
        }
    }
    delete[] uv_unbaked;
}

Mesh* Mesh::LoadMesh(Resource resource)
{
    if (resource.data != nullptr)
        return static_cast<Mesh*>(resource.data);
    File* file = File::Open(resource.path, FileMode::Read, Endian::BigEndian);
    if (file == nullptr)
        return nullptr;
    unsigned int vertexCount, uvCount, faceCount;
    file->Read<unsigned int>(vertexCount);
    file->Read<unsigned int>(uvCount);
    file->Read<unsigned int>(faceCount);
    Mesh* mesh = new Mesh(vertexCount, uvCount, faceCount);
    for (unsigned int i = 0; i < vertexCount; i++)
    {
        float x, y, z;
        file->Read<float>(x);
        file->Read<float>(y);
        file->Read<float>(z);
        mesh->vertices[i] = Vector3<fixed12_32>{fixed12_32(x), fixed12_32(y), fixed12_32(z)};
    }
    for (unsigned int i = 0; i < uvCount; i++)
    {
        float u, v;
        file->Read<float>(u);
        file->Read<float>(v);
        mesh->uvs[i] = Vector2<fixed16_32>{fixed16_32(u), fixed16_32(v)};
    }
    for (unsigned int i = 0; i < faceCount; i++)
    {
        unsigned char length;
        file->Read<unsigned char>(length);
        uint16_t vertices[4];
        uint16_t uvs[4];
        for (int j = 0; j < length; j++)
        {
            unsigned int vertex, uv;
            file->Read<unsigned int>(vertex);
            file->Read<unsigned int>(uv);
            vertices[j] = (uint16_t)vertex;
            uvs[j] = (uint16_t)uv;
        }
        mesh->SetFace(i, length, vertices, uvs, 0);
        float x, y, z;
        file->Read<float>(x);
        file->Read<float>(y);
        file->Read<float>(z);
        mesh->normals[i] = Vector3<fixed12_32>{fixed12_32(x), fixed12_32(y), fixed12_32(z)};
    }
    for (size_t i = 0; i < mesh->materials.size(); i++)
        mesh->materials[i].sprite = nullptr;
    //mesh->CalculateNormals();
    //mesh->BakeUV();
    File::Close(file);
    return mesh;
}