#include "LinkedListGroup.h"
#include "App.h"
#include <iostream>
#include <math.h>
#include <sstream>
#include <fstream>

static const std::vector<std::vector<std::string>> CODE_BLOCKS = {
    // 0: ADD
    {
        "void addNode(int val) {",
        "    Node* n = new Node(val);",
        "    if (head == null) { head = n; return; }",
        "    Node* curr = head;",
        "    while (curr->next != null)",
        "        curr = curr->next;",
        "    curr->next = n;",
        "}"
    },
    // 1: DELETE
    {
        "void deleteNode(int val) {",
        "    if (head == null) return;",
        "    if (head->value == val) { head = head->next; return; }",
        "    Node* curr = head;",
        "    while (curr->next != null && curr->next->value != val)",
        "        curr = curr->next;",
        "    if (curr->next != null)",
        "        curr->next = curr->next->next;",
        "}"
    },
    // 2: SEARCH
    {
        "int searchNode(int val) {",
        "    Node* curr = head;",
        "    int index = 0;",
        "    while (curr != null) {",
        "        if (curr->value == val) return index;",
        "        curr = curr->next;",
        "        index++;",
        "    }",
        "    return -1;",
        "}"
    },
    // 3: UPDATE
    {
        "void updateNode(int oldVal, int newVal) {",
        "    Node* curr = head;",
        "    while (curr != null) {",
        "        if (curr->value == oldVal) {",
        "            curr->value = newVal;",
        "            return;",
        "        }",
        "        curr = curr->next;",
        "    }",
        "}"
    },
    // 4: INIT
    {
        "// Initialization",
        "void initList(vector<int> vals) {",
        "    head = null;",
        "    for (int v : vals)",
        "        addNode(v);",
        "}"
    }
};

LinkedListGroup::LinkedListGroup(App* app, const std::string& dsName)
    : VisualizerState(app, dsName), m_head(nullptr), m_clearBtn(1050.0f, 80.0f, 150.0f, 40.0f, "Clear List", app->getFont())
{
    // Pre-initialize cached arrow shapes to avoid per-frame reconstruction
    m_arrowLine.setFillColor(sf::Color(100, 100, 100));
    m_arrowLine.setOrigin(0, 1.5f);

    m_arrowHead.setPointCount(3);
    m_arrowHead.setPoint(0, sf::Vector2f(0.0f,   0.0f));
    m_arrowHead.setPoint(1, sf::Vector2f(-12.0f,  6.0f));
    m_arrowHead.setPoint(2, sf::Vector2f(-12.0f, -6.0f));
    m_arrowHead.setFillColor(sf::Color(100, 100, 100));

    m_clearBtn.setColors(sf::Color(231, 76, 60), sf::Color(192, 57, 43), sf::Color(142, 68, 173));
    m_clearBtn.setCallback([this]() { this->initData(""); });
}

LinkedListGroup::~LinkedListGroup()
{
    clearList();
}

void LinkedListGroup::init()
{
    VisualizerState::init();

    m_sidebar.setInitCallback([this](std::string val) { this->initData(val); });
    m_sidebar.setAddCallback([this](int val) { this->addNode(val); });
    m_sidebar.setDeleteCallback([this](int val) { this->deleteNodeByValue(val); });
    m_sidebar.setSearchCallback([this](int val) { this->searchNode(val); });
    m_sidebar.setUpdateCallback([this](std::string val) { this->updateNode(val); });
    m_sidebar.setRearrangeCallback([this]() { this->rearrangeList(); });

    initData("15,30,45");
}

void LinkedListGroup::prepNewOperation()
{
    if (m_currentStep >= 0 && m_currentStep < (int)m_history.size() - 1) {
        m_history.erase(m_history.begin() + m_currentStep + 1, m_history.end());
        if (!m_history.empty()) m_logicalList = m_history.back().listValues;
    }
}

void LinkedListGroup::pushStep(int blockId, int highlightLine, int activeIdx, int foundIdx, const std::string& msg)
{
    m_history.push_back({m_logicalList, blockId, highlightLine, activeIdx, foundIdx, msg});
}

