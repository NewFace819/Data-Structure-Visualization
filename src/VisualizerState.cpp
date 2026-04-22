#include "VisualizerState.h"
#include "MenuState.h"
#include "App.h"

VisualizerState::VisualizerState(App* app, const std::string& dataStructureName)
    : m_app(app), 
      m_dsName(dataStructureName),
      m_sidebar(app->getFont(), 720.0f),
      m_codeBox(app->getFont(), 260.0f, 500.0f, 1020.0f, 220.0f),
      m_backButton(1150, 10, 100, 40, "Back", app->getFont()) // Top right back button inside the Top Bar
{
}

void VisualizerState::init()
{
    // Create the top bar with a dark navy color for contrast against the light background
    m_topBar.setSize(sf::Vector2f(1280.0f, 60.0f));
    m_topBar.setFillColor(sf::Color(25, 30, 45)); 

    const sf::Font& font = m_app->getFont();
    m_titleText.setFont(font);
    m_titleText.setString(m_dsName + " Visualizer");
    m_titleText.setCharacterSize(28);
    m_titleText.setFillColor(sf::Color::White); // White title text on dark top bar
    m_titleText.setPosition(30.0f, 15.0f); // Left-aligned inside the top bar

    m_backButton.setCallback([this]() {
        m_app->changeState(std::make_unique<MenuState>(m_app));
    });

    // Hook up media controls
    m_sidebar.setPlayPauseCallback([this]() {
        m_isPaused = !m_isPaused;
        m_sidebar.setPlayButtonText(m_isPaused ? "Play" : "Pause");
    });

    m_sidebar.setBackCallback([this]() {
        m_isPaused = true;
        m_sidebar.setPlayButtonText("Play");
        stepBack();
    });

    m_sidebar.setNextCallback([this]() {
        m_isPaused = true;
        m_sidebar.setPlayButtonText("Play");
        stepForward();
    });

    // Initialize Notification UI
    m_notifyBg.setFillColor(sf::Color(231, 76, 60, 0)); // Soft Red, initially invisible
    m_notifyBg.setOutlineThickness(1.0f);
    m_notifyBg.setOutlineColor(sf::Color(255, 255, 255, 0));
    
    m_notifyText.setFont(m_app->getFont());
    m_notifyText.setCharacterSize(18);
    m_notifyText.setFillColor(sf::Color(255, 255, 255, 0));
}

void VisualizerState::handleInput(const sf::Event& event)
{
    sf::RenderWindow& window = m_app->getWindow();
    m_sidebar.handleEvent(event, window);
    m_backButton.handleEvent(event, window);
}

void VisualizerState::update(float dt)
{
    sf::RenderWindow& window = m_app->getWindow();
    // Query mouse position ONCE per frame and pass it down to avoid redundant calls
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos(static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y));
    m_sidebar.update(mousePos);
    m_backButton.update(mousePos);

    // Update Notification logic
    if (m_notifyTimer > 0) {
        m_notifyTimer -= dt;
        if (m_notifyTimer < 0.5f) { // Fade out in the last 0.5s
            m_notifyAlpha = (m_notifyTimer / 0.5f) * 255.0f;
        } else if (m_notifyAlpha < 255.0f) { // Fade in fast
            m_notifyAlpha = std::min(255.0f, m_notifyAlpha + dt * 1000.0f);
        }
    } else {
        m_notifyAlpha = std::max(0.0f, m_notifyAlpha - dt * 1000.0f);
    }

    uint8_t alpha = static_cast<uint8_t>(m_notifyAlpha);
    m_notifyBg.setFillColor(sf::Color(231, 76, 60, alpha));
    m_notifyBg.setOutlineColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha * 0.5f)));
    m_notifyText.setFillColor(sf::Color(255, 255, 255, alpha));
}

void VisualizerState::draw(sf::RenderWindow& window)
{
    // Basic visualization area is the rest of the window
    // Draw background grid or other visualizer elements here
    
    // UI elements
    window.draw(m_topBar);
    window.draw(m_titleText);
    m_backButton.draw(window);
    m_sidebar.draw(window);
    m_codeBox.draw(window);

    if (m_notifyAlpha > 0) {
        window.draw(m_notifyBg);
        window.draw(m_notifyText);
    }
}

void VisualizerState::showNotification(const std::string& message)
{
    m_notifyText.setString(message);
    m_notifyTimer = 3.0f; // Show for 3 seconds
    m_notifyAlpha = 1.0f; // Trigger fade-in

    sf::FloatRect textBounds = m_notifyText.getLocalBounds();
    float padding = 15.0f;
    float bgWidth = textBounds.width + padding * 2;
    float bgHeight = textBounds.height + padding * 2;

    // Position at top-right of CodeBox
    // CodeBox is at (260, 500, 1020, 220)
    float cbX = 260.0f, cbY = 500.0f, cbW = 1020.0f;
    float posX = cbX + cbW - bgWidth - 10.0f;
    float posY = cbY + 10.0f;

    m_notifyBg.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_notifyBg.setPosition(posX, posY);
    
    // Center text in background
    m_notifyText.setOrigin(textBounds.left, textBounds.top);
    m_notifyText.setPosition(posX + padding, posY + padding);
}
