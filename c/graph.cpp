#include <algorithm>
#include "graph.h"

Graph::Graph() noexcept : neighbors_(), edgeCount_(0) {}

Graph::Graph(const std::vector<std::string>& vertices) noexcept : neighbors_(), edgeCount_(0) {
    this->addVertices(vertices);
}

bool Graph::hasVertex(const std::string& vertex) const noexcept {
    return this->neighbors_.contains(vertex);
}

bool Graph::hasEdge(const std::string& vertex1, const std::string& vertex2) const noexcept {
    if (!this->hasVertex(vertex1) || !this->hasVertex(vertex2)) return false;
    const std::vector<std::string>& neighbor1 = this->neighbors_.at(vertex1);
    return std::find(neighbor1.begin(), neighbor1.end(), vertex2) != neighbor1.end();
}

size_t Graph::countV() const noexcept {
    return this->neighbors_.size();
}

size_t Graph::countE() const noexcept {
    return this->edgeCount_;
}

size_t Graph::addVertex(const std::string& vertex) noexcept {
    if (this->hasVertex(vertex)) return 0;
    this->neighbors_[vertex];
    return 1;
}

size_t Graph::addVertices(const std::vector<std::string>& vertices) noexcept {
    size_t prevSize = this->neighbors_.size();
    for (const std::string& vertex : vertices) {
        this->neighbors_[vertex];
    }
    return this->neighbors_.size() - prevSize;
}

size_t Graph::addEdge(const std::string& vertex1, const std::string& vertex2) noexcept {
    if (!this->hasVertex(vertex1) || !this->hasVertex(vertex2)) return 0;
    if (this->hasEdge(vertex1, vertex2)) return 0;
    this->neighbors_[vertex1].push_back(vertex2);
    this->neighbors_[vertex2].push_back(vertex1);
    edgeCount_++;
    return 1;
}

size_t Graph::removeVertex(const std::string& vertex) noexcept {
    if (!this->hasVertex(vertex)) return 0;
    // Remove edges
    for (const std::string& neighbor : this->neighbors_[vertex]) {
        std::erase(this->neighbors_[neighbor], vertex);
        edgeCount_--;
    }
    // Remove the vertex
    this->neighbors_.erase(vertex);
    return 1;
}

size_t Graph::removeEdge(const std::string& vertex1, const std::string& vertex2) noexcept {
    if (!this->hasEdge(vertex1, vertex2)) return 0;
    std::erase(this->neighbors_[vertex1], vertex2);
    std::erase(this->neighbors_[vertex2], vertex1);
    edgeCount_--;
    return 1;
}

void Graph::clearEdges() noexcept {
    for (auto& [v, _] : this->neighbors_) {
        this->neighbors_[v].clear();
    }
    this->edgeCount_ = 0;
}

void Graph::clear() noexcept {
    this->neighbors_.clear();
    this->edgeCount_ = 0;
}
