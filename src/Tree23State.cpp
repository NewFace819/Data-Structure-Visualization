#include "Tree23State.h"
#include "App.h"
#include "TreeGroup.h"
#include <cstdlib>
#include <ctime>
#include <sstream>

static void updateAnimationText(VisualizerUI& ui, const std:: string& text, const std::string& speedLabel);


static bool tryInsertFromInput(Tree23& tree, VisualizerUI& ui,
                        std::vector<Tree23Node*>& highlightPath, bool& searchFound,
                        std::vector<Tree23Node*>& fullSearchPath,
                        int& currentAnimationStep, bool& isPlaying)
{
    bool isValid = false;
    int value = getInputValue(ui, isValid);

    if (isValid == false)
    {
        setStatus(ui, "Invalid input");
        return false;
    }

    if (tree.insert(value))
    {
        setStatus(ui, "Insert successful");
        ui.inputBuffer = "";
        ui.inputText.setString("");
        highlightPath.clear();
        searchFound = false;
        fullSearchPath.clear();
        currentAnimationStep = -1;
        isPlaying = false;
        ui.animationText.setString("Animation: idle");
        return true;
    }

    setStatus(ui, "Value already exists");
    return false;
}

static void resetAnimation(std::vector<Tree23Node*>& highlightPath,
                    std::vector<Tree23Node*>& fullSearchPath,
                    int& currentAnimationStep, bool& isPlaying,
                    bool& searchFound, VisualizerUI& ui)
{
    highlightPath.clear();
    fullSearchPath.clear();
    currentAnimationStep = -1;
    isPlaying = false;
    searchFound = false;
    ui.animationText.setString("Animation: idle");
}

static void resetInsertAnimation(std::vector<Tree23Step>& insertSteps,
                          int& currentInsertStep,
                          bool& isInsertPlaying,
                          int& pendingInsertValue,
                          VisualizerUI& ui,
                          const std::string& speedLabel)
{
    insertSteps.clear();
    currentInsertStep = -1;
    isInsertPlaying = false;
    pendingInsertValue = 0;
    updateAnimationText(ui, "Animation: idle", speedLabel);
}

static void resetDeleteAnimation(std::vector<Tree23Step>& deleteSteps,
                          int& currentDeleteStep,
                          bool& isDeletePlaying,
                          int& pendingDeleteValue,
                          VisualizerUI& ui,
                          const std::string& speedLabel)
{
    deleteSteps.clear();
    currentDeleteStep = -1;
    isDeletePlaying = false;
    pendingDeleteValue = 0;
    updateAnimationText(ui, "Animation: idle", speedLabel);
}

static void initializeRandomTree(Tree23& tree)
{
    tree.clear();

    int count = 6;
    int values[6];

    for (int i = 0; i < count; i++)
    {
        int x = rand() % 50 + 1;

        bool duplicate = true;
        while (duplicate)
        {
            duplicate = false;
            for (int j = 0; j < i; j++)
            {
                if (values[j] == x)
                {
                    duplicate = true;
                    x = rand() % 50 + 1;
                    break;
                }
            }
        }

        values[i] = x;
        tree.insert(x);
    }
}

static void initializeSampleTree(Tree23& tree)
{
    tree.clear();

    int values[6] = {2, 3, 4, 5, 7, 10};

    for (int i = 0; i < 6; i++)
    {
        tree.insert(values[i]);
    }
}

static bool parseUpdateInput(const std:: string& s, int& oldValue, int& newValue)
{
    std::stringstream ss(s);
    char comma = 0;

    ss >> oldValue;
    if (ss.fail())
    {
        return false;
    }

    ss >> comma;
    if (ss.fail() || comma != ',')
    {
        return false;
    }

    ss >> newValue;
    if (ss.fail())
    {
        return false;
    }

    return true;
}

static void updateAnimationText(VisualizerUI& ui, const std::string& text, const std::string& speedLabel)
{
    ui.animationText.setString(text + " | Speed: " + speedLabel);
}

