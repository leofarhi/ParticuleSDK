#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Core/Transform.hpp>
#include <Particule/Engine/Scene/Scene.hpp>
#include <Particule/Engine/Scene/SceneManager.hpp>

namespace Particule::Engine {

    GameObject::GameObject(Scene *scene): GameObject(scene, "GameObject")
    {}

    GameObject::GameObject(Scene *scene, std::string name):
        m_activeSelf(true), scene(scene), transform(*this), name(name), components(), layer(Layer::LAYER_Default), tag(Tag::TAG_Untagged), isStatic(false)
    {}

    GameObject::~GameObject()
    {
        this->components.clear();
        if (this->transform.parent() != nullptr)
            this->transform.SetParent(nullptr);
        if (this->scene != nullptr)
            this->scene->RemoveGameObject(this);
    }

    void GameObject::SetActive(bool value)
    {
        const bool parentActive =
            (transform.parent() == nullptr)
                ? true
                : transform.parent()->gameObject.activeInHierarchy();

        const bool wasSelf = m_activeSelf;
        const bool was     = parentActive && wasSelf;

        m_activeSelf = value;

        const bool now = parentActive && m_activeSelf;

        // Notifie this si son état effectif change
        if (was != now) {
            if (now) CallComponent(&Component::OnEnable, false);
            else     CallComponent(&Component::OnDisable, false);
        } else {
            // Si rien ne change pour this, rien ne change pour les descendants
            return;
        }

        // Propage proprement aux enfants
        auto propagate = [&](auto&& self, GameObject* parent, bool parentWas, bool parentNow) -> void {
            auto& kids = parent->transform.children();
            for (Transform* ct : kids) {
                GameObject* child = &ct->gameObject;

                const bool childSelf = child->activeSelf();
                const bool childWas  = parentWas && childSelf;
                const bool childNow  = parentNow && childSelf;

                if (childWas != childNow) {
                    if (childNow) child->CallComponent(&Component::OnEnable, false);
                    else          child->CallComponent(&Component::OnDisable, false);
                }

                self(self, child, childWas, childNow);
            }
        };

        propagate(propagate, this, was, now);
    }


}