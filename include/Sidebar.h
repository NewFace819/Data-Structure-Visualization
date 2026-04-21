#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"
#include "InputBox.h"
#include <vector>

class Sidebar
{
public:
    Sidebar(const sf::Font& font, float windowHeight);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void update(sf::Vector2f mousePos); // preferred: pass pre-computed mouse pos
    void draw(sf::RenderWindow& window) const;

    void setInitCallback(std::function<void(std::string)> cb);
    void setAddCallback(std::function<void(int)> cb);
    void setDeleteCallback(std::function<void(int)> cb);
    void setSearchCallback(std::function<void(int)> cb);
    void setUpdateCallback(std::function<void(std::string)> cb);
    void setRearrangeCallback(std::function<void()> cb);

    void setBackCallback(std::function<void()> cb);
    void setPlayPauseCallback(std::function<void()> cb);
    void setNextCallback(std::function<void()> cb);
    void setPlayButtonText(const std::string& text);

    float getSpeed() const { return m_sliderValue; }

private:
    sf::RectangleShape m_background;
    std::vector<Button> m_buttons;
    const sf::Font& m_font;
    
    sf::Text m_titleText;
    
    // Speed Slider
    sf::Text m_speedText;
    sf::RectangleShape m_sliderBar;
    sf::RectangleShape m_sliderKnob;

    bool m_isSliderDragging = false;
    float m_sliderValue = 0.1f; 
    void updateSliderPosition(float mouseX);

    InputBox m_inputBox;
    std::function<void(std::string)> m_initCb, m_updateCb;
    std::function<void(int)> m_addCb, m_delCb, m_searchCb;
    std::function<void()> m_rearrangeCb;
};
