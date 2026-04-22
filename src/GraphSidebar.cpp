#include "GraphSidebar.h"

#include <algorithm>

GraphSidebar::GraphSidebar(const sf::Font& font, float windowHeight) {
    const float panelWidth = 260.0f;
    const float panelHeight = windowHeight - 60.0f;
    const float panelX = 0.0f;
    const float panelY = 60.0f;

    m_background.setSize(sf::Vector2f(panelWidth, panelHeight));
    m_background.setFillColor(sf::Color(245, 245, 245, 230));
    m_background.setPosition(panelX, panelY);

    m_titleText.setFont(font);
    m_titleText.setString("Graph Controls");
    m_titleText.setCharacterSize(18);
    m_titleText.setFillColor(sf::Color(100, 100, 100));
    m_titleText.setPosition(25.0f, panelY + 20.0f);

    const float btnX = 25.0f;
    const float btnY = panelY + 70.0f;
    const float btnW = 210.0f;
    const float btnH = 45.0f;
    const float gap = 16.0f;

    m_buttons.emplace_back(btnX, btnY, btnW, btnH, "Back", font);
    m_buttons.emplace_back(btnX, btnY + (btnH + gap), btnW, btnH, "Start", font);
    m_buttons.emplace_back(btnX, btnY + 2.0f * (btnH + gap), btnW, btnH, "Next", font);

    const float sliderY = btnY + 3.0f * (btnH + gap) + 28.0f;

    m_speedText.setFont(font);
    m_speedText.setString("Speed");
    m_speedText.setCharacterSize(18);
    m_speedText.setFillColor(sf::Color(40, 40, 40));
    m_speedText.setPosition(btnX, sliderY);

    m_sliderBar.setSize(sf::Vector2f(btnW, 6.0f));
    m_sliderBar.setFillColor(sf::Color(200, 200, 200));
    m_sliderBar.setPosition(btnX, sliderY + 35.0f);

    m_sliderKnob.setSize(sf::Vector2f(16.0f, 24.0f));
    m_sliderKnob.setFillColor(sf::Color(52, 152, 219));
    m_sliderKnob.setPosition(btnX + btnW * m_sliderValue - 8.0f, sliderY + 26.0f);
}

void GraphSidebar::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    for (auto& button : m_buttons) {
        button.handleEvent(event, window);
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                                    static_cast<float>(event.mouseButton.y));
        sf::FloatRect knobBounds = m_sliderKnob.getGlobalBounds();
        knobBounds.left -= 5.0f;
        knobBounds.width += 10.0f;
        knobBounds.top -= 5.0f;
        knobBounds.height += 10.0f;

        if (knobBounds.contains(mousePos)) {
            m_isSliderDragging = true;
        } else if (m_sliderBar.getGlobalBounds().contains(mousePos)) {
            m_isSliderDragging = true;
            updateSliderPosition(mousePos.x);
        }
    } else if (event.type == sf::Event::MouseButtonReleased &&
               event.mouseButton.button == sf::Mouse::Left) {
        m_isSliderDragging = false;
    } else if (event.type == sf::Event::MouseMoved && m_isSliderDragging) {
        updateSliderPosition(static_cast<float>(event.mouseMove.x));
    }
}

void GraphSidebar::update(const sf::RenderWindow& window) {
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    update(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
}

void GraphSidebar::update(sf::Vector2f mousePos) {
    for (auto& button : m_buttons) {
        button.update(mousePos);
    }
}

void GraphSidebar::draw(sf::RenderWindow& window) const {
    window.draw(m_background);
    window.draw(m_titleText);

    for (const auto& button : m_buttons) {
        button.draw(window);
    }

    window.draw(m_speedText);
    window.draw(m_sliderBar);
    window.draw(m_sliderKnob);
}

void GraphSidebar::setBackCallback(std::function<void()> cb) {
    if (m_buttons.size() > 0) {
        m_buttons[0].setCallback(cb);
    }
}

void GraphSidebar::setPlayPauseCallback(std::function<void()> cb) {
    if (m_buttons.size() > 1) {
        m_buttons[1].setCallback(cb);
    }
}

void GraphSidebar::setNextCallback(std::function<void()> cb) {
    if (m_buttons.size() > 2) {
        m_buttons[2].setCallback(cb);
    }
}

void GraphSidebar::setPlayButtonText(const std::string& text) {
    if (m_buttons.size() > 1) {
        m_buttons[1].setText(text);
    }
}

void GraphSidebar::updateSliderPosition(float mouseX) {
    const float barLeft = m_sliderBar.getPosition().x;
    const float barRight = barLeft + m_sliderBar.getSize().x;
    const float clampedX = std::max(barLeft, std::min(mouseX, barRight));

    m_sliderKnob.setPosition(clampedX - m_sliderKnob.getSize().x / 2.0f,
                             m_sliderKnob.getPosition().y);
    m_sliderValue = (clampedX - barLeft) / m_sliderBar.getSize().x;
}
