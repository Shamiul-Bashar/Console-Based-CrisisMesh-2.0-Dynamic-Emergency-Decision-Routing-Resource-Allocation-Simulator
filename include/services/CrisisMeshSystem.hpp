#pragma once

#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "algorithms/MergeSort.hpp"
#include "dsa/AVLTree.hpp"
#include "dsa/Array.hpp"
#include "dsa/HashTable.hpp"
#include "dsa/LinkedList.hpp"
#include "dsa/MaxHeap.hpp"
#include "dsa/Queue.hpp"
#include "dsa/Stack.hpp"
#include "graph/Graph.hpp"
#include "models/Models.hpp"

#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

namespace crisismesh {

class CrisisMeshSystem {
    static constexpr std::size_t MAX_USERS = 100;
    static constexpr std::size_t MAX_INCIDENTS = 250;
    static constexpr std::size_t MAX_RESPONDERS = 32;
    static constexpr std::size_t MAX_SHELTERS = 8;
    static constexpr std::size_t MAX_RESOURCES = 16;

    Graph graph_;
    StaticArray<User, MAX_USERS> users_;
    StaticArray<Incident, MAX_INCIDENTS> incidents_;
    StaticArray<Responder, MAX_RESPONDERS> responders_;
    StaticArray<Shelter, MAX_SHELTERS> shelters_;
    StaticArray<SupplyResource, MAX_RESOURCES> resources_;

    Queue<int> intakeQueue_; // Manual FIFO queue.
    MaxHeap<IncidentHeapEntry, IncidentHigherPriority> priorityHeap_; // Manual max heap.
    Stack<int> roadUndoStack_; // Manual LIFO stack.
    LinkedList<HistoryEntry> history_; // Manual linked list.
    LinkedList<Message> messages_;
    AVLTree archive_; // Manual balanced tree.
    HashTable<int> incidentIndex_; // Manual separate-chaining hash table.
    HashTable<int> usernameIndex_;

    long long sequence_{0};
    int nextIncidentNumber_{201};
    int nextMessageId_{1};

    Responder* responderById(const std::string& id) {
        for (std::size_t i = 0; i < responders_.size(); ++i)
            if (responders_[i].id == id) return &responders_[i];
        return nullptr;
    }

    bool routeUsesEdge(const Incident& incident, int edgeIndex) const {
        for (std::size_t i = 0; i < incident.routeEdges.size(); ++i)
            if (incident.routeEdges[i] == edgeIndex) return true;
        return false;
    }

    bool reroute(Incident& incident) {
        Responder* responder = responderById(incident.assignedResponderId);
        if (!responder) return false;
        RouteResult route = Dijkstra::shortestPath(graph_, responder->currentLocation, incident.locationIndex);
        if (!route.reachable) {
            incident.status = IncidentStatus::Unreachable;
            incident.routeNodes.clear();
            incident.routeEdges.clear();
            return false;
        }
        incident.routeNodes = route.nodes;
        incident.routeEdges = route.edges;
        incident.routeCost = route.cost;
        incident.routeDistance = route.distance;
        incident.routeTravelTime = route.travelTime;
        incident.status = IncidentStatus::EnRoute;
        return true;
    }

