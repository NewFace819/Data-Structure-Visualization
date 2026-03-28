#include "graph/GraphModel.h"

#include <algorithm>
#include <cmath>

namespace graph {

GraphModel::GraphModel(bool directed) : m_directed(directed) {}

void GraphModel::clear() {
    m_nodes.clear();
    m_edges.clear();
    m_nodeIndex.clear();
    m_edgeSet.clear();
    m_nextNodeId = 0;
    m_start = kInvalidNodeId;
    m_goal = kInvalidNodeId;
    m_gridRows = 0;
    m_gridCols = 0;
}

void GraphModel::clearEdges() {
    if (m_mode == GraphMode::Grid) {
        return;
    }

    m_edges.clear();
    m_edgeSet.clear();
}

void GraphModel::initializeGrid(int rows, int cols) {
    clear();
    m_mode = GraphMode::Grid;
    m_directed = false;

    if (rows <= 0 || cols <= 0) {
        return;
    }

    m_gridRows = rows;
    m_gridCols = cols;

    m_nodes.reserve(static_cast<std::size_t>(rows * cols));

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            appendNode(std::to_string(row) + "," + std::to_string(col),
                       Vec2{static_cast<float>(col), static_cast<float>(row)});
        }
    }

    setStart(nodeIdAt(rows / 2, cols / 4));
    setGoal(nodeIdAt(rows / 2, (cols * 3) / 4));
}

void GraphModel::setMode(GraphMode mode) {
    m_mode = mode;
    if (m_mode == GraphMode::Coordinate) {
        for (auto& edge : m_edges) {
            edge.weight = distance(edge.from, edge.to);
        }
    }
}

GraphMode GraphModel::mode() const { return m_mode; }

void GraphModel::setDirected(bool directed) {
    if (m_mode == GraphMode::Grid) {
        m_directed = false;
        return;
    }

    m_directed = directed;
    rebuildEdgeSet();
}

bool GraphModel::directed() const { return m_directed; }

NodeId GraphModel::addNode(const std::string& name, Vec2 position) {
    if (m_mode == GraphMode::Grid) {
        return kInvalidNodeId;
    }

    return appendNode(name, position);
}

NodeId GraphModel::appendNode(const std::string& name, Vec2 position) {
    Node node;
    node.id = m_nextNodeId++;
    node.name = name;
    node.position = position;

    m_nodeIndex[node.id] = m_nodes.size();
    m_nodes.push_back(node);

    return node.id;
}

bool GraphModel::removeNode(NodeId nodeId) {
    if (m_mode == GraphMode::Grid) {
        return false;
    }

    const auto found = m_nodeIndex.find(nodeId);
    if (found == m_nodeIndex.end()) {
        return false;
    }

    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(),
                                 [nodeId](const Edge& edge) {
                                     return edge.from == nodeId || edge.to == nodeId;
                                 }),
                  m_edges.end());

    const std::size_t idx = found->second;
    const std::size_t last = m_nodes.size() - 1;

    if (idx != last) {
        std::swap(m_nodes[idx], m_nodes[last]);
        m_nodeIndex[m_nodes[idx].id] = idx;
    }

    m_nodes.pop_back();
    m_nodeIndex.erase(found);

    if (m_start == nodeId) {
        m_start = kInvalidNodeId;
    }
    if (m_goal == nodeId) {
        m_goal = kInvalidNodeId;
    }

    rebuildEdgeSet();
    return true;
}

bool GraphModel::moveNode(NodeId nodeId, Vec2 newPosition) {
    if (m_mode == GraphMode::Grid) {
        return false;
    }

    Node* node = getNode(nodeId);
    if (!node) {
        return false;
    }

    node->position = newPosition;

    if (m_mode == GraphMode::Coordinate) {
        for (auto& edge : m_edges) {
            if (edge.from == nodeId || edge.to == nodeId) {
                edge.weight = distance(edge.from, edge.to);
            }
        }
    }

    return true;
}

bool GraphModel::renameNode(NodeId nodeId, const std::string& newName) {
    if (m_mode == GraphMode::Grid) {
        return false;
    }

    Node* node = getNode(nodeId);
    if (!node) {
        return false;
    }

    node->name = newName;
    return true;
}

