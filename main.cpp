#include <SFML/Graphics.hpp>
#include <vector>
#include "Tree23.h"
#include "VisualizerUI.h"

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

int main()
{
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

    bool searchFound = false;
    bool isPlaying = false;
    int currentAnimationStep = -1;

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
                        tryInsertFromInput(tree, ui, highlightPath, searchFound,
                                           fullSearchPath, currentAnimationStep, isPlaying);
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
                        else {
                            if (tree.remove(value))
                            {
                                setStatus(ui, "Delete successful");
                                ui.inputBuffer = "";
                                ui.inputText.setString("");
                                resetAnimation(highlightPath, fullSearchPath,
                                               currentAnimationStep, isPlaying,
                                               searchFound, ui);
                            }
                            else {
                                setStatus(ui, "Value does not exist");
                            }
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
                    }

                    if (isButtonClicked(ui.playButton, mousePos))
                    {
                        if (!fullSearchPath.empty() && currentAnimationStep + 1 < (int)fullSearchPath.size())
                        {
                            isPlaying = true;
                            ui.animationText.setString("Animation: playing");
                            animationClock.restart();
                        }
                    }

                    if (isButtonClicked(ui.pauseButton, mousePos))
                    {
                        isPlaying = false;
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
                    tryInsertFromInput(tree, ui, highlightPath, searchFound,
                                       fullSearchPath, currentAnimationStep, isPlaying);
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