    void seedOperationalData() {
        const struct R { const char* id; const char* type; int loc; int cap; const char* base; } rs[] = {
            {"FIRE-UNIT-01","FIRE_TRUCK",2,1,"Main Fire Station"},
            {"FIRE-UNIT-02","FIRE_TRUCK",3,1,"East Fire Station"},
            {"FIRE-UNIT-03","FIRE_TRUCK",17,1,"South Terminal Fire Post"},
            {"AMB-UNIT-01","AMBULANCE",0,2,"Central Hospital"},
            {"AMB-UNIT-02","AMBULANCE",1,2,"North Hospital"},
            {"AMB-UNIT-03","AMBULANCE",9,2,"East Medical Post"},
            {"AMB-UNIT-04","AMBULANCE",11,2,"West Shelter Medical Post"},
            {"POLICE-UNIT-01","POLICE_UNIT",4,2,"Central Police Station"},
            {"POLICE-UNIT-02","POLICE_UNIT",5,2,"North Police Station"},
            {"POLICE-UNIT-03","POLICE_UNIT",15,2,"East Police Post"},
            {"RESCUE-UNIT-01","RESCUE_TEAM",11,4,"West Shelter Rescue Base"},
            {"RESCUE-UNIT-02","RESCUE_TEAM",12,4,"East Shelter Rescue Base"}
        };
        for (const auto& r : rs)
            responders_.pushBack({r.id,r.type,r.loc,ResponderAvailability::Available,r.cap,r.base,""});

        shelters_.pushBack({"SHELTER-01",11,120,35,true});
        shelters_.pushBack({"SHELTER-02",12,100,20,true});
        resources_.pushBack({"WATER",500,"Central Warehouse"});
        resources_.pushBack({"FOOD_PACK",350,"Central Warehouse"});
        resources_.pushBack({"MEDICAL_KIT",120,"Central Hospital"});
        resources_.pushBack({"RESCUE_KIT",80,"Rescue Depot"});
    }

public:
    CrisisMeshSystem() { graph_.seedCrisisMeshCity(); seedOperationalData(); }
    const Graph& graph() const { return graph_; }

    int registerUser(const User& input) {
        if (users_.full() || usernameIndex_.get(input.username)) return -1;
        User user = input;
        user.id = static_cast<int>(users_.size()) + 1;
        const int index = static_cast<int>(users_.size());
        users_.pushBack(user);
        usernameIndex_.put(user.username, index);
        return user.id;
    }

    int authenticateUser(const std::string& username, const std::string& password) const {
        const int* index = usernameIndex_.get(username);
        if (!index) return -1;
        return users_[*index].password == password ? users_[*index].id : -1;
    }

    bool resetPassword(const std::string& username, const std::string& password) {
        int* index = usernameIndex_.get(username);
        if (!index) return false;
        users_[*index].password = password;
        return true;
    }

    const User* user(int id) const {
        return id > 0 && id <= static_cast<int>(users_.size()) ? &users_[id - 1] : nullptr;
    }

    int findLocationBinary(const std::string& id) const {
        int left = 0, right = graph_.nodeCount() - 1;
        while (left <= right) {
            const int mid = left + (right - left) / 2;
            const std::string& value = graph_.node(mid).id;
            if (value == id) return mid;
            if (value < id) left = mid + 1; else right = mid - 1;
        }
        return -1;
    }

    void listLocations() const {
        std::cout << "\nCITY LOCATIONS\n";
        for (int i = 0; i < graph_.nodeCount(); ++i)
            std::cout << std::setw(8) << graph_.node(i).id << " | "
                      << std::setw(27) << graph_.node(i).name << " | " << graph_.node(i).category << '\n';
    }

    std::string reportIncident(int userId, IncidentType type, const std::string& locationId,
                               int severity, int urgency, int victims, const std::string& description) {
        const int location = findLocationBinary(locationId);
        if (incidents_.full() || location < 0 || severity < 1 || severity > 5 ||
            urgency < 1 || urgency > 5 || victims < 0) return "";
        Incident incident;
        incident.id = "INC-" + std::to_string(nextIncidentNumber_++);
        incident.type = type;
        incident.locationIndex = location;
        incident.locationId = locationId;
        incident.severity = severity;
        incident.urgency = urgency;
        incident.victimCount = victims;
        incident.priorityScore = calculatePriority(severity, urgency, victims, type);
        incident.sequence = ++sequence_;
        incident.status = IncidentStatus::Queued;
        incident.reportedByUserId = userId;
        incident.description = description;
        incident.requiredResponder = requiredResponderType(type);
        const int index = static_cast<int>(incidents_.size());
        incidents_.pushBack(incident);
        incidentIndex_.put(incident.id, index);
        intakeQueue_.enqueue(index);
        return incident.id;
    }

