#include <Particule/Engine/Core/Transform.hpp>
#include <Particule/Engine/Core/GameObject.hpp>
#include <algorithm>


Transform::Transform(GameObject &gameObject): m_parent(nullptr), m_children(), gameObject(gameObject), localPosition(Vector3<fixed12_32>(0,0,0)),
    localRotation(Vector3<fixed12_32>(0,0,0)), localScale(Vector3<fixed12_32>(1,1,1))
{}

Transform::Transform(GameObject &gameObject, Vector3<fixed12_32> position): m_parent(nullptr), m_children(), gameObject(gameObject), localPosition(position),
    localRotation(Vector3<fixed12_32>(0,0,0)), localScale(Vector3<fixed12_32>(1,1,1))
{}

Transform::~Transform()
{
    if (this->m_parent != nullptr)
    {
        this->m_parent->m_children.erase(std::remove(this->m_parent->m_children.begin(), this->m_parent->m_children.end(), this), this->m_parent->m_children.end());
        this->m_parent = nullptr;
    }
    for (auto child : m_children)
        child->SetParent(nullptr);
}

void Transform::SetParent(Transform *parent)
{
    Vector3<fixed12_32> pos = this->position();
    if (this->m_parent != nullptr)
    {
        this->m_parent->m_children.erase(std::remove(this->m_parent->m_children.begin(), this->m_parent->m_children.end(), this), this->m_parent->m_children.end());
        this->m_parent = nullptr;
    }
    this->m_parent = parent;
    parent->m_children.push_back(this);
    this->SetPosition(pos);
}
