#pragma once
#include "VisualizerState.h"
#include <vector>
#include <string>

struct ListNode {
    int value;
    sf::Vector2f position;
    sf::CircleShape circle;
    sf::Text text;
    ListNode* next;

    ListNode(int val, sf::Vector2f pos, const sf::Font& font) 
        : value(val), position(pos), next(nullptr) 
    {
        circle.setRadius(20.0f);
        circle.setOrigin(20.0f, 20.0f);
        circle.setPosition(position);
        circle.setFillColor(sf::Color(52, 152, 219)); // Soft Blue
        circle.setOutlineThickness(2.0f);
        circle.setOutlineColor(sf::Color(41, 128, 185));

        text.setFont(font);
        text.setString(std::to_string(value));
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::White);
        // Center text
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        text.setPosition(position);
    }
};

class LinkedListGroup : public VisualizerState
{
public:
    LinkedListGroup(App* app, const std::string& dsName);
    ~LinkedListGroup();

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    struct StepState {
        std::vector<int> listValues;
        int codeBlockId;
        int codeHighlightLine;
        int activeNodeIndex;
        int foundNodeIndex;
    };
    
    ListNode* m_head;
    std::vector<StepState> m_history;
    std::vector<int> m_logicalList;
    int m_currentStep = 0;

    void initData(const std::string& input);
    void addNode(int val);
    void deleteNodeByValue(int val);
    void searchNode(int val);
    void updateNode(const std::string& input);
    
    void prepNewOperation();
    void pushStep(int blockId, int highlightLine, int activeIdx, int foundIdx = -1);
    void loadState(int index);
    void clearList();

    void stepBack() override;
    void stepForward() override;
    
    void drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end) const;
    
    // Cached shapes for drawArrow to avoid per-frame heap allocation
    mutable sf::RectangleShape m_arrowLine;
    mutable sf::ConvexShape   m_arrowHead;
};