void LinkedListGroup::initData(const std::string& input)
{
    m_history.clear();
    m_currentStep = 0;
    m_logicalList.clear();

    // Discard dying nodes
    for (auto* n : m_dyingNodes) delete n;
    m_dyingNodes.clear();
    
    std::vector<int> vals;
    if (input.empty()) {
        // empty
    } else if (input.find(',') != std::string::npos || input.find(' ') != std::string::npos) {
        std::string temp = input;
        for (char& c : temp) { if (c == ',') c = ' '; }
        std::stringstream ss(temp);
        std::string token;
        while(ss >> token) { try { vals.push_back(std::stoi(token)); } catch(...) {} }
    } else if (input.length() > 4 && input.substr(input.length() - 4) == ".txt") {
        std::ifstream file(input);
        std::string token;
        if(file.is_open()) while(file >> token) { try { vals.push_back(std::stoi(token)); } catch(...){} }
    } else {
        try { int N = std::stoi(input); for (int i=0; i<N; i++) vals.push_back(rand()%100); } catch(...) {}
    }

    m_logicalList = vals;
    pushStep(4, 4, -1);
    
    // Instantly load
    m_currentStep = 0;
    loadState(m_currentStep);

    // Force target pos
    float startX = 300.0f;
    float startY = 280.0f;
    float gapX = 100.0f;
    int idx = 0;
    ListNode* curr = m_head;
    while(curr) {
        curr->updatePosition(sf::Vector2f(startX + idx * gapX, startY));
        curr = curr->next;
        idx++;
    }

    m_isPaused = true;
    m_sidebar.setPlayButtonText("Play");
}

void LinkedListGroup::addNode(int val)
{
    prepNewOperation();
    pushStep(0, 0, -1); 
    pushStep(0, 1, -1); 
    if (m_logicalList.empty()) {
        pushStep(0, 2, -1); 
        m_logicalList.push_back(val);
        pushStep(0, 2, 0); 
    } else {
        pushStep(0, 3, 0); 
        int currIdx = 0;
        while (currIdx < (int)m_logicalList.size() - 1) {
            pushStep(0, 4, currIdx); 
            currIdx++;
            pushStep(0, 5, currIdx); 
        }
        pushStep(0, 4, currIdx); 
        m_logicalList.push_back(val);
        pushStep(0, 6, currIdx); 
    }
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
}

void LinkedListGroup::deleteNodeByValue(int val)
{
    prepNewOperation();
    pushStep(1, 0, -1); 
    if (m_logicalList.empty()) { 
        pushStep(1, 1, -1, -1, "List is empty!"); 
        m_isPaused = false; m_sidebar.setPlayButtonText("Pause"); 
        return; 
    }
    
    pushStep(1, 2, 0); 
    if (m_logicalList[0] == val) {
        m_logicalList.erase(m_logicalList.begin());
        pushStep(1, 2, -1); 
        m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
        return;
    }
    
    pushStep(1, 4, 0); 
    int currIdx = 0;
    bool found = false;
    while (currIdx < (int)m_logicalList.size() - 1) {
        pushStep(1, 4, currIdx); 
        if (m_logicalList[currIdx+1] == val) {
            found = true;
            break;
        }
        currIdx++;
        pushStep(1, 5, currIdx); 
    }
    
    if (found) {
        pushStep(1, 6, currIdx);
        m_logicalList.erase(m_logicalList.begin() + currIdx + 1);
        pushStep(1, 7, currIdx);
    } else {
        pushStep(1, 4, -1, -1, "Value " + std::to_string(val) + " not found!");
    }
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
}

void LinkedListGroup::searchNode(int val)
{
    prepNewOperation();
    pushStep(2, 0, -1);
    
    if (m_logicalList.empty()) {
        pushStep(2, 1, -1); 
        pushStep(2, 3, -1); 
        pushStep(2, 8, -1, -1, "List is empty!"); 
        m_isPaused = false; m_sidebar.setPlayButtonText("Pause"); return;
    }
    pushStep(2, 1, 0);
    pushStep(2, 2, 0);

    int currIdx = 0;
    while (currIdx < (int)m_logicalList.size()) {
        pushStep(2, 3, currIdx); 
        pushStep(2, 4, currIdx); 
        if (m_logicalList[currIdx] == val) {
            pushStep(2, 4, currIdx, currIdx); 
            m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
            return;
        }
        currIdx++;
        if (currIdx < (int)m_logicalList.size()) {
            pushStep(2, 5, currIdx); 
            pushStep(2, 6, currIdx); 
        }
    }
    pushStep(2, 8, -1, -1, "Value " + std::to_string(val) + " not found!"); 
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
}

void LinkedListGroup::rearrangeList()
{
    ListNode* curr = m_head;
    while (curr) {
        curr->hasCustomPos = false; 
        curr = curr->next;
    }
}

