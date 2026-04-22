#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Tree23.h"
#include "VisualizerUI.h"
#include <cstdlib>
#include <ctime>
#include<sstream>
#include "TreeSnapshot.h"

void updateAnimationText(VisualizerUI& ui, const std:: string& text, const std::string& speedLabel);


bool tryInsertFromInput(Tree23& tree, VisualizerUI& ui,
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

void resetAnimation(std::vector<Tree23Node*>& highlightPath,
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

void resetInsertAnimation(std::vector<Tree23Step>& insertSteps,
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

void resetDeleteAnimation(std::vector<Tree23Step>& deleteSteps,
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

void initializeRandomTree(Tree23& tree)
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

void initializeSampleTree(Tree23& tree)
{
    tree.clear();

    int values[6] = {2, 3, 4, 5, 7, 10};

    for (int i = 0; i < 6; i++)
    {
        tree.insert(values[i]);
    }
}

bool parseUpdateInput(const std:: string& s, int& oldValue, int& newValue)
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

void updateAnimationText(VisualizerUI& ui, const std::string& text, const std::string& speedLabel)
{
    ui.animationText.setString(text + " | Speed: " + speedLabel);
}

void resetInsertPathState(Tree23Node*& currentHighlightedNode,
                          std::vector<Tree23Node*>& insertPath)
{
    currentHighlightedNode = nullptr;
    insertPath.clear();
}

std::string buildNodeId(Tree23Node* node, int level, int index)
{
    return "L" + std::to_string(level) + "_" + std::to_string(index);
}

int countLeaves(Tree23Node* node)
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

std::string buildSnapshotRecursive(Tree23Node* node,
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

TreeSnapshot buildSnapshotFromTree(const Tree23& tree)
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
int main()
{
    srand((unsigned int)time(nullptr));

    sf::RenderWindow window(sf::VideoMode(1400, 800), "2-3 Tree Visualizer");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("assets/SF-Pro-Display-Regular.otf"))
    {
        return 0;
    }

    VisualizerUI ui;
    setupUI(ui, font, window.getSize());
    ui.animationText.setString("Animation: idle | Speed: Normal");

    Tree23 tree;
    TreeSnapshot currentSnapshot;
    TreeSnapshot snapshotFrom;
    TreeSnapshot snapshotTo;

    bool isSnapshotTransitionPlaying = false;
    float snapshotTransitionProgress = 0.f;
    float snapshotTransitionDuration = 0.45f;

    sf::Clock snapshotTransitionClock;
    currentSnapshot = buildSnapshotFromTree(tree);

    std::vector<Tree23Node*> highlightPath;
    std::vector<Tree23Node*> fullSearchPath;
    Tree23Node* currentHighlightedNode = nullptr;
    std::vector<Tree23Step> insertSteps;
    std::vector<Tree23Step> deleteSteps;
    std::vector<Tree23Node*> insertPath;

    bool searchFound = false;
    bool isPlaying = false;
    int currentAnimationStep = -1;
    bool isInsertPlaying = false;
    int currentInsertStep = -1;
    int pendingInsertValue = 0;
    bool isDeletePlaying = false;
    int currentDeleteStep = -1;
    int pendingDeleteValue = 0;

    sf::Clock animationClock;
    float animationDelay = 0.8f;
    std::string speedLabel = "Normal";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
                    );

                    if (isInside(ui.inputBox, mousePos))
                    {
                        ui.isTyping = true;
                    }
                    else {
                        ui.isTyping = false;
                    }

                    if (isButtonClicked(ui.insertButton, mousePos))
                    {
                        bool isValid = false;
                        int value = getInputValue(ui, isValid);

                        if (isValid == false)
                        {
                            setStatus(ui, "Invalid input");
                        }
                        else if (tree.search(value))
                        {
                            setStatus(ui, "Value already exists");
                        }
                        else {
                            resetAnimation(highlightPath, fullSearchPath,
                                           currentAnimationStep, isPlaying,
                                           searchFound, ui);

                            resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                                 isDeletePlaying, pendingDeleteValue, ui, speedLabel);

                            resetInsertPathState(currentHighlightedNode, insertPath);
                            resetInsertAnimation(insertSteps, currentInsertStep,
                                                 isInsertPlaying, pendingInsertValue, ui, speedLabel);

                            insertPath = tree.getInsertPath(value);
                            insertSteps = tree.getInsertSteps(value);

                            pendingInsertValue = value;
                            currentInsertStep = 0;
                            isInsertPlaying = true;

                            if (!insertPath.empty())
                            {
                                highlightPath.clear();
                                highlightPath.push_back(insertPath[0]);
                                currentHighlightedNode = insertPath[0];
                            }

                            if (!insertSteps.empty())
                            {
                                updateAnimationText(ui, "Animation: " + insertSteps[0].action, speedLabel);
                            }
                            else {
                                updateAnimationText(ui, "Animation: idle", speedLabel);
                            }

                            animationClock.restart();
                            setStatus(ui, "Insert animation started");
                        }
                    }

                    if (isButtonClicked(ui.searchButton, mousePos))
                    {
                        bool isValid = false;
                        int value = getInputValue(ui, isValid);

                        if (isValid == false)
                        {
                            setStatus(ui, "Invalid input");
                        }
                        else {
                            resetInsertPathState(currentHighlightedNode, insertPath);
                            resetInsertAnimation(insertSteps, currentInsertStep,
                                                 isInsertPlaying, pendingInsertValue, ui, speedLabel);

                            resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                                 isDeletePlaying, pendingDeleteValue, ui, speedLabel);

                            fullSearchPath = tree.getSearchPath(value, searchFound);
                            highlightPath.clear();
                            currentHighlightedNode = nullptr;

                            if (fullSearchPath.empty())
                            {
                                if (searchFound)
                                {
                                    setStatus(ui, "Found");
                                }
                                else {
                                    setStatus(ui, "Not found");
                                }

                                isPlaying = false;
                                currentAnimationStep = -1;
                                updateAnimationText(ui, "Animation: idle", speedLabel);
                            }
                            else {
                                currentAnimationStep = 0;
                                highlightPath.push_back(fullSearchPath[0]);
                                currentHighlightedNode = fullSearchPath[0];

                                if ((int)fullSearchPath.size() > 1)
                                {
                                    isPlaying = true;
                                    updateAnimationText(ui, "Animation: playing", speedLabel);
                                    animationClock.restart();
                                }
                                else {
                                    isPlaying = false;

                                    highlightPath.clear();
                                    highlightPath.push_back(fullSearchPath[0]);
                                    currentHighlightedNode = fullSearchPath[0];

                                    updateAnimationText(ui, "Animation: finished", speedLabel);

                                    if (searchFound)
                                    {
                                        setStatus(ui, "Found");
                                    }
                                    else {
                                        setStatus(ui, "Not found");
                                    }
                                }
                            }
                        }
                    }

                    if (isButtonClicked(ui.deleteButton, mousePos))
                    {
                        bool isValid = false;
                        int value = getInputValue(ui, isValid);

                        if (isValid == false)
                        {
                            setStatus(ui, "Invalid input");
                        }
                        else if (tree.search(value) == false)
                        {
                            setStatus(ui, "Value does not exist");
                        }
                        else {
                            resetAnimation(highlightPath, fullSearchPath,
                                           currentAnimationStep, isPlaying,
                                           searchFound, ui);

                            resetInsertPathState(currentHighlightedNode, insertPath);
                            resetInsertAnimation(insertSteps, currentInsertStep,
                                                 isInsertPlaying, pendingInsertValue, ui, speedLabel);

                            resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                                 isDeletePlaying, pendingDeleteValue, ui, speedLabel);

                            deleteSteps = tree.getDeleteSteps(value);
                            pendingDeleteValue = value;
                            currentDeleteStep = 0;
                            isDeletePlaying = true;

                            if (!deleteSteps.empty())
                            {
                                updateAnimationText(ui, "Animation: " + deleteSteps[0].action, speedLabel);
                            }
                            else {
                                updateAnimationText(ui, "Animation: idle", speedLabel);
                            }

                            animationClock.restart();
                            setStatus(ui, "Delete animation started");
                        }
                    }

                    if (isButtonClicked(ui.updateButton, mousePos))
                    {
                        int oldValue = 0;
                        int newValue = 0;

                        if (parseUpdateInput(ui.inputBuffer, oldValue, newValue) == false)
                        {
                            setStatus(ui, "Invalid update format");
                        }
                        else {
                            if (tree.update(oldValue, newValue))
                            {
                                setStatus(ui, "Update successful");
                                currentSnapshot = buildSnapshotFromTree(tree);
                                ui.inputBuffer = "";
                                ui.inputText.setString("");

                                resetInsertPathState(currentHighlightedNode, insertPath);

                                resetAnimation(highlightPath, fullSearchPath,
                                               currentAnimationStep, isPlaying,
                                               searchFound, ui);

                                resetInsertAnimation(insertSteps, currentInsertStep,
                                                     isInsertPlaying, pendingInsertValue, ui, speedLabel);

                                resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                                     isDeletePlaying, pendingDeleteValue, ui, speedLabel);
                            }
                            else {
                            setStatus(ui, "Update failed");
                            }
                        }
                    }

                    if (isButtonClicked(ui.resetButton, mousePos))
                    {
                        tree.clear();
                        currentSnapshot = buildSnapshotFromTree(tree);
                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Tree reset");

                        resetInsertPathState(currentHighlightedNode, insertPath);

                        resetAnimation(highlightPath, fullSearchPath,
                                       currentAnimationStep, isPlaying,
                                       searchFound, ui);

                        resetInsertAnimation(insertSteps, currentInsertStep,
                                             isInsertPlaying, pendingInsertValue, ui, speedLabel);

                        resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui, speedLabel);
                    }

                    if (isButtonClicked(ui.initRandomButton, mousePos))
                    {
                        initializeRandomTree(tree);
                        currentSnapshot = buildSnapshotFromTree(tree);

                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Random tree initialized");

                        resetInsertPathState(currentHighlightedNode, insertPath);

                        resetAnimation(highlightPath, fullSearchPath,
                                       currentAnimationStep, isPlaying,
                                       searchFound, ui);

                        resetInsertAnimation(insertSteps, currentInsertStep,
                                             isInsertPlaying, pendingInsertValue, ui, speedLabel);

                        resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui, speedLabel);
                    }

                    if (isButtonClicked(ui.initSampleButton, mousePos))
                    {
                        initializeSampleTree(tree);
                        currentSnapshot = buildSnapshotFromTree(tree);

                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Sample tree initialized");

                        resetInsertPathState(currentHighlightedNode, insertPath);

                        resetAnimation(highlightPath, fullSearchPath,
                                       currentAnimationStep, isPlaying,
                                       searchFound, ui);

                        resetInsertAnimation(insertSteps, currentInsertStep,
                                             isInsertPlaying, pendingInsertValue, ui, speedLabel);

                        resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui, speedLabel);
                    }

                    if (isButtonClicked(ui.playButton, mousePos))
                    {
                        if (!insertSteps.empty() && currentInsertStep >= 0 && currentInsertStep + 1 < (int)insertSteps.size())
                        {
                            isInsertPlaying = true;
                            updateAnimationText(ui, "Animation: playing insert", speedLabel);
                            animationClock.restart();
                        }
                        else if (!deleteSteps.empty() && currentDeleteStep >= 0 && currentDeleteStep + 1 < (int)deleteSteps.size())
                        {
                            isDeletePlaying = true;
                            updateAnimationText(ui, "Animation: playing delete", speedLabel);
                            animationClock.restart();
                        }
                        else if (!fullSearchPath.empty() && currentAnimationStep + 1 < (int)fullSearchPath.size())
                        {
                            isPlaying = true;
                            updateAnimationText(ui, "Animation: playing", speedLabel);
                            animationClock.restart();
                        }
                    }

                    if (isButtonClicked(ui.pauseButton, mousePos))
                    {
                        isPlaying = false;
                        isInsertPlaying = false;
                        isDeletePlaying = false;
                        updateAnimationText(ui, "Animation: paused", speedLabel);
                    }

                    if (isButtonClicked(ui.slowButton, mousePos))
                    {
                        animationDelay = 1.2f;
                        speedLabel = "Slow";
                        updateAnimationText(ui, "Animation: idle", speedLabel);
                        setStatus(ui, "Speed set to slow");
                    }

                    if (isButtonClicked(ui.normalButton, mousePos))
                    {
                        animationDelay = 0.8f;
                        speedLabel = "Normal";
                        updateAnimationText(ui, "Animation: idle", speedLabel);
                        setStatus(ui, "Speed set to normal");
                    }

                    if (isButtonClicked(ui.fastButton, mousePos))
                    {
                        animationDelay = 0.35f;
                        speedLabel = "Fast";
                        updateAnimationText(ui, "Animation: idle", speedLabel);
                        setStatus(ui, "Speed set to fast");
                    }
                }
            }
            else if (event.type == sf::Event::TextEntered)
            {
                if (ui.isTyping)
                {
                    handleTextEntered(ui, event.text.unicode);
                }
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (ui.isTyping && event.key.code == sf::Keyboard::Enter)
                {
                    bool isValid = false;
                    int value = getInputValue(ui, isValid);

                    if (isValid == false)
                    {
                        setStatus(ui, "Invalid input");
                    }
                    else if (tree.search(value))
                    {
                        setStatus(ui, "Value already exists");
                    }
                    else {
                        resetAnimation(highlightPath, fullSearchPath,
                                       currentAnimationStep, isPlaying,
                                       searchFound, ui);

                        resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui, speedLabel);

                        resetInsertPathState(currentHighlightedNode, insertPath);
                        resetInsertAnimation(insertSteps, currentInsertStep,
                                             isInsertPlaying, pendingInsertValue, ui, speedLabel);

                        insertPath = tree.getInsertPath(value);
                        insertSteps = tree.getInsertSteps(value);

                        pendingInsertValue = value;
                        currentInsertStep = 0;
                        isInsertPlaying = true;

                        if (!insertPath.empty())
                        {
                            highlightPath.clear();
                            highlightPath.push_back(insertPath[0]);
                            currentHighlightedNode = insertPath[0];
                        }

                        if (!insertSteps.empty())
                        {
                            updateAnimationText(ui, "Animation: " + insertSteps[0].action, speedLabel);
                        }
                        else {
                            updateAnimationText(ui, "Animation: idle", speedLabel);
                        }

                        animationClock.restart();
                        setStatus(ui, "Insert animation started");
                    }
                }
            }
        }

        if (isInsertPlaying)
        {
            if (animationClock.getElapsedTime().asSeconds() >= animationDelay)
            {
                animationClock.restart();

                if (currentInsertStep + 1 < (int)insertSteps.size())
                {
                    currentInsertStep++;
                    updateAnimationText(ui, "Animation: " + insertSteps[currentInsertStep].action, speedLabel);

                    if (currentInsertStep < (int)insertPath.size())
                    {
                        highlightPath.clear();
                        highlightPath.push_back(insertPath[currentInsertStep]);
                        currentHighlightedNode = insertPath[currentInsertStep];
                    }
                }
                else {
                    snapshotFrom = currentSnapshot;

                    tree.insert(pendingInsertValue);

                    snapshotTo = buildSnapshotFromTree(tree);

                    isSnapshotTransitionPlaying = true;
                    snapshotTransitionProgress = 0.f;
                    snapshotTransitionClock.restart();

                    ui.inputBuffer = "";
                    ui.inputText.setString("");
                    setStatus(ui, "Insert successful");

                    resetInsertPathState(currentHighlightedNode, insertPath);
                    resetInsertAnimation(insertSteps, currentInsertStep,
                                         isInsertPlaying, pendingInsertValue, ui, speedLabel);

                    highlightPath.clear();
                    updateAnimationText(ui, "Animation: finished insert", speedLabel);
                }
            }
        }

        if (isDeletePlaying)
        {
            if (animationClock.getElapsedTime().asSeconds() >= animationDelay)
            {
                animationClock.restart();

                if (currentDeleteStep + 1 < (int)deleteSteps.size())
                {
                    currentDeleteStep++;
                    updateAnimationText(ui, "Animation: " + deleteSteps[currentDeleteStep].action, speedLabel);
                }
                else {
                    tree.remove(pendingDeleteValue);
                    currentSnapshot = buildSnapshotFromTree(tree);

                    ui.inputBuffer = "";
                    ui.inputText.setString("");
                    setStatus(ui, "Delete successful");
                    currentHighlightedNode = nullptr;

                    deleteSteps.clear();
                    currentDeleteStep = -1;
                    isDeletePlaying = false;
                    pendingDeleteValue = 0;
                    updateAnimationText(ui, "Animation: finished delete", speedLabel);

                    highlightPath.clear();
                    fullSearchPath.clear();
                    currentAnimationStep = -1;
                    isPlaying = false;
                    searchFound = false;
                }
            }
        }

        if (isPlaying)
        {
            if (animationClock.getElapsedTime().asSeconds() >= animationDelay)
            {
                animationClock.restart();

                if (currentAnimationStep + 1 < (int)fullSearchPath.size())
                {
                    currentAnimationStep++;

                    highlightPath.clear();
                    highlightPath.push_back(fullSearchPath[currentAnimationStep]);
                    currentHighlightedNode = fullSearchPath[currentAnimationStep];
                }
                else {
                    isPlaying = false;

                    highlightPath.clear();
                    if (!fullSearchPath.empty())
                    {
                        highlightPath.push_back(fullSearchPath.back());
                        currentHighlightedNode = fullSearchPath.back();
                    }
                    else {
                        currentHighlightedNode = nullptr;
                    }

                    updateAnimationText(ui, "Animation: finished", speedLabel);

                    if (searchFound)
                    {
                        setStatus(ui, "Found");
                    }
                    else {
                        setStatus(ui, "Not found");
                    }
                }
            }
        }
        
        if (isSnapshotTransitionPlaying)
        {
            float elapsed = snapshotTransitionClock.getElapsedTime().asSeconds();
            snapshotTransitionProgress = elapsed / snapshotTransitionDuration;

            if (snapshotTransitionProgress >= 1.f)
            {
                snapshotTransitionProgress = 1.f;
                isSnapshotTransitionPlaying = false;
                currentSnapshot = snapshotTo;
            }
        }

        window.clear();
        drawUI(window, ui);
        if (isSnapshotTransitionPlaying)
        {
            TreeSnapshot interpolated = interpolateSnapshot(snapshotFrom, snapshotTo, snapshotTransitionProgress);
            drawTreeSnapshot(window, ui, interpolated, currentHighlightedNode);
        }
        else {
            drawTreeSnapshot(window, ui, currentSnapshot, currentHighlightedNode);
        }
        window.display();
    }

    return 0;
}
