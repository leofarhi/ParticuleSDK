#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Core/System/AssetManager.hpp>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    Texture* ceil;
    Texture* floor;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnStart() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnStart()
{
    ceil = AssetManager::Load<Texture>(GetResourceID("assets/ceil.png"));
    floor = AssetManager::Load<Texture>(GetResourceID("assets/floor.png"));
}

void BasicWindow::OnDraw()
{
    this->Clear();
    ceil->Draw(0, 0);
    floor->Draw(0, 100);
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Load Textures");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}