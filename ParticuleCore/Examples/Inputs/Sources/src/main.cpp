#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Core/System/Redefine.hpp>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    bool pressed;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnUpdate() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnUpdate()
{
    pressed = GetInput("OK").IsKeyPressed();
}

void BasicWindow::OnDraw()
{
    this->Clear();
    if (pressed)
    {
        DrawRectFilled(0, 0, 100, 100, Color::Green);
    }
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Simple Input");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}