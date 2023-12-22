// Mateusz Ka³wa

// SFML
#include "SFML/Graphics.hpp"

// ImGUI
#include "imgui.h"
#include "imgui-SFML.h"

#include "VisualBinaryTree.h"
#include <iostream>
#include <sstream>

sf::CircleShape circle;
sf::Text text;
sf::Vertex line[2];

float viewSize = 150.f;
float nextViewSize = 0.f;
int maxDepth = 0;

constexpr float offset_x_multi = 15.f;
constexpr float offset_y = 80.f;

constexpr float circle_size = 22.f;

float getSizeX()
{
    float sum = 0;
    for (int n = maxDepth; n >= 1; --n)
        sum += std::pow(static_cast<float>(n), 2.f) * offset_x_multi;
    return sum * 2;
}

float getSizeY()
{
    return static_cast<float>(maxDepth) * offset_y;
}

void drawCircle(sf::RenderWindow& window, const sf::Vector2f& position, const int& key)
{
    circle.setPosition(position);
    circle.setFillColor(sf::Color::White);
    circle.setRadius(circle_size);
    circle.setOrigin(circle_size, circle_size);
    window.draw(circle);

    circle.setPosition(position);
    circle.setFillColor(sf::Color::Black);
    circle.setRadius(circle_size - 2.f);
    circle.setOrigin(circle_size - 2.f, circle_size - 2.f);
    window.draw(circle);

    std::string strText = std::to_string(key);
    if (strText.size() < 2)
        strText = "0" + strText;
    text.setString(strText);
    text.setPosition(sf::Vector2f(position.x - 13, position.y - 15));
    window.draw(text);
}

bool containsNumber(const char* input)
{
    while (*input) {
        if (std::isdigit(*input)) {
            return true;
        }
        ++input;
    }
    return false;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1100, 1100), "Window Title");
    ImGui::SFML::Init(window);
    sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(viewSize, viewSize));

    window.setView(view);

    sf::Font font;
    if (!font.loadFromFile("res/font.ttf")) {
        std::cerr << "Cannot load font.ttf" << std::endl;
        exit(EXIT_FAILURE);
    }

    sf::Clock clock;
    sf::Clock imguiClock;
    float deltatime = 0.f;

    sf::Text insertText;
    insertText.setFont(font);
    insertText.setCharacterSize(78);
    insertText.setFillColor(sf::Color::White);
    insertText.setPosition(15.f, -15.f);

    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    circle.setFillColor(sf::Color::White);
    circle.setRadius(10.f);
    circle.setOrigin(10.f, 10.f);

    vbt::VisualBinaryTree<int> vbt(font);
    /*vbt.insert(50);
    vbt.insert(25);
    vbt.insert(75);
    vbt.insert(10);
    vbt.insert(40);
    vbt.insert(60);
    vbt.insert(90);
    vbt.insert(5);
    vbt.insert(15);
    vbt.insert(30);
    vbt.insert(45);
    vbt.insert(55);
    vbt.insert(65);
    vbt.insert(80);
    vbt.insert(95);*/

    std::string getLineStr;
    std::string word;

    std::string insertWord;
    int insertKey = 0;

    static char inputCharText[3] = "";

    deltatime = clock.restart().asSeconds();

    while (window.isOpen())
    {
        deltatime = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;

                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        ImGui::SFML::Update(window, imguiClock.restart());

        ImGui::Begin("Visual Binary Tree");

        ImGui::InputText("Insert:", inputCharText, IM_ARRAYSIZE(inputCharText));
        if (ImGui::Button("Insert"))
        {
            if (containsNumber(inputCharText))
                vbt.insert(std::stoi(inputCharText));
        }

        ImGui::Text("Inorder:");
        ImGui::Text(vbt.inorder().c_str());
        ImGui::Text("Preorder:");
        ImGui::Text(vbt.preorder().c_str());
        ImGui::Text("Postorder:");
        ImGui::Text(vbt.postorder().c_str());
        ImGui::End();

        vbt.update(deltatime);

        maxDepth = vbt.depth();

        nextViewSize = std::max(getSizeX(), getSizeY()) + 100.f;
        viewSize += (nextViewSize - viewSize) * deltatime;
        view.setSize(sf::Vector2f(viewSize, viewSize));
        window.setView(view);

        window.clear(sf::Color(18, 33, 43));
        drawCircle(window, sf::Vector2f(circle_size - (viewSize / 2), circle_size - (viewSize / 2)), insertKey);
        vbt.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
