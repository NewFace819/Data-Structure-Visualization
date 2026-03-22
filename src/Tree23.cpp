#include "Tree23.h"
#include <sstream>
#include <algorithm>

SplitResult::SplitResult()
{
    isSplit = false;
    promotedKey = 0;
    leftNode = nullptr;
    rightNode = nullptr;
}

Tree23::Tree23()
{
    root = nullptr;
}

Tree23::~Tree23()
{
    clear();
}

Tree23Node* Tree23::getRoot() const
{
    return root;
}

bool Tree23::isEmpty() const
{
    return root == nullptr;
}

void Tree23::clear(Tree23Node* node)
{
    if (node == nullptr)
    {
        return;
    }

    clear(node->child[0]);
    clear(node->child[1]);
    clear(node->child[2]);

    delete node;
}

void Tree23::clear()
{
    clear(root);
    root = nullptr;
    values.clear();
}

int Tree23::getChildIndex(Tree23Node* node, int value) const
{
    if (node->keyCount == 1)
    {
        if (value < node->keys[0])
        {
            return 0;
        }

        return 2;
    }

    if (value < node->keys[0])
    {
        return 0;
    }

    if (value > node->keys[1])
    {
        return 2;
    }

    return 1;
}

void Tree23::insertIntoOneKeyNode(Tree23Node* node, int value)
{
    if (value < node->keys[0])
    {
        node->keys[1] = node->keys[0];
        node->keys[0] = value;
    }
    else {
        node->keys[1] = value;
    }

    node->keyCount = 2;
}

int Tree23::getMiddleOfThree(int a, int b, int c, int& small, int& large) const
{
    if ((a > b && a < c) || (a > c && a < b))
    {
        if (b < c)
        {
            small = b;
            large = c;
        }
        else {
            small = c;
            large = b;
        }
        return a;
    }

    if ((b > a && b < c) || (b > c && b < a))
    {
        if (a < c)
        {
            small = a;
            large = c;
        }
        else {
            small = c;
            large = a;
        }
        return b;
    }

    if (a < b)
    {
        small = a;
        large = b;
    }
    else {
        small = b;
        large = a;
    }

    return c;
}

SplitResult Tree23::insertHelper(Tree23Node* node, int value)
{
    SplitResult result;

    if (node == nullptr)
    {
        return result;
    }

    if (node->contains(value))
    {
        return result;
    }

    if (node->isLeaf())
    {
        if (node->keyCount == 1)
        {
            insertIntoOneKeyNode(node, value);
            return result;
        }

        int small = 0;
        int large = 0;
        int middle = getMiddleOfThree(node->keys[0], node->keys[1], value, small, large);

        result.isSplit = true;
        result.promotedKey = middle;
        result.leftNode = new Tree23Node(small);
        result.rightNode = new Tree23Node(large);

        delete node;
        return result;
    }

    int childIndex = getChildIndex(node, value);
    SplitResult promoted;

    if (childIndex == 0)
    {
        promoted = insertHelper(node->child[0], value);
    }
    else if (childIndex == 1)
    {
        promoted = insertHelper(node->child[1], value);
    }
    else {
        promoted = insertHelper(node->child[2], value);
    }

    if (promoted.isSplit == false)
    {
        return result;
    }

    if (node->keyCount == 1)
    {
        if (childIndex == 0)
        {
            node->keys[1] = node->keys[0];
            node->keys[0] = promoted.promotedKey;

            node->child[2] = node->child[2];
            node->child[1] = promoted.rightNode;
            node->child[0] = promoted.leftNode;
        }
        else {
            node->keys[1] = promoted.promotedKey;
            node->child[1] = promoted.leftNode;
            node->child[2] = promoted.rightNode;
        }

        node->keyCount = 2;

        if (node->child[0] != nullptr)
        {
            node->child[0]->parent = node;
        }
        if (node->child[1] != nullptr)
        {
            node->child[1]->parent = node;
        }
        if (node->child[2] != nullptr)
        {
            node->child[2]->parent = node;
        }

        return result;
    }

    int small = 0;
    int large = 0;
    int middle = getMiddleOfThree(node->keys[0], node->keys[1], promoted.promotedKey, small, large);

    result.isSplit = true;
    result.promotedKey = middle;
    result.leftNode = new Tree23Node(small);
    result.rightNode = new Tree23Node(large);

    if (childIndex == 0)
    {
        result.leftNode->child[0] = promoted.leftNode;
        result.leftNode->child[2] = promoted.rightNode;

        result.rightNode->child[0] = node->child[1];
        result.rightNode->child[2] = node->child[2];
    }
    else if (childIndex == 1)
    {
        result.leftNode->child[0] = node->child[0];
        result.leftNode->child[2] = promoted.leftNode;

        result.rightNode->child[0] = promoted.rightNode;
        result.rightNode->child[2] = node->child[2];
    }
    else {
        result.leftNode->child[0] = node->child[0];
        result.leftNode->child[2] = node->child[1];

        result.rightNode->child[0] = promoted.leftNode;
        result.rightNode->child[2] = promoted.rightNode;
    }

    if (result.leftNode->child[0] != nullptr)
    {
        result.leftNode->child[0]->parent = result.leftNode;
    }
    if (result.leftNode->child[2] != nullptr)
    {
        result.leftNode->child[2]->parent = result.leftNode;
    }
    if (result.rightNode->child[0] != nullptr)
    {
        result.rightNode->child[0]->parent = result.rightNode;
    }
    if (result.rightNode->child[2] != nullptr)
    {
        result.rightNode->child[2]->parent = result.rightNode;
    }

    delete node;
    return result;
}

