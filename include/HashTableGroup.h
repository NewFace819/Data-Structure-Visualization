#pragma once
#include "VisualizerState.h"

class HashTableGroup : public VisualizerState
{
public:
    HashTableGroup(App* app);
    ~HashTableGroup() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
};
