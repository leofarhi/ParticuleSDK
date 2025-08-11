#ifndef PE_CORE_TRANSFORM_HPP
#define PE_CORE_TRANSFORM_HPP

#include <Particule/Core/ParticuleCore.hpp>
#include <vector>

namespace Particule::Engine {

    using namespace Particule::Core;

    class GameObject;
    class Transform
    {
    private:
        Transform *m_parent;
        std::vector<Transform *> m_children;
    public:
        GameObject &gameObject;
        Vector3<fixed12_32> localPosition;
        Vector3<fixed12_32> localRotation;
        Vector3<fixed12_32> localScale;

        Transform(GameObject &gameObject);
        Transform(GameObject &gameObject, Vector3<fixed12_32> position);
        ~Transform();

        inline void SetPosition(Vector3<fixed12_32> position){
            if (this->m_parent == nullptr)
                this->localPosition = position;
            else
                this->localPosition = position - this->m_parent->position();
        };

        inline Vector3<fixed12_32> position(){
            if (this->m_parent == nullptr)
                return this->localPosition;
            return this->m_parent->position() + this->localPosition;
        };

        inline Vector3<fixed12_32> rotation(){
            if (this->m_parent == nullptr)
                return this->localRotation;
            return this->m_parent->rotation() + this->localRotation;
        };

        inline Vector3<fixed12_32> scale(){
            if (this->m_parent == nullptr)
                return this->localScale;
            return Vector3<fixed12_32>(
                this->m_parent->scale().x * this->localScale.x,
                this->m_parent->scale().y * this->localScale.y,
                this->m_parent->scale().z * this->localScale.z
            );
        };
        
        void SetParent(Transform *parent);
        inline Transform *parent() const { return m_parent; }
        inline std::vector<Transform *>& children() { return m_children; }
    };
}

#endif // PE_CORE_TRANSFORM_HPP