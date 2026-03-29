#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct ToolbarButton
{
    sf::RectangleShape box;
    sf::Text label;
};

struct VisualizerUI
{
    sf::RectangleShape leftPanel;
    sf::RectangleShape rightPanel;

    sf::RectangleShape inputBox;
    sf::Text inputLabel;
    sf::Text inputText;

    ToolbarButton insertButton;
    ToolbarButton resetButton;

    sf::Text titleText;
    sf::Text statusText;
    sf::Text treePlaceholderText;

    std::string inputBuffer;
    bool isTyping;

    VisualizerUI();
};

void setupButton(ToolbarButton& button, const sf::Font& font, const std::string& text,
                 sf::Vector2f size, sf::Vector2f position);

void setupUI(VisualizerUI& ui, const sf::Font& font, sf::Vector2u windowSize);

void drawUI(sf::RenderWindow& window, VisualizerUI& ui);

bool isInside(const sf::RectangleShape& box, sf::Vector2f mousePos);
bool isButtonClicked(const ToolbarButton& button, sf::Vector2f mousePos);

void handleTextEntered(VisualizerUI& ui, sf::Uint32 unicode);
int getInputValue(const VisualizerUI& ui, bool& isValid);
void setStatus(VisualizerUI& ui, const std::string& message);