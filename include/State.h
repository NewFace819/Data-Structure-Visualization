#pragma once

namespace sf {
    class Event;
    class RenderWindow;
}

class State
{
public:
    virtual ~State() = default;

    virtual void init() = 0;
    virtual void handleInput(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};
