#include "VisualizerState.h"

#include "App.h"
#include "MenuState.h"

namespace {
constexpr float kTopBarHeight = 60.0f;
constexpr float kSidebarWidth = 260.0f;
constexpr float kCodeBoxTop = 500.0f;
constexpr float kCanvasPadding = 30.0f;
}  // namespace

VisualizerState::VisualizerState(App* app, const std::string& dataStructureName)
    : m_app(app),
      m_dsName(dataStructureName),
      m_sidebar(app->getFont(), (float)app->getWindow().getSize().y),
      m_codeBox(app->getFont(), kSidebarWidth, kCodeBoxTop,
                (float)app->getWindow().getSize().x - kSidebarWidth,
                (float)app->getWindow().getSize().y - kCodeBoxTop),
      m_backButton((float)app->getWindow().getSize().x - 130.0f, 10.0f, 100.0f, 40.0f, "Back",
                   app->getFont()) {
    const sf::Vector2u windowSize = m_app->getWindow().getSize();
    m_codeBoxBounds =
        sf::FloatRect(kSidebarWidth, kCodeBoxTop, static_cast<float>(windowSize.x) - kSidebarWidth,
                      static_cast<float>(windowSize.y) - kCodeBoxTop);
    m_visualizationBounds =
        sf::FloatRect(kSidebarWidth + kCanvasPadding, kTopBarHeight + kCanvasPadding,
                      static_cast<float>(windowSize.x) - kSidebarWidth - 2.0f * kCanvasPadding,
                      kCodeBoxTop - kTopBarHeight - 2.0f * kCanvasPadding);
}

void VisualizerState::init() {
    sf::Vector2u size = m_app->getWindow().getSize();
    float winW = (float)size.x;
    float winH = (float)size.y;

    m_topBar.setSize(sf::Vector2f(winW, 60.0f));
    m_topBar.setFillColor(sf::Color(25, 30, 45));

    const sf::Font& font = m_app->getFont();
    m_titleText.setFont(font);
    m_titleText.setString(m_dsName + " Visualizer");
    m_titleText.setCharacterSize(28);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition(30.0f, 15.0f);

    m_backButton.setCallback([this]() { m_app->changeState(std::make_unique<MenuState>(m_app)); });

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

    m_notifyBg.setFillColor(sf::Color(231, 76, 60, 0));
    m_notifyBg.setOutlineThickness(1.0f);
    m_notifyBg.setOutlineColor(sf::Color(255, 255, 255, 0));

    m_notifyText.setFont(font);
    m_notifyText.setCharacterSize(18);
    m_notifyText.setFillColor(sf::Color(255, 255, 255, 0));

    (void)winH;
}

void VisualizerState::handleInput(const sf::Event& event) {
    sf::RenderWindow& window = m_app->getWindow();
    m_sidebar.handleEvent(event, window);
    m_backButton.handleEvent(event, window);
}

void VisualizerState::update(float dt) {
    sf::RenderWindow& window = m_app->getWindow();

    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos((float)mousePixel.x, (float)mousePixel.y);

    m_sidebar.update(mousePos);
    m_backButton.update(mousePos);

    if (m_notifyTimer > 0.0f) {
        m_notifyTimer -= dt;

        if (m_notifyTimer < 0.5f) {
            m_notifyAlpha = (m_notifyTimer / 0.5f) * 255.0f;
        } else if (m_notifyAlpha < 255.0f) {
            m_notifyAlpha = std::min(255.0f, m_notifyAlpha + dt * 1000.0f);
        }
    } else {
        m_notifyAlpha = std::max(0.0f, m_notifyAlpha - dt * 1000.0f);
    }

    uint8_t alpha = (uint8_t)m_notifyAlpha;
    sf::Color c = m_notifyColor;
    c.a = alpha;
    m_notifyBg.setFillColor(c);
    m_notifyBg.setOutlineColor(sf::Color(255, 255, 255, (uint8_t)(alpha * 0.5f)));
    m_notifyText.setFillColor(sf::Color(255, 255, 255, alpha));
}

void VisualizerState::draw(sf::RenderWindow& window) {
    drawChrome(window);
}

void VisualizerState::drawChrome(sf::RenderWindow& window) {
    window.draw(m_topBar);
    window.draw(m_titleText);
    m_backButton.draw(window);
    m_sidebar.draw(window);
    m_codeBox.draw(window);

    if (m_notifyAlpha > 0.0f) {
        window.draw(m_notifyBg);
        window.draw(m_notifyText);
    }
}

void VisualizerState::showNotification(const std::string& message, sf::Color color) {
    m_notifyColor = color;
    m_notifyText.setString(message);
    m_notifyTimer = 3.0f;
    m_notifyAlpha = 1.0f;

    sf::FloatRect textBounds = m_notifyText.getLocalBounds();
    float padding = 15.0f;
    float bgWidth = textBounds.width + padding * 2.0f;
    float bgHeight = textBounds.height + padding * 2.0f;

    float posX = m_codeBoxBounds.left + m_codeBoxBounds.width - bgWidth - 10.0f;
    float posY = m_codeBoxBounds.top + 10.0f;

    m_notifyBg.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_notifyBg.setPosition(posX, posY);

    m_notifyText.setOrigin(textBounds.left, textBounds.top);
    m_notifyText.setPosition(posX + padding, posY + padding);
}
