#include "../include/heapgroup.h"
#include "../include/App.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>


namespace {
	std::vector<int> StringToInts(const std::string& str) {
		std::vector<int> result;
		std::istringstream ss(str);
		std::string item;
		while (std::getline(ss, item, ',')) {
			item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
			if (!item.empty()) {
				try {
					result.push_back(std::stoi(item));
				} catch (const std::invalid_argument&) {
					std::cerr << "Invalid input: " << item << " is not an integer." << std::endl;
				}
			}
		}
		return result;
	}
}
//Init state heap group
HeapGroup::HeapGroup(App* app)
	: VisualizerState(app, "Heap")
{
}

// Init data and ui
void HeapGroup::init()
{
	VisualizerState::init();
	// Load font
	if (!m_font.loadFromFile("assets/SF-Pro-Display-Bold.otf")) {
		std::cerr << "Failed to load font!" << std::endl;
	}
	m_nodeRadius = 22.0f;
	changeColors(1);
	//Init heap for demo
	m_heap.init({ 5, 3, 8, 1, 4 });
	m_currentStep = 0;
	m_isPaused = true;
	// Pseudo code for CodeBox
	setCode({
		"void init(vector<int>& data) {",
		"	clear();",
		"	for (int value : data) {",
		"		push(value);",
		"  }",
		"}"
		});

	m_sidebar.setInitCallback([this](std::string input) {
		auto data = StringToInts(input);
		if (data.empty()) data = { 5, 3, 8, 1, 4 };
		m_heap.init(data);
		m_currentStep = 0;
		m_isPaused = false;

		setCode({
			"void init(vector<int>& data) {",
			"	clear();",
			"	for (int value : data) {",
			"		push(value);",
			"  }",
			"}"
			});
		});

	m_sidebar.setAddCallback([this](int value) {
		m_heap.push(value);
		m_currentStep = 0;
		m_isPaused = false;
		setCode({
			"void push(int value) {",
			"	heap.push_back(value);",
			"	siftUp(heap.size() - 1);",
			"}"
			});
		});

	m_sidebar.setDeleteCallback([this](int value) {
		m_heap.pop();
		m_currentStep = 0;
		m_isPaused = false;
		setCode({
			"void pop() {",
			"	if (heap.empty()) return;",
			"	swap(heap[0], heap.back());",
			"	heap.pop_back();",
			"	siftDown(0);",
			"}"
			});
		});

	m_sidebar.setUpdateCallback([this](std::string input) {
		auto parts = StringToInts(input);
		if (parts.size() != 2) {
			std::cerr << "Invalid input for update. Expected format: index,newValue" << std::endl;
			return;
		}
		int index = parts[0];
		int newValue = parts[1];
		m_heap.update(index, newValue);
		m_currentStep = 0;
		m_isPaused = false;
		setCode({
			"void update(int index, int newValue) {",
			"	if (index < 0 || index >= heap.size()) return;",
			"	int oldValue = heap[index];",
			"	heap[index] = newValue;",
			"	if (newValue > oldValue) siftUp(index);",
			"	else siftDown(index);",
			"}"
			});
		});

	m_sidebar.setSearchCallback([this](int value) {
		m_heap.search(value);
		m_currentStep = 0;
		m_isPaused = false;
		setCode({
			"bool search(int value) {",
			"	for (int i = 0; i < heap.size(); i++) {",
			"		if (heap[i] == value) return true;",
			"	}",
			"	return false;",
			"}"
			});
		});

	m_sidebar.setPlayPauseCallback([this]() {
		m_isPaused = !m_isPaused;
		m_sidebar.setPlayButtonText(m_isPaused ? "Play" : "Pause");
		});
	m_sidebar.setNextCallback([this]() {stepForward(); });
	m_sidebar.setBackCallback([this]() {stepBack(); });
}

// Event and update handling
void HeapGroup::handleInput(const sf::Event& event)
{
	VisualizerState::handleInput(event);
}

void HeapGroup::update(float dt)
{
	VisualizerState::update(dt);
	
	const auto& steps = m_heap.getSteps();
	if (steps.empty()) return;
	if (!m_isPaused) {
		m_playTimer += dt;
		float speed = m_sidebar.getSpeed();
		float delay = std::max(0.1f, 1.5f - speed*1.4f);

		if (m_playTimer >= delay) {
			stepForward();
			m_playTimer = 0.0f;
		}
	}

	if (m_currentStep < (int)steps.size()) {
		highlightCodeLine(steps[m_currentStep].lineID);
	}
}