    Incident* findIncident(const std::string& id) {
        int* index = incidentIndex_.get(id);
        return index ? &incidents_[*index] : nullptr;
    }
    const Incident* findIncident(const std::string& id) const {
        const int* index = incidentIndex_.get(id);
        return index ? &incidents_[*index] : nullptr;
    }

    bool processNext(std::string& out) {
        if (intakeQueue_.empty()) { out = "FIFO intake queue is empty."; return false; }
        const int index = intakeQueue_.dequeue();
        Incident& incident = incidents_[index];
        incident.status = IncidentStatus::Triaged;
        incident.priorityScore = calculatePriority(incident.severity, incident.urgency, incident.victimCount, incident.type);
        incident.status = IncidentStatus::Prioritized;
        priorityHeap_.push({index, incident.priorityScore, incident.sequence});
        out = incident.id + " moved FIFO Queue -> Triage -> Max Heap; priority=" + std::to_string(incident.priorityScore);
        return true;
    }

    bool dispatchHighest(std::string& out) {
        if (priorityHeap_.empty()) { out = "Priority Max Heap is empty."; return false; }
        Incident& incident = incidents_[priorityHeap_.pop().incidentIndex];
        Candidate candidates[32]{};
        std::size_t count = 0;
        for (std::size_t i = 0; i < responders_.size(); ++i) {
            Responder& r = responders_[i];
            if (r.type != incident.requiredResponder || r.availability != ResponderAvailability::Available) continue;
            RouteResult route = Dijkstra::shortestPath(graph_, r.currentLocation, incident.locationIndex);
            candidates[count++] = {static_cast<int>(i), r.id, route.reachable, route.cost, route.distance, route.travelTime};
        }
        if (!count) { incident.status = IncidentStatus::WaitingForResource; out = "No compatible available responder."; return false; }
        MergeSort::sort(candidates, count, [](const Candidate& a, const Candidate& b){ return candidateComesBefore(a,b); });
        if (!candidates[0].reachable) { incident.status = IncidentStatus::Unreachable; out = "All compatible responders are unreachable."; return false; }

        Responder& selected = responders_[candidates[0].responderIndex];
        RouteResult route = Dijkstra::shortestPath(graph_, selected.currentLocation, incident.locationIndex);
        selected.availability = ResponderAvailability::Assigned;
        selected.assignedIncidentId = incident.id;
        incident.assignedResponderId = selected.id;
        incident.routeNodes = route.nodes;
        incident.routeEdges = route.edges;
        incident.routeCost = route.cost;
        incident.routeDistance = route.distance;
        incident.routeTravelTime = route.travelTime;
        incident.status = IncidentStatus::EnRoute;
        out = incident.id + " -> " + selected.id + " (Array candidates + Merge Sort + Dijkstra).";
        return true;
    }

    bool markResponseCompleted(const std::string& id, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident || (incident->status != IncidentStatus::EnRoute && incident->status != IncidentStatus::Assigned)) {
            out = "Incident is not actively dispatched."; return false;
        }
        Responder* responder = responderById(incident->assignedResponderId);
        if (responder) {
            responder->currentLocation = incident->locationIndex;
            responder->availability = ResponderAvailability::Available;
            responder->assignedIncidentId.clear();
        }
        incident->status = IncidentStatus::AwaitingUserConfirmation;
        out = "Field response complete; reporting user must answer YES/NO.";
        return true;
    }

