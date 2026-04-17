#include "MenuState.h"
#include "VisualizerState.h"
#include "LinkedListGroup.h"
#include "TreeGroup.h"
#include "GraphGroup.h"
#include "HashTableGroup.h"
#include "heapgroup.h"
#include "App.h"
#include <iostream>
#include <algorithm>

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
    
    // We have 5 buttons now, let's arrange them in a 3-column layout.
    // Total width for 3 columns: 3 * 320 + 2 * 40 = 1040
    float startX = (1280.0f - 1040.0f) / 2.0f;
    float startY = 320.0f;

    auto setupCard = [&](Button& btn) {
        // White background cards, subtle borders
        btn.setColors(sf::Color(255, 255, 255), sf::Color(248, 250, 252), sf::Color(241, 245, 249));
        btn.setTextColor(sf::Color(15, 23, 42)); // Dark text
        btn.setOutline(2.0f, sf::Color(226, 232, 240)); // Slate 200 light border
    };

    // Row 1, Col 1
    m_buttons.emplace_back(startX, startY, cardW, cardH, "Linked List", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<LinkedListGroup>(m_app, "Linked List"));
    });

    // Row 1, Col 2
    m_buttons.emplace_back(startX + cardW + gapX, startY, cardW, cardH, "Tree Configs", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<TreeGroup>(m_app));
    });

    // Row 1, Col 3
    m_buttons.emplace_back(startX + 2 * (cardW + gapX), startY, cardW, cardH, "Graph Algos", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<GraphGroup>(m_app));
    });

    // Row 2
    // Center the 2 items in Row 2 by offsetting startX
    float row2StartX = startX + (cardW + gapX) / 2.0f;

    // Row 2, Col 1
    m_buttons.emplace_back(row2StartX, startY + cardH + gapY, cardW, cardH, "Hash Table", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<HashTableGroup>(m_app));
    });

    // Row 2, Col 2
    m_buttons.emplace_back(row2StartX + cardW + gapX, startY + cardH + gapY, cardW, cardH, "Heap", font);
    setupCard(m_buttons.back());
    m_buttons.back().setCallback([this]() {
        m_app->changeState(std::make_unique<HeapGroup>(m_app));
    });

    if (m_llLogoTex.loadFromFile("assets/Linked_List_Logo.png")) {
        m_llLogoTex.setSmooth(true);
        m_llLogoSprite.setTexture(m_llLogoTex);
        sf::Vector2u size = m_llLogoTex.getSize();
        
        float scaleY = 280.0f / size.y; // Full card height minus margins
        float scaleX = 512.0f / size.x; // Full card width minus margins
        float scale = std::min(scaleX, scaleY);
        
        m_llLogoSprite.setScale(scale, scale);
        m_llLogoSprite.setOrigin(static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f);
        m_hasLogo = true;

        // Hide regular text because the logo already has text in it
        if (!m_buttons.empty()) {
            m_buttons[0].setText(""); 
        }
    }
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

namespace {
    void drawLinkedListLogo(sf::RenderWindow& window, sf::Vector2f center) {
        sf::RectangleShape box(sf::Vector2f(24.0f, 16.0f));
        box.setOrigin(12.0f, 8.0f);
        box.setOutlineThickness(1.5f);
        box.setOutlineColor(sf::Color(15, 23, 42)); 
        box.setFillColor(sf::Color::White);

        sf::RectangleShape ptrBg(sf::Vector2f(8.0f, 16.0f)); 
        ptrBg.setOrigin(4.0f, 8.0f);
        ptrBg.setFillColor(sf::Color(226, 232, 240));

        sf::CircleShape dot(1.5f);
        dot.setFillColor(sf::Color(15, 23, 42));
        dot.setOrigin(1.5f, 1.5f);

        auto drawNode = [&](float cx, float cy) {
            box.setPosition(cx, cy);  window.draw(box);
            ptrBg.setPosition(cx + 8.0f, cy); window.draw(ptrBg);
            dot.setPosition(cx + 8.0f, cy);   window.draw(dot);
        };

        drawNode(center.x - 45.0f, center.y);
        drawNode(center.x, center.y);
        drawNode(center.x + 45.0f, center.y);

        auto drawArr = [&](float cx) {
            sf::RectangleShape arrLine(sf::Vector2f(21.0f, 1.5f));
            arrLine.setPosition(cx + 8.0f, center.y - 0.75f);
            arrLine.setFillColor(sf::Color(15, 23, 42));
            window.draw(arrLine);

            sf::ConvexShape ah;
            ah.setPointCount(3);
            ah.setPoint(0, sf::Vector2f(0.0f, -3.0f));
            ah.setPoint(1, sf::Vector2f(4.0f, 0.0f));
            ah.setPoint(2, sf::Vector2f(0.0f, 3.0f));
            ah.setFillColor(sf::Color(15, 23, 42));
            ah.setPosition(cx + 29.0f, center.y);
            window.draw(ah);
        };
        drawArr(center.x - 45.0f);
        drawArr(center.x);
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
    
    // Draw the Linked List icon over the first button
    float startX = (1280.0f - 1040.0f) / 2.0f;
    float startY = 320.0f;
    if (m_hasLogo) {
        // Centered perfectly inside the empty card since text is removed
        m_llLogoSprite.setPosition(startX + 160.0f, startY + 60.0f);
        window.draw(m_llLogoSprite);
    } else {
        drawLinkedListLogo(window, sf::Vector2f(startX + 160.0f, startY + 26.0f));
    }
}
