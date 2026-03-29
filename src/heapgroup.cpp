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
		"  for (int value : data) {",
		"	push(value);",
		"  }"
		"}"
	});
}

// Change colors
void HeapGroup::changeColors(int ColorsId)
{
	switch (ColorsId) {
	case 1:
		m_nodeColor = sf::Color(100, 149, 237); // Cornflower Blue
		m_highlightColor1 = sf::Color(255, 165, 0); // Orange
		m_highlightColor2 = sf::Color(255, 69, 0); // Red-Orange
		break;
	case 2:
		m_nodeColor = sf::Color(60, 179, 113); // Medium Sea Green
		m_highlightColor1 = sf::Color(255, 215, 0); // Gold
		m_highlightColor2 = sf::Color(255, 140, 0); // Dark Orange
		break;
	default:
		m_nodeColor = sf::Color(100, 149, 237);
		m_highlightColor1 = sf::Color(255, 165, 0);
		m_highlightColor2 = sf::Color(255, 69, 0);
		break;
	}
}

// Handle user input
void HeapGroup::handleInput(const sf::Event& event)
{
	VisualizerState::handleInput(event);
}

