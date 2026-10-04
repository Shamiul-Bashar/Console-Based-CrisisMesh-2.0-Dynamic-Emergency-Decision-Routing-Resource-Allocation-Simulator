#pragma once

#include "dsa/Array.hpp"
#include "dsa/Stack.hpp"
#include "graph/Graph.hpp"

namespace crisismesh {

class DFS {
public:
    static DynamicArray<int> traverse(const Graph& graph, int start) {
        DynamicArray<int> order;
        if (start < 0 || start >= graph.nodeCount()) return order;

        bool* visited = new bool[graph.nodeCount()]{};
        Stack<int> stack;
        stack.push(start);

        while (!stack.empty()) {
            const int u = stack.pop();
            if (visited[u]) continue;
            visited[u] = true;
            order.pushBack(u);

            const auto& n = graph.neighbors(u);
            for (int i = static_cast<int>(n.size()) - 1; i >= 0; --i) {
                if (graph.edge(n[i].edgeIndex).blocked) continue;
                if (!visited[n[i].to]) stack.push(n[i].to);
            }
        }
        delete[] visited;
        return order;
    }
};

} // namespace crisismesh
