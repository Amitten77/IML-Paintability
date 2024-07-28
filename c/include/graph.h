/**
 * @file graph.h
 *
 * @brief This file contains the declaration of the Graph class and the Hopcroft-Karp function.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @class Graph
 * @brief A class for simple unweighted directed graphs.
 */
class Graph {
public:
    /// @brief Default constructor. Creates an empty graph.
    Graph() noexcept;

    /**
     * @param vertex Label of the vertex.
     * @return Whether the vertex is in the graph.
     *
     * Time complexity: O(1)
     */
    bool hasVertex(const std::string& vertex) const noexcept;

    /**
     * @param start Start point of the edge.
     * @param end End point of the edge.
     * @return Whether the edge is in the graph.
     *
     * Time complexity: O(|E|) for worst case
     */
    bool hasEdge(const std::string& start, const std::string& end) const noexcept;

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
     * @return List of all vertices in the graph.
     *
     * Time complexity: O(|V|)
     */
    std::vector<std::string> getVertices() const noexcept;

    /**
     * @param vertex Given vertex.
     * @return Neighbors of the given vertex in the graph, or empty vector for invalid vertices.
     *
     * Time complexity: O(1)
     */
    const std::vector<std::string>& getNeighbors(const std::string& vertex) const noexcept;

    /**
     * @brief Adds a new vertex to the graph. Empty vertex names are disallowed.
     * @param vertex Label of the new vertex.
     * @return Number of vertices actually added (1 if success, 0 if fail).
     *
     * Time complexity: O(1)
     */
    size_t addVertex(const std::string& vertex) noexcept;

    /**
     * @brief Adds a list of new vertices to the graph. Empty vertex names are disallowed.
     * @tparam Iterator Iterator type.
     * @param begin Begin iterator of the container of new vertices.
     * @param end End iterator of the container of new vertices.
     * @return Number of vertices actually added.
     *
     * Time complexity: O(n) where n is size of input
     */
    template<typename Iterator>
    size_t addVertices(Iterator begin, Iterator end) noexcept {
        size_t prevSize = this->neighbors_.size();
        for (Iterator it = begin; it != end; ++it) {
            const std::string& vertex = *it;
            if (vertex.empty()) continue;
            this->neighbors_[vertex];
        }
        return this->neighbors_.size() - prevSize;
    }

    /**
     * @brief Adds a new edge to the graph.
     * @param start Label of start point.
     * @param end Label of end point.
     * @return Number of edges actually added (1 if success, 0 if fail).
     *
     * Time complexity: O(|E|) for worst case
     */
    size_t addEdge(const std::string& start, const std::string& end) noexcept;

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
     * @param start Start point of the edge.
     * @param end End point of the edge.
     * @return Number of edges actually removed (1 if success, 0 if fail).
     *
     * Time complexity: O(|E|) for worst case
     */
    size_t removeEdge(const std::string& start, const std::string& end) noexcept;

    /**
     * @brief Clears the graph of all edges.
     *
     * Time complexity: O(|E|)
     */
    void clearEdges() noexcept;

    /**
     * @brief Clears the graph of all vertices and edges.
     *
     * Time complexity: O(|E|)
     */
    void clear() noexcept;

private:
    std::unordered_map<std::string, std::vector<std::string>> neighbors_;
    size_t edgeCount_;
};

/**
 * @param graph A simple undirected unweighted bipartite graph. (To be undirected, just insert edges in both directions)
 * @param partition Partition of the bipartite graph. Maps vertices to either 0 or 1 according to their partition group.
 * @return The maximum number of matching in the graph.
 *
 * Note this function will not validate the input.
 *
 * Time complexity: O(|E|sqrt(|V|))
 */
size_t hopcroftKarp(const Graph& graph, const std::unordered_map<std::string, int>& partition, int verbose = 0) noexcept;

#endif // GRAPH_H
