#ifndef Heap_h
#define Heap_h

#include <vector>
#include <string>

struct HeapStep
{
	std::vector<int> currentHeap;
	int highlightedNode1;
	int highlightedNode2;
	int lindID;
};

struct VisualHeap 
{
	std::vector<HeapStep> steps;
	std::vector<int> heap;
	void init(const std::vector<int>& data);
	void push(int value);
	void pop();
	void shiftUp(int index);
	void shiftDown(int index);

	void clear() {
		steps.clear();
		heap.clear();
	}
};
#endif // !Heap_h
