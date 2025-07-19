#include <Particule/Core/ParticuleCore.hpp>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnDraw()
{
    this->Clear();
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Simple Window");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}