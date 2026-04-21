#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Tree23.h"
#include "VisualizerUI.h"
#include <cstdlib>
#include <ctime>

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
                          VisualizerUI& ui)
{
    insertSteps.clear();
    currentInsertStep = -1;
    isInsertPlaying = false;
    pendingInsertValue = 0;
    ui.animationText.setString("Animation: idle");
}

void resetDeleteAnimation(std::vector<Tree23Step>& deleteSteps,
                          int& currentDeleteStep,
                          bool& isDeletePlaying,
                          int& pendingDeleteValue,
                          VisualizerUI& ui)
{
    deleteSteps.clear();
    currentDeleteStep = -1;
    isDeletePlaying = false;
    pendingDeleteValue = 0;
    ui.animationText.setString("Animation: idle");
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

    Tree23 tree;

    std::vector<Tree23Node*> highlightPath;
    std::vector<Tree23Node*> fullSearchPath;
    std::vector<Tree23Step> insertSteps;
    std::vector<Tree23Step> deleteSteps;

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
                            insertSteps = tree.getInsertSteps(value);
                            pendingInsertValue = value;
                            currentInsertStep = 0;
                            isInsertPlaying = true;

                            if (!insertSteps.empty())
                            {
                                ui.animationText.setString("Animation: " + insertSteps[0].action);
                            }
                            else {
                                ui.animationText.setString("Animation: idle");
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
                            fullSearchPath = tree.getSearchPath(value, searchFound);
                            highlightPath.clear();

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
                                ui.animationText.setString("Animation: idle");
                            }
                            else {
                                currentAnimationStep = 0;
                                highlightPath.push_back(fullSearchPath[0]);

                                if ((int)fullSearchPath.size() > 1)
                                {
                                    isPlaying = true;
                                    ui.animationText.setString("Animation: playing");
                                    animationClock.restart();
                                }
                                else {
                                    isPlaying = false;
                                    ui.animationText.setString("Animation: finished");

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
                            deleteSteps = tree.getDeleteSteps(value);
                            pendingDeleteValue = value;
                            currentDeleteStep = 0;
                            isDeletePlaying = true;

                            if (!deleteSteps.empty())
                            {
                                ui.animationText.setString("Animation: " + deleteSteps[0].action);
                            }
                            else {
                                ui.animationText.setString("Animation: idle");
                            }

                            animationClock.restart();
                            setStatus(ui, "Delete animation started");
                        }
                    }

                    if (isButtonClicked(ui.resetButton, mousePos))
                    {
                        tree.clear();
                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Tree reset");
                        resetAnimation(highlightPath, fullSearchPath,
                                       currentAnimationStep, isPlaying,
                                       searchFound, ui);
                        resetInsertAnimation(insertSteps, currentInsertStep, 
                                             isInsertPlaying, pendingInsertValue, ui);
                        resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui);
                    }

                    if (isButtonClicked(ui.initRandomButton, mousePos))
                    {
                        initializeRandomTree(tree);

                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Random tree initialized");

                        highlightPath.clear();
                        fullSearchPath.clear();
                        currentAnimationStep = -1;
                        isPlaying = false;
                        searchFound = false;

                        resetInsertAnimation(insertSteps, currentInsertStep,
                                             isInsertPlaying, pendingInsertValue, ui);

                        resetDeleteAnimation(deleteSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui);
                    }

                    if (isButtonClicked(ui.initSampleButton, mousePos))
                    {
                        initializeSampleTree(tree);

                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Sample tree initialized");

                        highlightPath.clear();
                        fullSearchPath.clear();
                        currentAnimationStep = -1;
                        isPlaying = false;
                        searchFound = false;

                        resetInsertAnimation(insertSteps, currentInsertStep,
                                             isInsertPlaying, pendingInsertValue, ui);

                        resetDeleteAnimation(insertSteps, currentDeleteStep,
                                             isDeletePlaying, pendingDeleteValue, ui);
                    }

                    if (isButtonClicked(ui.playButton, mousePos))
                    {
                        if (!insertSteps.empty() && currentInsertStep >= 0 && currentInsertStep + 1 < (int)insertSteps.size())
                        {
                            isInsertPlaying = true;
                            ui.animationText.setString("Animation: playing insert");
                            animationClock.restart();
                        }
                        else if (!deleteSteps.empty() && currentDeleteStep >= 0 && currentDeleteStep + 1 < (int)deleteSteps.size())
                        {
                            isDeletePlaying = true;
                            ui.animationText.setString("Animation: playing delete");
                            animationClock.restart();
                        }
                        else if (!fullSearchPath.empty() && currentAnimationStep + 1 < (int)fullSearchPath.size())
                        {
                            isPlaying = true;
                            ui.animationText.setString("Animation: playing");
                            animationClock.restart();
                        }
                    }

                    if (isButtonClicked(ui.pauseButton, mousePos))
                    {
                        isPlaying = false;
                        isInsertPlaying = false;
                        isDeletePlaying = false;
                        ui.animationText.setString("Animation: paused");
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
                        insertSteps = tree.getInsertSteps(value);
                        pendingInsertValue = value;
                        currentInsertStep = 0;
                        isInsertPlaying = true;

                        if (!insertSteps.empty())
                        {
                            ui.animationText.setString("Animation: " + insertSteps[0].action);
                        }
                        else {
                            ui.animationText.setString("Animation: idle");
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
                    ui.animationText.setString("Animation: " + insertSteps[currentInsertStep].action);
                }
                else {
                    tree.insert(pendingInsertValue);

                    ui.inputBuffer = "";
                    ui.inputText.setString("");
                    setStatus(ui, "Insert successful");

                    insertSteps.clear();
                    currentInsertStep = -1;
                    isInsertPlaying = false;
                    pendingInsertValue = 0;
                    ui.animationText.setString("Animation: finished insert");

                    deleteSteps.clear();
                    currentDeleteStep = -1;
                    isDeletePlaying = false;
                    pendingDeleteValue = 0;
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
                    ui.animationText.setString("Animation: " + deleteSteps[currentDeleteStep].action);
                }
                else {
                    tree.remove(pendingDeleteValue);

                    ui.inputBuffer = "";
                    ui.inputText.setString("");
                    setStatus(ui, "Delete successful");

                    deleteSteps.clear();
                    currentDeleteStep = -1;
                    isDeletePlaying = false;
                    pendingDeleteValue = 0;
                    ui.animationText.setString("Animation: finished delete");

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

                    for (int i = 0; i <= currentAnimationStep; i++)
                    {
                        highlightPath.push_back(fullSearchPath[i]);
                    }
                }
                else {
                    isPlaying = false;
                    ui.animationText.setString("Animation: finished");

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

        window.clear();
        drawUI(window, ui);
        drawTreeVisual(window, ui, tree, highlightPath, searchFound);
        window.display();
    }

    return 0;
}