void LinkedListGroup::updateNode(const std::string& input)
{
    int oldVal = -1, newVal = -1;
    std::stringstream ss(input);
    std::string token;
    std::getline(ss, token, ','); try { oldVal = std::stoi(token); } catch(...) {}
    std::getline(ss, token, ','); try { newVal = std::stoi(token); } catch(...) {}
    
    prepNewOperation();
    pushStep(3, 0, -1);
    
    if (m_logicalList.empty()) {
        pushStep(3, 1, -1); 
        pushStep(3, 2, -1, -1, "List is empty!"); 
        m_isPaused = false; m_sidebar.setPlayButtonText("Pause"); return;
    }
    pushStep(3, 1, 0);

    int currIdx = 0;
    while(currIdx < (int)m_logicalList.size()) {
        pushStep(3, 2, currIdx); 
        pushStep(3, 3, currIdx); 
        if (m_logicalList[currIdx] == oldVal) {
            m_logicalList[currIdx] = newVal;
            pushStep(3, 4, currIdx, currIdx); 
            pushStep(3, 5, currIdx, currIdx); 
            m_isPaused = false; m_sidebar.setPlayButtonText("Pause"); return;
        }
        currIdx++;
        if (currIdx < (int)m_logicalList.size()) {
             pushStep(3, 7, currIdx); 
        }
    }
    pushStep(3, 2, -1, -1, "Value " + std::to_string(oldVal) + " not found!");
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
}

void LinkedListGroup::clearList()
{
    while (m_head) {
        ListNode* temp = m_head;
        m_head = m_head->next;
        delete temp;
    }
    for (auto* n : m_dyingNodes) delete n;
    m_dyingNodes.clear();
    m_draggedNode = nullptr;
}

void LinkedListGroup::loadState(int index)
{
    if (index < 0 || index >= (int)m_history.size()) return;
    
    if (m_draggedNode) {
        m_draggedNode->isDragging = false;
        m_draggedNode = nullptr;
    }

    StepState& state = m_history[index];
    
    setCode(CODE_BLOCKS[state.codeBlockId]);
    highlightCodeLine(state.codeHighlightLine);

    if (!state.notificationMsg.empty()) {
        showNotification(state.notificationMsg);
    }

    std::vector<ListNode*> oldNodes;
    ListNode* curr = m_head;
    while(curr) { oldNodes.push_back(curr); curr = curr->next; }
    
    m_head = nullptr;
    ListNode* tail = nullptr;
    
    for (size_t i = 0; i < state.listValues.size(); i++) {
        ListNode* n = nullptr;
        if (i < oldNodes.size()) {
            n = oldNodes[i];
            n->value = state.listValues[i];
            n->text.setString(std::to_string(n->value));
            sf::FloatRect bounds = n->text.getLocalBounds();
            n->text.setOrigin(bounds.left + bounds.width  / 2.0f,
                              bounds.top  + bounds.height / 2.0f);
        } else {
            // Spawn offset
            float spawnX = 300.0f + oldNodes.size() * 100.0f;
            n = new ListNode(state.listValues[i], sf::Vector2f(spawnX, 100.0f), m_app->getFont());
        }
        n->next = nullptr;
        
        if ((int)i == state.foundNodeIndex) {
            n->leftBg.setFillColor(sf::Color(46, 204, 113)); // Match
        } else if ((int)i == state.activeNodeIndex) {
            n->leftBg.setFillColor(sf::Color(230, 126, 34)); // Active
        } else {
            n->leftBg.setFillColor(sf::Color(60, 60, 60)); // Default
        }
        
        if (!m_head) m_head = n;
        if (tail) tail->next = n;
        tail = n;
    }
    
    for (size_t i = state.listValues.size(); i < oldNodes.size(); i++) {
        ListNode* dying = oldNodes[i];
        dying->next  = nullptr;
        dying->alpha = 255.0f;
        m_dyingNodes.push_back(dying);
    }
}

void LinkedListGroup::stepBack()
{
    if (m_currentStep > 0) {
        m_currentStep--;
        loadState(m_currentStep);
    }
}

void LinkedListGroup::stepForward()
{
    if (m_currentStep < (int)m_history.size() - 1) {
        m_currentStep++;
        loadState(m_currentStep);
    }
}

void LinkedListGroup::handleInput(const sf::Event& event)
{
    VisualizerState::handleInput(event);
    m_clearBtn.handleEvent(event, m_app->getWindow());
    
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Delete || event.key.code == sf::Keyboard::BackSpace) {
            initData(""); // Fast reset hotkey
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        ListNode* curr = m_head;
        while (curr) {
            if (curr->contains(mousePos)) {
                curr->isDragging = true;
                m_draggedNode = curr;
                m_dragOffset = curr->position - mousePos;
                break;
            }
            curr = curr->next;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        if (m_draggedNode) {
            m_draggedNode->isDragging = false;
            m_draggedNode->hasCustomPos = true;
            m_draggedNode->targetPos = m_draggedNode->position;
            m_draggedNode = nullptr;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        if (m_draggedNode && m_draggedNode->isDragging) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            m_draggedNode->position = mousePos + m_dragOffset;
            m_draggedNode->updatePosition(m_draggedNode->position);
        }
    }
}

