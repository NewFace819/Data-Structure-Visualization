#include "VisualizerUI.h"
#include <sstream>
#include <vector>
#include <cmath>

VisualizerUI::VisualizerUI()
{
    inputBuffer = "";
    isTyping = false;
}

void setupButton(ToolbarButton& button, const sf::Font& font, const std::string& text,
                 sf::Vector2f size, sf::Vector2f position)
{
    button.box.setSize(size);
    button.box.setPosition(position);
    button.box.setFillColor(sf::Color(70, 70, 90));
    button.box.setOutlineThickness(2.f);
    button.box.setOutlineColor(sf::Color::White);

    button.label.setFont(font);
    button.label.setCharacterSize(22);
    button.label.setString(text);
    button.label.setFillColor(sf::Color::White);

    sf::FloatRect bounds = button.label.getLocalBounds();
    button.label.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    button.label.setPosition(
        position.x + size.x / 2.f,
        position.y + size.y / 2.f
    );
}

void setupUI(VisualizerUI& ui, const sf::Font& font, sf::Vector2u windowSize)
{
    float leftWidth = 320.f;
    float margin = 20.f;

    ui.leftPanel.setSize(sf::Vector2f(leftWidth, (float)windowSize.y));
    ui.leftPanel.setPosition(sf::Vector2f(0.f, 0.f));
    ui.leftPanel.setFillColor(sf::Color(35, 35, 50));

    ui.rightPanel.setSize(sf::Vector2f((float)windowSize.x - leftWidth, (float)windowSize.y));
    ui.rightPanel.setPosition(sf::Vector2f(leftWidth, 0.f));
    ui.rightPanel.setFillColor(sf::Color(245, 245, 245));

    ui.titleText.setFont(font);
    ui.titleText.setCharacterSize(28);
    ui.titleText.setString("2-3 Tree Visualizer");
    ui.titleText.setFillColor(sf::Color::White);
    ui.titleText.setPosition(sf::Vector2f(margin, 20.f));

    ui.inputLabel.setFont(font);
    ui.inputLabel.setCharacterSize(20);
    ui.inputLabel.setString("Input value");
    ui.inputLabel.setFillColor(sf::Color::White);
    ui.inputLabel.setPosition(sf::Vector2f(margin, 90.f));

    ui.inputBox.setSize(sf::Vector2f(leftWidth - 2 * margin, 45.f));
    ui.inputBox.setPosition(sf::Vector2f(margin, 125.f));
    ui.inputBox.setFillColor(sf::Color::White);
    ui.inputBox.setOutlineThickness(2.f);
    ui.inputBox.setOutlineColor(sf::Color::Black);

    ui.inputText.setFont(font);
    ui.inputText.setCharacterSize(22);
    ui.inputText.setFillColor(sf::Color::Black);
    ui.inputText.setPosition(sf::Vector2f(margin + 10.f, 132.f));

setupButton(ui.insertButton, font, "Insert",
            sf::Vector2f(leftWidth - 2 * margin, 50.f),
            sf::Vector2f(margin, 190.f));

setupButton(ui.searchButton, font, "Search",
            sf::Vector2f(leftWidth - 2 * margin, 50.f),
            sf::Vector2f(margin, 255.f));

setupButton(ui.deleteButton, font, "Delete",
            sf::Vector2f(leftWidth - 2 * margin, 50.f),
            sf::Vector2f(margin, 320.f));

setupButton(ui.resetButton, font, "Reset",
            sf::Vector2f(leftWidth - 2 * margin, 50.f),
            sf::Vector2f(margin, 385.f));

setupButton(ui.initRandomButton, font, "Init Random",
            sf::Vector2f(leftWidth - 2 * margin, 50.f),
            sf::Vector2f(margin, 450.f));

setupButton(ui.initSampleButton, font, "Init Sample",
            sf::Vector2f(leftWidth - 2 * margin, 50.f),
            sf::Vector2f(margin, 515.f));

setupButton(ui.playButton, font, "Play",
            sf::Vector2f((leftWidth - 3 * margin) / 2.f, 45.f),
            sf::Vector2f(margin, 585.f));

setupButton(ui.pauseButton, font, "Pause",
            sf::Vector2f((leftWidth - 3 * margin) / 2.f, 45.f),
            sf::Vector2f(margin + (leftWidth - 3 * margin) / 2.f + margin, 585.f));

    ui.statusText.setFont(font);
    ui.statusText.setCharacterSize(18);
    ui.statusText.setFillColor(sf::Color(220, 220, 220));
    ui.statusText.setString("Ready");
    ui.statusText.setPosition(sf::Vector2f(margin, 655.f));

    ui.animationText.setFont(font);
    ui.animationText.setCharacterSize(18);
    ui.animationText.setFillColor(sf::Color(220, 220, 220));
    ui.animationText.setString("Animation: idle");
    ui.animationText.setPosition(sf::Vector2f(margin, 690.f));

    ui.treePlaceholderText.setFont(font);
    ui.treePlaceholderText.setCharacterSize(28);
    ui.treePlaceholderText.setFillColor(sf::Color(80, 80, 80));
    ui.treePlaceholderText.setString("Tree is empty");

    sf::FloatRect bounds = ui.treePlaceholderText.getLocalBounds();
    ui.treePlaceholderText.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    ui.treePlaceholderText.setPosition(
        sf::Vector2f(leftWidth + ((float)windowSize.x - leftWidth) / 2.f,
                     (float)windowSize.y / 2.f)
    );
}

