#include "Sidebar.h"

// Setup Sidebar -> Refactored into a Bottom Control Panel
Sidebar::Sidebar(const sf::Font& font, float windowHeight)
    : m_font(font), m_inputBox(font, 120.0f, 40.0f)
{
    float panelWidth = 260.0f;
    float panelHeight = windowHeight - 60.0f; // Below Top Bar
    float panelX = 0.0f;
    float panelY = 60.0f;
    
    m_background.setSize(sf::Vector2f(panelWidth, panelHeight));
    // Dùng tông sáng (Trắng/Xám) như nền tổng thể của project
    m_background.setFillColor(sf::Color(245, 245, 245, 230)); 
    m_background.setPosition(panelX, panelY);

    // Title 
    m_titleText.setFont(m_font);
    m_titleText.setString("Control Panel");
    m_titleText.setCharacterSize(18);
    m_titleText.setFillColor(sf::Color(100, 100, 100)); // Xám mờ
    m_titleText.setPosition(25, panelY + 20);

    // Action Buttons (arranged vertically)
    float btnX = 25.0f;
    float startY = panelY + 60.0f;
    float btnW = 210.0f;
    float btnH = 45.0f;
    float gap = 20.0f;

    m_buttons.emplace_back(btnX, startY, btnW, btnH, "Init", m_font);
    m_buttons.emplace_back(btnX, startY + (btnH + gap), btnW, btnH, "Add", m_font);
    m_buttons.emplace_back(btnX, startY + 2*(btnH + gap), btnW, btnH, "Delete", m_font);
    m_buttons.emplace_back(btnX, startY + 3*(btnH + gap), btnW, btnH, "Search", m_font);
    m_buttons.emplace_back(btnX, startY + 4*(btnH + gap), btnW, btnH, "Update", m_font);

    // Setup input box popups - pop out to the RIGHT of the sidebar
    float popupX = panelWidth + 20.0f;
    m_buttons[0].setCallback([this, popupX, startY]() {
        m_inputBox.setPosition(popupX, startY);
        m_inputBox.show("N(Random), CSV, File:", [this](std::string val) { if (m_initCb) m_initCb(val); }, false);
    });
    m_buttons[1].setCallback([this, popupX, startY, btnH, gap]() {
        m_inputBox.setPosition(popupX, startY + (btnH + gap));
        m_inputBox.show("Value to Add:", [this](std::string val) { if (m_addCb) try{ m_addCb(std::stoi(val)); }catch(...){} }, true);
    });
    m_buttons[2].setCallback([this, popupX, startY, btnH, gap]() {
        m_inputBox.setPosition(popupX, startY + 2*(btnH + gap));
        m_inputBox.show("Value to Delete:", [this](std::string val) { if (m_delCb) try{ m_delCb(std::stoi(val)); }catch(...){} }, true);
    });
    m_buttons[3].setCallback([this, popupX, startY, btnH, gap]() {
        m_inputBox.setPosition(popupX, startY + 3*(btnH + gap));
        m_inputBox.show("Value to Search:", [this](std::string val) { if (m_searchCb) try{ m_searchCb(std::stoi(val)); }catch(...){} }, true);
    });
    m_buttons[4].setCallback([this, popupX, startY, btnH, gap]() {
        m_inputBox.setPosition(popupX, startY + 4*(btnH + gap));
        m_inputBox.show("oldVal,newVal:", [this](std::string val) { if (m_updateCb) m_updateCb(val); }, false);
    });

    // Speed Slider (Below buttons)
    float sliderY = startY + 5*(btnH + gap) + 40.0f;
    
    m_speedText.setFont(m_font);
    m_speedText.setString("Speed");
    m_speedText.setCharacterSize(18);
    m_speedText.setFillColor(sf::Color(40, 40, 40));
    m_speedText.setPosition(btnX, sliderY);

    m_sliderBar.setSize(sf::Vector2f(btnW, 6));
    m_sliderBar.setFillColor(sf::Color(200, 200, 200)); 
    m_sliderBar.setPosition(btnX, sliderY + 35);

    m_sliderValue = 0.5f; 
    m_sliderKnob.setSize(sf::Vector2f(16, 24));
    m_sliderKnob.setFillColor(sf::Color(52, 152, 219)); // Soft Blue
    m_sliderKnob.setPosition(btnX + btnW * m_sliderValue - 8.0f, sliderY + 35 - 9.0f);

    // Media Controls 
    float mediaY = sliderY + 80.0f;
    float mediaW = (btnW - 20.0f) / 3.0f;
    m_buttons.emplace_back(btnX, mediaY, mediaW, btnH, "Back", m_font);
    m_buttons.emplace_back(btnX + mediaW + 10.0f, mediaY, mediaW, btnH, "Play", m_font);
    m_buttons.emplace_back(btnX + 2*(mediaW + 10.0f), mediaY, mediaW, btnH, "Next", m_font);
}

