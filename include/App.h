#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include "State.h"

class App
{
public:
    App();
    ~App();

    void run();

    void pushState(std::unique_ptr<State> state);
    void popState();
    void changeState(std::unique_ptr<State> state);

    sf::RenderWindow& getWindow();
    const sf::Font& getFont() const;

private:
    void loadResources();
    void processEvents();
    void update(float dt);
    void render();

    sf::RenderWindow m_window;
    std::stack<std::unique_ptr<State>> m_states;
    sf::Font m_mainFont;

    std::unique_ptr<State> m_newState;
    bool m_isChangingState;
};
