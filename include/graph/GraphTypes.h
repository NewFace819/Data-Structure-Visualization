#pragma once

#include <cstdint>
#include <string>

namespace graph {

using NodeId = std::int32_t;

constexpr NodeId kInvalidNodeId = -1;

enum class GraphMode : std::uint8_t { Grid, Coordinate, Normal };

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Node {
    NodeId id = kInvalidNodeId;
    std::string name;
    Vec2 position{};
    bool blocked = false;
};

struct Edge {
    NodeId from = kInvalidNodeId;
    NodeId to = kInvalidNodeId;
    float weight = 1.0f;
};

}  