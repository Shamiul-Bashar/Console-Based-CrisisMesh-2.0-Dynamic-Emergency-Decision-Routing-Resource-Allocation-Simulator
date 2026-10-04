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
    double cost{0.0};       // Operational weighted cost of the selected path.
    double distance{0.0};   // Physical distance in km.
    int travelTime{0};      // Total travel time in minutes.
    DynamicArray<int> nodes;
    DynamicArray<int> edges;
};

// DSA Algorithm: Dijkstra shortest path using the manual Min Heap.
class Dijkstra {
private:
    static RouteResult compute(const Graph& graph, int source, int target, bool distanceOnly) {
        RouteResult result;
        const int n = graph.nodeCount();
        if (source < 0 || target < 0 || source >= n || target >= n) return result;

        const double INF = std::numeric_limits<double>::infinity();
        double* best = new double[n];
        int* parent = new int[n];
        int* parentEdge = new int[n];
        bool* done = new bool[n]{};

        for (int i = 0; i < n; ++i) {
            best[i] = INF;
            parent[i] = -1;
            parentEdge[i] = -1;
        }

        MinHeap<DijkstraNode, DijkstraLower> heap;
        best[source] = 0.0;
        heap.push({source, 0.0});

        // DSA operation: extract the minimum frontier and relax open edges.
        while (!heap.empty()) {
            const DijkstraNode current = heap.pop();
            const int u = current.node;
            if (done[u]) continue;
            done[u] = true;
            if (u == target) break;

            for (const auto& adj : graph.neighbors(u)) {
                const GraphEdge& edge = graph.edge(adj.edgeIndex);
                if (edge.blocked) continue;

                const double edgeWeight = distanceOnly ? edge.distance : edge.weightedCost();
                const double next = best[u] + edgeWeight;

                if (next < best[adj.to]) {
                    best[adj.to] = next;
                    parent[adj.to] = u;
                    parentEdge[adj.to] = adj.edgeIndex;
                    heap.push({adj.to, next});
                }
            }
        }

        if (best[target] != INF) {
            result.reachable = true;
            // DSA: Dynamic Arrays store reconstructed route nodes and edges.
            DynamicArray<int> reverseNodes;
            DynamicArray<int> reverseEdges;

            int cur = target;
            reverseNodes.pushBack(cur);

            while (cur != source) {
                const int edgeIndex = parentEdge[cur];
                if (edgeIndex < 0) {
                    result.reachable = false;
                    result.nodes.clear();
                    result.edges.clear();
                    result.distance = 0.0;
                    result.travelTime = 0;
                    result.cost = 0.0;
                    break;
                }

                reverseEdges.pushBack(edgeIndex);
                const GraphEdge& edge = graph.edge(edgeIndex);
                result.distance += edge.distance;
                result.travelTime += edge.travelTime;
                result.cost += edge.weightedCost();

                cur = parent[cur];
                reverseNodes.pushBack(cur);
            }

            if (result.reachable) {
                for (int i = static_cast<int>(reverseNodes.size()) - 1; i >= 0; --i)
                    result.nodes.pushBack(reverseNodes[i]);
                for (int i = static_cast<int>(reverseEdges.size()) - 1; i >= 0; --i)
                    result.edges.pushBack(reverseEdges[i]);
            }
        }

        delete[] best;
        delete[] parent;
        delete[] parentEdge;
        delete[] done;
        return result;
    }

public:
    // Operational route: minimizes the weighted road cost
    // (distance + time + risk + congestion + capacity penalty).
    static RouteResult shortestPath(const Graph& graph, int source, int target) {
        return compute(graph, source, target, false);
    }

    // Physical shortest-distance route: minimizes kilometers only.
    // Used by Incident Analysis so the displayed "shortest distance/path"
    // exactly matches the requested metric.
    static RouteResult shortestDistancePath(const Graph& graph, int source, int target) {
        return compute(graph, source, target, true);
    }
};

} // namespace crisismesh
