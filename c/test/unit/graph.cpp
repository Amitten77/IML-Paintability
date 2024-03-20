#include <string>
#include <vector>
#include <unordered_map>
#include <gtest/gtest.h>
#include "graph.h"

void construct(
        const std::vector<std::string>& part1, const std::vector<std::string>& part2,
        std::vector<std::string>& vertices, std::unordered_map<std::string, int>& partition) {

    vertices.clear();
    partition.clear();
    for (const std::string& vertex : part1) {
        vertices.push_back(vertex);
        partition[vertex] = 0;
    }
    for (const std::string& vertex : part2) {
        vertices.push_back(vertex);
        partition[vertex] = 1;
    }
}

void populate_graph(
        Graph& graph, const std::vector<std::string>& vertices,
        const std::vector<std::pair<std::string, std::string>>& edges) {

    graph.clear();
    graph.addVertices(vertices.begin(), vertices.end());
    for (const auto& [a, b] : edges) {
        graph.addEdge(a, b);
        graph.addEdge(b, a);
    }
}

TEST(graph, hopcroft_karp) {
    Graph graph;
    std::vector<std::string> part1;
    std::vector<std::string> part2;
    std::vector<std::string> vertices;
    std::unordered_map<std::string, int> partition;

    part1 = { "A0", "A1", "A2" };
    part2 = { "B0", "B1", "B2" };
    construct(part1, part2, vertices, partition);
    populate_graph(graph, vertices, {
        { "A0", "B0" }, { "A1", "B1" }, { "A2", "B2" }
    });
    EXPECT_EQ(hopcroftKarp(graph, partition), 3);

    part1 = { "A0", "A1", "A2" };
    part2 = { "B0", "B1", "B2" };
    construct(part1, part2, vertices, partition);
    populate_graph(graph, vertices, {
            { "A0", "B0" }, { "A0", "B1" }, { "A0", "B2" }
    });
    EXPECT_EQ(hopcroftKarp(graph, partition), 1);
}
