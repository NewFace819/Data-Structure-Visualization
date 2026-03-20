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
    // Tạo Top Bar màu Đen/Xanh sẫm tương đối tương phản với nền trắng xám
    m_topBar.setSize(sf::Vector2f(1280.0f, 60.0f));
    m_topBar.setFillColor(sf::Color(25, 30, 45)); 

    const sf::Font& font = m_app->getFont();
    m_titleText.setFont(font);
    m_titleText.setString(m_dsName + " Visualizer");
    m_titleText.setCharacterSize(28);
    m_titleText.setFillColor(sf::Color::White); // Titile màu trắng trên nền tối Topbar
    m_titleText.setPosition(30.0f, 15.0f); // Căn lề trái của Top Bar

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
}
