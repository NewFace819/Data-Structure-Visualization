#pragma once
#include "State.h"
#include "Button.h"
#include <vector>

class App;

class MenuState : public State
{
public:
    MenuState(App* app);
    ~MenuState() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    App* m_app;
    sf::RectangleShape m_topBar;
    sf::Text m_topBarTitle;
    sf::Text m_topBarRightText;
    sf::Text m_titleText;
    sf::Text m_subtitleText;
    std::vector<Button> m_buttons;

    sf::Texture m_llLogoTex;
    sf::Sprite m_llLogoSprite;
    bool m_hasLogo = false;
};
