#include <Particule/Engine/Scene/Scene.hpp>
#include <algorithm>

using namespace Particule::Core;

Scene::Scene(std::string name): name(name), gameObjects(), enabled(true), skybox(COLOR_BLUE,COLOR_CYAN)
{}

Scene::~Scene()
{
    for (GameObject *gameObject : this->gameObjects)
        delete gameObject;
    this->gameObjects.clear();
    //this->gameObjects.shrink_to_fit();
}

void Scene::DrawSky()
{
    Window* window = Window::GetCurrentWindow();
    if (skybox.top.A() == 0 && skybox.bottom.A() == 0)
        return;
    if (skybox.top == skybox.bottom)
        window->Clear(skybox.top );
    else
    {
        int heightInt = window->Height();
        if (skybox.height != heightInt)
            skybox.CalculateGradient(heightInt);

        fixed12_32 rgb[3] = {skybox.rgb_start[0], skybox.rgb_start[1], skybox.rgb_start[2]};
        for (int y = 0; y < heightInt; y++)
        {
            Color color(static_cast<int>(rgb[0]), static_cast<int>(rgb[1]), static_cast<int>(rgb[2]), 255);
            DrawHLine(y, color);
            rgb[0] += skybox.rgbStep[0];
            rgb[1] += skybox.rgbStep[1];
            rgb[2] += skybox.rgbStep[2];
        }
    }
}

GameObject* Scene::AddGameObject(GameObject *gameObject)
{
    if (gameObject == nullptr)
        return gameObject;
    // Check if the gameObject is already in the scene
    if (std::find(gameObjects.begin(), gameObjects.end(), gameObject) != gameObjects.end())
        return gameObject;
    gameObjects.push_back(gameObject);
    gameObject->scene = this;
    return gameObject;
}

void Scene::RemoveGameObject(GameObject *gameObject)
{
    gameObjectsToRemove.push_back(gameObject);
}

GameObject* Scene::FindGameObject(std::string name)
{
    for (GameObject *gameObject : this->gameObjects)
    {
        if (gameObject->name == name)
            return gameObject;
    }
    return nullptr;
}

void Scene::EndMainLoop()
{
    if (gameObjectsToRemove.size() > 0)
    {
        for (GameObject *gameObject : gameObjectsToRemove)
        {
            if (gameObject->scene == this)
            {
                gameObject->scene = nullptr;
                gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
                delete gameObject;
            }
        }
        gameObjectsToRemove.clear();
    }
}
