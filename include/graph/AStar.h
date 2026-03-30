#pragma once

#include <vector>

#include "graph/GraphModel.h"

namespace graph {

struct AStarStep {
    std::vector<NodeId> inQueue;
    std::vector<NodeId> visited;
    std::vector<NodeId> path;
    NodeId current = kInvalidNodeId;
    bool reachedGoal = false;
};

class AStar {
public:
    static std::vector<AStarStep> run(const GraphModel& graph);
};

}
