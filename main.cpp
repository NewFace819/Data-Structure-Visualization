#include <SFML/Graphics.hpp>
#include <vector>
#include "Tree23.h"
#include "VisualizerUI.h"

bool tryInsertFromInput(Tree23& tree, VisualizerUI& ui,
                        std::vector<Tree23Node*>& highlightPath, bool& searchFound)
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
        return true;
    }

    setStatus(ui, "Value already exists");
    return false;
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
    bool searchFound = false;

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
                        tryInsertFromInput(tree, ui, highlightPath, searchFound);
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
                            highlightPath = tree.getSearchPath(value, searchFound);

                            if (searchFound)
                            {
                                setStatus(ui, "Found");
                            }
                            else {
                                setStatus(ui, "Not found");
                            }
                        }
                    }

                    if (isButtonClicked(ui.resetButton, mousePos))
                    {
                        tree.clear();
                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Tree reset");
                        highlightPath.clear();
                        searchFound = false;
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
                    tryInsertFromInput(tree, ui, highlightPath, searchFound);
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