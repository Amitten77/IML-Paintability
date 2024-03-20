#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @class Graph
 * @brief A class for simple undirected unweighted graphs.
 */
class Graph {
public:
    /// @brief Default constructor. Creates an empty graph.
    Graph() noexcept;

    /// @brief Constructs a graph with the given vertex labels. Repeated labels are ignored.
    explicit Graph(const std::vector<std::string>& vertices) noexcept;

    /**
     * @param vertex Label of the vertex.
     * @return Whether the vertex is in the graph.
     *
     * Time complexity: O(1)
     */
    bool hasVertex(const std::string& vertex) const noexcept;

    /**
     * @param vertex1 One endpoint of the edge.
     * @param vertex2 The other endpoint of the edge.
     * @return Whether the edge is in the graph.
     *
     * Time complexity: O(|E|) for worst case
     */
    bool hasEdge(const std::string& vertex1, const std::string& vertex2) const noexcept;

    /**
     * @return Number of vertices.
     *
     * Time complexity: O(1)
     */
    size_t countV() const noexcept;

    /**
     * @return Number of edges.
     *
     * Time complexity: O(1)
     */
    size_t countE() const noexcept;

    /**
     * @brief Adds a new vertex to the graph.
     * @param vertex Label of the new vertex.
     * @return Number of vertices actually added (1 if success, 0 if fail).
     *
     * Time complexity: O(1)
     */
    size_t addVertex(const std::string& vertex) noexcept;

    /**
     * @brief Adds a list of new vertices to the graph.
     * @param vertices Labels of the new vertices.
     * @return Number of vertices actually added.
     *
     * Time complexity: O(n) where n is size of input
     */
    size_t addVertices(const std::vector<std::string>& vertices) noexcept;

    /**
     * @brief Adds a new edge to the graph.
     * @param vertex1 Label of one endpoint.
     * @param vertex2 Label of another endpoint.
     * @return Number of edges actually added (1 if success, 0 if fail).
     *
     * Time complexity: O(|E|) for worst case
     */
    size_t addEdge(const std::string& vertex1, const std::string& vertex2) noexcept;

    /**
     * @brief Removes a vertex from the graph.
     * @param vertex Label of the vertex.
     * @return Number of vertices actually removed (1 if success, 0 if fail).
     *
     * Time complexity: O(|E|) for worst case
     */
    size_t removeVertex(const std::string& vertex) noexcept;

    /**
     * @brief Removes an edge from the graph.
     * @param vertex1 Label of one endpoint.
     * @param vertex2 Label of another endpoint.
     * @return Number of edges actually removed (1 if success, 0 if fail).
     *
     * Time complexity: O(|E|) for worst case
     */
    size_t removeEdge(const std::string& vertex1, const std::string& vertex2) noexcept;

    /// @brief Clears the graph of all edges.
    void clearEdges() noexcept;

    /**
     * @brief Clears the graph of all vertices and edges.
     */
    void clear() noexcept;

private:
    std::unordered_map<std::string, std::vector<std::string>> neighbors_;
    size_t edgeCount_;
};

#endif // GRAPH_H
