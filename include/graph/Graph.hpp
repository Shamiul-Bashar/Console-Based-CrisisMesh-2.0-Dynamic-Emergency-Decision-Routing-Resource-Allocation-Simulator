#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace crisismesh {

struct GraphNode {
    std::string id;
    std::string name;
    std::string category;
};

struct GraphEdge {
    std::string id;
    int from{-1};
    int to{-1};
    double distance{0.0};
    int travelTime{0};
    int risk{0};
    int congestion{0};
    int capacity{0};
    bool blocked{false};

    double weightedCost() const {
        return distance + 0.35 * travelTime + 0.75 * risk + 0.45 * congestion
             + 0.03 * std::max(0, 70 - capacity);
    }
};

struct AdjEdge {
    int to{-1};
    int edgeIndex{-1};
};

class Graph {
private:
    std::vector<GraphNode> nodes_;
    std::vector<GraphEdge> edges_;
    std::vector<std::vector<AdjEdge>> adjacency_;

public:
    int addNode(const std::string& id, const std::string& name, const std::string& category) {
        nodes_.push_back({id, name, category});
        adjacency_.push_back({});
        return static_cast<int>(nodes_.size()) - 1;
    }

    int addEdge(const std::string& id, int from, int to, double distance, int time,
                int risk, int congestion, int capacity) {
        const int index = static_cast<int>(edges_.size());
        edges_.push_back({id, from, to, distance, time, risk, congestion, capacity, false});
        adjacency_[from].push_back({to, index});
        adjacency_[to].push_back({from, index});
        return index;
    }

    int findNodeIndex(const std::string& id) const {
        for (std::size_t i = 0; i < nodes_.size(); ++i)
            if (nodes_[i].id == id) return static_cast<int>(i);
        return -1;
    }

    int findEdgeIndex(const std::string& id) const {
        for (std::size_t i = 0; i < edges_.size(); ++i)
            if (edges_[i].id == id) return static_cast<int>(i);
        return -1;
    }

    bool blockEdge(int edgeIndex) {
        if (edgeIndex < 0 || edgeIndex >= static_cast<int>(edges_.size()) || edges_[edgeIndex].blocked) return false;
        edges_[edgeIndex].blocked = true;
        return true;
    }

    bool openEdge(int edgeIndex) {
        if (edgeIndex < 0 || edgeIndex >= static_cast<int>(edges_.size()) || !edges_[edgeIndex].blocked) return false;
        edges_[edgeIndex].blocked = false;
        return true;
    }

    const GraphNode& node(int index) const { return nodes_[index]; }
    GraphNode& node(int index) { return nodes_[index]; }
    const GraphEdge& edge(int index) const { return edges_[index]; }
    GraphEdge& edge(int index) { return edges_[index]; }
    const std::vector<AdjEdge>& neighbors(int index) const { return adjacency_[index]; }
    int nodeCount() const { return static_cast<int>(nodes_.size()); }
    int edgeCount() const { return static_cast<int>(edges_.size()); }

    void seedCrisisMeshCity() {
        nodes_.clear();
        edges_.clear();
        adjacency_.clear();

        // One shared 20-location city model is used by both User and Author portals.
        const char* ids[] = {
            "LOC-001","LOC-002","LOC-003","LOC-004","LOC-005",
            "LOC-006","LOC-007","LOC-008","LOC-009","LOC-010",
            "LOC-011","LOC-012","LOC-013","LOC-014","LOC-015",
            "LOC-016","LOC-017","LOC-018","LOC-019","LOC-020"
        };
        const char* names[] = {
            "Central Hospital", "North Hospital", "Main Fire Station", "East Fire Station", "Central Police Station",
            "North Police Station", "City Market", "North Market", "Central School", "East School",
            "South School", "Emergency Shelter A", "Emergency Shelter B", "Residential Zone A", "Residential Zone B",
            "Residential Zone C", "City Hall", "Bus Terminal", "Industrial Zone", "Central Junction"
        };
        const char* cats[] = {
            "HOSPITAL", "HOSPITAL", "FIRE_STATION", "FIRE_STATION", "POLICE_STATION",
            "POLICE_STATION", "MARKET", "MARKET", "SCHOOL", "SCHOOL",
            "SCHOOL", "SHELTER", "SHELTER", "RESIDENTIAL", "RESIDENTIAL",
            "RESIDENTIAL", "CIVIC", "TRANSPORT", "INDUSTRIAL", "INTERSECTION"
        };

        for (int i = 0; i < 20; ++i) addNode(ids[i], names[i], cats[i]);

        // The road network forms a simple 4 x 5 connected grid: easy to view and explain.
        struct E { const char* id; int a,b; double d; int t,r,c,cap; };
        const E es[] = {
            {"R-001",0,1,1.5,3,1,2,80}, {"R-002",1,2,1.8,4,1,3,75}, {"R-003",2,3,1.4,3,2,2,75}, {"R-004",3,4,1.7,4,1,3,70},
            {"R-005",5,6,1.3,3,1,2,80}, {"R-006",6,7,1.6,4,2,4,70}, {"R-007",7,8,1.2,3,1,3,75}, {"R-008",8,9,1.9,4,2,5,65},
            {"R-009",10,11,1.4,3,1,2,80}, {"R-010",11,12,1.5,3,1,3,75}, {"R-011",12,13,1.8,4,2,4,70}, {"R-012",13,14,1.3,3,1,2,75},
            {"R-013",15,16,1.7,4,2,3,70}, {"R-014",16,17,1.4,3,1,3,75}, {"R-015",17,18,2.0,5,3,5,60}, {"R-016",18,19,1.6,4,2,4,65},
            {"R-017",0,5,1.6,4,1,2,80}, {"R-018",1,6,1.5,3,1,3,75}, {"R-019",2,7,1.7,4,2,3,70}, {"R-020",3,8,1.4,3,1,4,70}, {"R-021",4,9,1.8,4,2,4,65},
            {"R-022",5,10,1.5,3,1,2,80}, {"R-023",6,11,1.9,4,2,4,70}, {"R-024",7,12,1.3,3,1,3,75}, {"R-025",8,13,1.6,4,2,4,70}, {"R-026",9,14,1.7,4,2,5,65},
            {"R-027",10,15,1.8,4,2,3,70}, {"R-028",11,16,1.4,3,1,2,75}, {"R-029",12,17,1.6,4,1,3,70}, {"R-030",13,18,1.9,5,3,5,60}, {"R-031",14,19,1.5,3,2,3,70}
        };

        for (const auto& e : es) addEdge(e.id, e.a, e.b, e.d, e.t, e.r, e.c, e.cap);
    }
};

} // namespace crisismesh
