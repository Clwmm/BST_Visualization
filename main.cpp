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
sf::Vector2f viewPosition(0.f, 0.f);
sf::Vector2f nextViewPosition(0.f, 0.f);
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

bool validateText(const char* input)
{
    if (*input == '\0')
        return false;
    while (*input) {
        if (!std::isdigit(*input)) {
            return false;
        }
        ++input;
    }
    return true;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1100, 1100), "Window Title");
    
    ImGui::SFML::Init(window);

    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.f;

    sf::View view(viewPosition, sf::Vector2f(viewSize, viewSize));
    window.setView(view);

    sf::Font font;
    if (!font.loadFromFile("res/font.ttf")) {
        std::cerr << "Cannot load font.ttf" << std::endl;
        exit(EXIT_FAILURE);
    }

    sf::Clock clock;
    sf::Clock imguiClock;
    std::string alert = "";
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
    vbt.insert(50);
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
    vbt.insert(95);


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
        ImGui::InputText("##", inputCharText, IM_ARRAYSIZE(inputCharText));
        ImGui::Columns(4);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.98f, 1.0f, 1.0f));
        if (ImGui::Button("Insert"))
        {
            if (validateText(inputCharText))
            {
                vbt.insert(std::stoi(inputCharText));
                alert = "Added: " + std::string(inputCharText);
            }
                
        }
            
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Button("Delete"))
        {
            if (validateText(inputCharText))
            {
                if (vbt.remove(std::stoi(inputCharText)))
                    alert = "Removed: " + std::string(inputCharText);
                else
                    alert = "Not found: " + std::string(inputCharText);
            }
        }
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        if (ImGui::Button("Search"))
        {
            if (validateText(inputCharText))
            {
                vbt.search(std::stoi(inputCharText));
                /*if (vbt.search(std::stoi(inputCharText)))
                    alert = "Found: " + std::string(inputCharText);
                else
                    alert = "Not found: " + std::string(inputCharText);*/
            }
        }
        ImGui::PopStyleColor();
        ImGui::NextColumn();
        if (ImGui::Button("Clear"))
        {
            vbt.clear();
            alert = "Tree cleared";
        }
        ImGui::Columns(1);
            
        ImGui::Text(alert.c_str());


        std::string temp;
        temp = "Size: " + std::to_string(vbt.getSize());
        ImGui::Text(temp.c_str());
        temp = "Depth: " + std::to_string(vbt.depth());
        ImGui::Text(temp.c_str());
        temp = "Minimum: " + std::to_string(vbt.minimum());
        ImGui::Text(temp.c_str());
        temp = "Maximum: " + std::to_string(vbt.maximum());
        ImGui::Text(temp.c_str());
        ImGui::Text("Inorder:");
        ImGui::Text(vbt.inorder().c_str());
        ImGui::End();

        vbt.update(deltatime, alert);

        maxDepth = vbt.depth();

        if (vbt.getRoot())
        {
            sf::Vector2f dir = vbt.getRoot()->position - viewPosition;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist > 1.0f)
            {
                dir /= dist;
                viewPosition.x += dir.x * vbt::MOVE_SPEED * deltatime;
                viewPosition.y += dir.y * vbt::MOVE_SPEED * deltatime;
            }

            view.setCenter(viewPosition);
        }
        
        nextViewSize = std::max(getSizeX(), getSizeY()) + 100.f;
        viewSize += (nextViewSize - viewSize) * deltatime;
        view.setSize(sf::Vector2f(viewSize, viewSize));
        window.setView(view);

        window.clear(sf::Color(18, 33, 43));
        vbt.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
