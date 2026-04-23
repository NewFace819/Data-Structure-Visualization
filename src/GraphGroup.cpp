#include "GraphGroup.h"

#include "App.h"

#include <cmath>

namespace {

const std::vector<std::string> kAStarCode = {"AStar(start, goal):",
                                             "    push start into frontier",
                                             "    while frontier is not empty:",
                                             "        current = node with lowest f score",
                                             "        mark current as visited",
                                             "        if current == goal: rebuild path",
                                             "        for each valid neighbor:",
                                             "            if better path found: push neighbor"};

}

GraphGroup::GraphGroup(App* app)
    : VisualizerState(app, "Graph Algorithms"),
      m_graphModel(false),
      m_graphSidebar(app->getFont(), 720.0f) {}

void GraphGroup::init() {
    VisualizerState::init();

    m_graphModel.initializeGrid(20, 30);
    configureLayout();
    setupToolbox();
    setCode(kAStarCode);
    highlightCodeLine(-1);
    m_graphSidebar.setPlayButtonText("Start");
    m_graphSidebar.setBackCallback([this]() {
        m_isPaused = true;
        m_graphSidebar.setPlayButtonText("Start");
        stepBack();
    });
    m_graphSidebar.setNextCallback([this]() {
        m_isPaused = true;
        m_graphSidebar.setPlayButtonText("Start");
        stepForward();
    });
    m_graphSidebar.setPlayPauseCallback([this]() {
        const bool shouldRestart =
            m_traversalSteps.empty() ||
            m_currentTraversalStep >= static_cast<int>(m_traversalSteps.size()) - 1;

        if (shouldRestart) {
            runTraversal();
            return;
        }

        m_isPaused = !m_isPaused;
        m_graphSidebar.setPlayButtonText(m_isPaused ? "Start" : "Pause");
    });

    m_boardBackground.setPosition(m_gridOrigin);
    m_boardBackground.setSize(
        sf::Vector2f(m_graphModel.cols() * m_cellSize, m_graphModel.rows() * m_cellSize));
    m_boardBackground.setFillColor(sf::Color(248, 250, 252));
    m_boardBackground.setOutlineThickness(2.0f);
    m_boardBackground.setOutlineColor(sf::Color(203, 213, 225));

    m_cellShape.setSize(sf::Vector2f(m_cellSize - 1.0f, m_cellSize - 1.0f));
    m_cellShape.setFillColor(sf::Color::White);
}

void GraphGroup::configureLayout() {
    const sf::Vector2u windowSize = m_app->getWindow().getSize();
    const float topPadding = 20.0f;
    const float leftPanelWidth = 260.0f;
    const float contentGap = 20.0f;
    const float rightPanelWidth = 300.0f;

    const float codeX = static_cast<float>(windowSize.x) - rightPanelWidth - topPadding;
    const float codeY = 80.0f;
    const float codeHeight = static_cast<float>(windowSize.y) - codeY - topPadding;

    m_codeBoxBounds = sf::FloatRect(codeX, codeY, rightPanelWidth, codeHeight);
    m_codeBox.setBounds(codeX, codeY, rightPanelWidth, codeHeight);

    const float visualX = leftPanelWidth + topPadding;
    const float visualY = 80.0f;
    const float visualWidth = codeX - visualX - contentGap;
    const float visualHeight = static_cast<float>(windowSize.y) - visualY - topPadding;
    m_visualizationBounds = sf::FloatRect(visualX, visualY, visualWidth, visualHeight);
}

void GraphGroup::handleInput(const sf::Event& event) {
    sf::RenderWindow& window = m_app->getWindow();
    m_graphSidebar.handleEvent(event, window);
    m_backButton.handleEvent(event, window);

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

void GraphGroup::update(float dt) {
    sf::RenderWindow& window = m_app->getWindow();
    const sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    const sf::Vector2f mousePos(static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y));

    m_graphSidebar.update(mousePos);
    m_backButton.update(mousePos);

    for (auto& button : m_toolButtons) {
        button.update(mousePos);
    }

    if (!m_isPaused && !m_traversalSteps.empty()) {
        m_playTimer += dt;
        const float speed = m_graphSidebar.getSpeed();
        const float invSpeed = 1.0f - speed;
        const float delay = 0.02f + 1.48f * invSpeed * invSpeed;

        if (m_playTimer > delay) {
            m_playTimer = 0.0f;
            if (m_currentTraversalStep < static_cast<int>(m_traversalSteps.size()) - 1) {
                stepForward();
            } else {
                m_isPaused = true;
                m_graphSidebar.setPlayButtonText("Start");
            }
        }
    }
}

