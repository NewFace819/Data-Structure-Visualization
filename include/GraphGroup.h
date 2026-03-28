#pragma once
#include "VisualizerState.h"
#include "graph/GraphModel.h"

class GraphGroup : public VisualizerState
{
public:
    GraphGroup(App* app);
    ~GraphGroup() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    void drawGrid(sf::RenderWindow& window);

    graph::GraphModel m_graphModel;
    sf::RectangleShape m_boardBackground;
    sf::RectangleShape m_cellShape;
    sf::Vector2f m_gridOrigin{290.0f, 90.0f};
    float m_cellSize = 24.0f;
};
