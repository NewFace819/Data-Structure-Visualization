#pragma once
#include "State.h"
#include "Sidebar.h"
#include "Button.h"
#include "CodeBox.h"
#include <string>

class App;

class VisualizerState : public State
{
public:
    VisualizerState(App* app, const std::string& dataStructureName);
    ~VisualizerState() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void showNotification(const std::string& message);

protected:
    App* m_app;
    std::string m_dsName;
    Sidebar m_sidebar;
    CodeBox m_codeBox;
    sf::RectangleShape m_topBar;
    sf::Text m_titleText;
    Button m_backButton;
    bool m_isPaused = true;
    float m_playTimer = 0.0f;

    sf::RectangleShape m_notifyBg;
    sf::Text m_notifyText;
    float m_notifyTimer = 0.0f;
    float m_notifyAlpha = 0.0f;

    virtual void stepBack() {}
    virtual void stepForward() {}

    void setCode(const std::vector<std::string>& code) { m_codeBox.setCode(code); }
    void highlightCodeLine(int line) { m_codeBox.highlightLine(line); }
};
