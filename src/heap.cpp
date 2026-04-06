#include "heap.h"
#include <algorithm>

void VisualHeap::recordStep(int node1, int node2, int line) {
	steps.push_back(HeapStep(heap, node1, node2, line));
}

void VisualHeap::init(const std::vector<int>& data) {
	clear();
	for (int value : data) {
		push(value);
	}
}

void VisualHeap::push(int value) {
	steps.clear();
	heap.push_back(value);
	int current = heap.size() - 1;
	recordStep(current, -1, 0); 
	siftUp(current);
}

void VisualHeap::siftUp(int index) {
	while (index > 0) {
		int parent = (index - 1) / 2;
		recordStep(index, parent, 1);
		if (heap[index] > heap[parent]) {
			std::swap(heap[index], heap[parent]);
			recordStep(index, parent, 2); 
			index = parent;
		} else {
			recordStep(index, parent, 3);
			break;
		}
	}
	recordStep(-1, -1, 4);
}

void VisualHeap::pop() {
	steps.clear();
	if (heap.empty()) return;
	if (heap.size() == 1) {
		recordStep(0, -1, 5);
		heap.pop_back();
		recordStep(0, -1, 6);
		return;
	}
	recordStep(0, heap.size() - 1, 7);
	std::swap(heap[0], heap.back());
	heap.pop_back();
	recordStep(0, -1, 8);
	siftDown(0);
}

void VisualHeap::siftDown(int index) {
	int size = heap.size();
	while (true) {
		int left = 2 * index + 1;
		int right = 2 * index + 2;
		int largest = index;
		recordStep(largest, left, 9);
		if (left < size && heap[left] > heap[largest]) {
			largest = left;
		}
		if (right < size) {
			recordStep(largest, right, 10);
		}
		if (right < size && heap[right] > heap[largest]) {
			largest = right;
		}
		if (largest != index) {
			recordStep(index, largest, 11);
			std::swap(heap[index], heap[largest]);
			recordStep(index, largest, 12);
			index = largest;
		} else {
			recordStep(index, -1, 13);
			break;
		}
	}
	recordStep(-1, -1, 14);
}

void VisualHeap::update(int index, int newValue) {
	steps.clear();
	if (index < 0 || index >= heap.size()) return;
	int oldValue = heap[index];
	heap[index] = newValue;
	recordStep(index, -1, 15);
	if (newValue > oldValue) {
		siftUp(index);
	} else if (newValue < oldValue) {
		siftDown(index);
	}
}

bool VisualHeap::search(int value) {
	steps.clear();
	for (size_t i = 0; i < heap.size(); ++i) {
		recordStep(i, -1, 16);
		if (heap[i] == value) {
			recordStep(i, -1, 17);
			return true;
		}
	}
	recordStep(-1, -1, 18);
	return false;
}