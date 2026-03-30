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
    setupToolbox();

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

    sf::RenderWindow& window = m_app->getWindow();
    for (auto& button : m_toolButtons) {
        button.handleEvent(event, window);
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        const sf::Vector2f mousePosition(static_cast<float>(event.mouseButton.x),
                                         static_cast<float>(event.mouseButton.y));
        handleGridInteraction(mousePosition);
        return;
    }

    if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        const sf::Vector2f mousePosition(static_cast<float>(event.mouseMove.x),
                                         static_cast<float>(event.mouseMove.y));
        handleGridInteraction(mousePosition);
        return;
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
        m_lastPaintedNode = graph::kInvalidNodeId;
    }
}

void GraphGroup::update(float dt)
{
    VisualizerState::update(dt);

    sf::RenderWindow& window = m_app->getWindow();
    for (auto& button : m_toolButtons) {
        button.update(window);
    }
}

void GraphGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);
    drawToolbox(window);
    drawGrid(window);
}

void GraphGroup::setupToolbox()
{
    m_toolboxBackground.setPosition(290.0f, 90.0f);
    m_toolboxBackground.setSize(sf::Vector2f(720.0f, 78.0f));
    m_toolboxBackground.setFillColor(sf::Color(255, 255, 255, 235));
    m_toolboxBackground.setOutlineThickness(2.0f);
    m_toolboxBackground.setOutlineColor(sf::Color(226, 232, 240));

    m_toolboxTitle.setFont(m_app->getFont());
    m_toolboxTitle.setString("Edit Tool");
    m_toolboxTitle.setCharacterSize(18);
    m_toolboxTitle.setFillColor(sf::Color(71, 85, 105));
    m_toolboxTitle.setPosition(312.0f, 102.0f);

    m_toolButtons.clear();
    m_toolButtons.emplace_back(430.0f, 106.0f, 120.0f, 38.0f, "Wall", m_app->getFont());
    m_toolButtons.emplace_back(565.0f, 106.0f, 120.0f, 38.0f, "Start", m_app->getFont());
    m_toolButtons.emplace_back(700.0f, 106.0f, 120.0f, 38.0f, "End", m_app->getFont());
    m_toolButtons.emplace_back(835.0f, 106.0f, 120.0f, 38.0f, "Remove", m_app->getFont());

    m_toolButtons[0].setCallback([this]() {
        m_selectedTool = ToolSelection::Wall;
        refreshToolButtonStyles();
    });
    m_toolButtons[1].setCallback([this]() {
        m_selectedTool = ToolSelection::Start;
        refreshToolButtonStyles();
    });
    m_toolButtons[2].setCallback([this]() {
        m_selectedTool = ToolSelection::End;
        refreshToolButtonStyles();
    });
    m_toolButtons[3].setCallback([this]() {
        m_selectedTool = ToolSelection::Remove;
        refreshToolButtonStyles();
    });

    refreshToolButtonStyles();
    m_gridOrigin.y = 180.0f;
}

void GraphGroup::refreshToolButtonStyles()
{
    const sf::Color inactiveFill(241, 245, 249);
    const sf::Color inactiveHover(226, 232, 240);
    const sf::Color inactiveActive(203, 213, 225);
    const sf::Color activeFill(14, 116, 144);
    const sf::Color activeHover(8, 145, 178);
    const sf::Color activeActive(21, 94, 117);

    for (std::size_t i = 0; i < m_toolButtons.size(); ++i) {
        const bool isSelected =
            (i == 0 && m_selectedTool == ToolSelection::Wall) ||
            (i == 1 && m_selectedTool == ToolSelection::Start) ||
            (i == 2 && m_selectedTool == ToolSelection::End) ||
            (i == 3 && m_selectedTool == ToolSelection::Remove);

        if (isSelected) {
            m_toolButtons[i].setColors(activeFill, activeHover, activeActive);
            m_toolButtons[i].setTextColor(sf::Color::White);
            m_toolButtons[i].setOutline(0.0f, sf::Color::Transparent);
        } else {
            m_toolButtons[i].setColors(inactiveFill, inactiveHover, inactiveActive);
            m_toolButtons[i].setTextColor(sf::Color(30, 41, 59));
            m_toolButtons[i].setOutline(1.0f, sf::Color(203, 213, 225));
        }
    }
}

void GraphGroup::handleGridInteraction(sf::Vector2f mousePosition)
{
    if (!isMouseOnGrid(mousePosition)) {
        return;
    }

    const graph::NodeId nodeId = nodeIdAtPosition(mousePosition);
    if (nodeId == graph::kInvalidNodeId || nodeId == m_lastPaintedNode) {
        return;
    }

    switch (m_selectedTool) {
    case ToolSelection::Wall:
        m_graphModel.setBlocked(nodeId, true);
        break;
    case ToolSelection::Start:
        m_graphModel.setStart(nodeId);
        break;
    case ToolSelection::End:
        m_graphModel.setGoal(nodeId);
        break;
    case ToolSelection::Remove:
        m_graphModel.clearCell(nodeId);
        break;
    }

    m_lastPaintedNode = nodeId;
}

bool GraphGroup::isMouseOnGrid(sf::Vector2f mousePosition) const
{
    return mousePosition.x >= m_gridOrigin.x &&
           mousePosition.y >= m_gridOrigin.y &&
           mousePosition.x < m_gridOrigin.x + m_graphModel.cols() * m_cellSize &&
           mousePosition.y < m_gridOrigin.y + m_graphModel.rows() * m_cellSize;
}

graph::NodeId GraphGroup::nodeIdAtPosition(sf::Vector2f mousePosition) const
{
    if (!isMouseOnGrid(mousePosition)) {
        return graph::kInvalidNodeId;
    }

    const int col = static_cast<int>((mousePosition.x - m_gridOrigin.x) / m_cellSize);
    const int row = static_cast<int>((mousePosition.y - m_gridOrigin.y) / m_cellSize);
    return m_graphModel.nodeIdAt(row, col);
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

void GraphGroup::drawToolbox(sf::RenderWindow& window)
{
    window.draw(m_toolboxBackground);
    window.draw(m_toolboxTitle);

    for (const auto& button : m_toolButtons) {
        button.draw(window);
    }
}
