#include "TreeGroup.h"
#include "App.h"
#include "Tree23State.h"

TreeGroup::TreeGroup(App* app)
    : VisualizerState(app, "Tree Configurations"),
      m_tree23Button(420, 180, 220, 70, "2-3 Tree", app->getFont())
{
}

void TreeGroup::init()
{
    VisualizerState::init();

    m_tree23Button.setCallback([this]() {
        m_app->changeState(std::make_unique<Tree23State>(m_app));
    });
}

void TreeGroup::handleInput(const sf::Event& event)
{
    VisualizerState::handleInput(event);

    sf::RenderWindow& window = m_app->getWindow();
    m_tree23Button.handleEvent(event, window);
}

void TreeGroup::update(float dt)
{
    VisualizerState::update(dt);

    sf::RenderWindow& window = m_app->getWindow();
    m_tree23Button.update(window);
}

void TreeGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);

    sf::Text text;
    text.setFont(m_app->getFont());
    text.setCharacterSize(22);
    text.setFillColor(sf::Color::Black);
    text.setString("Choose a tree type");
    text.setPosition(400.f, 120.f);
    window.draw(text);

    m_tree23Button.draw(window);
}