static void resetInsertPathState(Tree23Node*& currentHighlightedNode,
                          std::vector<Tree23Node*>& insertPath)
{
    currentHighlightedNode = nullptr;
    insertPath.clear();
}

static std::string buildNodeId(Tree23Node* node, int level, int index)
{
    return "L" + std::to_string(level) + "_" + std::to_string(index);
}

static int countLeaves(Tree23Node* node)
{
    if (node == nullptr)
    {
        return 0;
    }

    if (node->isLeaf())
    {
        return 1;
    }

    int total = 0;
    for (int i = 0; i < 3; i++)
    {
        if (node->child[i] != nullptr)
        {
            total += countLeaves(node->child[i]);
        }
    }

    return total;
}

static std::string buildSnapshotRecursive(Tree23Node* node,
                                   TreeSnapshot& snapshot,
                                   int level,
                                   float leftX,
                                   float rightX,
                                   float startY,
                                   float levelGap,
                                   const std::string& path)
{
    if (node == nullptr)
    {
        return "";
    }

    float centerX = (leftX + rightX) / 2.f;
    float y = startY + level * levelGap;

    std::string nodeId = path;
    
    VisualNode visualNode;
    visualNode.id = nodeId;
    visualNode.keyCount = node->keyCount;
    visualNode.keys[0] = node->keys[0];
    visualNode.keys[1] = (node->keyCount == 2 ? node->keys[1] : 0);
    visualNode.x = centerX;
    visualNode.y = y;
    visualNode.isHighlighted = false;
    visualNode.isOverflow = false;
    visualNode.isNewNode = false;

    snapshot.nodes.push_back(visualNode);

    if (node->isLeaf())
    {
        return nodeId;
    }

    int totalLeaves = 0;
    for (int i = 0; i < 3; i++)
    {
        if (node->child[i] != nullptr)
        {
            totalLeaves += countLeaves(node->child[i]);
        }
    }

    float currentLeft = leftX;

    for (int i = 0; i < 3; i++)
    {
        if (node->child[i] != nullptr)
        {
            int childLeaves = countLeaves(node->child[i]);
            float childWidth = (rightX - leftX) * childLeaves / (float)totalLeaves;
            float childLeft = currentLeft;
            float childRight = currentLeft + childWidth;

            std::string childPath = path + "_" + std::to_string(i);
            std::string childId = buildSnapshotRecursive(node->child[i], snapshot, level + 1,
                                                         childLeft, childRight,
                                                         startY, levelGap, childPath);

            if (!childId.empty())
            {
                VisualEdge edge;
                edge.parentId = nodeId;
                edge.childId = childId;
                snapshot.edges.push_back(edge);
            }

            currentLeft += childWidth;
        }
    }

    return nodeId;
}

static TreeSnapshot buildSnapshotFromTree(const Tree23& tree)
{
    TreeSnapshot snapshot;

    Tree23Node* root = tree.getRoot();
    if (root == nullptr)
    {
        return snapshot;
    }

    float startY = 90.f;
    float levelGap = 110.f;
    float leftBound = 380.f;
    float rightBound = 1290.f;

    buildSnapshotRecursive(root, snapshot, 0, leftBound, rightBound,
                           startY, levelGap, "root");

    return snapshot;
}

static VisualNode* findMutableVisualNodeById(TreeSnapshot& snapshot, const std::string& id)
{
    for (int i = 0; i < (int)snapshot.nodes.size(); i++)
    {
        if (snapshot.nodes[i].id == id)
        {
            return &snapshot.nodes[i];
        }
    }

    return nullptr;
}