void drawUI(sf::RenderWindow& window, VisualizerUI& ui)
{
    window.draw(ui.leftPanel);
    window.draw(ui.rightPanel);

    window.draw(ui.titleText);
    window.draw(ui.inputLabel);
    window.draw(ui.inputBox);
    window.draw(ui.inputText);

    window.draw(ui.insertButton.box);
    window.draw(ui.insertButton.label);

    window.draw(ui.searchButton.box);
    window.draw(ui.searchButton.label);

    window.draw(ui.deleteButton.box);
    window.draw(ui.deleteButton.label);

    window.draw(ui.resetButton.box);
    window.draw(ui.resetButton.label);

    window.draw(ui.initRandomButton.box);
    window.draw(ui.initRandomButton.label);

    window.draw(ui.initSampleButton.box);
    window.draw(ui.initSampleButton.label);

    window.draw(ui.playButton.box);
    window.draw(ui.playButton.label);

    window.draw(ui.pauseButton.box);
    window.draw(ui.pauseButton.label);

    window.draw(ui.statusText);
    window.draw(ui.animationText);
}

bool isInside(const sf::RectangleShape& box, sf::Vector2f mousePos)
{
    return box.getGlobalBounds().contains(mousePos);
}

bool isButtonClicked(const ToolbarButton& button, sf::Vector2f mousePos)
{
    return button.box.getGlobalBounds().contains(mousePos);
}

void handleTextEntered(VisualizerUI& ui, sf::Uint32 unicode)
{
    if (unicode == 8)
    {
        if (!ui.inputBuffer.empty())
        {
            ui.inputBuffer.pop_back();
        }
    }
    else if (unicode >= '0' && unicode <= '9')
    {
        ui.inputBuffer += (char)unicode;
    }

    ui.inputText.setString(ui.inputBuffer);
}

int getInputValue(const VisualizerUI& ui, bool& isValid)
{
    if (ui.inputBuffer.empty())
    {
        isValid = false;
        return 0;
    }

    std::stringstream ss(ui.inputBuffer);
    int value = 0;
    ss >> value;

    if (ss.fail())
    {
        isValid = false;
        return 0;
    }

    isValid = true;
    return value;
}

void setStatus(VisualizerUI& ui, const std::string& message)
{
    ui.statusText.setString(message);
}

bool isHighlighted(Tree23Node* node, const std::vector<Tree23Node*>& highlightPath)
{
    for (int i = 0; i < (int)highlightPath.size(); i++)
    {
        if (highlightPath[i] == node)
        {
            return true;
        }
    }

    return false;
}

void drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end)
{
    sf::Vertex line[] =
    {
        sf::Vertex(start, sf::Color::Black),
        sf::Vertex(end, sf::Color::Black)
    };
    window.draw(line, 2, sf::Lines);

    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float angle = std::atan2(dy, dx);

    float arrowSize = 10.f;

    sf::ConvexShape arrowHead;
    arrowHead.setPointCount(3);
    arrowHead.setPoint(0, end);
    arrowHead.setPoint(1, sf::Vector2f(
        end.x - arrowSize * std::cos(angle - 0.4f),
        end.y - arrowSize * std::sin(angle - 0.4f)
    ));
    arrowHead.setPoint(2, sf::Vector2f(
        end.x - arrowSize * std::cos(angle + 0.4f),
        end.y - arrowSize * std::sin(angle + 0.4f)
    ));
    arrowHead.setFillColor(sf::Color::Black);

    window.draw(arrowHead);
}

