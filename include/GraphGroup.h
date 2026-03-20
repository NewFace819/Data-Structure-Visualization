#pragma once
#include "VisualizerState.h"

class GraphGroup : public VisualizerState
{
public:
    GraphGroup(App* app);
    ~GraphGroup() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
};