static SnapshotSequence buildInsertSequence(const Tree23& tree, int value)
{
    SnapshotSequence sequence;

    TreeSnapshot baseSnapshot = buildSnapshotFromTree(tree);
    sequence.frames.push_back(baseSnapshot);
    sequence.labels.push_back("Start insert");

    std::vector<Tree23Node*> path = tree.getInsertPath(value);

    for (int i = 0; i < (int)path.size(); i++)
    {
        TreeSnapshot stepSnapshot = buildSnapshotFromTree(tree);

        std::string nodeId;
        if (i == 0)
        {
            nodeId = "root";
        }
        else {
            nodeId = "root";
            for (int j = 1; j <= i; j++)
            {
                Tree23Node* parent = path[j - 1];
                Tree23Node* child = path[j];

                int childIndex = 0;
                for (int c = 0; c < 3; c++)
                {
                    if (parent->child[c] == child)
                    {
                        childIndex = c;
                        break;
                    }
                }

                nodeId += "_" + std::to_string(childIndex);
            }
        }

        VisualNode* visualNode = findMutableVisualNodeById(stepSnapshot, nodeId);
        if (visualNode != nullptr)
        {
            visualNode->isHighlighted = true;
        }

        sequence.frames.push_back(stepSnapshot);
        if (i == 0)
        {
            sequence.labels.push_back("visit root");
        }
        else {
            sequence.labels.push_back("visit node");
        }
    }

    if (!path.empty())
    {
        Tree23Node* leaf = path.back();

        if (leaf->keyCount == 2)
        {
            TreeSnapshot overflowSnapshot = buildSnapshotFromTree(tree);

            std::string nodeId;
            if (path.size() == 1)
            {
                nodeId = "root";
            }
            else {
                nodeId = "root";
                for (int j = 1; j < (int)path.size(); j++)
                {
                    Tree23Node* parent = path[j - 1];
                    Tree23Node* child = path[j];

                    int childIndex = 0;
                    for (int c = 0; c < 3; c++)
                    {
                        if (parent->child[c] == child)
                        {
                            childIndex = c;
                            break;
                        }
                    }

                    nodeId += "_" + std::to_string(childIndex);
                }
            }

            VisualNode* overflowNode = findMutableVisualNodeById(overflowSnapshot, nodeId);
            if (overflowNode != nullptr)
            {
                overflowNode->isOverflow = true;
                overflowNode->isHighlighted = true;
            }

            sequence.frames.push_back(overflowSnapshot);
            sequence.labels.push_back("Split leaf");
        }
    }

    Tree23 tempTree;

    std::vector<int> inorderValues = tree.getInorder();
    for (int i = 0; i < (int)inorderValues.size(); i++)
    {
        tempTree.insert(inorderValues[i]);
    }
    tempTree.insert(value);

    TreeSnapshot finalSnapshot = buildSnapshotFromTree(tempTree);
    sequence.frames.push_back(finalSnapshot);
    sequence.labels.push_back("Finish insert");

    return sequence;
}

static void clearSnapshotHighlight(TreeSnapshot& snapshot)
{
    for (int i = 0; i < (int)snapshot.nodes.size(); i++)
    {
        snapshot.nodes[i].isHighlighted = false;
        snapshot.nodes[i].isOverflow = false;
        snapshot.nodes[i].isNewNode = false;
    }
}

static void highlightSnapshotNodeByPath(TreeSnapshot& snapshot, const std::string& nodeId)
{
    clearSnapshotHighlight(snapshot);

    for (int i = 0; i < (int)snapshot.nodes.size(); i++)
    {
        if (snapshot.nodes[i].id == nodeId)
        {
            snapshot.nodes[i].isHighlighted = true;
            return;
        }
    }
}

static std::string buildPathIdFromSearchPath(const std::vector<Tree23Node*>& path, int stepIndex)
{
    if (path.empty() || stepIndex < 0 || stepIndex >= (int)path.size())
    {
        return "";
    }

    if (stepIndex == 0)
    {
        return "root";
    }

    std::string nodeId = "root";

    for (int i = 1; i <= stepIndex; i++)
    {
        Tree23Node* parent = path[i - 1];
        Tree23Node* child = path[i];

        int childIndex = 0;
        for (int c = 0; c < 3; c++)
        {
            if (parent->child[c] == child)
            {
                childIndex = c;
                break;
            }
        }

        nodeId += "_" + std::to_string(childIndex);
    }

    return nodeId;
}
Tree23State::Tree23State(App* app)
    : VisualizerState(app, "2-3 Tree")
{
}

