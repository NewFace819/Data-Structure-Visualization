#pragma once
#include <vector>

#include "Button.h"
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
    enum class ToolSelection {
        Wall,
        Start,
        End,
        Remove
    };

    void setupToolbox();
    void refreshToolButtonStyles();
    void handleGridInteraction(sf::Vector2f mousePosition);
    bool isMouseOnGrid(sf::Vector2f mousePosition) const;
    graph::NodeId nodeIdAtPosition(sf::Vector2f mousePosition) const;
    void drawGrid(sf::RenderWindow& window);
    void drawToolbox(sf::RenderWindow& window);

    graph::GraphModel m_graphModel;
    sf::RectangleShape m_boardBackground;
    sf::RectangleShape m_cellShape;
    sf::RectangleShape m_toolboxBackground;
    sf::Text m_toolboxTitle;
    std::vector<Button> m_toolButtons;
    sf::Vector2f m_gridOrigin{290.0f, 90.0f};
    float m_cellSize = 24.0f;
    ToolSelection m_selectedTool = ToolSelection::Wall;
    graph::NodeId m_lastPaintedNode = graph::kInvalidNodeId;
};
