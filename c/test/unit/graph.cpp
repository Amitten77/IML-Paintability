#include <string>
#include <vector>
#include <unordered_map>
#include <gtest/gtest.h>
#include "graph.h"

namespace test::graph {
    void construct(
            const std::vector<std::string>& part1, const std::vector<std::string>& part2,
            std::vector<std::string>& vertices, std::unordered_map<std::string, int>& partition) {

        vertices.clear();
        partition.clear();
        for (const std::string& vertex: part1) {
            vertices.push_back(vertex);
            partition[vertex] = 0;
        }
        for (const std::string& vertex: part2) {
            vertices.push_back(vertex);
            partition[vertex] = 1;
        }
    }

    void populate_graph(
            Graph& graph, const std::vector<std::string>& vertices,
            const std::vector<std::pair<std::string, std::string>>& edges) {

        graph.clear();
        graph.addVertices(vertices.begin(), vertices.end());
        for (const auto& [a, b]: edges) {
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

        // 3x3, no edges
        part1 = { "A0", "A1", "A2" };
        part2 = { "B0", "B1", "B2" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {});
        EXPECT_EQ(hopcroftKarp(graph, partition), 0);

        // 3x3, connected in pairs
        part1 = { "A0", "A1", "A2" };
        part2 = { "B0", "B1", "B2" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" },
                { "A1", "B1" },
                { "A2", "B2" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 3);

        // 3x3, one A connected to three B
        part1 = { "A0", "A1", "A2" };
        part2 = { "B0", "B1", "B2" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" }, { "A0", "B1" }, { "A0", "B2" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 1);

        // 3x3, three A connected to one B
        part1 = { "A0", "A1", "A2" };
        part2 = { "B0", "B1", "B2" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" },
                { "A1", "B0" },
                { "A2", "B0" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 1);

        // 3x3, random 1
        part1 = { "A0", "A1", "A2" };
        part2 = { "B0", "B1", "B2" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B1" },
                { "A1", "B0" }, { "A1", "B2" },
                { "A2", "B2" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 3);

        // 3x3, random 2
        part1 = { "A0", "A1", "A2" };
        part2 = { "B0", "B1", "B2" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" }, { "A0", "B1" },
                { "A1", "B1" },
                { "A2", "B2" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 3);

        // 4x2, random
        part1 = { "A0", "A1", "A2", "A3" };
        part2 = { "B0", "B1" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" },
                { "A1", "B1" },
                { "A2", "B0" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 2);

        // 4x4, loop
        part1 = { "A0", "A1", "A2", "A3" };
        part2 = { "B0", "B1", "B2", "B3" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" }, { "A0", "B1" },
                { "A1", "B1" }, { "A1", "B2" },
                { "A2", "B2" }, { "A2", "B3" },
                { "A3", "B3" }, { "A3", "B0" }
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 4);

        // 4x4
        part1 = { "A0", "A1", "A2", "A3" };
        part2 = { "B0", "B1", "B2", "B3" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" }, { "A0", "B1" }, { "A0", "B2" }, { "A0", "B3" },
                { "A1", "B0" },
                { "A2", "B1" },
                { "A3", "B2" },
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 4);

        // 4x4
        part1 = { "A0", "A1", "A2", "A3" };
        part2 = { "B0", "B1", "B2", "B3" };
        construct(part1, part2, vertices, partition);
        populate_graph(graph, vertices, {
                { "A0", "B0" }, { "A0", "B1" }, { "A0", "B2" }, { "A0", "B3" },
                { "A1", "B0" },
                { "A2", "B1" },
                { "A3", "B1" },
        });
        EXPECT_EQ(hopcroftKarp(graph, partition), 3);
    }
}