void Tree23State::init()
{
    m_tree.clear();

    m_highlightPath.clear();
    m_fullSearchPath.clear();
    m_insertPath.clear();
    m_currentHighlightedNode = nullptr;

    m_insertSteps.clear();
    m_deleteSteps.clear();

    m_searchFound = false;
    m_isPlaying = false;
    m_currentAnimationStep = -1;

    m_isInsertPlaying = false;
    m_currentInsertStep = -1;
    m_pendingInsertValue = 0;

    m_isDeletePlaying = false;
    m_currentDeleteStep = -1;
    m_pendingDeleteValue = 0;

    m_currentSnapshot = TreeSnapshot();
    m_snapshotFrom = TreeSnapshot();
    m_snapshotTo = TreeSnapshot();

    m_isSnapshotTransitionPlaying = false;
    m_snapshotTransitionProgress = 0.f;
    m_snapshotTransitionDuration = 0.45f;

    m_currentSequence.frames.clear();
    m_currentSequence.labels.clear();
    m_currentSequenceIndex = 0;

    m_currentSearchSequence.frames.clear();
    m_currentSearchSequence.labels.clear();
    m_isSearchSequencePlaying = false;
    m_currentSearchSequenceIndex = 0;

    m_speedLabel = "Normal";
    m_animationDelay = 0.8f;

    setupUI(m_ui, m_app->getFont(), m_app->getWindow().getSize());
    m_ui.statusText.setString("Ready");
    updateAnimationText(m_ui, "Animation: idle", m_speedLabel);

    std::srand((unsigned int)std::time(nullptr));
}

