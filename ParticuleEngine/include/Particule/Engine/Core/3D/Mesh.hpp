#pragma once
#include <Particule/Core/ParticuleCore.hpp>
#include <vector>
#include "Material.hpp"
#include "RenderBuffer.hpp"
#include "../Transform.hpp"
#include "../../Components/Camera.hpp"
using namespace Particule::Core;

typedef Vector3<fixed12_32> Vertex;

struct Face
{
    uint8_t length;
    uint16_t* vertices;
    uint16_t* uvs;
    uint8_t material;
    bool tilling;
    bool square_uv;
};

class Mesh
{
public:
    Face* faces;
    Vector3<fixed12_32>* normals;
    std::vector<Material> materials;

    Vertex* vertices;
    Vector2<fixed16_32>* uvs;
    unsigned int vertex_count;
    unsigned int uv_count;
    unsigned int face_count;
    Mesh(unsigned int vertex_count, unsigned int uv_count, unsigned int face_count);
    ~Mesh();
    
    void CalculateTransform(Transform* transform, RenderBuffer* render_buffer);
    void CalculateProjection(Transform* transform, Camera* camera, RenderBuffer* render_buffer);
    void Render(Camera* camera, RenderBuffer* render_buffer);
    void CalculateNormals();
    void BakeUV();//TODO solve same uv but different Material
    void UnbakeUV();

    bool IsBackface(Camera* camera, RenderBuffer* render_buffer, unsigned int face_index);

    static Mesh* LoadMesh(Resource resource);

    inline void SetFace(int index, uint8_t length, uint16_t V[4], uint16_t UV[4], uint8_t material)
    {
        if (faces[index].length != length)
        {
            if (faces[index].vertices != nullptr)
                delete[] faces[index].vertices;
            if (faces[index].uvs != nullptr)
                delete[] faces[index].uvs;
            faces[index].vertices = new uint16_t[length];
            faces[index].uvs = new uint16_t[length];
        }
        faces[index].length = length;
        faces[index].tilling = false;
        for (int i = 0; i < length; i++)
        {
            faces[index].vertices[i] = V[i];
            faces[index].uvs[i] = UV[i];
            if (uvs[UV[i]].x > fixed16_32::one() || uvs[UV[i]].y > fixed16_32::one() ||
                uvs[UV[i]].x < fixed16_32::zero() || uvs[UV[i]].y < fixed16_32::zero())
                faces[index].tilling = true;
        }
        faces[index].square_uv = false;
        if (uvs[UV[0]].x == uvs[UV[1]].x && uvs[UV[0]].y == uvs[UV[1]].y &&
            uvs[UV[2]].x == uvs[UV[3]].x && uvs[UV[2]].y == uvs[UV[3]].y)
            faces[index].square_uv = true;
        faces[index].material = material;
        if (material >= materials.size())
            materials.resize(material + 1);
    }

    inline Vector3<fixed12_32> CalculateNormalFace(int index)
    {
        Vector3<fixed12_32> v1 = vertices[faces[index].vertices[0]];
        Vector3<fixed12_32> v2 = vertices[faces[index].vertices[1]];
        Vector3<fixed12_32> v3 = vertices[faces[index].vertices[2]];

        Vector3<fixed12_32> edge1 = v2 - v1;
        Vector3<fixed12_32> edge2 = v3 - v1;

        return edge2.cross(edge1).normalize();
    }
};