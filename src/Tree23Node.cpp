#include "Tree23Node.h"

Tree23Node::Tree23Node()
{
    keys[0] = 0;
    keys[1] = 0;
    keyCount = 0;

    child[0] = nullptr;
    child[1] = nullptr;
    child[2] = nullptr;
    parent = nullptr;
}

Tree23Node::Tree23Node(int value)
{
    keys[0] = value;
    keys[1] = 0;
    keyCount = 1;

    child[0] = nullptr;
    child[1] = nullptr;
    child[2] = nullptr;
    parent = nullptr;
}

bool Tree23Node::isLeaf() const
{
    return child[0] == nullptr && child[1] == nullptr && child[2] == nullptr;
}

bool Tree23Node::contains(int value) const
{
    if (keyCount >= 1 && keys[0] == value)
    {
        return true;
    }

    if (keyCount == 2 && keys[1] == value)
    {
        return true;
    }

    return false;
}
