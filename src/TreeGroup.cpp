#include "TreeGroup.h"
#include "App.h"

TreeGroup::TreeGroup(App* app) 
    : VisualizerState(app, "Tree Configurations")
{
}

void TreeGroup::init()
{
    VisualizerState::init();
    // Thêm các setup riêng cho Tree nhóm ở đây (ví dụ: khởi tạo node gốc)
}

void TreeGroup::handleInput(const sf::Event& event)
{
    VisualizerState::handleInput(event);
    // Xử lý sự kiện riêng (click vào node...)
}

void TreeGroup::update(float dt)
{
    VisualizerState::update(dt);
    // Logic cập nhật animation riêng cho Tree
}

void TreeGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);
    // Vẽ các node của Tree ở đây
}
