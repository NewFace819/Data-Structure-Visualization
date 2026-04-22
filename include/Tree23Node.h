#pragma once

struct Tree23Node
{
    int keys[2];
    int keyCount;

    Tree23Node* child[3];
    Tree23Node* parent;

    Tree23Node();
    Tree23Node(int value);

    bool isLeaf() const;
    bool contains(int value) const;
};
