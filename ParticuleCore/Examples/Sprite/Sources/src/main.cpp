#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Core/System/Redefine.hpp>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    Asset<Sprite> p1;
    Asset<Sprite> p2;
    Asset<Sprite> p3;
    Asset<Sprite> p4;
    Sprite my_player;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnStart() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnStart()
{
    p1 = Asset<Sprite>(GetResourceID("assets/sprite.png:P1"));
    p2 = Asset<Sprite>(GetResourceID("assets/sprite.png:P2"));
    p3 = Asset<Sprite>(GetResourceID("assets/sprite.png:P3"));
    p4 = Asset<Sprite>(GetResourceID("assets/sprite.png:P4"));
    my_player = Sprite(GetResourceID("assets/sprite.png"), Rect(173, 3, 33, 31));

    //Vous pouvez utiliser l'une des deux méthodes suivantes pour charger les textures
    //AssetManager::Load charge l'asset en fonction de son ID
    //alors que AssetManager::LoadUsed charge tous les assets utilisés
    
    //AssetManager::Load<Texture>(GetResourceID("assets/sprite.png"));
    AssetManager::LoadUsed();
}

void BasicWindow::OnDraw()
{
    this->Clear();
    p1->Draw(0, 0);
    p2->Draw(0, 40);
    p3->Draw(0, 80);
    p4->Draw(0, 120);
    my_player.Draw(80, 0);
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