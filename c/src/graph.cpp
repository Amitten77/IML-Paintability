#include <algorithm>
#include <queue>
#include <unordered_set>
#include "graph.h"

Graph::Graph() noexcept : neighbors_(), edgeCount_(0) {}

bool Graph::hasVertex(const std::string& vertex) const noexcept {
    return this->neighbors_.contains(vertex);
}

bool Graph::hasEdge(const std::string& start, const std::string& end) const noexcept {
    if (!this->hasVertex(start) || !this->hasVertex(end)) return false;
    const std::vector<std::string>& neighbors = this->neighbors_.at(start);
    return std::find(neighbors.begin(), neighbors.end(), end) != neighbors.end();
}

size_t Graph::countV() const noexcept {
    return this->neighbors_.size();
}

size_t Graph::countE() const noexcept {
    return this->edgeCount_;
}

std::vector<std::string> Graph::getVertices() const noexcept {
    std::vector<std::string> vertices;
    vertices.reserve(this->countV());
    for (const auto& [vertex, _] : this->neighbors_) {
        vertices.push_back(vertex);
    }
    return vertices;
}

const std::vector<std::string>& Graph::getNeighbors(const std::string& vertex) const noexcept {
    static const std::vector<std::string> empty;
    if (!this->hasVertex(vertex)) return empty;
    return this->neighbors_.at(vertex);
}

size_t Graph::addVertex(const std::string& vertex) noexcept {
    if (this->hasVertex(vertex)) return 0;
    this->neighbors_[vertex];
    return 1;
}

size_t Graph::addEdge(const std::string& start, const std::string& end) noexcept {
    if (!this->hasVertex(start) || !this->hasVertex(end)) return 0;
    if (this->hasEdge(start, end)) return 0;
    this->neighbors_[start].push_back(end);
    edgeCount_++;
    return 1;
}

size_t Graph::removeVertex(const std::string& vertex) noexcept {
    if (!this->hasVertex(vertex)) return 0;

    // Remove edges to the vertex
    for (auto& [start, neighbors] : this->neighbors_) {
        edgeCount_ -= std::erase(neighbors, vertex);
    }

    // Remove the edges from the vertex and the vertex itself
    edgeCount_ -= this->neighbors_[vertex].size();
    this->neighbors_.erase(vertex);

    return 1;
}

size_t Graph::removeEdge(const std::string& start, const std::string& end) noexcept {
    if (!this->hasEdge(start, end)) return 0;
    std::erase(this->neighbors_[start], end);
    edgeCount_--;
    return 1;
}

void Graph::clearEdges() noexcept {
    for (auto& [vertex, _] : this->neighbors_) {
        this->neighbors_[vertex].clear();
    }
    this->edgeCount_ = 0;
}

void Graph::clear() noexcept {
    this->neighbors_.clear();
    this->edgeCount_ = 0;
}

