#include "InputBox.h"
#include <iostream>

InputBox::InputBox(const sf::Font& font, float width, float height)
    : m_isVisible(false), m_isActive(false), m_font(font)
{
    m_box.setSize(sf::Vector2f(width, height));
    m_box.setFillColor(sf::Color::White);
    m_box.setOutlineColor(sf::Color(200, 200, 200));
    m_box.setOutlineThickness(2.0f);

    m_text.setFont(font);
    m_text.setCharacterSize(18);
    m_text.setFillColor(sf::Color::Black);

    m_placeholderText.setFont(font);
    m_placeholderText.setCharacterSize(14);
    m_placeholderText.setFillColor(sf::Color(100, 100, 100)); // Dark Gray text for label
}

void InputBox::setPosition(float x, float y)
{
    m_box.setPosition(x, y);
    m_text.setPosition(x + 10, y + 10); // padding
    m_placeholderText.setPosition(x, y - 22); // Label above the box
}

void InputBox::show(const std::string& placeholder, std::function<void(std::string)> onSubmit, bool onlyNumbers)
{
    m_isVisible = true;
    m_isActive = true; // Auto-focus when shown
    m_inputString = "";
    m_onlyNumbers = onlyNumbers;
    m_placeholderText.setString(placeholder);
    m_text.setString(m_inputString);
    m_onSubmit = std::move(onSubmit);
    // Highlight box
    m_box.setOutlineColor(sf::Color(52, 152, 219)); // Blue highlight
}

void InputBox::hide()
{
    m_isVisible = false;
    m_isActive = false;
}

void InputBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (!m_isVisible) return;

    if (event.type == sf::Event::MouseButtonPressed)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = m_box.getGlobalBounds();
        if (bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            m_isActive = true;
            m_box.setOutlineColor(sf::Color(52, 152, 219));
        } else {
            // Clicked outside, hide it
            hide();
        }
    }

    if (m_isActive && event.type == sf::Event::TextEntered)
    {
        // Handle backspace
        if (event.text.unicode == 8 && !m_inputString.empty())
        {
            m_inputString.pop_back();
        }
        // Handle enter/return
        else if (event.text.unicode == 13 || event.text.unicode == 10)
        {
            if (m_onSubmit) // Send even if empty (for empty initialization)
            {
                m_onSubmit(m_inputString);
            }
            hide(); // Hide upon submission
        }
        else
        {
            char c = static_cast<char>(event.text.unicode);
            if (m_onlyNumbers) {
                // Handle digits and negative sign
                if ((c >= '0' && c <= '9') || (c == '-' && m_inputString.empty()))
                {
                    if (m_inputString.length() < 9) m_inputString += c;
                }
            } else {
                // Allow regular characters (letters, numbers, comma, space, period)
                if (c >= 32 && c <= 126 && m_inputString.length() < 24) {
                    m_inputString += c;
                }
            }
        }
        m_text.setString(m_inputString);
    }
}

void InputBox::update(const sf::RenderWindow& window)
{
    // Keep it simple
}

void InputBox::draw(sf::RenderWindow& window) const
{
    if (!m_isVisible) return;
    window.draw(m_placeholderText);
    window.draw(m_box);
    window.draw(m_text);
}
