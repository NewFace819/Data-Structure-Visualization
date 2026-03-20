#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button
{
public:
    Button(float x, float y, float width, float height, const std::string& text, const sf::Font& font);
    
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void update(sf::Vector2f mousePos); // preferred: pass pre-computed mouse pos
    void draw(sf::RenderWindow& window) const;

    void setCallback(std::function<void()> callback);
    void setColors(const sf::Color& defaultC, const sf::Color& hoverC, const sf::Color& activeC);
    void setTextColor(const sf::Color& textC);
    void setOutline(float thickness, const sf::Color& color);
    void setText(const std::string& text);

private:
    sf::ConvexShape m_shape;
    sf::Text m_text;
    
    sf::Color m_defaultColor;
    sf::Color m_hoverColor;
    sf::Color m_activeColor;

    std::function<void()> m_callback;
    bool m_isHovered;
};
