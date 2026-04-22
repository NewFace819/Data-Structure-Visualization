#include "graph/AStar.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace graph {

namespace {

struct QueueEntry {
    NodeId nodeId = kInvalidNodeId;
    float priority = 0.0f;
};

struct QueueEntryCompare {
    bool operator()(const QueueEntry& lhs, const QueueEntry& rhs) const
    {
        return lhs.priority > rhs.priority;
    }
};

float heuristic(const GraphModel& graph, NodeId from, NodeId to)
{
    const GridCoord fromCoord = graph.coordOf(from);
    const GridCoord toCoord = graph.coordOf(to);
    if (fromCoord.row >= 0 && toCoord.row >= 0) {
        return static_cast<float>(std::abs(fromCoord.row - toCoord.row) +
                                  std::abs(fromCoord.col - toCoord.col));
    }

    const Node* fromNode = graph.getNode(from);
    const Node* toNode = graph.getNode(to);
    if (!fromNode || !toNode) {
        return 0.0f;
    }

    const float dx = fromNode->position.x - toNode->position.x;
    const float dy = fromNode->position.y - toNode->position.y;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<NodeId> sortedNodes(const std::unordered_set<NodeId>& nodes)
{
    std::vector<NodeId> out(nodes.begin(), nodes.end());
    std::sort(out.begin(), out.end());
    return out;
}

std::vector<NodeId> rebuildPath(NodeId goal,
                                const std::unordered_map<NodeId, NodeId>& cameFrom)
{
    std::vector<NodeId> path;
    NodeId current = goal;

    while (current != kInvalidNodeId) {
        path.push_back(current);
        const auto it = cameFrom.find(current);
        if (it == cameFrom.end()) {
            break;
        }
        current = it->second;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

}  // namespace

std::vector<AStarStep> AStar::run(const GraphModel& graph)
{
    std::vector<AStarStep> steps;

    const NodeId start = graph.start();
    const NodeId goal = graph.goal();
    if (!graph.containsNode(start) || !graph.containsNode(goal)) {
        return steps;
    }

    std::priority_queue<QueueEntry, std::vector<QueueEntry>, QueueEntryCompare> frontier;
    std::unordered_map<NodeId, float> gScore;
    std::unordered_map<NodeId, NodeId> cameFrom;
    std::unordered_set<NodeId> inQueue;
    std::unordered_set<NodeId> visited;

    frontier.push({start, heuristic(graph, start, goal)});
    gScore[start] = 0.0f;
    cameFrom[start] = kInvalidNodeId;
    inQueue.insert(start);
    steps.push_back({sortedNodes(inQueue), {}, {}, start, false});

    while (!frontier.empty()) {
        const QueueEntry top = frontier.top();
        frontier.pop();

        if (visited.find(top.nodeId) != visited.end()) {
            continue;
        }

        inQueue.erase(top.nodeId);
        visited.insert(top.nodeId);
        steps.push_back({sortedNodes(inQueue), sortedNodes(visited), {}, top.nodeId, false});

        if (top.nodeId == goal) {
            steps.push_back({sortedNodes(inQueue), sortedNodes(visited),
                             rebuildPath(goal, cameFrom), top.nodeId, true});
            break;
        }

        const float baseCost = gScore[top.nodeId];
        for (const auto& [neighborId, weight] : graph.neighbors(top.nodeId)) {
            if (visited.find(neighborId) != visited.end()) {
                continue;
            }

            const float tentative = baseCost + weight;
            const auto gIt = gScore.find(neighborId);
            if (gIt != gScore.end() && tentative >= gIt->second) {
                continue;
            }

            gScore[neighborId] = tentative;
            cameFrom[neighborId] = top.nodeId;
            frontier.push({neighborId, tentative + heuristic(graph, neighborId, goal)});
            inQueue.insert(neighborId);
            steps.push_back({sortedNodes(inQueue), sortedNodes(visited), {}, neighborId, false});
        }
    }

    if (steps.empty()) {
        steps.push_back({});
    }

    return steps;
}

}  // namespace graph