void Tree23State::handleInput(const sf::Event& event)
{
    sf::RenderWindow& window = m_app->getWindow();

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
    {
        m_app->changeState(std::make_unique<TreeGroup>(m_app));
        return;
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
            );

            if (isInside(m_ui.inputBox, mousePos))
            {
                m_ui.isTyping = true;
            }
            else {
                m_ui.isTyping = false;
            }

            if (isButtonClicked(m_ui.insertButton, mousePos))
            {
                bool isValid = false;
                int value = getInputValue(m_ui, isValid);

                if (isValid == false)
                {
                    setStatus(m_ui, "Invalid input");
                }
                else if (m_tree.search(value))
                {
                    setStatus(m_ui, "Value already exists");
                }
                else {
                    resetAnimation(m_highlightPath, m_fullSearchPath,
                                   m_currentAnimationStep, m_isPlaying,
                                   m_searchFound, m_ui);

                    resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                         m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);

                    resetInsertPathState(m_currentHighlightedNode, m_insertPath);
                    resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                         m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                    m_insertPath = m_tree.getInsertPath(value);
                    m_insertSteps = m_tree.getInsertSteps(value);
                    m_currentSequence = buildInsertSequence(m_tree, value);
                    m_currentSequenceIndex = 0;

                    m_pendingInsertValue = value;
                    m_currentInsertStep = 0;
                    m_isInsertPlaying = true;

                    if (!m_insertPath.empty())
                    {
                        m_highlightPath.clear();
                        m_highlightPath.push_back(m_insertPath[0]);
                        m_currentHighlightedNode = m_insertPath[0];
                    }

                    if (!m_currentSequence.labels.empty())
                    {
                        updateAnimationText(m_ui, "Animation: " + m_currentSequence.labels[0], m_speedLabel);
                    }
                    else if (!m_insertSteps.empty())
                    {
                        updateAnimationText(m_ui, "Animation: " + m_insertSteps[0].action, m_speedLabel);
                    }
                    else {
                        updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                    }

                    m_animationClock.restart();
                    setStatus(m_ui, "Insert animation started");
                }
            }

            if (isButtonClicked(m_ui.searchButton, mousePos))
            {
                bool isValid = false;
                int value = getInputValue(m_ui, isValid);

                if (isValid == false)
                {
                    setStatus(m_ui, "Invalid input");
                }
                else {
                    resetInsertPathState(m_currentHighlightedNode, m_insertPath);
                    resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                         m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                    resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                         m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);

                    m_fullSearchPath = m_tree.getSearchPath(value, m_searchFound);
                    m_highlightPath.clear();
                    m_currentHighlightedNode = nullptr;

                    if (m_fullSearchPath.empty())
                    {
                        if (m_searchFound)
                        {
                            setStatus(m_ui, "Found");
                        }
                        else {
                            setStatus(m_ui, "Not found");
                        }

                        m_isPlaying = false;
                        m_currentAnimationStep = -1;
                        updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                    }
                    else {
                        m_currentAnimationStep = 0;
                        m_highlightPath.push_back(m_fullSearchPath[0]);
                        m_currentHighlightedNode = m_fullSearchPath[0];
                        highlightSnapshotNodeByPath(m_currentSnapshot, buildPathIdFromSearchPath(m_fullSearchPath, 0));

                        if ((int)m_fullSearchPath.size() > 1)
                        {
                            m_isPlaying = true;
                            updateAnimationText(m_ui, "Animation: playing", m_speedLabel);
                            m_animationClock.restart();
                        }
                        else {
                            m_isPlaying = false;

                            m_highlightPath.clear();
                            m_highlightPath.push_back(m_fullSearchPath[0]);
                            m_currentHighlightedNode = m_fullSearchPath[0];

                            updateAnimationText(m_ui, "Animation: finished", m_speedLabel);

                            if (m_searchFound)
                            {
                                setStatus(m_ui, "Found");
                            }
                            else {
                                setStatus(m_ui, "Not found");
                            }
                        }
                    }
                }
            }

            if (isButtonClicked(m_ui.deleteButton, mousePos))
            {
                bool isValid = false;
                int value = getInputValue(m_ui, isValid);

                if (isValid == false)
                {
                    setStatus(m_ui, "Invalid input");
                }
                else if (m_tree.search(value) == false)
                {
                    setStatus(m_ui, "Value does not exist");
                }
                else {
                    resetAnimation(m_highlightPath, m_fullSearchPath,
                                   m_currentAnimationStep, m_isPlaying,
                                   m_searchFound, m_ui);

                    resetInsertPathState(m_currentHighlightedNode, m_insertPath);
                    resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                         m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                    resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                         m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);

                    m_deleteSteps = m_tree.getDeleteSteps(value);
                    m_pendingDeleteValue = value;
                    m_currentDeleteStep = 0;
                    m_isDeletePlaying = true;

                    if (!m_deleteSteps.empty())
                    {
                        updateAnimationText(m_ui, "Animation: " + m_deleteSteps[0].action, m_speedLabel);
                    }
                    else {
                        updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                    }

                    m_animationClock.restart();
                    setStatus(m_ui, "Delete animation started");
                }
            }

            if (isButtonClicked(m_ui.updateButton, mousePos))
            {
                int oldValue = 0;
                int newValue = 0;

                if (parseUpdateInput(m_ui.inputBuffer, oldValue, newValue) == false)
                {
                    setStatus(m_ui, "Invalid update format");
                }
                else {
                    if (m_tree.update(oldValue, newValue))
                    {
                        setStatus(m_ui, "Update successful");
                        m_currentSnapshot = buildSnapshotFromTree(m_tree);
                        clearSnapshotHighlight(m_currentSnapshot);
                        m_ui.inputBuffer = "";
                        m_ui.inputText.setString("");

                        resetInsertPathState(m_currentHighlightedNode, m_insertPath);

                        resetAnimation(m_highlightPath, m_fullSearchPath,
                                       m_currentAnimationStep, m_isPlaying,
                                       m_searchFound, m_ui);

                        resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                             m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                        resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                             m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);
                    }
                    else {
                        setStatus(m_ui, "Update failed");
                    }
                }
            }

            if (isButtonClicked(m_ui.resetButton, mousePos))
            {
                m_tree.clear();
                m_currentSnapshot = buildSnapshotFromTree(m_tree);
                clearSnapshotHighlight(m_currentSnapshot);
                m_ui.inputBuffer = "";
                m_ui.inputText.setString("");
                setStatus(m_ui, "Tree reset");

                resetInsertPathState(m_currentHighlightedNode, m_insertPath);

                resetAnimation(m_highlightPath, m_fullSearchPath,
                               m_currentAnimationStep, m_isPlaying,
                               m_searchFound, m_ui);

                resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                     m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                     m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);
            }

            if (isButtonClicked(m_ui.initRandomButton, mousePos))
            {
                initializeRandomTree(m_tree);
                m_currentSnapshot = buildSnapshotFromTree(m_tree);
                clearSnapshotHighlight(m_currentSnapshot);

                m_ui.inputBuffer = "";
                m_ui.inputText.setString("");
                setStatus(m_ui, "Random tree initialized");

                resetInsertPathState(m_currentHighlightedNode, m_insertPath);

                resetAnimation(m_highlightPath, m_fullSearchPath,
                               m_currentAnimationStep, m_isPlaying,
                               m_searchFound, m_ui);

                resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                     m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                     m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);
            }

            if (isButtonClicked(m_ui.initSampleButton, mousePos))
            {
                initializeSampleTree(m_tree);
                m_currentSnapshot = buildSnapshotFromTree(m_tree);
                clearSnapshotHighlight(m_currentSnapshot);

                m_ui.inputBuffer = "";
                m_ui.inputText.setString("");
                setStatus(m_ui, "Sample tree initialized");

                resetInsertPathState(m_currentHighlightedNode, m_insertPath);

                resetAnimation(m_highlightPath, m_fullSearchPath,
                               m_currentAnimationStep, m_isPlaying,
                               m_searchFound, m_ui);

                resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                     m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                     m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);
            }

            if (isButtonClicked(m_ui.playButton, mousePos))
            {
                if (!m_insertSteps.empty() && m_currentInsertStep >= 0 && m_currentInsertStep + 1 < (int)m_insertSteps.size())
                {
                    m_isInsertPlaying = true;
                    updateAnimationText(m_ui, "Animation: playing insert", m_speedLabel);
                    m_animationClock.restart();
                }
                else if (!m_deleteSteps.empty() && m_currentDeleteStep >= 0 && m_currentDeleteStep + 1 < (int)m_deleteSteps.size())
                {
                    m_isDeletePlaying = true;
                    updateAnimationText(m_ui, "Animation: playing delete", m_speedLabel);
                    m_animationClock.restart();
                }
                else if (!m_fullSearchPath.empty() && m_currentAnimationStep + 1 < (int)m_fullSearchPath.size())
                {
                    m_isPlaying = true;
                    updateAnimationText(m_ui, "Animation: playing", m_speedLabel);
                    m_animationClock.restart();
                }
            }

            if (isButtonClicked(m_ui.pauseButton, mousePos))
            {
                m_isPlaying = false;
                m_isInsertPlaying = false;
                m_isDeletePlaying = false;
                updateAnimationText(m_ui, "Animation: paused", m_speedLabel);
            }

            if (isButtonClicked(m_ui.slowButton, mousePos))
            {
                m_animationDelay = 1.2f;
                m_speedLabel = "Slow";
                updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                setStatus(m_ui, "Speed set to slow");
            }

            if (isButtonClicked(m_ui.normalButton, mousePos))
            {
                m_animationDelay = 0.8f;
                m_speedLabel = "Normal";
                updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                setStatus(m_ui, "Speed set to normal");
            }

            if (isButtonClicked(m_ui.fastButton, mousePos))
            {
                m_animationDelay = 0.35f;
                m_speedLabel = "Fast";
                updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                setStatus(m_ui, "Speed set to fast");
            }
        }
    }
    else if (event.type == sf::Event::TextEntered)
    {
        if (m_ui.isTyping)
        {
            handleTextEntered(m_ui, event.text.unicode);
        }
    }
    else if (event.type == sf::Event::KeyPressed)
    {
        if (m_ui.isTyping && event.key.code == sf::Keyboard::Enter)
        {
            bool isValid = false;
            int value = getInputValue(m_ui, isValid);

            if (isValid == false)
            {
                setStatus(m_ui, "Invalid input");
            }
            else if (m_tree.search(value))
            {
                setStatus(m_ui, "Value already exists");
            }
            else {
                resetAnimation(m_highlightPath, m_fullSearchPath,
                               m_currentAnimationStep, m_isPlaying,
                               m_searchFound, m_ui);

                resetDeleteAnimation(m_deleteSteps, m_currentDeleteStep,
                                     m_isDeletePlaying, m_pendingDeleteValue, m_ui, m_speedLabel);

                resetInsertPathState(m_currentHighlightedNode, m_insertPath);
                resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                     m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                m_insertPath = m_tree.getInsertPath(value);
                m_insertSteps = m_tree.getInsertSteps(value);
                m_currentSequence = buildInsertSequence(m_tree, value);
                m_currentSequenceIndex = 0;

                m_pendingInsertValue = value;
                m_currentInsertStep = 0;
                m_isInsertPlaying = true;

                if (!m_insertPath.empty())
                {
                    m_highlightPath.clear();
                    m_highlightPath.push_back(m_insertPath[0]);
                    m_currentHighlightedNode = m_insertPath[0];
                }

                if (!m_insertSteps.empty())
                {
                    updateAnimationText(m_ui, "Animation: " + m_insertSteps[0].action, m_speedLabel);
                }
                else {
                    updateAnimationText(m_ui, "Animation: idle", m_speedLabel);
                }

                m_animationClock.restart();
                setStatus(m_ui, "Insert animation started");
            }
        }
    }
}

