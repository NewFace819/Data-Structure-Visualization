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
    : VisualizerState(app, dsName), m_head(nullptr)
{
    // Pre-initialize cached arrow shapes to avoid per-frame reconstruction
    m_arrowLine.setFillColor(sf::Color(100, 100, 100));
    m_arrowLine.setOrigin(0, 1.5f);

    m_arrowHead.setPointCount(3);
    m_arrowHead.setPoint(0, sf::Vector2f(0.0f,   0.0f));
    m_arrowHead.setPoint(1, sf::Vector2f(-12.0f,  6.0f));
    m_arrowHead.setPoint(2, sf::Vector2f(-12.0f, -6.0f));
    m_arrowHead.setFillColor(sf::Color(100, 100, 100));
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

    initData("15,30,45");
}

void LinkedListGroup::prepNewOperation()
{
    if (m_currentStep >= 0 && m_currentStep < (int)m_history.size() - 1) {
        m_history.erase(m_history.begin() + m_currentStep + 1, m_history.end());
        if (!m_history.empty()) m_logicalList = m_history.back().listValues;
    }
}

void LinkedListGroup::pushStep(int blockId, int highlightLine, int activeIdx, int foundIdx)
{
    m_history.push_back({m_logicalList, blockId, highlightLine, activeIdx, foundIdx});
}

void LinkedListGroup::initData(const std::string& input)
{
    m_history.clear();
    m_currentStep = 0;
    m_logicalList.clear();

    // Immediately discard any leftover fade-out nodes from a previous operation
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
    
    // Load it immediately without animating
    m_currentStep = 0;
    loadState(m_currentStep);

    // Force positions to target immediately so they don't fly in
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
    if (m_logicalList.empty()) { pushStep(1, 1, -1); stepForward(); m_isPaused = true; m_sidebar.setPlayButtonText("Play"); return; }
    
    pushStep(1, 2, 0); 
    if (m_logicalList[0] == val) {
        m_logicalList.erase(m_logicalList.begin());
        pushStep(1, 2, -1); 
        stepForward(); m_isPaused = true; m_sidebar.setPlayButtonText("Play");
        return;
    }
    
    pushStep(1, 3, 0); 
    int currIdx = 0;
    while (currIdx < (int)m_logicalList.size() - 1) {
        pushStep(1, 4, currIdx); 
        if (m_logicalList[currIdx+1] == val) break;
        currIdx++;
        pushStep(1, 5, currIdx); 
    }
    
    pushStep(1, 6, currIdx); 
    if (currIdx < (int)m_logicalList.size() - 1) {
        m_logicalList.erase(m_logicalList.begin() + currIdx + 1);
        pushStep(1, 7, currIdx); 
    }
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
}

void LinkedListGroup::searchNode(int val)
{
    prepNewOperation();
    pushStep(2, 0, -1); 
    pushStep(2, 1, 0); 
    pushStep(2, 2, 0); 
    
    int currIdx = 0;
    while (currIdx < (int)m_logicalList.size()) {
        pushStep(2, 3, currIdx); 
        pushStep(2, 4, currIdx); 
        if (m_logicalList[currIdx] == val) {
            pushStep(2, 4, currIdx, currIdx); 
            stepForward(); m_isPaused = true; m_sidebar.setPlayButtonText("Play");
            return;
        }
        currIdx++;
        if (currIdx < (int)m_logicalList.size()) {
            pushStep(2, 5, currIdx); 
            pushStep(2, 6, currIdx); 
        }
    }
    pushStep(2, 8, -1); 
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
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
    pushStep(3, 1, 0); 
    
    int currIdx = 0;
    while(currIdx < (int)m_logicalList.size()) {
        pushStep(3, 2, currIdx); 
        pushStep(3, 3, currIdx); 
        if (m_logicalList[currIdx] == oldVal) {
            m_logicalList[currIdx] = newVal;
            pushStep(3, 4, currIdx, currIdx); 
            pushStep(3, 5, currIdx, currIdx); 
            stepForward(); m_isPaused = true; m_sidebar.setPlayButtonText("Play"); return;
        }
        currIdx++;
        if (currIdx < (int)m_logicalList.size()) {
             pushStep(3, 7, currIdx); 
        }
    }
    m_isPaused = false; m_sidebar.setPlayButtonText("Pause");
}