void drawNodeBox(sf::RenderWindow& window, const sf::Font& font, Tree23Node* node,
                 float centerX, float topY,
                 const std::vector<Tree23Node*>& highlightPath, bool found)
{
    float keyWidth = 55.f;
    float nodeHeight = 45.f;
    float totalWidth = keyWidth * (float)node->keyCount;
    float leftX = centerX - totalWidth / 2.f;

    sf::RectangleShape box;
    box.setSize(sf::Vector2f(totalWidth, nodeHeight));
    box.setPosition(sf::Vector2f(leftX, topY));

    if (isHighlighted(node, highlightPath))
    {
        if (found)
        {
            box.setFillColor(sf::Color(200, 255, 200));
        }
        else {
            box.setFillColor(sf::Color(255, 230, 180));
        }
    }
    else {
        box.setFillColor(sf::Color::White);
    }

    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color::Black);
    window.draw(box);

    if (node->keyCount == 2)
    {
        sf::Vertex divider[] =
        {
            sf::Vertex(sf::Vector2f(leftX + keyWidth, topY), sf::Color::Black),
            sf::Vertex(sf::Vector2f(leftX + keyWidth, topY + nodeHeight), sf::Color::Black)
        };
        window.draw(divider, 2, sf::Lines);
    }

    sf::Text text1;
    text1.setFont(font);
    text1.setCharacterSize(22);
    text1.setFillColor(sf::Color::Black);
    text1.setString(std::to_string(node->keys[0]));
    sf::FloatRect b1 = text1.getLocalBounds();
    text1.setOrigin(b1.left + b1.width / 2.f, b1.top + b1.height / 2.f);
    text1.setPosition(sf::Vector2f(leftX + keyWidth / 2.f, topY + nodeHeight / 2.f));
    window.draw(text1);

    if (node->keyCount == 2)
    {
        sf::Text text2;
        text2.setFont(font);
        text2.setCharacterSize(22);
        text2.setFillColor(sf::Color::Black);
        text2.setString(std::to_string(node->keys[1]));
        sf::FloatRect b2 = text2.getLocalBounds();
        text2.setOrigin(b2.left + b2.width / 2.f, b2.top + b2.height / 2.f);
        text2.setPosition(sf::Vector2f(leftX + keyWidth + keyWidth / 2.f, topY + nodeHeight / 2.f));
        window.draw(text2);
    }
}

void drawTreeRecursive(sf::RenderWindow& window, const sf::Font& font, Tree23Node* node,
                       float centerX, float topY, float offset,
                       const std::vector<Tree23Node*>& highlightPath, bool found)
{
    if (node == nullptr)
    {
        return;
    }

    float nodeHeight = 45.f;

    drawNodeBox(window, font, node, centerX, topY, highlightPath, found);

    float nextY = topY + 110.f;
    float nextOffset = offset * 0.55f;

    if (nextOffset < 70.f)
    {
        nextOffset = 70.f;
    }

    float parentBottomX = centerX;
    float parentBottomY = topY + nodeHeight;

    if (node->keyCount == 1)
    {
        if (node->child[0] != nullptr)
        {
            float childX = centerX - offset;
            drawArrow(window,
                      sf::Vector2f(parentBottomX, parentBottomY),
                      sf::Vector2f(childX, nextY));
            drawTreeRecursive(window, font, node->child[0], childX, nextY, nextOffset, highlightPath, found);
        }

        if (node->child[2] != nullptr)
        {
            float childX = centerX + offset;
            drawArrow(window,
                      sf::Vector2f(parentBottomX, parentBottomY),
                      sf::Vector2f(childX, nextY));
            drawTreeRecursive(window, font, node->child[2], childX, nextY, nextOffset, highlightPath, found);
        }
    }
    else
    {
        if (node->child[0] != nullptr)
        {
            float childX = centerX - offset;
            drawArrow(window,
                      sf::Vector2f(parentBottomX, parentBottomY),
                      sf::Vector2f(childX, nextY));
            drawTreeRecursive(window, font, node->child[0], childX, nextY, nextOffset, highlightPath, found);
        }

        if (node->child[1] != nullptr)
        {
            float childX = centerX;
            drawArrow(window,
                      sf::Vector2f(parentBottomX, parentBottomY),
                      sf::Vector2f(childX, nextY));
            drawTreeRecursive(window, font, node->child[1], childX, nextY, nextOffset, highlightPath, found);
        }

        if (node->child[2] != nullptr)
        {
            float childX = centerX + offset;
            drawArrow(window,
                      sf::Vector2f(parentBottomX, parentBottomY),
                      sf::Vector2f(childX, nextY));
            drawTreeRecursive(window, font, node->child[2], childX, nextY, nextOffset, highlightPath, found);
        }
    }
}

void drawTreeVisual(sf::RenderWindow& window, VisualizerUI& ui, const Tree23& tree,
                    const std::vector<Tree23Node*>& highlightPath, bool found)
{
    if (tree.isEmpty())
    {
        window.draw(ui.treePlaceholderText);
        return;
    }

    const sf::Font* font = ui.treePlaceholderText.getFont();
    if (font == nullptr)
    {
        return;
    }

    Tree23Node* root = tree.getRoot();
    if (root == nullptr)
    {
        window.draw(ui.treePlaceholderText);
        return;
    }

    float startX = 860.f;
    float startY = 90.f;
    float startOffset = 220.f;

    drawTreeRecursive(window, *font, root, startX, startY, startOffset, highlightPath, found);
}