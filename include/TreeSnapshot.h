#pragma once

#include <string>
#include <vector>

struct VisualNode
{
    std::string id;
    int keyCount;
    int keys[2];
    float x;
    float y;

    bool isHighlighted;
    bool isOverflow;
    bool isNewNode;
};

struct VisualEdge
{
    std::string parentId;
    std::string childId;
};

struct TreeSnapshot
{
    std::vector<VisualNode> nodes;
    std::vector<VisualEdge> edges;
};

struct SnapshotSequence
{
    std::vector<TreeSnapshot> frames;
    std::vector<std::string> labels;
};