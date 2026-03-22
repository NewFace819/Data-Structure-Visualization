#pragma once

#include "Tree23Node.h"
#include <vector>
#include <string>

struct SplitResult
{
    bool isSplit;
    int promotedKey;
    Tree23Node* leftNode;
    Tree23Node* rightNode;

    SplitResult();
};

class Tree23
{
private:
    Tree23Node* root;
    std::vector<int> values;

private:
    void clear(Tree23Node* node);

    int getChildIndex(Tree23Node* node, int value) const;
    void insertIntoOneKeyNode(Tree23Node* node, int value);
    int getMiddleOfThree(int a, int b, int c, int& small, int& large) const;

    SplitResult insertHelper(Tree23Node* node, int value);
    bool insertWithoutTracking(int value);
    void rebuildFromValues(const std::vector<int>& arr);

    void inorder(Tree23Node* node, std::vector<int>& result) const;
    int height(Tree23Node* node) const;
    void collectLevel(Tree23Node* node, int targetDepth, std::vector<std::string>& result) const;

public:
    Tree23();
    ~Tree23();

    Tree23Node* getRoot() const;

    bool isEmpty() const;
    bool search(int value) const;
    bool insert(int value);
    bool remove(int value);
    bool update(int oldValue, int newValue);
    void clear();

    std::vector<int> getInorder() const;
    std::vector<std::vector<std::string>> getLevelOrderText() const;
    std::string toInorderString() const;
};
