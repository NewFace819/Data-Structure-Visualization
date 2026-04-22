#include "App.h"
#include "MenuState.h"
#include <iostream>

App::App() : m_window(sf::VideoMode(1400, 800), "Data Structure Visualizer", sf::Style::Titlebar | sf::Style::Close, sf::ContextSettings(0, 0, 8)), m_isChangingState(false)
{
    m_window.setVerticalSyncEnabled(true); // Enable VSync
    loadResources();
    pushState(std::make_unique<MenuState>(this));
}

App::~App()
{
    while (!m_states.empty())
        m_states.pop();
}

void App::run()
{
    sf::Clock clock;
    while (m_window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();

        // Delay state switch
        if (m_isChangingState) {
            if (!m_states.empty())
                m_states.pop();
            
            m_states.push(std::move(m_newState));
            m_states.top()->init();
            m_isChangingState = false;
        }
    }
}

void App::pushState(std::unique_ptr<State> state)
{
    m_states.push(std::move(state));
    m_states.top()->init();
}

void App::popState()
{
    if (!m_states.empty())
    {
        m_states.pop();
    }
}

void App::changeState(std::unique_ptr<State> state)
{
    m_newState = std::move(state);
    m_isChangingState = true;
}

sf::RenderWindow& App::getWindow()
{
    return m_window;
}

const sf::Font& App::getFont() const
{
    return m_mainFont;
}

void App::loadResources()
{
    // Load main UI font
    if (!m_mainFont.loadFromFile("assets/SF-Pro-Display-Regular.otf"))
    {
        std::cerr << "Warning: Failed to load font\n";
    }
}

void App::processEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            m_window.close();

        if (!m_states.empty())
            m_states.top()->handleInput(event);
    }
}

void App::update(float dt)
{
    if (!m_states.empty())
        m_states.top()->update(dt);
}

void App::render()
{
    m_window.clear(); 
    
    // Gradient bg
    sf::Color colorLeft(250, 250, 250);
    sf::Color colorRight(190, 190, 190);

    sf::Vector2u size = m_window.getSize();

    sf::Vertex bg[] = {
        sf::Vertex(sf::Vector2f(0.0f, 0.0f), colorLeft),
        sf::Vertex(sf::Vector2f((float)size.x, 0.0f), colorRight),
        sf::Vertex(sf::Vector2f((float)size.x, (float)size.y), colorRight),
        sf::Vertex(sf::Vector2f(0.0f, (float)size.y), colorLeft)
    };
    m_window.draw(bg, 4, sf::Quads);
    
    if (!m_states.empty())
        m_states.top()->draw(m_window);
        
    m_window.display();
}
