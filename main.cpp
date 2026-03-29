#include <SFML/Graphics.hpp>
#include "Tree23.h"
#include "VisualizerUI.h"

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
                        else {
                            if (tree.insert(value))
                            {
                                setStatus(ui, "Insert successful");
                                ui.inputBuffer = "";
                                ui.inputText.setString("");
                            }
                            else {
                                setStatus(ui, "Value already exists");
                            }
                        }
                    }

                    if (isButtonClicked(ui.resetButton, mousePos))
                    {
                        tree.clear();
                        ui.inputBuffer = "";
                        ui.inputText.setString("");
                        setStatus(ui, "Tree reset");
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
        }

        window.clear();
        drawUI(window, ui);
        drawTreeVisual(window, ui, tree);
        window.display();
    }

    return 0;
}