    bool confirmResolution(int userId, const std::string& id, bool solved,
                           const std::string& reason, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident || incident->reportedByUserId != userId ||
            incident->status != IncidentStatus::AwaitingUserConfirmation) {
            out = "Confirmation rejected: wrong owner or invalid state."; return false;
        }
        if (solved) {
            incident->userConfirmedResolved = true;
            incident->status = IncidentStatus::Resolved;
            history_.pushBack({incident->sequence, incident->id, incident->id + " resolved by reporting user"});
            incident->status = IncidentStatus::Closed;
            archive_.insert(incident->sequence, incident->id);
            out = "YES -> RESOLVED -> CLOSED; stored in Linked List history + AVL archive.";
            return true;
        }
        incident->status = IncidentStatus::Escalated;
        incident->escalationReason = reason;
        if (incident->urgency < 5) ++incident->urgency;
        incident->priorityScore = calculatePriority(incident->severity, incident->urgency, incident->victimCount, incident->type);
        incident->assignedResponderId.clear();
        incident->routeNodes.clear(); incident->routeEdges.clear();
        incident->status = IncidentStatus::Queued;
        const int* index = incidentIndex_.get(id);
        if (index) intakeQueue_.enqueue(*index);
        out = "NO -> ESCALATED -> urgency increased -> requeued in FIFO Queue.";
        return true;
    }

    bool blockRoad(const std::string& roadId, std::string& out) {
        const int edgeIndex = graph_.findEdgeIndex(roadId);
        if (edgeIndex < 0 || !graph_.blockEdge(edgeIndex)) { out = "Road not found or already blocked."; return false; }
        roadUndoStack_.push(edgeIndex);
        int rerouted = 0, unreachable = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            Incident& incident = incidents_[i];
            if (incident.status == IncidentStatus::EnRoute && routeUsesEdge(incident, edgeIndex)) {
                incident.status = IncidentStatus::RerouteRequired;
                if (reroute(incident)) ++rerouted; else ++unreachable;
            }
        }
        out = roadId + " blocked; stack depth=" + std::to_string(roadUndoStack_.size()) +
              ", rerouted=" + std::to_string(rerouted) + ", unreachable=" + std::to_string(unreachable);
        return true;
    }

    bool undoRoadBlock(std::string& out) {
        if (roadUndoStack_.empty()) { out = "Road undo Stack is empty."; return false; }
        const int edgeIndex = roadUndoStack_.pop();
        graph_.openEdge(edgeIndex);
        int recovered = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i)
            if (incidents_[i].status == IncidentStatus::Unreachable && !incidents_[i].assignedResponderId.empty() && reroute(incidents_[i])) ++recovered;
        out = graph_.edge(edgeIndex).id + " reopened using LIFO Stack undo; recovered=" + std::to_string(recovered);
        return true;
    }

    bool allocateShelter(const std::string& id, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident) { out = "Incident not found."; return false; }
        int best = -1; double bestCost = std::numeric_limits<double>::infinity();
        const int people = incident->victimCount > 0 ? incident->victimCount : 1;
        for (std::size_t i = 0; i < shelters_.size(); ++i) {
            if (!shelters_[i].operational || shelters_[i].available() < people) continue;
            RouteResult route = Dijkstra::shortestPath(graph_, incident->locationIndex, shelters_[i].locationIndex);
            if (route.reachable && route.cost < bestCost) { best = static_cast<int>(i); bestCost = route.cost; }
        }
        if (best < 0) { out = "No reachable shelter with sufficient capacity."; return false; }
        shelters_[best].occupancy += people;
        incident->shelterId = shelters_[best].id;
        out = "Allocated " + shelters_[best].id + " using Dijkstra + capacity check.";
        return true;
    }

    bool allocateSupply(const std::string& id, const std::string& type, int quantity, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident || quantity <= 0) { out = "Invalid incident/quantity."; return false; }
        for (std::size_t i = 0; i < resources_.size(); ++i) {
            if (resources_[i].type == type && resources_[i].quantity >= quantity) {
                resources_[i].quantity -= quantity;
                incident->allocatedResourceType = type;
                incident->allocatedResourceQuantity += quantity;
                out = "Allocated " + std::to_string(quantity) + " x " + type + " to " + id;
                return true;
            }
        }
        out = "Resource unavailable or insufficient."; return false;
    }

    void sendMessage(int recipientUserId, const std::string& text) {
        messages_.pushBack({nextMessageId_++, recipientUserId, text});
    }

    void showMessages(int userId) const {
        int shown = 0;
        std::cout << "\nMESSAGES\n";
        messages_.forEach([&](const Message& m){
            if (m.recipientUserId == -1 || m.recipientUserId == userId) {
                std::cout << '#' << m.id << (m.recipientUserId == -1 ? " [BROADCAST] " : " [DIRECT] ") << m.text << '\n'; ++shown;
            }
        });
        if (!shown) std::cout << "No messages.\n";
    }

    void listUsers() const {
        std::cout << "\nREGISTERED USERS\n";
        for (std::size_t i = 0; i < users_.size(); ++i)
            std::cout << users_[i].id << " | " << users_[i].name << " | " << users_[i].email << " | " << users_[i].username << '\n';
        if (users_.empty()) std::cout << "No registered users.\n";
    }

    void showUserIncidents(int userId, bool closedOnly = false) const {
        int shown = 0;
        std::cout << "\nYOUR INCIDENTS\n";
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            const Incident& in = incidents_[i];
            if (in.reportedByUserId != userId) continue;
            const bool closed = in.status == IncidentStatus::Closed || in.status == IncidentStatus::Resolved;
            if (closedOnly && !closed) continue;
            std::cout << in.id << " | " << toString(in.type) << " | " << in.locationId << " | P=" << in.priorityScore
                      << " | " << toString(in.status) << " | responder=" << (in.assignedResponderId.empty()?"-":in.assignedResponderId) << '\n';
            ++shown;
        }
        if (!shown) std::cout << "No matching incidents.\n";
    }

    void showAllIncidents() const {
        std::cout << "\nALL INCIDENTS\n";
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            const Incident& in = incidents_[i];
            std::cout << std::setw(8) << in.id << " | " << std::setw(10) << toString(in.type) << " | " << in.locationId
                      << " | P=" << std::setw(3) << in.priorityScore << " | " << std::setw(28) << toString(in.status)
                      << " | " << (in.assignedResponderId.empty()?"-":in.assignedResponderId) << '\n';
        }
        if (incidents_.empty()) std::cout << "No incidents.\n";
    }

    void showIncident(const std::string& id) const {
        const Incident* in = findIncident(id);
        if (!in) { std::cout << "Incident not found.\n"; return; }
        std::cout << "\n" << in->id << " | " << toString(in->type) << " | " << in->locationId
                  << "\nstatus=" << toString(in->status) << " priority=" << in->priorityScore
                  << " responder=" << (in->assignedResponderId.empty()?"-":in->assignedResponderId)
                  << "\nroute cost/distance/time=" << in->routeCost << "/" << in->routeDistance << "/" << in->routeTravelTime << " min"
                  << "\ndescription=" << in->description << '\n';
    }

    void showRespondersResources() const {
        std::cout << "\nRESPONDERS\n";
        for (std::size_t i = 0; i < responders_.size(); ++i)
            std::cout << responders_[i].id << " | " << responders_[i].type << " | " << toString(responders_[i].availability)
                      << " | " << graph_.node(responders_[i].currentLocation).id << '\n';
        std::cout << "SHELTERS\n";
        for (std::size_t i = 0; i < shelters_.size(); ++i)
            std::cout << shelters_[i].id << " capacity=" << shelters_[i].capacity << " occupancy=" << shelters_[i].occupancy << " available=" << shelters_[i].available() << '\n';
        std::cout << "SUPPLIES\n";
        for (std::size_t i = 0; i < resources_.size(); ++i)
            std::cout << resources_[i].type << " qty=" << resources_[i].quantity << " | " << resources_[i].source << '\n';
    }

    void showRoads() const {
        for (int i = 0; i < graph_.edgeCount(); ++i) {
            const auto& e = graph_.edge(i);
            std::cout << e.id << " | " << graph_.node(e.from).id << " <-> " << graph_.node(e.to).id
                      << " | dist=" << e.distance << " time=" << e.travelTime << " risk=" << e.risk
                      << " traffic=" << e.congestion << " | " << (e.blocked?"BLOCKED":"OPEN") << '\n';
        }
    }

    void runBfs(const std::string& startId) const {
        const int start = findLocationBinary(startId); if (start < 0) { std::cout << "Invalid location.\n"; return; }
        DynamicArray<int> order = BFS::traverse(graph_, start);
        std::cout << "BFS: "; for (std::size_t i=0;i<order.size();++i) { if(i) std::cout << " -> "; std::cout << graph_.node(order[i]).id; } std::cout << '\n';
    }
    void runDfs(const std::string& startId) const {
        const int start = findLocationBinary(startId); if (start < 0) { std::cout << "Invalid location.\n"; return; }
        DynamicArray<int> order = DFS::traverse(graph_, start);
        std::cout << "DFS: "; for (std::size_t i=0;i<order.size();++i) { if(i) std::cout << " -> "; std::cout << graph_.node(order[i]).id; } std::cout << '\n';
    }
    void runDijkstra(const std::string& fromId, const std::string& toId) const {
        RouteResult route = Dijkstra::shortestPath(graph_, findLocationBinary(fromId), findLocationBinary(toId));
        if (!route.reachable) { std::cout << "No reachable path.\n"; return; }
        std::cout << "cost=" << route.cost << " distance=" << route.distance << " time=" << route.travelTime << " min\nPath: ";
        for (std::size_t i=0;i<route.nodes.size();++i) { if(i) std::cout << " -> "; std::cout << graph_.node(route.nodes[i]).id; } std::cout << '\n';
    }

    void showArchive() const {
        std::cout << "\nAVL CLOSED-INCIDENT ARCHIVE\n";
        archive_.inorder([](long long seq, const std::string& id){ std::cout << seq << " | " << id << '\n'; });
        if (!archive_.size()) std::cout << "Archive empty.\n";
        std::cout << "LINKED-LIST HISTORY\n";
        history_.forEach([](const HistoryEntry& h){ std::cout << h.sequence << " | " << h.summary << '\n'; });
    }

    // Compatibility names keep the console menu readable while delegating to the core methods.
    std::string createIncident(int userId, IncidentType type, const std::string& locationId,
                               int severity, int urgency, int victims, const std::string& description) {
        return reportIncident(userId, type, locationId, severity, urgency, victims, description);
    }
    const User* getUser(int id) const { return user(id); }
    void showMessagesForUser(int id) const { showMessages(id); }
    bool processNextIntake(std::string& out) { return processNext(out); }
    void showIncidentDetails(const std::string& id) const { showIncident(id); }
    void showRespondersAndResources() const { showRespondersResources(); }
    bool undoLastRoadBlock(std::string& out) { return undoRoadBlock(out); }

    void dashboard() const {
        int active=0, closed=0, available=0, blocked=0;
        for (std::size_t i=0;i<incidents_.size();++i)
            (incidents_[i].status==IncidentStatus::Closed ? ++closed : ++active);
        for (std::size_t i=0;i<responders_.size();++i)
            if (responders_[i].availability==ResponderAvailability::Available) ++available;
        for (int i=0;i<graph_.edgeCount();++i) if (graph_.edge(i).blocked) ++blocked;
        std::cout << "\n===== CRISISMESH COMMAND DASHBOARD =====\n"
                  << "Users: " << users_.size() << " | Incidents: " << incidents_.size() << " | Active: " << active << " | Closed: " << closed << '\n'
                  << "FIFO Queue: " << intakeQueue_.size() << " | Max Heap: " << priorityHeap_.size() << " | AVL Archive: " << archive_.size() << '\n'
                  << "Available Responders: " << available << "/" << responders_.size() << " | Blocked Roads: " << blocked << "/" << graph_.edgeCount()
                  << " | Undo Stack: " << roadUndoStack_.size() << '\n';
    }
};

} // namespace crisismesh