// Helper
std::vector<std::vector<std::string>> getDisjointPaths(
        const Graph& graph, const std::unordered_map<std::string, std::string>& matching,
        const std::unordered_set<std::string>& unmatched1, const std::unordered_set<std::string>& unmatched2,
        int verbose) {

    using namespace std;

    Graph pathGraph;

    vector<string> frontier(unmatched1.begin(), unmatched1.end());
    pathGraph.addVertices(unmatched1.begin(), unmatched1.end());
    int curLevel = 0;
    bool reachedEnd = false;

    if (verbose) {
        printf("Path graph layers:\n");
        if (!frontier.empty()) {
            for (const string& vertex: frontier) {
                printf("   %s", vertex.c_str());
            }
            printf("\n");
        }
    }

    // Do BFS to construct path graph
    while (!reachedEnd && !frontier.empty()) {
        curLevel++;
        vector<string> newFrontier;

        // Each time add a new layer
        for (const string& vertex : frontier) {
            const string& match = matching.at(vertex);

            // Partition can be determined from the level
            if ((curLevel - 1) % 2 == 0) {
                // If vertex is in the first partition, then look for edges not in the matching
                for (const string& neighbor : graph.getNeighbors(vertex)) {
                    if (neighbor == match) continue;
                    if (pathGraph.hasVertex(neighbor)) continue;

                    // Add neighbor to graph
                    pathGraph.addVertex(neighbor);
                    pathGraph.addEdge(neighbor, vertex);

                    // Add neighbor to frontier
                    newFrontier.push_back(neighbor);

                    // Check if should end
                    if (unmatched2.contains(neighbor)) reachedEnd = true;
                }
            } else {
                // If vertex is in the second partition, then look for edges in the matching
                if (match.empty()) continue;
                if (pathGraph.hasVertex(match)) continue;

                // Add neighbor to graph
                pathGraph.addVertex(match);
                pathGraph.addEdge(match, vertex);

                // Add neighbor to frontier
                newFrontier.push_back(match);

                // Check if should end
                if (unmatched2.contains(match)) reachedEnd = true;
            }
        }

        frontier = std::move(newFrontier);
        if (verbose) {
            printf(" ");
            for (const string& vertex : frontier) {
                printf(" (%s)", pathGraph.getNeighbors(vertex)[0].c_str());
            }
            printf("\n");
            for (const string& vertex : frontier) {
                printf("   %s", vertex.c_str());
            }
            printf("\n");
        }
    }

    // Use DFS to find disjoint paths
    vector<vector<string>> paths;
    unordered_set<string> usedVertices;
    for (const string& start : unmatched2) {
        if (!pathGraph.hasVertex(start)) continue;
        vector<string> path;
        string curr = start;
        path.push_back(start);

        // Find a disjoint path
        while (true) {
            // Try finding a valid next step
            bool found = false;
            for (const string& next : pathGraph.getNeighbors(curr)) {
                if (usedVertices.contains(next)) continue;
                path.push_back(next);
                curr = next;
                found = true;
            }

            // If no valid next step, then end search for path
            if (!found) {
                break;
            }

            // If reached end point, then record this path
            if (unmatched1.contains(curr)) {
                paths.push_back(path);
                usedVertices.insert(path.begin(), path.end());
                break;
            }
        }
    }

    return paths;
}

size_t hopcroftKarp(const Graph& graph, const std::unordered_map<std::string, int>& partition, int verbose) noexcept {
    using namespace std;

    // Find the partitions
    vector<string> part1;
    vector<string> part2;
    for (const auto& [vertex, part] : partition) {
        (part ? part2 : part1).push_back(vertex);
    }

    // Matching: Maps a vertex in part1 to a vertex in part2 and vice versa. If no match then maps to empty string.
    unordered_map<string, string> matching;
    // Unmatched vertices in part1
    unordered_set<string> unmatched1(part1.begin(), part1.end());
    unordered_set<string> unmatched2(part2.begin(), part2.end());

    // Start with empty set of matching
    for (const auto& [vertex, part] : partition) {
        matching[vertex];
    }

    while (!part1.empty() && !part2.empty()) {
        if (verbose) {
            printf("Current matching:\n");
            for (const string& vertex : part1) {
                printf("  %s -- %s\n", vertex.c_str(), matching[vertex].c_str());
            }
        }

        // Get the disjoint paths
        vector<vector<string>> disjointPaths = getDisjointPaths(graph, matching, unmatched1, unmatched2, verbose);

        if (verbose) {
            printf("Disjoint paths:\n");
            for (const vector<string>& path : disjointPaths) {
                if (path.empty()) {
                    printf("  (empty)\n");
                    break;
                }
                for (size_t i = 0; i < path.size(); i++) {
                    printf(i ? " --" : " ");
                    printf(" %s", path[i].c_str());
                }
                printf("\n");
            }
        }

        // If empty, we have the maximum matching
        if (disjointPaths.empty()) break;

        for (const vector<string>& path : disjointPaths) {
            // Augment the original matching
            for (size_t i = 0; i + 1 < path.size(); i += 2) {
                const string& v1 = path[i];
                const string& v2 = path[i+1];
                matching[v1] = v2;
                matching[v2] = v1;
            }
        }

        // Update unmatched vertices
        erase_if(unmatched1, [&matching](const string& vertex) {
            return !matching[vertex].empty();
        });
        erase_if(unmatched2, [&matching](const string& vertex) {
            return !matching[vertex].empty();
        });
    }

    // Count final matching
    size_t count = 0;
    for (const string& vertex : part1) {
        if (!matching[vertex].empty()) count++;
    }
    return count;
}
