#pragma once
#include "VisualizerState.h"
#include <vector>
#include <string>
#include <cmath>

struct ListNode {
    int value;
    sf::Vector2f position;
    sf::ConvexShape leftBg;
    sf::ConvexShape rightBg;
    sf::RectangleShape separator;
    sf::CircleShape pointerDot;
    sf::Text text;
    ListNode* next;
    float alpha = 255.0f; // Opacity
    bool isDragging = false;
    bool hasCustomPos = false; 
    sf::Vector2f targetPos;


    ListNode(int val, sf::Vector2f pos, const sf::Font& font) 
        : value(val), position(pos), next(nullptr) 
    {
        float width = 70.0f;
        float height = 40.0f;
        float radius = 5.0f;
        
        float divX = width * 0.7f;
        int pointsPerCorner = 8;
        
        // Left Bg (70%)
        leftBg.setPointCount(pointsPerCorner * 2 + 2);
        for (int i = 0; i < pointsPerCorner; ++i) { // TL
            float angle = 3.141592654f + i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
            leftBg.setPoint(i, sf::Vector2f(radius + radius * std::cos(angle), radius + radius * std::sin(angle)));
        }
        leftBg.setPoint(pointsPerCorner, sf::Vector2f(divX, 0.0f)); // TR(sq)
        leftBg.setPoint(pointsPerCorner + 1, sf::Vector2f(divX, height)); // BR(sq)
        for (int i = 0; i < pointsPerCorner; ++i) { // BL
            float angle = 3.141592654f / 2.0f + i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
            leftBg.setPoint(pointsPerCorner + 2 + i, sf::Vector2f(radius + radius * std::cos(angle), height - radius + radius * std::sin(angle)));
        }
        leftBg.setOrigin(width / 2.0f, height / 2.0f);
        leftBg.setFillColor(sf::Color(60, 60, 60)); // Dim
        leftBg.setOutlineThickness(2.0f);
        leftBg.setOutlineColor(sf::Color(100, 100, 100));

        // Right Bg (30%)
        rightBg.setPointCount(pointsPerCorner * 2 + 2);
        rightBg.setPoint(0, sf::Vector2f(divX, 0.0f)); // TL(sq)
        for (int i = 0; i < pointsPerCorner; ++i) { // TR
            float angle = 3.141592654f * 1.5f + i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
            rightBg.setPoint(1 + i, sf::Vector2f(width - radius + radius * std::cos(angle), radius + radius * std::sin(angle)));
        }
        for (int i = 0; i < pointsPerCorner; ++i) { // BR
            float angle = i * (3.141592654f / 2.0f) / (pointsPerCorner - 1);
            rightBg.setPoint(1 + pointsPerCorner + i, sf::Vector2f(width - radius + radius * std::cos(angle), height - radius + radius * std::sin(angle)));
        }
        rightBg.setPoint(1 + pointsPerCorner * 2, sf::Vector2f(divX, height)); // BL(sq)
        
        rightBg.setOrigin(width / 2.0f, height / 2.0f);
        rightBg.setFillColor(sf::Color::White); // Bright
        rightBg.setOutlineThickness(2.0f);
        rightBg.setOutlineColor(sf::Color(100, 100, 100));

        separator.setSize(sf::Vector2f(2.0f, height));
        separator.setOrigin(1.0f, height / 2.0f);
        separator.setFillColor(sf::Color(100, 100, 100));

        pointerDot.setRadius(3.0f);
        pointerDot.setOrigin(3.0f, 3.0f);
        pointerDot.setFillColor(sf::Color(100, 100, 100));

        text.setFont(font);
        text.setString(std::to_string(value));
        text.setCharacterSize(14);
        text.setFillColor(sf::Color::White);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

        updatePosition(position);
    }

    bool contains(sf::Vector2f pt) const {
        return leftBg.getGlobalBounds().contains(pt) || rightBg.getGlobalBounds().contains(pt);
    }

    void updatePosition(sf::Vector2f pos) {
        position = pos;
        float width = 70.0f;
        float divX = width * 0.7f;
        
        leftBg.setPosition(position);
        rightBg.setPosition(position);
        separator.setPosition(position.x - width/2.0f + divX, position.y);
        pointerDot.setPosition(position.x - width/2.0f + divX + (width - divX)/2.0f, position.y);
        text.setPosition(position.x - width/2.0f + divX/2.0f, position.y);
    }

    // Set alpha
    void setAlpha(uint8_t ua) {
        sf::Color c;
        c = leftBg.getFillColor();    c.a = ua; leftBg.setFillColor(c);
        c = leftBg.getOutlineColor(); c.a = ua; leftBg.setOutlineColor(c);
        c = rightBg.getFillColor();   c.a = ua; rightBg.setFillColor(c);
        c = rightBg.getOutlineColor();c.a = ua; rightBg.setOutlineColor(c);
        c = separator.getFillColor(); c.a = ua; separator.setFillColor(c);
        c = pointerDot.getFillColor();c.a = ua; pointerDot.setFillColor(c);
        c = text.getFillColor();      c.a = ua; text.setFillColor(c);
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
        std::string notificationMsg;
    };
    
    ListNode* m_head;
    ListNode* m_draggedNode = nullptr;
    sf::Vector2f m_dragOffset;
    std::vector<ListNode*> m_dyingNodes; // Fading
    std::vector<StepState> m_history;
    std::vector<int> m_logicalList;
    int m_currentStep = 0;
    
    Button m_clearBtn;

    void initData(const std::string& input);
    void addNode(int val);
    void deleteNodeByValue(int val);
    void searchNode(int val);
    void updateNode(const std::string& input);
    void rearrangeList();
    
    void prepNewOperation();
    void pushStep(int blockId, int highlightLine, int activeIdx, int foundIdx = -1, const std::string& msg = "");
    void loadState(int index);
    void clearList();

    void stepBack() override;
    void stepForward() override;
    
    void drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end) const;
    
    // Shape cache
    mutable sf::RectangleShape m_arrowLine;
    mutable sf::ConvexShape    m_arrowHead;
};
