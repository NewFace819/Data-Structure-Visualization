#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>

#include "Button.h"

class GraphSidebar {
public:
    GraphSidebar(const sf::Font& font, float windowHeight);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window) const;

    void setBackCallback(std::function<void()> cb);
    void setPlayPauseCallback(std::function<void()> cb);
    void setNextCallback(std::function<void()> cb);
    void setPlayButtonText(const std::string& text);

    float getSpeed() const { return m_sliderValue; }

private:
    void updateSliderPosition(float mouseX);

    sf::RectangleShape m_background;
    sf::Text m_titleText;
    sf::Text m_speedText;
    sf::RectangleShape m_sliderBar;
    sf::RectangleShape m_sliderKnob;

    std::vector<Button> m_buttons;
    bool m_isSliderDragging = false;
    float m_sliderValue = 0.5f;
};
