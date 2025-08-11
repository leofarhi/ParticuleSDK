#include <Particule/Engine/Scene/SceneManager.hpp>
#include <Particule/Engine/Core/GameObject.hpp>
#include <Particule/Engine/Core/Component.hpp>
#include <Particule/Engine/Components/Camera.hpp>
#include <algorithm>

namespace Particule::Engine {

    SceneManager *SceneManager::sceneManager = nullptr;

    SceneManager::SceneManager() : availableScenes(), loadedScenes(), to_load(), to_unload(), loading(false)
    {
        SceneManager::sceneManager = this;
    }

    SceneManager::~SceneManager()
    {
        for (Scene *scene : loadedScenes)
            delete scene;
        loadedScenes.clear();
        //loadedScenes.shrink_to_fit();
    }

    bool SceneManager::isRunning()
    {
        return loadedScenes.size() > 0 || (to_load.size() > 0) || (to_unload.size() > 0);
    }

    void SceneManager::AddScene(std::string name, Scene* (*loadScene)(void))
    {
        availableScenes.push_back(SceneManager::SceneLoader(name, loadScene));
    }

    void SceneManager::LoadScene(int index)
    {
        if (index < 0 || index >= (int)availableScenes.size())
            throw std::out_of_range("Scene index out of range (" + std::to_string(index) + ")");
        to_load.insert(index);
        loading = true;
    }

    void SceneManager::LoadScene(std::string name)
    {
        for (int i = 0; i < (int)availableScenes.size(); i++)
        {
            if (availableScenes[i].name == name)
            {
                this->LoadScene(i);
                return;
            }
        }
        throw std::out_of_range("Scene name not found (" + name + ")");
    }

    void SceneManager::ChangeScene(int index)
    {
        if (index < 0 || index >= (int)availableScenes.size())
            throw std::out_of_range("Scene index out of range (" + std::to_string(index) + ")");
        for (Scene *scene : loadedScenes)
            UnloadScene(scene);
        to_load.insert(index);
        loading = true;
    }

    void SceneManager::ChangeScene(std::string name)
    {
        for (int i = 0; i < (int)availableScenes.size(); i++)
        {
            if (availableScenes[i].name == name)
            {
                this->ChangeScene(i);
                return;
            }
        }
        throw std::out_of_range("Scene name not found (" + name + ")");
    }

    void SceneManager::UnloadScene(Scene *scene)
    {
        if (scene == nullptr)
            return;
        to_unload.insert(scene);
        loading = true;
    }

    void SceneManager::UnloadScene(std::string name)
    {
        for (Scene *scene : loadedScenes)
        {
            if (scene->name == name)
                this->UnloadScene(scene);
        }
    }

    Scene *SceneManager::GetScene(std::string name)
    {
        for (Scene *scene : loadedScenes)
        {
            if (scene->name == name)
                return scene;
        }
        return nullptr;
    }

    Scene *SceneManager::activeScene()
    {
        if (loadedScenes.size() == 0)
            return nullptr;
        return loadedScenes[0];
    }

    void SceneManager::MainLoop()
    {
        if (loading)
        {
            // Unload and load scenes
            for (Scene *scene : to_unload)
            {
                scene->CallAllComponents(&Component::OnDisable, false);
                scene->CallAllComponents(&Component::OnDestroy, true);
                delete scene;
            }
            loadedScenes.erase(
                std::remove_if(loadedScenes.begin(), loadedScenes.end(),
                    [&](Scene* scene) { return to_unload.count(scene) > 0; }),
                loadedScenes.end()
            );
            to_unload.clear();
            for (int index : to_load)
            {
                Scene *scene = availableScenes[index].loadScene();
                loadedScenes.push_back(scene);
                scene->CallAllComponents(&Component::Awake, true);
                scene->CallAllComponents(&Component::OnEnable, false);
                scene->CallAllComponents(&Component::Start, false);
            }
            to_load.clear();
        }
        
        CallAllComponents(&Component::FixedUpdate, false);
        CallAllComponents(&Component::Update, false);
        CoroutineManager::instance().update();
        CallAllComponents(&Component::LateUpdate, false);
        for (Scene *scene : loadedScenes)
            scene->EndMainLoop();
    }

    void SceneManager::Draw()
    {
        if (loadedScenes.size() == 0 || Camera::main == nullptr)
            return;
        Camera::main->Render();
    }

}