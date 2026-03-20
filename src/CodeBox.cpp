#include "CodeBox.h"

CodeBox::CodeBox(const sf::Font& font, float x, float y, float width, float height)
    : m_font(font), m_x(x), m_y(y), m_width(width), m_height(height)
{
    m_background.setPosition(x, y);
    m_background.setSize(sf::Vector2f(width, height));
    // Dark terminal-like background for code
    m_background.setFillColor(sf::Color(30, 30, 35, 240)); 

    m_titleText.setFont(font);
    m_titleText.setCharacterSize(16);
    m_titleText.setFillColor(sf::Color(200, 200, 200));
    m_titleText.setPosition(x + 15, y + 10);
    m_titleText.setString("Code & Process");

    m_highlightRect.setFillColor(sf::Color(100, 100, 100, 150));
    m_highlightRect.setSize(sf::Vector2f(width - 20, 22)); 
}

void CodeBox::setCode(const std::vector<std::string>& lines)
{
    m_lineTexts.clear();
    m_highlightedLine = -1;
    float startY = m_y + 40.0f;
    for (size_t i = 0; i < lines.size(); ++i) {
        sf::Text t;
        t.setFont(m_font);
        t.setString(lines[i]);
        t.setCharacterSize(15);
        t.setFillColor(sf::Color(220, 220, 220)); // Light Gray
        t.setPosition(m_x + 20.0f, startY + i * 22.0f);
        m_lineTexts.push_back(t);
    }
}

void CodeBox::highlightLine(int lineIndex)
{
    m_highlightedLine = lineIndex;
    if (lineIndex >= 0 && lineIndex < (int)m_lineTexts.size()) {
        m_highlightRect.setPosition(m_x + 10.0f, m_y + 40.0f + lineIndex * 22.0f);
    }
}

void CodeBox::setTitle(const std::string& title)
{
    m_titleText.setString(title);
}

void CodeBox::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    window.draw(m_titleText);
    if (m_highlightedLine >= 0 && m_highlightedLine < (int)m_lineTexts.size()) {
        window.draw(m_highlightRect);
    }
    for (const auto& t : m_lineTexts) {
        window.draw(t);
    }
}
