#ifndef PE_CORE_TRANSFORM_HPP
#define PE_CORE_TRANSFORM_HPP

#include <Particule/Core/ParticuleCore.hpp>
#include <vector>
#include <algorithm>
#include <Particule/Engine/Core/Types/MethodPropertyVec3.hpp>


namespace Particule::Engine {

    using namespace Particule::Core;

    class GameObject;
    class Transform {
    private:
        Transform* m_parent = nullptr;
        std::vector<Transform*> m_children;

        // Locals réels (privés)
        Vector3<fixed12_32> m_localPosition{ fixed12_32(0), fixed12_32(0), fixed12_32(0) };
        Vector3<fixed12_32> m_localRotation{ fixed12_32(0), fixed12_32(0), fixed12_32(0) };
        Vector3<fixed12_32> m_localScale   { fixed12_32(1), fixed12_32(1), fixed12_32(1) };

        // Caches monde
        mutable Vector3<fixed12_32> m_worldPosition{};
        mutable Vector3<fixed12_32> m_worldRotation{};
        mutable Vector3<fixed12_32> m_worldScale{ fixed12_32(1), fixed12_32(1), fixed12_32(1) };
        mutable bool m_worldDirty = true;

        inline void markWorldDirty() noexcept {
            if (m_worldDirty) return;
            m_worldDirty = true;
            for (auto* c : m_children) c->markWorldDirty();
        }

        inline void ensureWorldUpToDate() const noexcept {
            if (!m_worldDirty) return;
            if (m_parent) m_parent->ensureWorldUpToDate();

            if (!m_parent) {
                m_worldPosition = m_localPosition;
                m_worldRotation = m_localRotation;
                m_worldScale    = m_localScale;
            } else {
                m_worldPosition = m_parent->m_worldPosition + m_localPosition;
                m_worldRotation = m_parent->m_worldRotation + m_localRotation;
                m_worldScale    = m_parent->m_worldScale *  m_localScale; // hadamard
            }
            m_worldDirty = false;
        }

        // ——— setters/ getters MONDE (utilisés par properties world) ———
        inline void setWorldPosition(const Vector3<fixed12_32>& w) noexcept {
            if (m_parent) { m_parent->ensureWorldUpToDate(); m_localPosition = w - m_parent->m_worldPosition; }
            else          { m_localPosition = w; }
            markWorldDirty();
        }
        inline void setWorldRotation(const Vector3<fixed12_32>& w) noexcept {
            if (m_parent) { m_parent->ensureWorldUpToDate(); m_localRotation = w - m_parent->m_worldRotation; }
            else          { m_localRotation = w; }
            markWorldDirty();
        }
        inline void setWorldScale(const Vector3<fixed12_32>& w) noexcept {
            if (m_parent) { m_parent->ensureWorldUpToDate(); m_localScale = w / m_parent->m_worldScale; }
            else          { m_localScale = w; }
            markWorldDirty();
        }
        inline Vector3<fixed12_32> getWorldPosition() const noexcept { ensureWorldUpToDate(); return m_worldPosition; }
        inline Vector3<fixed12_32> getWorldRotation() const noexcept { ensureWorldUpToDate(); return m_worldRotation; }
        inline Vector3<fixed12_32> getWorldScale()    const noexcept { ensureWorldUpToDate(); return m_worldScale; }

        // ——— setters/ getters LOCAL (utilisés par properties local) ———
        inline void setLocalPosition(const Vector3<fixed12_32>& v) noexcept { m_localPosition = v; markWorldDirty(); }
        inline void setLocalRotation(const Vector3<fixed12_32>& v) noexcept { m_localRotation = v; markWorldDirty(); }
        inline void setLocalScale   (const Vector3<fixed12_32>& v) noexcept { m_localScale    = v; markWorldDirty(); }

        inline Vector3<fixed12_32> getLocalPosition() const noexcept { return m_localPosition; }
        inline Vector3<fixed12_32> getLocalRotation() const noexcept { return m_localRotation; }
        inline Vector3<fixed12_32> getLocalScale()    const noexcept { return m_localScale; }

    public:
        GameObject& gameObject;

        // -------- Properties --------
        // Monde
        using WorldPosProp = MethodPropertyVec3<Transform, Vector3<fixed12_32>,
                                                &Transform::getWorldPosition, &Transform::setWorldPosition>;
        using WorldRotProp = MethodPropertyVec3<Transform, Vector3<fixed12_32>,
                                                &Transform::getWorldRotation, &Transform::setWorldRotation>;
        using WorldSclProp = MethodPropertyVec3<Transform, Vector3<fixed12_32>,
                                                &Transform::getWorldScale,    &Transform::setWorldScale>;

        WorldPosProp position { this };
        WorldRotProp rotation { this };
        WorldSclProp scale    { this };

        // Local (intercepte les écritures pour marquer dirty)
        using LocalPosProp = MethodPropertyVec3<Transform, Vector3<fixed12_32>,
                                                &Transform::getLocalPosition, &Transform::setLocalPosition>;
        using LocalRotProp = MethodPropertyVec3<Transform, Vector3<fixed12_32>,
                                                &Transform::getLocalRotation, &Transform::setLocalRotation>;
        using LocalSclProp = MethodPropertyVec3<Transform, Vector3<fixed12_32>,
                                                &Transform::getLocalScale,    &Transform::setLocalScale>;

        LocalPosProp localPosition { this };
        LocalRotProp localRotation { this };
        LocalSclProp localScale    { this };

        // -------- Ctors / Dtor --------
        explicit Transform(GameObject& go) : gameObject(go) {}

        Transform(GameObject& go, Vector3<fixed12_32> positionLocal) : gameObject(go) {
            setLocalPosition(positionLocal);
        }

        ~Transform() {
            if (m_parent) {
                auto& sib = m_parent->m_children;
                sib.erase(std::remove(sib.begin(), sib.end(), this), sib.end());
                m_parent = nullptr;
            }
            for (auto* child : m_children) child->SetParent(nullptr);
        }

        // -------- Parenting --------
        void SetParent(Transform* parent, bool keepWorld = true) noexcept {
            if (parent == m_parent) return;

            if (m_parent) {
                auto& sib = m_parent->m_children;
                sib.erase(std::remove(sib.begin(), sib.end(), this), sib.end());
            }

            Vector3<fixed12_32> wP{}, wR{}, wS{};
            if (keepWorld) { ensureWorldUpToDate(); wP=m_worldPosition; wR=m_worldRotation; wS=m_worldScale; }

            m_parent = parent;
            if (m_parent) m_parent->m_children.push_back(this);

            if (keepWorld) { setWorldPosition(wP); setWorldRotation(wR); setWorldScale(wS); }
            else           { markWorldDirty(); }
        }

        inline Transform* parent() const noexcept { return m_parent; }
        inline std::vector<Transform*>& children() noexcept { return m_children; }
    };

}

#endif // PE_CORE_TRANSFORM_HPP