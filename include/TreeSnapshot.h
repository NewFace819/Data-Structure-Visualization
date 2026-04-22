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