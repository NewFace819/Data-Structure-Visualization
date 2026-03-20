#include "GraphGroup.h"
#include "App.h"

GraphGroup::GraphGroup(App* app) 
    : VisualizerState(app, "Graph Algorithms")
{
}

void GraphGroup::init()
{
    VisualizerState::init();
    // Khởi tạo các thành phần giao diện/dữ liệu cho Graph
}

void GraphGroup::handleInput(const sf::Event& event)
{
    VisualizerState::handleInput(event);
    // Xử lý các sự kiện click đỉnh, cạnh...
}

void GraphGroup::update(float dt)
{
    VisualizerState::update(dt);
    // Cập nhật vị trí hoặc trạng thái đồ thị
}

void GraphGroup::draw(sf::RenderWindow& window)
{
    VisualizerState::draw(window);
    // Vẽ đồ thị lên window ở phía bên phải Sidebar
}
