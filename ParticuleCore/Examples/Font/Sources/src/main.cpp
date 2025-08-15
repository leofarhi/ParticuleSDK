#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Core/System/Redefine.hpp>
#include <Particule/Core/System/References/Resource.hpp>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    Font* font;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnStart() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnStart()
{
    font = AssetManager::Load<Font>(GetResourceID("assets/PressStart2P.ttf"));
}

void BasicWindow::OnDraw()
{
    this->Clear();
    font->DrawText("Hello, World!", 10, 10, Color::White, 16);
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Simple Font");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}