void Sidebar::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    m_inputBox.handleEvent(event, window);
    
    // Only handle button clicks if input box is NOT actively waiting for input
    // Hoặc cho phép click nút khác để đóng input box hiện tại
    if (!m_inputBox.isVisible() || event.type != sf::Event::MouseButtonPressed)
    {
        for (auto& btn : m_buttons)
        {
            btn.handleEvent(event, window);
        }
    }
    
    // Logic trượt Slider điều chỉnh tốc độ
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        sf::FloatRect knobBounds = m_sliderKnob.getGlobalBounds();
        
        // Mở rộng khu vực bấm ra để dễ kéo hơn
        knobBounds.left -= 5; knobBounds.width += 10; knobBounds.top -= 5; knobBounds.height += 10;
        
        if (knobBounds.contains(mousePos)) {
            m_isSliderDragging = true;
        } else if (m_sliderBar.getGlobalBounds().contains(mousePos)) {
            m_isSliderDragging = true;
            updateSliderPosition(mousePos.x);
        }
    } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        m_isSliderDragging = false;
    } else if (event.type == sf::Event::MouseMoved) {
        if (m_isSliderDragging) {
            updateSliderPosition(static_cast<float>(event.mouseMove.x));
        }
    }
}

void Sidebar::update(const sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    update(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
}

void Sidebar::update(sf::Vector2f mousePos)
{
    if (m_inputBox.isVisible()) {
        // Do not update buttons hover if input box is taking precedence
        return; 
    }

    for (auto& btn : m_buttons)
    {
        btn.update(mousePos);
    }
}

void Sidebar::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    window.draw(m_titleText);
    
    window.draw(m_speedText);
    window.draw(m_sliderBar);
    window.draw(m_sliderKnob);

    for (const auto& btn : m_buttons)
    {
        btn.draw(window);
    }
    
    // Draw input box on top of everything
    m_inputBox.draw(window);
}

void Sidebar::setInitCallback(std::function<void(std::string)> cb) { m_initCb = cb; }
void Sidebar::setAddCallback(std::function<void(int)> cb) { m_addCb = cb; }
void Sidebar::setDeleteCallback(std::function<void(int)> cb) { m_delCb = cb; }
void Sidebar::setSearchCallback(std::function<void(int)> cb) { m_searchCb = cb; }
void Sidebar::setUpdateCallback(std::function<void(std::string)> cb) { m_updateCb = cb; }

void Sidebar::setBackCallback(std::function<void()> cb) { if(m_buttons.size() > 5) m_buttons[5].setCallback(cb); }
void Sidebar::setPlayPauseCallback(std::function<void()> cb) { if(m_buttons.size() > 6) m_buttons[6].setCallback(cb); }
void Sidebar::setNextCallback(std::function<void()> cb) { if(m_buttons.size() > 7) m_buttons[7].setCallback(cb); }

void Sidebar::setPlayButtonText(const std::string& text) { if(m_buttons.size() > 6) m_buttons[6].setText(text); }

void Sidebar::updateSliderPosition(float mouseX)
{
    float barLeft = m_sliderBar.getPosition().x;
    float barRight = barLeft + m_sliderBar.getSize().x;
    float newX = std::max(barLeft, std::min(mouseX, barRight));
    
    m_sliderKnob.setPosition(newX - m_sliderKnob.getSize().x / 2.0f, m_sliderKnob.getPosition().y);
    m_sliderValue = (newX - barLeft) / m_sliderBar.getSize().x;
}
