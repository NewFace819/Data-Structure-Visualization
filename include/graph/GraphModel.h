#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "graph/GraphTypes.h"

namespace graph {

class GraphModel {
public:
    explicit GraphModel(bool directed = false);

    void clear();
    void clearEdges();
    void initializeGrid(int rows, int cols);

    void setMode(GraphMode mode);
    GraphMode mode() const;

    void setDirected(bool directed);
    bool directed() const;

    NodeId addNode(const std::string& name, Vec2 position);
    bool removeNode(NodeId nodeId);
    bool moveNode(NodeId nodeId, Vec2 newPosition);
    bool renameNode(NodeId nodeId, const std::string& newName);
    bool setBlocked(NodeId nodeId, bool blocked);
    bool clearCell(NodeId nodeId);

    bool addEdge(NodeId from, NodeId to, std::optional<float> manualWeight = std::nullopt);
    bool removeEdge(NodeId from, NodeId to);
    bool updateEdgeWeight(NodeId from, NodeId to, float newWeight);
    bool hasEdge(NodeId from, NodeId to) const;

    bool setStart(NodeId nodeId);
    bool setGoal(NodeId nodeId);
    NodeId start() const;
    NodeId goal() const;
    int rows() const;
    int cols() const;
    bool isGrid() const;
    bool inBounds(int row, int col) const;
    NodeId nodeIdAt(int row, int col) const;
    GridCoord coordOf(NodeId nodeId) const;

    bool containsNode(NodeId nodeId) const;
    bool nameExists(const std::string& name) const;

    const Node* getNode(NodeId nodeId) const;
    Node* getNode(NodeId nodeId);

    std::vector<std::pair<NodeId, float>> neighbors(NodeId nodeId) const;

    const std::vector<Node>& nodes() const;
    const std::vector<Edge>& edges() const;

private:
    std::uint64_t edgeKey(NodeId from, NodeId to) const;
    NodeId appendNode(const std::string& name, Vec2 position);
    void rebuildEdgeSet();
    float distance(NodeId a, NodeId b) const;
    float resolveNewEdgeWeight(NodeId from, NodeId to, std::optional<float> manualWeight) const;
    std::vector<Edge>::iterator findEdgeIt(NodeId from, NodeId to);
    std::vector<Edge>::const_iterator findEdgeIt(NodeId from, NodeId to) const;

    bool m_directed = false;
    GraphMode m_mode = GraphMode::Normal;
    int m_gridRows = 0;
    int m_gridCols = 0;

    NodeId m_nextNodeId = 0;
    NodeId m_start = kInvalidNodeId;
    NodeId m_goal = kInvalidNodeId;

    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;
    std::unordered_map<NodeId, std::size_t> m_nodeIndex;
    std::unordered_set<std::uint64_t> m_edgeSet;
};

}  