bool GraphModel::setBlocked(NodeId nodeId, bool blocked) {
    Node* node = getNode(nodeId);
    if (!node) {
        return false;
    }

    if (blocked && (nodeId == m_start || nodeId == m_goal)) {
        return false;
    }

    node->blocked = blocked;
    return true;
}

bool GraphModel::addEdge(NodeId from, NodeId to, std::optional<float> manualWeight) {
    if (m_mode == GraphMode::Grid) {
        return false;
    }

    if (!containsNode(from) || !containsNode(to) || from == to) {
        return false;
    }

    if (hasEdge(from, to)) {
        return false;
    }

    Edge edge;
    edge.from = from;
    edge.to = to;
    edge.weight = resolveNewEdgeWeight(from, to, manualWeight);
    m_edges.push_back(edge);

    rebuildEdgeSet();
    return true;
}

bool GraphModel::removeEdge(NodeId from, NodeId to) {
    if (m_mode == GraphMode::Grid) {
        return false;
    }

    const auto before = m_edges.size();

    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(),
                                 [from, to, this](const Edge& edge) {
                                     if (m_directed) {
                                         return edge.from == from && edge.to == to;
                                     }
                                     return (edge.from == from && edge.to == to) ||
                                            (edge.from == to && edge.to == from);
                                 }),
                  m_edges.end());

    if (m_edges.size() == before) {
        return false;
    }

    rebuildEdgeSet();
    return true;
}

bool GraphModel::updateEdgeWeight(NodeId from, NodeId to, float newWeight) {
    if (m_mode == GraphMode::Grid) {
        return false;
    }

    if (newWeight < 0.0f) {
        return false;
    }

    auto it = findEdgeIt(from, to);
    if (it == m_edges.end()) {
        return false;
    }

    if (m_mode == GraphMode::Coordinate) {
        it->weight = distance(it->from, it->to);
    } else {
        it->weight = newWeight;
    }

    return true;
}

bool GraphModel::hasEdge(NodeId from, NodeId to) const {
    if (m_mode == GraphMode::Grid) {
        const GridCoord fromCoord = coordOf(from);
        const GridCoord toCoord = coordOf(to);
        if (fromCoord.row < 0 || toCoord.row < 0) {
            return false;
        }

        const int rowDelta = std::abs(fromCoord.row - toCoord.row);
        const int colDelta = std::abs(fromCoord.col - toCoord.col);
        return rowDelta + colDelta == 1;
    }

    return m_edgeSet.find(edgeKey(from, to)) != m_edgeSet.end();
}

bool GraphModel::setStart(NodeId nodeId) {
    if (!containsNode(nodeId)) {
        return false;
    }

    Node* node = getNode(nodeId);
    if (!node) {
        return false;
    }

    node->blocked = false;
    m_start = nodeId;
    return true;
}

bool GraphModel::setGoal(NodeId nodeId) {
    if (!containsNode(nodeId)) {
        return false;
    }

    Node* node = getNode(nodeId);
    if (!node) {
        return false;
    }

    node->blocked = false;
    m_goal = nodeId;
    return true;
}

NodeId GraphModel::start() const { return m_start; }

NodeId GraphModel::goal() const { return m_goal; }

int GraphModel::rows() const { return m_gridRows; }

int GraphModel::cols() const { return m_gridCols; }

bool GraphModel::isGrid() const { return m_mode == GraphMode::Grid; }

bool GraphModel::inBounds(int row, int col) const {
    return row >= 0 && row < m_gridRows && col >= 0 && col < m_gridCols;
}

NodeId GraphModel::nodeIdAt(int row, int col) const {
    if (!inBounds(row, col)) {
        return kInvalidNodeId;
    }

    return static_cast<NodeId>(row * m_gridCols + col);
}

GridCoord GraphModel::coordOf(NodeId nodeId) const {
    if (!isGrid() || !containsNode(nodeId) || m_gridCols <= 0) {
        return {};
    }

    return GridCoord{nodeId / m_gridCols, nodeId % m_gridCols};
}

bool GraphModel::containsNode(NodeId nodeId) const {
    return m_nodeIndex.find(nodeId) != m_nodeIndex.end();
}

