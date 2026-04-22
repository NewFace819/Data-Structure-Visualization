#pragma once
#include "VisualizerState.h"
#include "Button.h"

class TreeGroup : public VisualizerState
{
public:
    TreeGroup(App* app);
    ~TreeGroup() = default;

    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

private:
    Button m_tree23Button;
};