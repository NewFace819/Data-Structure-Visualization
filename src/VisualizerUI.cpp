#include "VisualizerUI.h"
#include <sstream>

VisualizerUI::VisualizerUI()
{
    inputBuffer = "";
    isTyping = false;
}

void setupButton(ToolbarButton& button, const sf::Font& font, const std::string& text,
                 sf::Vector2f size, sf::Vector2f position)
{
    button.box.setSize(size);
    button.box.setPosition(position);
    button.box.setFillColor(sf::Color(70, 70, 90));
    button.box.setOutlineThickness(2.f);
    button.box.setOutlineColor(sf::Color::White);

    button.label.setFont(font);
    button.label.setCharacterSize(22);
    button.label.setString(text);
    button.label.setFillColor(sf::Color::White);

    sf::FloatRect bounds = button.label.getLocalBounds();
    button.label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    button.label.setPosition(
        position.x + size.x / 2.f,
        position.y + size.y / 2.f
    );
}

void setupUI(VisualizerUI& ui, const sf::Font& font, sf::Vector2u windowSize)
{
    float leftWidth = 320.f;
    float margin = 20.f;

    ui.leftPanel.setSize(sf::Vector2f(leftWidth, (float)windowSize.y));
    ui.leftPanel.setPosition(sf::Vector2f(0.f, 0.f));
    ui.leftPanel.setFillColor(sf::Color(35, 35, 50));

    ui.rightPanel.setSize(sf::Vector2f((float)windowSize.x - leftWidth, (float)windowSize.y));
    ui.rightPanel.setPosition(sf::Vector2f(leftWidth, 0.f));
    ui.rightPanel.setFillColor(sf::Color(245, 245, 245));

    ui.titleText.setFont(font);
    ui.titleText.setCharacterSize(28);
    ui.titleText.setString("2-3 Tree Visualizer");
    ui.titleText.setFillColor(sf::Color::White);
    ui.titleText.setPosition(sf::Vector2f(margin, 20.f));

    ui.inputLabel.setFont(font);
    ui.inputLabel.setCharacterSize(20);
    ui.inputLabel.setString("Input value");
    ui.inputLabel.setFillColor(sf::Color::White);
    ui.inputLabel.setPosition(sf::Vector2f(margin, 90.f));

    ui.inputBox.setSize(sf::Vector2f(leftWidth - 2 * margin, 45.f));
    ui.inputBox.setPosition(sf::Vector2f(margin, 125.f));
    ui.inputBox.setFillColor(sf::Color::White);
    ui.inputBox.setOutlineThickness(2.f);
    ui.inputBox.setOutlineColor(sf::Color::Black);

    ui.inputText.setFont(font);
    ui.inputText.setCharacterSize(22);
    ui.inputText.setFillColor(sf::Color::Black);
    ui.inputText.setPosition(sf::Vector2f(margin + 10.f, 132.f));

    setupButton(ui.insertButton, font, "Insert",
                sf::Vector2f(leftWidth - 2 * margin, 50.f),
                sf::Vector2f(margin, 190.f));

    setupButton(ui.resetButton, font, "Reset",
                sf::Vector2f(leftWidth - 2 * margin, 50.f),
                sf::Vector2f(margin, 255.f));

    ui.statusText.setFont(font);
    ui.statusText.setCharacterSize(18);
    ui.statusText.setFillColor(sf::Color(220, 220, 220));
    ui.statusText.setString("Ready");
    ui.statusText.setPosition(sf::Vector2f(margin, 330.f));

    ui.treePlaceholderText.setFont(font);
    ui.treePlaceholderText.setCharacterSize(28);
    ui.treePlaceholderText.setFillColor(sf::Color(80, 80, 80));
    ui.treePlaceholderText.setString("Tree is empty");

    sf::FloatRect bounds = ui.treePlaceholderText.getLocalBounds();
    ui.treePlaceholderText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    ui.treePlaceholderText.setPosition(
        sf::Vector2f(leftWidth + ((float)windowSize.x - leftWidth) / 2.f,
                     (float)windowSize.y / 2.f)
    );
}

void drawUI(sf::RenderWindow& window, VisualizerUI& ui)
{
    window.draw(ui.leftPanel);
    window.draw(ui.rightPanel);

    window.draw(ui.titleText);
    window.draw(ui.inputLabel);
    window.draw(ui.inputBox);
    window.draw(ui.inputText);

    window.draw(ui.insertButton.box);
    window.draw(ui.insertButton.label);

    window.draw(ui.resetButton.box);
    window.draw(ui.resetButton.label);

    window.draw(ui.statusText);
}

bool isInside(const sf::RectangleShape& box, sf::Vector2f mousePos)
{
    return box.getGlobalBounds().contains(mousePos);
}

bool isButtonClicked(const ToolbarButton& button, sf::Vector2f mousePos)
{
    return button.box.getGlobalBounds().contains(mousePos);
}

void handleTextEntered(VisualizerUI& ui, sf::Uint32 unicode)
{
    if (unicode == 8)
    {
        if (!ui.inputBuffer.empty())
        {
            ui.inputBuffer.pop_back();
        }
    }
    else if (unicode >= '0' && unicode <= '9')
    {
        ui.inputBuffer += (char)unicode;
    }

    ui.inputText.setString(ui.inputBuffer);
}

int getInputValue(const VisualizerUI& ui, bool& isValid)
{
    if (ui.inputBuffer.empty())
    {
        isValid = false;
        return 0;
    }

    std::stringstream ss(ui.inputBuffer);
    int value = 0;
    ss >> value;

    if (ss.fail())
    {
        isValid = false;
        return 0;
    }

    isValid = true;
    return value;
}

void setStatus(VisualizerUI& ui, const std::string& message)
{
    ui.statusText.setString(message);
}