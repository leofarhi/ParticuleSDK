#ifndef PE_SCENE_HPP
#define PE_SCENE_HPP

#include <Particule/Core/ParticuleCore.hpp>
#include "../Core/Component.hpp"
#include "../Core/Skybox.hpp"
#include "../Core/GameObject.hpp"
#include <vector>
#include <string>

using namespace Particule::Core;

class Scene
{
private:
    std::vector<GameObject*> gameObjectsToRemove;
    // Suppression des constructeurs de copie et de déplacement
    Scene(const Scene&) = delete; // Suppression du constructeur de copie
    Scene& operator=(const Scene&) = delete; // Suppression de l'opérateur d'affectation
    Scene(Scene&&) = delete; // Suppression du constructeur de déplacement
    Scene& operator=(Scene&&) = delete; // Suppression de l'opérateur d'affectation par déplacement
public:
    std::string name;
    std::vector<GameObject*> gameObjects;
    bool enabled;
    Skybox skybox;

    Scene(std::string name);
    ~Scene();
    void DrawSky();
    GameObject* AddGameObject(GameObject *gameObject);
    void RemoveGameObject(GameObject *gameObject);
    GameObject* FindGameObject(std::string name);
    void EndMainLoop();
    
    template<typename Method, typename... Args>
    void CallAllComponents(Method method, bool includeInactive, Args&&... args)
    {
        if (includeInactive)
        {
            for (GameObject *gameObject : this->gameObjects)
                gameObject->CallComponent(method, includeInactive, std::forward<Args>(args)...);
        }
        else
        {
            for (GameObject *gameObject : this->gameObjects)
            {
                if (gameObject->activeInHierarchy())
                    gameObject->CallComponent(method, includeInactive, std::forward<Args>(args)...);
            }
        }
    };
};


#endif // PE_SCENE_HPP