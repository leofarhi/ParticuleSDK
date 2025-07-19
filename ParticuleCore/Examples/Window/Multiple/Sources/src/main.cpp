#include <Particule/Core/ParticuleCore.hpp>

using namespace Particule::Core;

class BasicWindow1: public Window
{
public:
    BasicWindow1(int width, int height, const std::string& title);

    ~BasicWindow1() override;
    void OnDraw() override;
};

BasicWindow1::BasicWindow1(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow1::~BasicWindow1(){}

void BasicWindow1::OnDraw()
{
    this->Clear();
    DrawRectFilled(0, 0, 100, 100, Color::Red);
    this->Display();
}


class BasicWindow2: public Window
{
public:
    BasicWindow2(int width, int height, const std::string& title);

    ~BasicWindow2() override;
    void OnDraw() override;
};

BasicWindow2::BasicWindow2(int width, int height, const std::string& title) : Window(width, height, title){}

BasicWindow2::~BasicWindow2(){}

void BasicWindow2::OnDraw()
{
    this->Clear();
    DrawRectFilled(100, 0, 100, 100, Color::Blue);
    this->Display();
}

int MainApp(App* app, int argc, char* argv[])
{
    (void)argc; (void)argv;
    BasicWindow1* window1 = new BasicWindow1(396*2, 224*2, "Window 1");
    window1->SetResizable(true);
    app->AddWindow(window1);
    BasicWindow2* window2 = new BasicWindow2(396*2, 224*2, "Window 2");
    window2->SetResizable(true);
    app->AddWindow(window2);
    return EXIT_SUCCESS;
}