void LinkedListGroup::clearList()
{
    while (m_head) {
        ListNode* temp = m_head;
        m_head = m_head->next;
        delete temp;
    }
    // Also free any nodes still mid-fade
    for (auto* n : m_dyingNodes) delete n;
    m_dyingNodes.clear();
}

void LinkedListGroup::loadState(int index)
{
    if (index < 0 || index >= (int)m_history.size()) return;
    StepState& state = m_history[index];
    
    setCode(CODE_BLOCKS[state.codeBlockId]);
    highlightCodeLine(state.codeHighlightLine);

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
        } else {
            // Spawn new node slightly off the end position
            float spawnX = 300.0f + oldNodes.size() * 100.0f;
            n = new ListNode(state.listValues[i], sf::Vector2f(spawnX, 100.0f), m_app->getFont());
        }
        n->next = nullptr;
        
        if ((int)i == state.foundNodeIndex) {
            n->leftBg.setFillColor(sf::Color(46, 204, 113)); // Emerald Green
        } else if ((int)i == state.activeNodeIndex) {
            n->leftBg.setFillColor(sf::Color(230, 126, 34)); // Orange (Active)
        } else {
            n->leftBg.setFillColor(sf::Color(60, 60, 60)); // Dark Gray
        }
        
        if (!m_head) m_head = n;
        if (tail) tail->next = n;
        tail = n;
    }
    
    for (size_t i = state.listValues.size(); i < oldNodes.size(); i++) {
        // Do NOT delete immediately - push to dying list so they can fade out
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
}

void LinkedListGroup::update(float dt)
{
    VisualizerState::update(dt);
    
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
    
    float startX = 300.0f; // X position of the first node (DS area)
    float startY = 280.0f; // Vertical centre of the DS area
    float gapX = 100.0f;   // Horizontal spacing between nodes

    int idx = 0;
    ListNode* curr = m_head;
    
    float speed = m_sidebar.getSpeed();
    float speedMult = 5.0f + speed * 40.0f; 

    while (curr != nullptr)
    {
        float targetX = startX + idx * gapX;
        float targetY = startY;

        // Smooth lerp – clamp factor so nodes never overshoot their targets
        float lerpFactor = std::min(1.0f, speedMult * dt);
        curr->position.x += (targetX - curr->position.x) * lerpFactor;
        curr->position.y += (targetY - curr->position.y) * lerpFactor;

        curr->updatePosition(curr->position);

        curr = curr->next;
        idx++;
    }

    // --- Fade-out animation for dying (deleted) nodes ---
    // ~350 alpha units/sec  =>  node fully transparent in ~0.73 seconds
    static const float FADE_SPEED = 350.0f;
    for (auto* node : m_dyingNodes) {
        node->alpha -= FADE_SPEED * dt;
        uint8_t ua = static_cast<uint8_t>(std::max(0.0f, node->alpha));
        node->setAlpha(ua);
    }
    // Remove nodes that have finished fading (iterate backwards to safely erase)
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

    // Reuse cached line shape
    m_arrowLine.setSize(sf::Vector2f(len, 3.0f));
    m_arrowLine.setPosition(start);
    m_arrowLine.setRotation(angle);
    window.draw(m_arrowLine);

    // Reuse cached arrowhead shape
    m_arrowHead.setPosition(end);
    m_arrowHead.setRotation(angle);
    window.draw(m_arrowHead);
}

void LinkedListGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);

    // Draw dying nodes first so they appear behind live nodes
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
        curr = curr->next;
    }

    curr = m_head;
    while (curr != nullptr)
    {
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
}