void Tree23State::update(float dt)
{
    (void)dt;

    if (m_isInsertPlaying)
    {
        if (m_animationClock.getElapsedTime().asSeconds() >= m_animationDelay)
        {
            m_animationClock.restart();

            if (m_currentInsertStep + 1 < (int)m_insertSteps.size())
            {
                m_currentInsertStep++;
            }

            if (m_currentSequenceIndex + 1 < (int)m_currentSequence.frames.size())
            {
                m_snapshotFrom = m_currentSnapshot;
                m_snapshotTo = m_currentSequence.frames[m_currentSequenceIndex + 1];

                m_isSnapshotTransitionPlaying = true;
                m_snapshotTransitionProgress = 0.f;
                m_snapshotTransitionClock.restart();

                m_currentSequenceIndex++;

                if (m_currentSequenceIndex < (int)m_currentSequence.labels.size())
                {
                    updateAnimationText(m_ui, "Animation: " + m_currentSequence.labels[m_currentSequenceIndex], m_speedLabel);
                }
            }
            else {
                m_tree.insert(m_pendingInsertValue);
                m_currentSnapshot = buildSnapshotFromTree(m_tree);

                m_ui.inputBuffer = "";
                m_ui.inputText.setString("");
                setStatus(m_ui, "Insert successful");

                resetInsertPathState(m_currentHighlightedNode, m_insertPath);
                resetInsertAnimation(m_insertSteps, m_currentInsertStep,
                                     m_isInsertPlaying, m_pendingInsertValue, m_ui, m_speedLabel);

                m_highlightPath.clear();
                m_currentSequence.frames.clear();
                m_currentSequence.labels.clear();
                m_currentSequenceIndex = 0;

                clearSnapshotHighlight(m_currentSnapshot);
                updateAnimationText(m_ui, "Animation: finished insert", m_speedLabel);
            }
        }
    }

    if (m_isDeletePlaying)
    {
        if (m_animationClock.getElapsedTime().asSeconds() >= m_animationDelay)
        {
            m_animationClock.restart();

            if (m_currentDeleteStep + 1 < (int)m_deleteSteps.size())
            {
                m_currentDeleteStep++;
                updateAnimationText(m_ui, "Animation: " + m_deleteSteps[m_currentDeleteStep].action, m_speedLabel);
            }
            else {
                m_tree.remove(m_pendingDeleteValue);
                m_currentSnapshot = buildSnapshotFromTree(m_tree);

                m_ui.inputBuffer = "";
                m_ui.inputText.setString("");
                setStatus(m_ui, "Delete successful");
                m_currentHighlightedNode = nullptr;

                m_deleteSteps.clear();
                m_currentDeleteStep = -1;
                m_isDeletePlaying = false;
                m_pendingDeleteValue = 0;
                updateAnimationText(m_ui, "Animation: finished delete", m_speedLabel);
                clearSnapshotHighlight(m_currentSnapshot);

                m_highlightPath.clear();
                m_fullSearchPath.clear();
                m_currentAnimationStep = -1;
                m_isPlaying = false;
                m_searchFound = false;
            }
        }
    }

    if (m_isPlaying)
    {
        if (m_animationClock.getElapsedTime().asSeconds() >= m_animationDelay)
        {
            m_animationClock.restart();

            if (m_currentAnimationStep + 1 < (int)m_fullSearchPath.size())
            {
                m_currentAnimationStep++;

                m_highlightPath.clear();
                m_highlightPath.push_back(m_fullSearchPath[m_currentAnimationStep]);
                m_currentHighlightedNode = m_fullSearchPath[m_currentAnimationStep];

                highlightSnapshotNodeByPath(
                    m_currentSnapshot,
                    buildPathIdFromSearchPath(m_fullSearchPath, m_currentAnimationStep)
                );
            }
            else {
                m_isPlaying = false;

                m_highlightPath.clear();
                if (!m_fullSearchPath.empty())
                {
                    m_highlightPath.push_back(m_fullSearchPath.back());
                    m_currentHighlightedNode = m_fullSearchPath.back();

                    highlightSnapshotNodeByPath(
                        m_currentSnapshot,
                        buildPathIdFromSearchPath(m_fullSearchPath, (int)m_fullSearchPath.size() - 1)
                    );
                }
                else {
                    m_currentHighlightedNode = nullptr;
                    clearSnapshotHighlight(m_currentSnapshot);
                }

                updateAnimationText(m_ui, "Animation: finished", m_speedLabel);

                if (m_searchFound)
                {
                    setStatus(m_ui, "Found");
                }
                else {
                    setStatus(m_ui, "Not found");
                }
            }
        }
    }

    if (m_isSnapshotTransitionPlaying)
    {
        float elapsed = m_snapshotTransitionClock.getElapsedTime().asSeconds();
        m_snapshotTransitionProgress = elapsed / m_snapshotTransitionDuration;

        if (m_snapshotTransitionProgress >= 1.f)
        {
            m_snapshotTransitionProgress = 1.f;
            m_isSnapshotTransitionPlaying = false;
            m_currentSnapshot = m_snapshotTo;
        }
    }
}

void Tree23State::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(230, 230, 230));

    drawUI(window, m_ui);

    if (m_isSnapshotTransitionPlaying)
    {
        TreeSnapshot interpolated = interpolateSnapshot(
            m_snapshotFrom,
            m_snapshotTo,
            m_snapshotTransitionProgress
        );
        drawTreeSnapshot(window, m_ui, interpolated);
    }
    else {
        drawTreeSnapshot(window, m_ui, m_currentSnapshot);
    }
}