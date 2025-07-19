#include <Particule/Core/ParticuleCore.hpp>
using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    Texture* uv_map;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnStart() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){
    delete uv_map;
}

void BasicWindow::OnStart()
{
    uv_map = Texture::Create(200, 200);
    for (int x = 0; x < 200; x++)
    {
        for (int y = 0; y < 200; y++)
        {
            uv_map->SetPixel(x, y, Color(x*255.0f/200.0f, y*255.0f/200.0f, 0, 255));
        }
    }
    uv_map->UpdateTexture();
}

void BasicWindow::OnDraw()
{
    this->Clear();
    uv_map->Draw(0, 0);
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Create Texture");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}