bool Tree23::insertWithoutTracking(int value)
{
    if (root == nullptr)
    {
        root = new Tree23Node(value);
        return true;
    }

    SplitResult result = insertHelper(root, value);

    if (result.isSplit)
    {
        Tree23Node* newRoot = new Tree23Node(result.promotedKey);
        newRoot->child[0] = result.leftNode;
        newRoot->child[2] = result.rightNode;

        result.leftNode->parent = newRoot;
        result.rightNode->parent = newRoot;

        root = newRoot;
    }

    return true;
}

bool Tree23::search(int value) const
{
    Tree23Node* cur = root;

    while (cur != nullptr)
    {
        if (cur->contains(value))
        {
            return true;
        }

        int childIndex = getChildIndex(cur, value);
        cur = cur->child[childIndex];
    }

    return false;
}

bool Tree23::insert(int value)
{
    if (search(value))
    {
        return false;
    }

    insertWithoutTracking(value);
    values.push_back(value);
    return true;
}

void Tree23::rebuildFromValues(const std::vector<int>& arr)
{
    clear(root);
    root = nullptr;

    for (int i = 0; i < (int)arr.size(); i++)
    {
        insertWithoutTracking(arr[i]);
    }
}

bool Tree23::remove(int value)
{
    if (search(value) == false)
    {
        return false;
    }

    std::vector<int> nextValues;

    for (int i = 0; i < (int)values.size(); i++)
    {
        if (values[i] != value)
        {
            nextValues.push_back(values[i]);
        }
    }

    values = nextValues;
    rebuildFromValues(values);
    return true;
}

bool Tree23::update(int oldValue, int newValue)
{
    if (search(oldValue) == false)
    {
        return false;
    }

    if (oldValue != newValue && search(newValue))
    {
        return false;
    }

    std::vector<int> nextValues = values;

    for (int i = 0; i < (int)nextValues.size(); i++)
    {
        if (nextValues[i] == oldValue)
        {
            nextValues[i] = newValue;
            break;
        }
    }

    values = nextValues;
    rebuildFromValues(values);
    return true;
}

void Tree23::inorder(Tree23Node* node, std::vector<int>& result) const
{
    if (node == nullptr)
    {
        return;
    }

    if (node->keyCount == 1)
    {
        inorder(node->child[0], result);
        result.push_back(node->keys[0]);
        inorder(node->child[2], result);
        return;
    }

    inorder(node->child[0], result);
    result.push_back(node->keys[0]);
    inorder(node->child[1], result);
    result.push_back(node->keys[1]);
    inorder(node->child[2], result);
}

std::vector<int> Tree23::getInorder() const
{
    std::vector<int> result;
    inorder(root, result);
    return result;
}

std::string Tree23::toInorderString() const
{
    std::vector<int> arr = getInorder();
    std::stringstream ss;

    for (int i = 0; i < (int)arr.size(); i++)
    {
        ss << arr[i];
        if (i + 1 < (int)arr.size())
        {
            ss << ' ';
        }
    }

    return ss.str();
}

int Tree23::height(Tree23Node* node) const
{
    if (node == nullptr)
    {
        return 0;
    }

    int leftHeight = height(node->child[0]);
    int middleHeight = height(node->child[1]);
    int rightHeight = height(node->child[2]);

    return 1 + std::max(leftHeight, std::max(middleHeight, rightHeight));
}

void Tree23::collectLevel(Tree23Node* node, int targetDepth, std::vector<std::string>& result) const
{
    if (node == nullptr)
    {
        return;
    }

    if (targetDepth == 0)
    {
        std::stringstream ss;
        ss << '[' << node->keys[0];
        if (node->keyCount == 2)
        {
            ss << '|' << node->keys[1];
        }
        ss << ']';
        result.push_back(ss.str());
        return;
    }

    collectLevel(node->child[0], targetDepth - 1, result);
    if (node->keyCount == 2)
    {
        collectLevel(node->child[1], targetDepth - 1, result);
    }
    collectLevel(node->child[2], targetDepth - 1, result);
}

std::vector<std::vector<std::string>> Tree23::getLevelOrderText() const
{
    std::vector<std::vector<std::string>> result;
    int h = height(root);

    for (int level = 0; level < h; level++)
    {
        std::vector<std::string> curLevel;
        collectLevel(root, level, curLevel);
        result.push_back(curLevel);
    }

    return result;
}
