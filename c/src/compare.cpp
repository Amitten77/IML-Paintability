#include <algorithm>
#include <functional>
#include <ranges>
#include <queue>
#include "../include/compare.h"

#define NIL 0
#define INF INT_MAX

bool bfs(const std::vector<std::unordered_set<int>>& relation, std::vector<int>& distance, std::vector<int>& pairU, std::vector<int>& pairV, int n) {
    std::queue<int> Q; //an integer queue
 
    // First layer of vertices (set distance as 0)
    for (int u=1; u<=n; u++)
    {
        // If this is a free vertex, add it to queue
        if (pairU[u]==NIL)
        {
            // u is not matched
            distance[u] = 0;
            Q.push(u);
        }
 
        // Else set distance as infinite so that this vertex
        // is considered next time
        else distance[u] = INF;
    }
 
    // Initialize distance to NIL as infinite
    distance[NIL] = INF;
 
    // Q is going to contain vertices of left side only. 
    while (!Q.empty())
    {
        // Dequeue a vertex
        int u = Q.front();
        Q.pop();
 
        // If this node is not NIL and can provide a shorter path to NIL
        if (distance[u] < distance[NIL])
        {
            // Get all adjacent vertices of the dequeued vertex u
            for (int v : relation[u])
            {
 
                // If pair of v is not considered so far
                // (v, pairV[V]) is not yet explored edge.
                if (distance[pairV[v]] == INF)
                {
                    // Consider the pair and add it to queue
                    distance[pairV[v]] = distance[u] + 1;
                    Q.push(pairV[v]);
                }
            }
        }
    }
 
    // If we could come back to NIL using alternating path of distinct
    // vertices then there is an augmenting path
    return (distance[NIL] != INF);
}

bool dfs(int u, const std::vector<std::unordered_set<int>>& relation, std::vector<int>& distance, std::vector<int>& pairU, std::vector<int>& pairV) {
    if (u != NIL)
    {
        for (int v: relation[u])
        {
 
 
            // Follow the distances set by BFS
            if (distance[pairV[v]] == distance[u]+1)
            {
                // If dfs for pair of v also returns
                // true
                if (dfs(pairV[v], relation, distance, pairU, pairV) == true)
                {
                    pairV[v] = u;
                    pairU[u] = v;
                    return true;
                }
            }
        }
 
        // If there is no augmenting path beginning with u.
        distance[u] = INF;
        return false;
    }
    return true;
}


bool checkHopcroft(const std::vector<std::unordered_set<int>>& relation, int n) {
    // pairU[u] stores pair of u in matching where u
    // is a vertex on left side of Bipartite Graph.
    // If u doesn't have any pair, then pairU[u] is NIL
    std::vector<int> pairU(n + 1);
 
    // pairV[v] stores pair of v in matching. If v
    // doesn't have any pair, then pairU[v] is NIL
    std::vector<int> pairV(n + 1);
 
    // dist[u] stores distance of left side vertices
    // dist[u] is one more than dist[u'] if u is next
    // to u'in augmenting path
    std::vector<int> distance(n + 1);
 
    // Initialize NIL as pair of all vertices
    for (int u=0; u<=n; u++)
        pairU[u] = NIL;
    for (int v=0; v<=n; v++)
        pairV[v] = NIL;
 
    // Initialize result
    int result = 0;
 
    // Keep updating the result while there is an
    // augmenting path.
    while (bfs(relation, distance, pairU, pairV, n))
    {
        // Find a free vertex
        for (int u=1; u<=n; u++)
 
            // If current vertex is free and there is
            // an augmenting path from current vertex
            if (pairU[u]==NIL && dfs(u, relation, distance, pairU, pairV))
                result++;
    }
    return result == n;

}


int checkHallsCondition(const std::vector<std::unordered_set<int>>& relation, int n) {
    std::vector<int> subset;
    std::function<bool(int)> backtrack = [&](int start) -> bool {
        std::unordered_set<int> neighbors;
        for (int value : subset) {
            neighbors.insert(relation[value].begin(), relation[value].end());
        }
        if (neighbors.size() < subset.size()) {
            return false;
        }
        if (start == n) {
            return true;
        }

        for (int i = start; i < n; ++i) {
            subset.push_back(i);
            if (!backtrack(i + 1)) {
                return false;
            }
            subset.pop_back();
        }

        return true;
    };

    return backtrack(0);
}


CompResult compareBoards(const Board& board1, const Board& board2, Purpose purpose) {
    if (board1.n != board2.n || board1.k != board2.k) {
        return CompResult::INCOMPARABLE;
    }
    int n = board1.n;
    int k = board1.k;
    bool possLess = true;
    bool possMore = true;
    if (purpose == Purpose::LESS) {
        possMore = false;
    }
    if (purpose == Purpose::GREATER) {
        possLess = false;
    }
    if (board1.num_tokens > board2.num_tokens) {
        possLess = false;
    }

    if (board1.num_tokens < board2.num_tokens) {
        possMore = false;
    }
    if (!possLess && !possMore) {
        return CompResult::INCOMPARABLE;
    }
    std::vector<int> board1bst;
    std::vector<int> board2bst;
    for (int i = 0; i < n; ++i) {
        board1bst.push_back(board1.board[board1.get_index(i, k-1)]);
        board2bst.push_back(board2.board[board2.get_index(i, k-1)]);
    }
    std::sort(board1bst.begin(), board1bst.end());
    std::sort(board2bst.begin(), board2bst.end());
    for (int i = 0; i < n; ++i) {
        if (board1bst[i] > board2bst[i]) {
            possLess = false;
        }
        if (board1bst[i] < board2bst[i]) {
            possMore = false;
        }
        if (!possLess && !possMore) {
            return CompResult::INCOMPARABLE;
        }
    }

    std::vector<std::unordered_set<int>> lessThanRelation(n + 1), greaterThanRelation(n + 1);



    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            bool less = true, greater = true;
            for (int item = 0; item < board1.k; ++item) {
                int item1 = board1.board[board1.get_index(i, item)];
                int item2 = board2.board[board2.get_index(j, item)];
                if (item1 > item2) less = false;
                if (item1 < item2) greater = false;
                if (!greater && !less) break;
            }
            if (less) {
                lessThanRelation[1 + i].insert(1 + j);
            }
            if (greater) {
                greaterThanRelation[1 + i].insert(1 + j);
            } 
        }
        if (lessThanRelation[1 + i].size() == 0) {
            possLess = false;
        }
        if (greaterThanRelation[1 + i].size() == 0) {
            possMore = false;
        }
        if (!possLess && !possMore) {
            return CompResult::INCOMPARABLE;
        }
    }
    if (!possLess && !possMore) {
        return CompResult::INCOMPARABLE;
    }
    // Check Hall's condition for both relations
    bool isMore = false;
    bool isLess = false;
    if (possMore && !possLess) {
        isMore = checkHopcroft(greaterThanRelation, n);
    } else if (possLess && !possMore) {
        isLess = checkHopcroft(lessThanRelation, n);
    } else {
        isMore = checkHopcroft(greaterThanRelation, n);
        isLess = checkHopcroft(lessThanRelation, n);
    }
    if (isMore && isLess) return CompResult::EQUAL;
    if (isLess) return CompResult::LESS;
    if (isMore) return CompResult::GREATER;
    return CompResult::INCOMPARABLE;
}