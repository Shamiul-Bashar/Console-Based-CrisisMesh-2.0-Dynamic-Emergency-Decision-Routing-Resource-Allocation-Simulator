#pragma once

#include "dsa/Array.hpp"
#include "dsa/MinHeap.hpp"
#include "graph/Graph.hpp"
#include <limits>

namespace crisismesh {

struct DijkstraNode {
    int node{-1};
    double distance{0.0};
};

struct DijkstraLower {
    bool operator()(const DijkstraNode& a, const DijkstraNode& b) const {
        return a.distance < b.distance;
    }
};

struct RouteResult {
    bool reachable{false};
    double cost{0.0};
    double distance{0.0};
    int travelTime{0};
    DynamicArray<int> nodes;
    DynamicArray<int> edges;
};

class Dijkstra {
public:
    static RouteResult shortestPath(const Graph& graph, int source, int target) {
        RouteResult result;
        const int n = graph.nodeCount();
        if (source < 0 || target < 0 || source >= n || target >= n) return result;

        const double INF = std::numeric_limits<double>::infinity();
        double* dist = new double[n];
        int* parent = new int[n];
        int* parentEdge = new int[n];
        bool* done = new bool[n]{};
        for (int i = 0; i < n; ++i) {
            dist[i] = INF;
            parent[i] = -1;
            parentEdge[i] = -1;
        }

        MinHeap<DijkstraNode, DijkstraLower> heap;
        dist[source] = 0.0;
        heap.push({source, 0.0});

        while (!heap.empty()) {
            const DijkstraNode current = heap.pop();
            const int u = current.node;
            if (done[u]) continue;
            done[u] = true;
            if (u == target) break;

            for (const auto& adj : graph.neighbors(u)) {
                const GraphEdge& edge = graph.edge(adj.edgeIndex);
                if (edge.blocked) continue;
                const double next = dist[u] + edge.weightedCost();
                if (next < dist[adj.to]) {
                    dist[adj.to] = next;
                    parent[adj.to] = u;
                    parentEdge[adj.to] = adj.edgeIndex;
                    heap.push({adj.to, next});
                }
            }
        }

        if (dist[target] != INF) {
            result.reachable = true;
            result.cost = dist[target];
            DynamicArray<int> reverseNodes;
            DynamicArray<int> reverseEdges;
            int cur = target;
            reverseNodes.pushBack(cur);
            while (cur != source) {
                const int e = parentEdge[cur];
                if (e < 0) break;
                reverseEdges.pushBack(e);
                const GraphEdge& edge = graph.edge(e);
                result.distance += edge.distance;
                result.travelTime += edge.travelTime;
                cur = parent[cur];
                reverseNodes.pushBack(cur);
            }
            for (int i = static_cast<int>(reverseNodes.size()) - 1; i >= 0; --i) result.nodes.pushBack(reverseNodes[i]);
            for (int i = static_cast<int>(reverseEdges.size()) - 1; i >= 0; --i) result.edges.pushBack(reverseEdges[i]);
        }

        delete[] dist;
        delete[] parent;
        delete[] parentEdge;
        delete[] done;
        return result;
    }
};

} // namespace crisismesh
