#include "MenuState.h"
#include "VisualizerState.h"
#include "LinkedListGroup.h"
#include "TreeGroup.h"
#include "GraphGroup.h"
#include "HashTableGroup.h"
#include "App.h"
#include <iostream>

MenuState::MenuState(App* app) : m_app(app)
{
}

void MenuState::init()
{
    const sf::Font& font = m_app->getFont();
    
    // Top Bar #0B1120
    m_topBar.setSize(sf::Vector2f(1280.0f, 60.0f));
    m_topBar.setFillColor(sf::Color(11, 17, 32));
    m_topBar.setPosition(0.0f, 0.0f);

    m_topBarTitle.setFont(font);
    m_topBarTitle.setString("DS Visualizer");
    m_topBarTitle.setCharacterSize(20);
    m_topBarTitle.setFillColor(sf::Color::White);
    m_topBarTitle.setPosition(20.0f, 15.0f);

    m_topBarRightText.setFont(font);
    m_topBarRightText.setString("GitHub");
    m_topBarRightText.setCharacterSize(16);
    m_topBarRightText.setFillColor(sf::Color(148, 163, 184)); // Slate 400
    m_topBarRightText.setPosition(1200.0f, 20.0f);

    // Main Title
    m_titleText.setFont(font);
    m_titleText.setString("Data Structure Visualizer");
    m_titleText.setCharacterSize(52); // Large bold title
    m_titleText.setFillColor(sf::Color(15, 23, 42)); // Slate 900
    sf::FloatRect titleRect = m_titleText.getLocalBounds();
    m_titleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
    m_titleText.setPosition(1280 / 2.0f, 160.0f);

    // Subtitle
    m_subtitleText.setFont(font);
    m_subtitleText.setString("Interactive visualizations for common data structures.\nSelect a topic to explore.");
    m_subtitleText.setCharacterSize(20);
    m_subtitleText.setFillColor(sf::Color(71, 85, 105)); // Slate 600
    sf::FloatRect subRect = m_subtitleText.getLocalBounds();
    m_subtitleText.setOrigin(subRect.left + subRect.width / 2.0f, subRect.top + subRect.height / 2.0f);
    m_subtitleText.setPosition(1280 / 2.0f, 230.0f);

    // Create selection buttons as grid of Cards
    float cardW = 320.0f;
    float cardH = 120.0f;
    float gapX = 40.0f;
    float gapY = 40.0f;
    
    // Center the grid of 2 rows x 3 cols = but we only have 4 right now. Let's arrange as 2x2.
    // Total width for 2 columns = 2 * 320 + 40 = 680
    float startX = (1280.0f - (2 * cardW + gapX)) / 2.0f;
    float startY = 320.0f;

    auto setupCard = [&](Button& btn) {
        // White background cards, subtle borders
        btn.setColors(sf::Color(255, 255, 255), sf::Color(248, 250, 252), sf::Color(241, 245, 249));
        btn.setTextColor(sf::Color(15, 23, 42)); // Dark text
        btn.setOutline(2.0f, sf::Color(226, 232, 240)); // Slate 200 light border
    };

    m_buttons.emplace_back(startX, startY, cardW, cardH, "Linked List", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<LinkedListGroup>(m_app, "Linked List"));
    });

    m_buttons.emplace_back(startX + cardW + gapX, startY, cardW, cardH, "Tree Configurations", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<TreeGroup>(m_app));
    });

    m_buttons.emplace_back(startX, startY + cardH + gapY, cardW, cardH, "Graph Algorithms", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<GraphGroup>(m_app));
    });

    m_buttons.emplace_back(startX + cardW + gapX, startY + cardH + gapY, cardW, cardH, "Hash Table", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<HashTableGroup>(m_app));
    });
}

void MenuState::handleInput(const sf::Event& event)
{
    sf::RenderWindow& window = m_app->getWindow();
    for (auto& btn : m_buttons)
    {
        btn.handleEvent(event, window);
    }
}

void MenuState::update(float dt)
{
    sf::RenderWindow& window = m_app->getWindow();
    for (auto& btn : m_buttons)
    {
        btn.update(window);
    }
}

void MenuState::draw(sf::RenderWindow& window)
{
    window.draw(m_topBar);
    window.draw(m_topBarTitle);
    window.draw(m_topBarRightText);
    
    window.draw(m_titleText);
    window.draw(m_subtitleText);
    
    for (const auto& btn : m_buttons)
    {
        btn.draw(window);
    }
}