void GraphGroup::draw(sf::RenderWindow& window) {
    drawToolbox(window);
    drawGrid(window);

    window.draw(m_topBar);
    window.draw(m_titleText);
    m_backButton.draw(window);
    m_graphSidebar.draw(window);
    m_codeBox.draw(window);
}

void GraphGroup::setupToolbox() {
    const float toolboxHeight = 78.0f;
    const float innerPadding = 18.0f;
    const float buttonGap = 12.0f;
    const float titleColumnWidth = 120.0f;
    const float buttonY = m_visualizationBounds.top + 16.0f;
    const float buttonX = m_visualizationBounds.left + innerPadding + titleColumnWidth;
    const float availableButtonWidth =
        m_visualizationBounds.width - titleColumnWidth - innerPadding * 2.0f - buttonGap * 3.0f;
    const float buttonWidth = availableButtonWidth / 4.0f;

    m_toolboxBackground.setPosition(m_visualizationBounds.left, m_visualizationBounds.top);
    m_toolboxBackground.setSize(sf::Vector2f(m_visualizationBounds.width, toolboxHeight));
    m_toolboxBackground.setFillColor(sf::Color(255, 255, 255, 235));
    m_toolboxBackground.setOutlineThickness(2.0f);
    m_toolboxBackground.setOutlineColor(sf::Color(226, 232, 240));

    m_toolboxTitle.setFont(m_app->getFont());
    m_toolboxTitle.setString("Edit Tool");
    m_toolboxTitle.setCharacterSize(18);
    m_toolboxTitle.setFillColor(sf::Color(71, 85, 105));
    m_toolboxTitle.setPosition(m_visualizationBounds.left + innerPadding, m_visualizationBounds.top + 12.0f);

    m_toolButtons.clear();
    m_toolButtons.emplace_back(buttonX, buttonY, buttonWidth, 38.0f, "Wall", m_app->getFont());
    m_toolButtons.emplace_back(buttonX + buttonWidth + buttonGap, buttonY, buttonWidth, 38.0f,
                               "Start", m_app->getFont());
    m_toolButtons.emplace_back(buttonX + 2.0f * (buttonWidth + buttonGap), buttonY, buttonWidth,
                               38.0f, "End", m_app->getFont());
    m_toolButtons.emplace_back(buttonX + 3.0f * (buttonWidth + buttonGap), buttonY, buttonWidth,
                               38.0f, "Remove", m_app->getFont());

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

    const float boardTop = m_visualizationBounds.top + toolboxHeight + 18.0f;
    const float availableBoardHeight = m_visualizationBounds.height - toolboxHeight - 18.0f;
    const float maxCellWidth = m_visualizationBounds.width / static_cast<float>(m_graphModel.cols());
    const float maxCellHeight = availableBoardHeight / static_cast<float>(m_graphModel.rows());
    m_cellSize = std::floor(std::min(maxCellWidth, maxCellHeight));
    if (m_cellSize < 12.0f) {
        m_cellSize = 12.0f;
    }

    const float boardWidth = m_graphModel.cols() * m_cellSize;
    const float boardHeight = m_graphModel.rows() * m_cellSize;
    m_gridOrigin.x = m_visualizationBounds.left + (m_visualizationBounds.width - boardWidth) / 2.0f;
    m_gridOrigin.y = boardTop + (availableBoardHeight - boardHeight) / 2.0f;
}

