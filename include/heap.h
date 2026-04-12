#ifndef Heap_h
#define Heap_h

#include <vector>

struct HeapStep
{
	std::vector<int> currentHeap;
	int highlightedNode1;
	int highlightedNode2;
	int lineID;

	HeapStep(const std::vector<int>& heap, int node1 = -1, int node2 = -1, int line = -1)
		: currentHeap(heap), highlightedNode1(node1), highlightedNode2(node2), lineID(line) {}
};

class VisualHeap 
{
private:
	std::vector<HeapStep> steps;
	std::vector<int> heap;
	void siftUp(int index);
	void siftDown(int index);
	void recordStep(int node1 = -1, int node2 = -1, int line = -1) {
		steps.emplace_back(heap, node1, node2, line);
	}
public:
	void init(const std::vector<int>& data);
	void push(int value);
	void pop();
	void update(int index, int newValue);
	bool search(int value);
	const std::vector<HeapStep>& getSteps() const {
		return steps;
	}
	void clearSteps() {
		steps.clear();
		heap.clear();
	}
};
#endif // Heap_h