bool GraphModel::nameExists(const std::string& name) const {
    return std::any_of(m_nodes.begin(), m_nodes.end(),
                       [&name](const Node& node) { return node.name == name; });
}

const Node* GraphModel::getNode(NodeId nodeId) const {
    const auto it = m_nodeIndex.find(nodeId);
    if (it == m_nodeIndex.end()) {
        return nullptr;
    }

    return &m_nodes[it->second];
}

Node* GraphModel::getNode(NodeId nodeId) {
    const auto it = m_nodeIndex.find(nodeId);
    if (it == m_nodeIndex.end()) {
        return nullptr;
    }

    return &m_nodes[it->second];
}

std::vector<std::pair<NodeId, float>> GraphModel::neighbors(NodeId nodeId) const {
    std::vector<std::pair<NodeId, float>> out;
    if (!containsNode(nodeId)) {
        return out;
    }

    if (m_mode == GraphMode::Grid) {
        static constexpr int kDirs[4][2] = {
            {-1, 0},
            {1, 0},
            {0, -1},
            {0, 1},
        };

        const GridCoord coord = coordOf(nodeId);
        if (coord.row < 0) {
            return out;
        }

        out.reserve(4);
        for (const auto& dir : kDirs) {
            const int nextRow = coord.row + dir[0];
            const int nextCol = coord.col + dir[1];
            if (!inBounds(nextRow, nextCol)) {
                continue;
            }

            const NodeId nextId = nodeIdAt(nextRow, nextCol);
            const Node* nextNode = getNode(nextId);
            if (!nextNode || nextNode->blocked) {
                continue;
            }

            out.emplace_back(nextId, 1.0f);
        }

        return out;
    }

    for (const auto& edge : m_edges) {
        if (edge.from == nodeId) {
            out.emplace_back(edge.to, edge.weight);
        } else if (!m_directed && edge.to == nodeId) {
            out.emplace_back(edge.from, edge.weight);
        }
    }

    return out;
}

const std::vector<Node>& GraphModel::nodes() const { return m_nodes; }

const std::vector<Edge>& GraphModel::edges() const { return m_edges; }

std::uint64_t GraphModel::edgeKey(NodeId from, NodeId to) const {
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(from)) << 32ULL) |
           static_cast<std::uint32_t>(to);
}

void GraphModel::rebuildEdgeSet() {
    m_edgeSet.clear();

    for (const auto& edge : m_edges) {
        m_edgeSet.insert(edgeKey(edge.from, edge.to));
        if (!m_directed) {
            m_edgeSet.insert(edgeKey(edge.to, edge.from));
        }
    }
}

float GraphModel::distance(NodeId a, NodeId b) const {
    const Node* nodeA = getNode(a);
    const Node* nodeB = getNode(b);
    if (!nodeA || !nodeB) {
        return 1.0f;
    }

    const float dx = nodeA->position.x - nodeB->position.x;
    const float dy = nodeA->position.y - nodeB->position.y;

    return std::sqrt(dx * dx + dy * dy);
}

float GraphModel::resolveNewEdgeWeight(NodeId from, NodeId to,
                                       std::optional<float> manualWeight) const {
    if (m_mode == GraphMode::Coordinate) {
        return distance(from, to);
    }

    if (m_mode == GraphMode::Normal) {
        if (manualWeight.has_value() && manualWeight.value() >= 0.0f) {
            return manualWeight.value();
        }
        return 1.0f;
    }

    return 1.0f;
}

std::vector<Edge>::iterator GraphModel::findEdgeIt(NodeId from, NodeId to) {
    return std::find_if(m_edges.begin(), m_edges.end(), [from, to, this](const Edge& edge) {
        if (m_directed) {
            return edge.from == from && edge.to == to;
        }
        return (edge.from == from && edge.to == to) || (edge.from == to && edge.to == from);
    });
}

std::vector<Edge>::const_iterator GraphModel::findEdgeIt(NodeId from, NodeId to) const {
    return std::find_if(m_edges.cbegin(), m_edges.cend(), [from, to, this](const Edge& edge) {
        if (m_directed) {
            return edge.from == from && edge.to == to;
        }
        return (edge.from == from && edge.to == to) || (edge.from == to && edge.to == from);
    });
}

}  // namespace graph
