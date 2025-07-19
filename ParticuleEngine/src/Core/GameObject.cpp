#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Core/Transform.hpp>
#include <Particule/Engine/Scene/Scene.hpp>
#include <Particule/Engine/Scene/SceneManager.hpp>


GameObject::GameObject(): m_activeSelf(true),scene(nullptr), transform(*this), name("GameObject"),components(),layer(Layer::LAYER_Default), tag(Tag::TAG_Untagged), isStatic(false)
{
    this->scene = (Scene *)SceneManager::sceneManager->activeScene();
}

GameObject::GameObject(Scene *scene): GameObject(scene, "GameObject")
{}

GameObject::GameObject(Scene *scene, std::string name):
    m_activeSelf(true), scene(scene), transform(*this), name(name), components(), layer(Layer::LAYER_Default), tag(Tag::TAG_Untagged), isStatic(false)
{
    this->scene = scene;
    this->scene->AddGameObject(this);
}

GameObject::~GameObject()
{
    for (Component *component : this->components)
        delete component;
    this->components.clear();
    if (this->transform.parent() != nullptr)
        this->transform.SetParent(nullptr);
    if (this->scene != nullptr)
        this->scene->RemoveGameObject(this);
}

static void UpdateActive(GameObject *gameObject, bool value)
{
    bool currentValue = gameObject->activeSelf();
    if (currentValue != value)
    {
        if (value)
            gameObject->CallComponent(&Component::OnEnable, false);
        else
            gameObject->CallComponent(&Component::OnDisable, false);
        std::vector<Transform *>& children = gameObject->transform.children();
        for (Transform *child : children)
            UpdateActive(&child->gameObject, value);
    }
}

void GameObject::SetActive(bool value)
{
    bool oldValue = this->activeInHierarchy();
    if (!oldValue && this->m_activeSelf != value)
        UpdateActive(this, value);
    this->m_activeSelf = value;
}