#include "HashTableGroup.h"
#include "App.h"

HashTableGroup::HashTableGroup(App* app) 
    : VisualizerState(app, "Hash Table")
{
}

void HashTableGroup::init()
{
    VisualizerState::init();
    // Set up Hash Table (ví dụ mảng các buckets)
}

void HashTableGroup::handleInput(const sf::Event& event)
{
    VisualizerState::handleInput(event);
    // Thêm logic cho việc nhập key/value Hash Table
}

void HashTableGroup::update(float dt)
{
    VisualizerState::update(dt);
}

void HashTableGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);
    // Vẽ Hash Table
}
