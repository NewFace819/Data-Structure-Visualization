#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class InputBox
{
public:
    InputBox(const sf::Font& font, float width = 120.0f, float height = 40.0f);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

    void setPosition(float x, float y);
    void show(const std::string& placeholder, std::function<void(std::string)> onSubmit, bool onlyNumbers = false);
    void hide();
    bool isVisible() const { return m_isVisible; }

private:
    sf::RectangleShape m_box;
    sf::Text m_text;
    sf::Text m_placeholderText;
    bool m_isVisible;
    bool m_isActive;
    std::string m_inputString;
    const sf::Font& m_font;
    std::function<void(std::string)> m_onSubmit;
    bool m_onlyNumbers;
};
