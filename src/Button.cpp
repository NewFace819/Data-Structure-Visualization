#include "Button.h"
#include <cmath>

// Requirements: Minimalist dark colors, rounded corners
Button::Button(float x, float y, float width, float height, const std::string& text, const sf::Font& font)
    : m_isHovered(false)
{
    // Minimalist dark colors
    m_defaultColor = sf::Color(60, 60, 60); 
    m_hoverColor = sf::Color(80, 80, 80);   
    m_activeColor = sf::Color(40, 40, 40);  
    
    // Create completely rounded rectangle (pill shape) using ConvexShape
    float radius = std::min(width, height) / 2.0f;
    int pointsPerCorner = 15;
    m_shape.setPointCount(pointsPerCorner * 4);
    
    // Top-left
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = 3.141592654f + i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
        m_shape.setPoint(i, sf::Vector2f(radius + radius * std::cos(angle), radius + radius * std::sin(angle)));
    }
    // Top-right
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = 3.141592654f * 1.5f + i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
        m_shape.setPoint(pointsPerCorner + i, sf::Vector2f(width - radius + radius * std::cos(angle), radius + radius * std::sin(angle)));
    }
    // Bottom-right
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
        m_shape.setPoint(pointsPerCorner * 2 + i, sf::Vector2f(width - radius + radius * std::cos(angle), height - radius + radius * std::sin(angle)));
    }
    // Bottom-left
    for (int i = 0; i < pointsPerCorner; ++i) {
        float angle = 3.141592654f / 2.0f + i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
        m_shape.setPoint(pointsPerCorner * 3 + i, sf::Vector2f(radius + radius * std::cos(angle), height - radius + radius * std::sin(angle)));
    }

    m_shape.setPosition(x, y);
    m_shape.setFillColor(m_defaultColor);
    
    // Set up text
    m_text.setFont(font);
    m_text.setString(text);
    m_text.setCharacterSize(18); // Slightly smaller for minimalist look
    // Center text
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(x + width / 2.0f, y + height / 2.0f);
    m_text.setFillColor(sf::Color(245, 245, 245)); // Bright white text for contrast
}

void Button::setCallback(std::function<void()> callback)
{
    m_callback = callback;
}

void Button::setColors(const sf::Color& defaultC, const sf::Color& hoverC, const sf::Color& activeC)
{
    m_defaultColor = defaultC;
    m_hoverColor = hoverC;
    m_activeColor = activeC;
    m_shape.setFillColor(m_defaultColor);
}

void Button::setTextColor(const sf::Color& textC)
{
    m_text.setFillColor(textC);
}

void Button::setOutline(float thickness, const sf::Color& color)
{
    m_shape.setOutlineThickness(thickness);
    m_shape.setOutlineColor(color);
}

void Button::setText(const std::string& text)
{
    m_text.setString(text);
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        if (m_isHovered && m_callback)
        {
            m_shape.setFillColor(m_activeColor);
            m_callback();
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
    {
        if (m_isHovered)
        {
            m_shape.setFillColor(m_hoverColor);
        }
        else
        {
            m_shape.setFillColor(m_defaultColor);
        }
    }
}

void Button::update(const sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    update(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
}

void Button::update(sf::Vector2f mousePosF)
{
    m_isHovered = m_shape.getGlobalBounds().contains(mousePosF);
    
    // Hover effect
    if (m_isHovered && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        m_shape.setFillColor(m_hoverColor);
    }
    else if (!m_isHovered)
    {
        m_shape.setFillColor(m_defaultColor);
    }
}

void Button::draw(sf::RenderWindow& window) const
{
    window.draw(m_shape);
    window.draw(m_text);
}
