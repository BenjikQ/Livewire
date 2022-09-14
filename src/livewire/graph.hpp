#ifndef LIVEWIRE_GRAPH_HPP
#define LIVEWIRE_GRAPH_HPP

#include <memory>
#include <vector>

#include <opencv2/core/mat.hpp>

#include <lemon/dijkstra.h>
#include <lemon/dim2.h>
#include <lemon/smart_graph.h>

using Point = lemon::dim2::Point<int>;

template <typename CostFunction>
class DiagonalGraph {
public:
    using Graph = lemon::SmartGraph;
    using Node = Graph::Node;
    using Edge = Graph::Edge;
    using NodeMap = Graph::NodeMap<double>;
    using LengthMap = Graph::EdgeMap<double>;
    using Dijkstra = lemon::Dijkstra<Graph, LengthMap>;

    DiagonalGraph(CostFunction&& costFunction, const cv::Mat& image);

    Node operator()(int col, int row) const { return Graph::nodeFromId(m_cols * row + col); }

    std::vector<Point> shortestPath(Point source, Point destination);

private:
    void createNodes();
    void createEdges();

    void createVerticalEdges(int col, int row);
    void createHorizontalEdges(int col, int row);
    void createDiagonalEdges(int col, int row);

private:
    Graph m_graph{};
    NodeMap m_nodeMap;
    LengthMap m_lengthMap;
    cv::Mat m_cost;
    std::unique_ptr<Dijkstra> m_dijkstra{ nullptr };
    int m_rows;
    int m_cols;
};

template <typename CostFunction>
DiagonalGraph<CostFunction>::DiagonalGraph(CostFunction&& costFunction, const cv::Mat& image) :
    m_nodeMap{ m_graph },
    m_lengthMap{ m_graph },
    m_cols{ image.cols },
    m_rows{ image.rows },
    m_cost{ costFunction(image) } {
    m_graph.reserveNode(m_rows * m_cols);
    m_graph.reserveEdge(4 * m_rows * m_cols - 3 * (m_rows + m_cols) + 2);  // Number of edges in diagonal graph
    createNodes();
    createEdges();
    m_dijkstra = std::make_unique<Dijkstra>(m_graph, m_lengthMap);
}

template <typename CostFunction>
std::vector<Point> DiagonalGraph<CostFunction>::shortestPath(Point source, Point destination) {
    const Node start{ (*this)(source.x, source.y) };
    const Node end{ (*this)(destination.x, destination.y) };
    m_dijkstra->run(start, end);

    std::vector<Point> path{};
    path.reserve(1024);
    for (Node v{ end }; v != start; v = m_dijkstra->predNode(v)) {
        path.emplace_back(Graph::id(v) % m_cols, Graph::id(v) / m_cols);
    }
    path.emplace_back(Graph::id(start) % m_cols, Graph::id(start) / m_cols);

    return path;
}

template <typename CostFunction>
void DiagonalGraph<CostFunction>::createNodes() {
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            m_graph.addNode();
        }
    }
}

template <typename CostFunction>
void DiagonalGraph<CostFunction>::createEdges() {
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            createVerticalEdges(col, row);
            createHorizontalEdges(col, row);
            createDiagonalEdges(col, row);
        }
    }
}

template <typename CostFunction>
void DiagonalGraph<CostFunction>::createVerticalEdges(int col, int row) {
    if (col < m_cols - 1) {
        const Edge edge{ m_graph.addEdge((*this)(col, row), (*this)(col + 1, row)) };
        m_lengthMap[edge] = m_cost.at<double>(row, col);
    }
}

template <typename CostFunction>
void DiagonalGraph<CostFunction>::createHorizontalEdges(int col, int row) {
    if (row < m_rows - 1) {
        const Edge edge{ m_graph.addEdge((*this)(col, row), (*this)(col, row + 1)) };
        m_lengthMap[edge] = m_cost.at<double>(row, col);
    }
}

template <typename CostFunction>
void DiagonalGraph<CostFunction>::createDiagonalEdges(int col, int row) {
    if (row < m_rows - 1 && col < m_cols - 1) {
        const Edge edge{ m_graph.addEdge((*this)(col, row), (*this)(col + 1, row + 1)) };
        m_lengthMap[edge] = m_cost.at<double>(row, col);
    }

    if (row > 0 && col > 0) {
        const Edge edge{ m_graph.addEdge((*this)(col, row), (*this)(col - 1, row - 1)) };
        m_lengthMap[edge] = m_cost.at<double>(row, col);
    }
}

#endif  // LIVEWIRE_GRAPH_HPP
