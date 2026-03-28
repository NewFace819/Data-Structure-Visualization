#include "GraphGroup.h"
#include "App.h"

GraphGroup::GraphGroup(App* app) 
    : VisualizerState(app, "Graph Algorithms"),
      m_graphModel(false)
{
}

void GraphGroup::init()
{
    VisualizerState::init();

    m_graphModel.initializeGrid(20, 30);

    m_boardBackground.setPosition(m_gridOrigin);
    m_boardBackground.setSize(sf::Vector2f(m_graphModel.cols() * m_cellSize,
                                           m_graphModel.rows() * m_cellSize));
    m_boardBackground.setFillColor(sf::Color(248, 250, 252));
    m_boardBackground.setOutlineThickness(2.0f);
    m_boardBackground.setOutlineColor(sf::Color(203, 213, 225));

    m_cellShape.setSize(sf::Vector2f(m_cellSize - 1.0f, m_cellSize - 1.0f));
    m_cellShape.setFillColor(sf::Color::White);
}

void GraphGroup::handleInput(const sf::Event& event)
{
    VisualizerState::handleInput(event);
}

void GraphGroup::update(float dt)
{
    VisualizerState::update(dt);
}

void GraphGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);
    drawGrid(window);
}

void GraphGroup::drawGrid(sf::RenderWindow& window)
{
    window.draw(m_boardBackground);

    for (const auto& node : m_graphModel.nodes()) {
        const graph::GridCoord coord = m_graphModel.coordOf(node.id);
        if (coord.row < 0 || coord.col < 0) {
            continue;
        }

        sf::Color fillColor = sf::Color::White;
        if (node.id == m_graphModel.start()) {
            fillColor = sf::Color(34, 197, 94);
        } else if (node.id == m_graphModel.goal()) {
            fillColor = sf::Color(239, 68, 68);
        } else if (node.blocked) {
            fillColor = sf::Color(51, 65, 85);
        }

        m_cellShape.setPosition(m_gridOrigin.x + coord.col * m_cellSize,
                                m_gridOrigin.y + coord.row * m_cellSize);
        m_cellShape.setFillColor(fillColor);
        window.draw(m_cellShape);
    }
}