void HeapGroup::stepBack()
{
	if (m_currentStep > 0) {
		m_currentStep--;
	}
}

void HeapGroup::stepForward()
{
	if (m_currentStep < (int)m_heap.getSteps().size() - 1) {
		m_currentStep++;
	} else {
		m_isPaused = true;
		m_sidebar.setPlayButtonText("Play");
	}
}

void HeapGroup::draw(sf::RenderWindow& window)
{
	VisualizerState::draw(window);
	const auto& steps = m_heap.getSteps();
	if (!steps.empty() && m_currentStep < (int)steps.size()) {
		drawTree(window, steps[m_currentStep]);
	}
}

void HeapGroup::changeColors(int ColorsId)
{
	switch (ColorsId) {
	case 1:
		m_nodeColor = sf::Color(100, 149, 237); // Cornflower Blue
		m_highlightColor1 = sf::Color(255, 165, 0); // Orange
		m_highlightColor2 = sf::Color(220, 20, 60); // Crimson
		break;
	case 2:
		m_nodeColor = sf::Color(60, 179, 113); // Medium Sea Green
		m_highlightColor1 = sf::Color(255, 140, 0); // Dark Orange
		m_highlightColor2 = sf::Color(178, 34, 34); // Fire Brick
		break;
	default:
		m_nodeColor = sf::Color(70, 130, 180);
		m_highlightColor1 = sf::Color(255, 140, 0);
		m_highlightColor2 = sf::Color(220, 20, 60);
		break;
	}
}

void HeapGroup::drawTree(sf::RenderWindow& window, const HeapStep& currentStep)
{
	const auto& heap = currentStep.currentHeap;
	if (heap.empty()) return;

	float startX = window.getSize().x + 250.0f / 2.0f;
	float startY = 150.0f;
	float VerticalSpacing = 80.0f;
	float HorizontalSpacing = 200.0f;

	auto getPosition = [&](int index) -> sf::Vector2f {
		if (index == 0) return { startX, startY };
		int level = std::floor(std::log2(index + 1));
		int maxNodesInLevel = std::pow(2, level);
		int positionInLevel = index - (maxNodesInLevel - 1);

		float levelHorizontalSpacing = HorizontalSpacing / std::pow(2, level);
		float x = startX - (levelHorizontalSpacing * (maxNodesInLevel - 1)) / 2.0f + positionInLevel * levelHorizontalSpacing;
		float y = startY + level * VerticalSpacing;

		return { x, y };
		};

	for (size_t i = 1; i < heap.size(); i++) {
		int parentIndex = (i - 1) / 2;
		sf::Vector2f childPos = getPosition(i);
		sf::Vector2f parentPos = getPosition(parentIndex);

		sf::Vertex line[] = {
			sf::Vertex(parentPos, sf::Color(200, 200, 200)),
			sf::Vertex(childPos, sf::Color(200, 200, 200))
		};
		window.draw(line, 2, sf::Lines);
	}

	for (size_t i = 0; i < heap.size(); i++) {
		sf::Vector2f pos = getPosition(i);
		sf::CircleShape circle(m_nodeRadius);
		circle.setFillColor(m_nodeColor);
		circle.setOrigin(m_nodeRadius, m_nodeRadius);
		circle.setPosition(pos);

		if (i == currentStep.highlightedNode1) {
			circle.setFillColor(m_highlightColor1);
			circle.setOutlineColor(sf::Color::White);
			circle.setOutlineThickness(3.0f);
		}
		else if (i == currentStep.highlightedNode2) {
			circle.setFillColor(m_highlightColor2);
			circle.setOutlineColor(sf::Color::White);
			circle.setOutlineThickness(3.0f);
		}
		else
		{
			circle.setOutlineThickness(0.0f);
			circle.setOutlineColor(m_nodeColor);
		}
		window.draw(circle);

		sf::Text text(std::to_string(heap[i]), m_font, 16);
		text.setFillColor(sf::Color::White);

		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
		text.setPosition(pos);
		window.draw(text);
	}
}