void LinkedListGroup::update(float dt)
{
    VisualizerState::update(dt);
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_app->getWindow());
    sf::Vector2f mPosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    m_clearBtn.update(mPosF);
    
    if (!m_isPaused) {
        m_playTimer += dt;
        float speed = m_sidebar.getSpeed();
        float invSpeed = 1.0f - speed;
        float delay = 0.02f + 1.48f * invSpeed * invSpeed;

        if (m_playTimer > delay) {
            m_playTimer = 0.0f;
            if (m_currentStep < (int)m_history.size() - 1) {
                stepForward();
            } else {
                m_isPaused = true;
                m_sidebar.setPlayButtonText("Play");
            }
        }
    }
    
    float startX = 300.0f; // Base X
    float startY = 280.0f; // Base Y
    float gapX = 100.0f;   // Gap X

    int idx = 0;
    ListNode* curr = m_head;
    
    float speed = m_sidebar.getSpeed();
    float speedMult = 5.0f + speed * 40.0f; 

    while (curr != nullptr)
    {
        float targetX = startX + idx * gapX;
        float targetY = startY;

        if (curr->hasCustomPos) {
            targetX = curr->targetPos.x;
            targetY = curr->targetPos.y;
        }

        if (!curr->isDragging) {
            float lerpFactor = std::min(1.0f, speedMult * dt);
            curr->position.x += (targetX - curr->position.x) * lerpFactor;
            curr->position.y += (targetY - curr->position.y) * lerpFactor;

            curr->updatePosition(curr->position);
        }

        // Graphic Polish: Hover Neon Outline
        if (curr->contains(mPosF) || curr->isDragging) {
            curr->leftBg.setOutlineColor(sf::Color(0, 255, 255)); // Cyan Neon
            curr->rightBg.setOutlineColor(sf::Color(0, 255, 255));
            curr->leftBg.setOutlineThickness(3.0f);
            curr->rightBg.setOutlineThickness(3.0f);
        } else {
            curr->leftBg.setOutlineColor(sf::Color(100, 100, 100)); // Default gray
            curr->rightBg.setOutlineColor(sf::Color(100, 100, 100));
            curr->leftBg.setOutlineThickness(2.0f);
            curr->rightBg.setOutlineThickness(2.0f);
        }

        curr = curr->next;
        idx++;
    }

    // Fade anim
    static const float FADE_SPEED = 350.0f;
    for (auto* node : m_dyingNodes) {
        node->alpha -= FADE_SPEED * dt;
        uint8_t ua = static_cast<uint8_t>(std::max(0.0f, node->alpha));
        node->setAlpha(ua);
    }
    for (int i = static_cast<int>(m_dyingNodes.size()) - 1; i >= 0; i--) {
        if (m_dyingNodes[i]->alpha <= 0.0f) {
            delete m_dyingNodes[i];
            m_dyingNodes.erase(m_dyingNodes.begin() + i);
        }
    }
}

void LinkedListGroup::drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f end) const
{
    sf::Vector2f dir = end - start;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len == 0) return;
    dir /= len;

    float angle = std::atan2(dir.y, dir.x) * 180.0f / 3.14159265f;

    m_arrowLine.setSize(sf::Vector2f(len, 3.0f));
    m_arrowLine.setPosition(start);
    m_arrowLine.setRotation(angle);
    window.draw(m_arrowLine);

    m_arrowHead.setPosition(end);
    m_arrowHead.setRotation(angle);
    window.draw(m_arrowHead);
}

void LinkedListGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);

    for (auto* node : m_dyingNodes) {
        window.draw(node->leftBg);
        window.draw(node->rightBg);
        window.draw(node->separator);
        window.draw(node->pointerDot);
        window.draw(node->text);
    }

    ListNode* curr = m_head;
    while (curr != nullptr)
    {
        window.draw(curr->leftBg);
        window.draw(curr->rightBg);
        window.draw(curr->separator);
        window.draw(curr->pointerDot);
        window.draw(curr->text);

        if (curr->next != nullptr)
        {
            float width = 70.0f;
            float divX = width * 0.7f;
            sf::Vector2f start = curr->position + sf::Vector2f(-width/2.0f + divX + (width - divX)/2.0f, 0);
            sf::Vector2f end = curr->next->position - sf::Vector2f(width/2.0f, 0);
            drawArrow(window, start, end);
        }
        
        curr = curr->next;
    }

    m_clearBtn.draw(window);
}
