#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Core/System/Redefine.hpp>
#include <Particule/Core/System/References/Resource.hpp>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    Asset<Texture> ceil;
    Asset<Texture> floor;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnStart() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnStart()
{
    ceil = Asset<Texture>(GetResourceID("assets/ceil.png"));
    floor = Asset<Texture>(GetResourceID("assets/floor.png"));
    AssetManager::Load<Texture>(GetResourceID("assets/ceil.png"));
    AssetManager::Load<Texture>(GetResourceID("assets/floor.png"));
}

void BasicWindow::OnDraw()
{
    this->Clear();
    ceil->Draw(0, 0);
    floor->DrawSize(0, 65, 200, 100);
    ceil->DrawSubSize(200, 65, 200, 100, Rect(0, 0, 20, 20));
    floor->DrawColor(0, 165, Color(255, 0, 0));
    ceil->DrawSubColor(200, 165, Rect(0, 0, 20, 20), Color(0, 255, 0));
    floor->DrawSizeColor(0, 265, 200, 100, Color(0, 0, 255));
    ceil->DrawSubSizeColor(200, 265, 200, 100, Rect(0, 0, 20, 20), Color(255, 255, 0));
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Draw Textures");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}