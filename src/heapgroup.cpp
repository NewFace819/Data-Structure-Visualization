#include "HeapGroup.h"
#include "App.h"
#include <sstream>

HeapGroup::HeapGroup(App* app)
	: VisualizerState(app, "Heap")
	, m_currentStep(0)
	, m_nodeRadius(20.0f)
	, m_nodeColor(sf::Color::White)
	, m_highlightColor1(sf::Color::Yellow)
	, m_highlightColor2(sf::Color::Red)
{
}

void HeapGroup::init()
{
	VisualizerState::init();

	// Load font
	if (!m_font.loadFromFile("assets/fonts/arial.ttf"))
	{
		// Font loading failed, but continue anyway
	}

	// Set up code
	std::vector<std::string> code = {
		"void push(int value) {",
		"  heap.push_back(value);",
		"  siftUp(heap.size() - 1);",
		"}",
		"",
		"void siftUp(int index) {",
		"  while (index > 0) {",
		"    int parent = (index - 1) / 2;",
		"    if (heap[index] < heap[parent]) {",
		"      swap(heap[index], heap[parent]);",
		"      index = parent;",
		"    } else break;",
		"  }",
		"}"
	};
	setCode(code);

	// Initialize heap with some sample data
	std::vector<int> initialData = { 10, 20, 30, 40, 50 };
	m_heap.init(initialData);
}

void HeapGroup::handleInput(const sf::Event& event)
{
	VisualizerState::handleInput(event);

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Left)
		{
			stepBack();
		}
		else if (event.key.code == sf::Keyboard::Right)
		{
			stepForward();
		}
	}
}

void HeapGroup::update(float dt)
{
	VisualizerState::update(dt);

	// Update animation based on play timer
	if (!m_isPaused && m_playTimer >= 0.5f)
	{
		stepForward();
		m_playTimer = 0.0f;
	}
}

void HeapGroup::draw(sf::RenderWindow& window)
{
	VisualizerState::draw(window);

	// Get current step
	const auto& steps = m_heap.getSteps();
	if (!steps.empty() && m_currentStep < static_cast<int>(steps.size()))
	{
		drawTree(window, steps[m_currentStep]);
	}
}

void HeapGroup::drawTree(sf::RenderWindow& window, const HeapStep& currentStep)
{
	const auto& heap = currentStep.currentHeap;
	if (heap.empty())
		return;

	// Calculate layout parameters
	const float startX = 400.0f;
	const float startY = 150.0f;
	const float levelHeight = 80.0f;
	const float nodeSpacing = 60.0f;

	// Draw nodes level by level
	int nodeIndex = 0;
	int level = 0;
	int nodesInLevel = 1;
	int nodesDrawn = 0;

	while (nodeIndex < static_cast<int>(heap.size()))
	{
		int nodesInCurrentLevel = (nodesInLevel > static_cast<int>(heap.size()) - nodeIndex) 
			? (heap.size() - nodeIndex) 
			: nodesInLevel;

		float levelWidth = nodesInCurrentLevel * nodeSpacing;
		float levelStartX = startX - levelWidth / 2.0f;
		float posX = levelStartX;
		float posY = startY + level * levelHeight;

		for (int i = 0; i < nodesInCurrentLevel && nodeIndex < static_cast<int>(heap.size()); ++i)
		{
			// Draw connections to children
			int leftChild = 2 * nodeIndex + 1;
			int rightChild = 2 * nodeIndex + 2;

			if (leftChild < static_cast<int>(heap.size()))
			{
				// Calculate child position
				float childLevelWidth = (1 << (level + 1)) * nodeSpacing;
				float childLevelStartX = startX - childLevelWidth / 2.0f;
				float childStartX = childLevelStartX + (leftChild % (1 << (level + 1))) * nodeSpacing;
				float childPosY = posY + levelHeight;

				sf::Vertex line[] = {
					sf::Vertex(sf::Vector2f(posX, posY), sf::Color::White),
					sf::Vertex(sf::Vector2f(childStartX, childPosY), sf::Color::White)
				};
				window.draw(line, 2, sf::Lines);
			}

			// Draw node circle
			sf::CircleShape node(m_nodeRadius);
			node.setPosition(posX - m_nodeRadius, posY - m_nodeRadius);

			// Color the node based on highlight status
			if (nodeIndex == currentStep.highlightedNode1)
			{
				node.setFillColor(m_highlightColor1);
			}
			else if (nodeIndex == currentStep.highlightedNode2)
			{
				node.setFillColor(m_highlightColor2);
			}
			else
			{
				node.setFillColor(m_nodeColor);
			}

			window.draw(node);

			// Draw value text
			sf::Text text(std::to_string(heap[nodeIndex]), m_font, 16);
			text.setPosition(posX - 8.0f, posY - 8.0f);
			text.setFillColor(sf::Color::Black);
			window.draw(text);

			posX += nodeSpacing;
			nodeIndex++;
		}

		level++;
		nodesInLevel *= 2;
	}
}

void HeapGroup::stepBack()
{
	if (m_currentStep > 0)
	{
		m_currentStep--;
		const auto& steps = m_heap.getSteps();
		if (m_currentStep < static_cast<int>(steps.size()))
		{
			highlightCodeLine(steps[m_currentStep].lineID);
		}
	}
}

void HeapGroup::stepForward()
{
	const auto& steps = m_heap.getSteps();
	if (m_currentStep < static_cast<int>(steps.size()) - 1)
	{
		m_currentStep++;
		highlightCodeLine(steps[m_currentStep].lineID);
	}
}

void HeapGroup::saveVisualizationSettings(const std::string& filename)
{
	// TODO: Implement JSON or config file serialization
	// Save: m_nodeRadius, m_nodeColor, m_highlightColor1, m_highlightColor2
}

void HeapGroup::loadVisualizationSettings(const std::string& filename)
{
	// TODO: Implement JSON or config file deserialization
	// Load: m_nodeRadius, m_nodeColor, m_highlightColor1, m_highlightColor2
}

void HeapGroup::resetVisualizationSettings()
{
	m_nodeRadius = 20.0f;
	m_nodeColor = sf::Color::White;
	m_highlightColor1 = sf::Color::Yellow;
	m_highlightColor2 = sf::Color::Red;
}
