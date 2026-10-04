#pragma once

#include "dsa/Array.hpp"
#include "dsa/Queue.hpp"
#include "graph/Graph.hpp"

namespace crisismesh {

// DSA Algorithm: Breadth-First Search using the manual Queue and a visited array.
class BFS {
public:
    static DynamicArray<int> traverse(const Graph& graph, int start) {
        DynamicArray<int> order;
        if (start < 0 || start >= graph.nodeCount()) return order;

        bool* visited = new bool[graph.nodeCount()]{};
        Queue<int> queue;
        visited[start] = true;
        queue.enqueue(start);

        while (!queue.empty()) {
            const int u = queue.dequeue();
            order.pushBack(u);
            for (const auto& adj : graph.neighbors(u)) {
                if (graph.edge(adj.edgeIndex).blocked) continue;
                if (!visited[adj.to]) {
                    visited[adj.to] = true;
                    queue.enqueue(adj.to);
                }
            }
        }
        delete[] visited;
        return order;
    }
};

} // namespace crisismesh
