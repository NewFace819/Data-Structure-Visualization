#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CodeBox
{
public:
    CodeBox(const sf::Font& font, float x, float y, float width, float height);

    void setCode(const std::vector<std::string>& lines);
    void highlightLine(int lineIndex);
    void setTitle(const std::string& title);

    void draw(sf::RenderWindow& window) const;

private:
    sf::RectangleShape m_background;
    sf::Text m_titleText;
    std::vector<sf::Text> m_lineTexts;
    sf::RectangleShape m_highlightRect;
    const sf::Font& m_font;
    float m_x, m_y, m_width, m_height;
    int m_highlightedLine = -1;
};