void GraphGroup::refreshToolButtonStyles() {
    const sf::Color inactiveFill(241, 245, 249);
    const sf::Color inactiveHover(226, 232, 240);
    const sf::Color inactiveActive(203, 213, 225);
    const sf::Color activeFill(14, 116, 144);
    const sf::Color activeHover(8, 145, 178);
    const sf::Color activeActive(21, 94, 117);

    for (std::size_t i = 0; i < m_toolButtons.size(); ++i) {
        const bool isSelected = (i == 0 && m_selectedTool == ToolSelection::Wall) ||
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

void GraphGroup::handleGridInteraction(sf::Vector2f mousePosition) {
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

    resetTraversal();
    m_lastPaintedNode = nodeId;
}

bool GraphGroup::isMouseOnGrid(sf::Vector2f mousePosition) const {
    return mousePosition.x >= m_gridOrigin.x && mousePosition.y >= m_gridOrigin.y &&
           mousePosition.x < m_gridOrigin.x + m_graphModel.cols() * m_cellSize &&
           mousePosition.y < m_gridOrigin.y + m_graphModel.rows() * m_cellSize;
}

graph::NodeId GraphGroup::nodeIdAtPosition(sf::Vector2f mousePosition) const {
    if (!isMouseOnGrid(mousePosition)) {
        return graph::kInvalidNodeId;
    }

    const int col = static_cast<int>((mousePosition.x - m_gridOrigin.x) / m_cellSize);
    const int row = static_cast<int>((mousePosition.y - m_gridOrigin.y) / m_cellSize);
    return m_graphModel.nodeIdAt(row, col);
}

void GraphGroup::resetTraversal() {
    m_traversalSteps.clear();
    m_currentTraversalStep = -1;
    m_inQueueNodes.clear();
    m_visitedNodes.clear();
    m_pathNodes.clear();
    m_activeNode = graph::kInvalidNodeId;
    m_isPaused = true;
    m_playTimer = 0.0f;
    m_graphSidebar.setPlayButtonText("Start");
    highlightCodeLine(-1);
}

void GraphGroup::runTraversal() {
    resetTraversal();
    m_traversalSteps = graph::AStar::run(m_graphModel);
    if (m_traversalSteps.empty()) {
        return;
    }

    m_currentTraversalStep = 0;
    loadTraversalStep(m_currentTraversalStep);
    m_isPaused = false;
    m_graphSidebar.setPlayButtonText("Pause");
}

void GraphGroup::loadTraversalStep(int index) {
    if (index < 0 || index >= static_cast<int>(m_traversalSteps.size())) {
        return;
    }

    const graph::AStarStep& step = m_traversalSteps[index];
    m_inQueueNodes.clear();
    m_visitedNodes.clear();
    m_pathNodes.clear();

    m_inQueueNodes.insert(step.inQueue.begin(), step.inQueue.end());
    m_visitedNodes.insert(step.visited.begin(), step.visited.end());
    m_pathNodes.insert(step.path.begin(), step.path.end());
    m_activeNode = step.current;

    if (!step.path.empty()) {
        highlightCodeLine(5);
    } else if (m_visitedNodes.find(step.current) != m_visitedNodes.end()) {
        highlightCodeLine(4);
    } else if (m_inQueueNodes.find(step.current) != m_inQueueNodes.end()) {
        highlightCodeLine(7);
    } else if (step.current == m_graphModel.start()) {
        highlightCodeLine(1);
    } else {
        highlightCodeLine(2);
    }
}

void GraphGroup::stepBack() {
    if (m_currentTraversalStep > 0) {
        --m_currentTraversalStep;
        loadTraversalStep(m_currentTraversalStep);
    }
}

void GraphGroup::stepForward() {
    if (m_currentTraversalStep < static_cast<int>(m_traversalSteps.size()) - 1) {
        ++m_currentTraversalStep;
        loadTraversalStep(m_currentTraversalStep);
    }
}

void GraphGroup::drawGrid(sf::RenderWindow& window) {
    window.draw(m_boardBackground);

    for (const auto& node : m_graphModel.nodes()) {
        const graph::GridCoord coord = m_graphModel.coordOf(node.id);
        if (coord.row < 0 || coord.col < 0) {
            continue;
        }

        sf::Color fillColor = sf::Color::White;
        if (node.blocked) {
            fillColor = sf::Color(51, 65, 85);
        } else if (m_pathNodes.find(node.id) != m_pathNodes.end()) {
            fillColor = sf::Color(250, 204, 21);
        } else if (node.id == m_activeNode &&
                   m_visitedNodes.find(node.id) != m_visitedNodes.end()) {
            fillColor = sf::Color(245, 158, 11);
        } else if (m_visitedNodes.find(node.id) != m_visitedNodes.end()) {
            fillColor = sf::Color(14, 165, 233);
        } else if (m_inQueueNodes.find(node.id) != m_inQueueNodes.end()) {
            fillColor = sf::Color(167, 243, 208);
        }

        if (node.id == m_graphModel.start()) {
            fillColor = sf::Color(34, 197, 94);
        } else if (node.id == m_graphModel.goal()) {
            fillColor = sf::Color(239, 68, 68);
        }

        m_cellShape.setPosition(m_gridOrigin.x + coord.col * m_cellSize,
                                m_gridOrigin.y + coord.row * m_cellSize);
        m_cellShape.setFillColor(fillColor);
        window.draw(m_cellShape);
    }
}

void GraphGroup::drawToolbox(sf::RenderWindow& window) {
    window.draw(m_toolboxBackground);
    window.draw(m_toolboxTitle);

    for (const auto& button : m_toolButtons) {
        button.draw(window);
    }
}
