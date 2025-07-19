#include <Particule/Core/ParticuleCore.hpp>
#include <Particule/Core/System/Redefine.hpp>
#include <string>

using namespace Particule::Core;

class BasicWindow: public Window
{
public:
    Font* font;
    std::string text;
    BasicWindow(int width, int height, const std::string& title);

    ~BasicWindow() override;
    void OnStart() override;
    void OnDraw() override;
};

BasicWindow::BasicWindow(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow::~BasicWindow(){}

void BasicWindow::OnStart()
{
    File* file = File::Open("text.txt", FileMode::Read, Endian::LittleEndian);
    if (!file)
    {
        File::OpenWith("text.txt", FileMode::Write, Endian::LittleEndian, [&](File* file) {
            file->Write<char>("Hello, World!", 13);
        });
    }
    file = File::Open("text.txt", FileMode::Read, Endian::LittleEndian);
    if (file) {
        char buffer[14] = {0}; // 13 characters + null terminator
        file->Read(buffer, 13);
        text = std::string(buffer);
        File::Close(file);
    } else {
        text = "Failed to open file.";
    }
    font = AssetManager::Load<Font>(GetResourceID("assets/PressStart2P.ttf"));
}

void BasicWindow::OnDraw()
{
    this->Clear();
    font->DrawText(text, 10, 10, Color::White, 16);
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow* window = new BasicWindow(396*2, 224*2, "Simple IO");
    window->SetResizable(true);
    app->AddWindow(window);
    return EXIT_SUCCESS;
}