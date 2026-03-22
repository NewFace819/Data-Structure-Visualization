#pragma once

#include "VisualizerState.h"
#include "heap.h"

class App;

class HeapGroup : public VisualizerState
{
private:
	VisualHeap m_heap;
	int m_currentStep = 0;
	// Font and visual settings
	sf::Font m_font;
	float m_nodeRadius;
	// Colors
	sf::Color m_nodeColor;
	sf::Color m_highlightColor1;
	sf::Color m_highlightColor2;
	void changeHighlightColors(int id);

	void drawTree(sf::RenderWindow& window, const HeapStep& currentStep);

public:
	HeapGroup(App* app);
	~HeapGroup() = default;

	void init() override;
	void handleInput(const sf::Event& event) override;
	void update(float dt) override;
	void draw(sf::RenderWindow& window) override;

	void stepBack() override;
	void stepForward() override;
};