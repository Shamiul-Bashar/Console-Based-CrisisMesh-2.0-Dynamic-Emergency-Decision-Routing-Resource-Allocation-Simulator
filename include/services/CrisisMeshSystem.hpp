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
#include <sstream>

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

    const Responder* responderById(const std::string& id) const {
        for (std::size_t i = 0; i < responders_.size(); ++i)
            if (responders_[i].id == id) return &responders_[i];
        return nullptr;
    }

    const char* responderOperationalStatus(const Responder& responder) const {
        if (responder.availability == ResponderAvailability::Offline) return "OFFLINE";
        if (responder.availableStrength == 0 || responder.availability == ResponderAvailability::Busy) return "BUSY";
        if (responder.onOperation() > 0) return "PARTIAL";
        return "AVAILABLE";
    }

    Incident* highestReadyIncident() {
        while (!priorityHeap_.empty()) {
            const IncidentHeapEntry& top = priorityHeap_.top();
            if (top.incidentIndex >= 0 &&
                top.incidentIndex < static_cast<int>(incidents_.size()) &&
                incidents_[top.incidentIndex].status == IncidentStatus::Prioritized) {
                return &incidents_[top.incidentIndex];
            }
            priorityHeap_.pop();
        }
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
        RouteResult route = Dijkstra::shortestDistancePath(graph_, responder->currentLocation, incident.locationIndex);
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
        struct R {
            const char* id;
            const char* type;
            int loc;
            int cap;
            const char* base;
            int totalStrength;
            const char* contact;
        };

        const R rs[] = {
            {"FIRE-UNIT-01","FIRE_TRUCK",2,1,"Main Fire Station",1,"201"},
            {"FIRE-UNIT-02","FIRE_TRUCK",3,1,"East Fire Station",1,"202"},
            {"FIRE-UNIT-03","FIRE_TRUCK",17,1,"Bus Terminal Fire Unit",1,"203"},
            {"AMB-UNIT-01","AMBULANCE",0,2,"Central Hospital",1,"301"},
            {"AMB-UNIT-02","AMBULANCE",1,2,"North Hospital",1,"302"},
            {"AMB-UNIT-03","AMBULANCE",9,2,"East Medical Response Post",1,"303"},
            {"AMB-UNIT-04","AMBULANCE",11,2,"Shelter A Medical Post",1,"304"},
            {"POLICE-UNIT-01","POLICE_UNIT",4,2,"Central Police Station",30,"401"},
            {"POLICE-UNIT-02","POLICE_UNIT",5,2,"North Police Station",24,"402"},
            {"POLICE-UNIT-03","POLICE_UNIT",15,2,"Residential Zone C Police Post",18,"403"},
            {"RESCUE-UNIT-01","RESCUE_TEAM",11,4,"Shelter A Rescue Base",1,"501"},
            {"RESCUE-UNIT-02","RESCUE_TEAM",12,4,"Shelter B Rescue Base",1,"502"}
        };

        for (const auto& r : rs) {
            responders_.pushBack({
                r.id, r.type, r.loc, ResponderAvailability::Available, r.cap,
                r.base, "", r.totalStrength, r.totalStrength, r.contact
            });
        }

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
        if (input.name.empty() || input.username.empty() || input.email.empty()) return -1;
        if (users_.full() || usernameIndex_.get(input.username)) return -1;
        User user = input;
        user.id = static_cast<int>(users_.size()) + 1;
        const int index = static_cast<int>(users_.size());
        users_.pushBack(user);
        usernameIndex_.put(user.username, index);
        return user.id;
    }

    bool usernameExists(const std::string& username) const {
        return usernameIndex_.get(username) != nullptr;
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
        if (intakeQueue_.empty()) {
            out = "No pending incident is waiting in the FIFO queue.";
            return false;
        }
        const int index = intakeQueue_.dequeue();
        Incident& incident = incidents_[index];
        incident.status = IncidentStatus::Triaged;
        incident.priorityScore = calculatePriority(incident.severity, incident.urgency, incident.victimCount, incident.type);
        incident.status = IncidentStatus::Prioritized;
        priorityHeap_.push({index, incident.priorityScore, incident.sequence});
        out = "Incident " + incident.id + " processed successfully. Priority: " +
              std::to_string(incident.priorityScore) +
              " (moved to Max Heap and ready for Incident Analysis).";
        return true;
    }

    std::string highestReadyIncidentId() {
        Incident* incident = highestReadyIncident();
        return incident ? incident->id : "";
    }

    bool isReadyForAnalysis(const std::string& id) const {
        const Incident* incident = findIncident(id);
        return incident && incident->status == IncidentStatus::Prioritized;
    }

    std::size_t analysisReadyCount() const {
        std::size_t count = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i)
            if (incidents_[i].status == IncidentStatus::Prioritized) ++count;
        return count;
    }

    void showAnalysisReadyIncidents() const {
        struct ReadyRow {
            int incidentIndex{-1};
            int priority{0};
            long long sequence{0};
        };

        ReadyRow rows[MAX_INCIDENTS]{};
        std::size_t count = 0;

        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            if (incidents_[i].status != IncidentStatus::Prioritized) continue;
            rows[count++] = {static_cast<int>(i), incidents_[i].priorityScore, incidents_[i].sequence};
        }

        MergeSort::sort(rows, count, [](const ReadyRow& left, const ReadyRow& right) {
            if (left.priority != right.priority) return left.priority > right.priority;
            return left.sequence < right.sequence;
        });

        std::cout << "\n================ READY FOR INCIDENT ANALYSIS ================\n";
        std::cout << std::left << std::setw(7) << "Rank"
                  << std::setw(12) << "Incident"
                  << std::setw(13) << "Type"
                  << std::setw(12) << "Location"
                  << std::setw(11) << "Priority"
                  << std::setw(12) << "Level"
                  << "Required Response\n";
        std::cout << std::string(95, '-') << '\n';

        for (std::size_t i = 0; i < count; ++i) {
            const Incident& incident = incidents_[rows[i].incidentIndex];
            std::cout << std::left << std::setw(7) << (i + 1)
                      << std::setw(12) << incident.id
                      << std::setw(13) << toString(incident.type)
                      << std::setw(12) << incident.locationId
                      << std::setw(11) << incident.priorityScore
                      << std::setw(12) << operationalPriorityName(incident.priorityScore)
                      << responseCategoryName(incident.type) << '\n';
        }

        if (!count)
            std::cout << "No processed incident is waiting for analysis. Process an incident in Incident Center first.\n";
    }

    void showIncidentResponseProfile(const std::string& id) const {
        const Incident* incident = findIncident(id);
        if (!incident) {
            std::cout << "\nIncident not found.\n";
            return;
        }

        std::cout << "\n================ INCIDENT RESPONSE PROFILE ================\n"
                  << std::left << std::setw(20) << "Incident ID" << ": " << incident->id << '\n'
                  << std::setw(20) << "Incident Type" << ": " << toString(incident->type) << '\n'
                  << std::setw(20) << "Incident Location" << ": " << incident->locationId
                  << " - " << graph_.node(incident->locationIndex).name << '\n'
                  << std::setw(20) << "Priority Score" << ": " << incident->priorityScore << '\n'
                  << std::setw(20) << "Operational Priority" << ": " << operationalPriorityName(incident->priorityScore) << '\n'
                  << std::setw(20) << "Severity / Urgency" << ": " << incident->severity << " / " << incident->urgency << '\n'
                  << std::setw(20) << "Response Category" << ": " << responseCategoryName(incident->type) << '\n'
                  << std::setw(20) << "Required Resource" << ": " << incident->requiredResponder << '\n';

        std::cout << "\nCompatible Response Resources\n";
        std::cout << std::left << std::setw(18) << "Responder"
                  << std::setw(12) << "Location"
                  << std::setw(28) << "Facility"
                  << std::setw(11) << "Measure"
                  << std::setw(8) << "Total"
                  << std::setw(11) << "Available"
                  << std::setw(13) << "Operation"
                  << "Status\n";
        std::cout << std::string(112, '-') << '\n';

        int shown = 0;
        for (std::size_t i = 0; i < responders_.size(); ++i) {
            const Responder& r = responders_[i];
            if (r.type != incident->requiredResponder) continue;
            std::cout << std::left << std::setw(18) << r.id
                      << std::setw(12) << graph_.node(r.currentLocation).id
                      << std::setw(28) << r.baseFacility
                      << std::setw(11) << responderStrengthLabel(r.type)
                      << std::setw(8) << r.totalStrength
                      << std::setw(11) << r.availableStrength
                      << std::setw(13) << r.onOperation()
                      << responderOperationalStatus(r) << '\n';
            ++shown;
        }
        if (!shown) std::cout << "No compatible response resource configured.\n";
    }

    void runIncidentBfs(const std::string& id) const {
        const Incident* incident = findIncident(id);
        if (!incident) { std::cout << "Incident not found.\n"; return; }
        std::cout << "\nBFS reachability starting from the incident location.\n";
        runBfs(incident->locationId);
    }

    void runIncidentDfs(const std::string& id) const {
        const Incident* incident = findIncident(id);
        if (!incident) { std::cout << "Incident not found.\n"; return; }
        std::cout << "\nDFS reachability starting from the incident location.\n";
        runDfs(incident->locationId);
    }

    void runIncidentDijkstraAnalysis(const std::string& id) const {
        const Incident* incident = findIncident(id);
        if (!incident) { std::cout << "Incident not found.\n"; return; }

        Candidate candidates[32]{};
        std::size_t count = 0;

        for (std::size_t i = 0; i < responders_.size(); ++i) {
            const Responder& r = responders_[i];
            if (r.type != incident->requiredResponder) continue;
            RouteResult route = Dijkstra::shortestDistancePath(graph_, r.currentLocation, incident->locationIndex);
            candidates[count++] = {
                static_cast<int>(i), r.id, route.reachable,
                route.cost, route.distance, route.travelTime
            };
        }

        if (!count) {
            std::cout << "\nNo compatible response resource is configured.\n";
            return;
        }

        MergeSort::sort(candidates, count, [](const Candidate& a, const Candidate& b) {
            if (a.reachable != b.reachable) return a.reachable && !b.reachable;
            if (a.distance != b.distance) return a.distance < b.distance;
            if (a.travelTime != b.travelTime) return a.travelTime < b.travelTime;
            return a.responderId < b.responderId;
        });

        std::cout << "\n================ DIJKSTRA RESPONSE ANALYSIS ================\n"
                  << "Incident : " << incident->id << " | " << toString(incident->type)
                  << " | " << incident->locationId << " - " << graph_.node(incident->locationIndex).name << "\n\n";

        std::cout << std::left << std::setw(18) << "Responder"
                  << std::setw(12) << "Location"
                  << std::setw(12) << "Available"
                  << std::setw(13) << "Distance"
                  << std::setw(11) << "Time"
                  << std::setw(12) << "Resource"
                  << "Route\n";
        std::cout << std::string(92, '-') << '\n';

        const Candidate* best = nullptr;
        for (std::size_t i = 0; i < count; ++i) {
            const Candidate& c = candidates[i];
            const Responder& r = responders_[c.responderIndex];
            std::ostringstream distanceText;
            if (c.reachable) distanceText << std::fixed << std::setprecision(1) << c.distance << " km";
            const std::string timeText = c.reachable ? (std::to_string(c.travelTime) + " min") : "-";
            std::cout << std::left << std::setw(18) << r.id
                      << std::setw(12) << graph_.node(r.currentLocation).id
                      << std::setw(12) << r.availableStrength
                      << std::setw(13) << (c.reachable ? distanceText.str() : "-")
                      << std::setw(11) << timeText
                      << std::setw(12) << responderOperationalStatus(r)
                      << (c.reachable ? "REACHABLE" : "UNREACHABLE") << '\n';

            if (!best && c.reachable && r.availableStrength > 0 &&
                r.availability != ResponderAvailability::Offline &&
                r.availability != ResponderAvailability::Busy) {
                best = &candidates[i];
            }
        }

        if (!best) {
            std::cout << "\nNo currently available compatible responder has a reachable route.\n";
            return;
        }

        const Responder& selected = responders_[best->responderIndex];
        RouteResult route = Dijkstra::shortestDistancePath(graph_, selected.currentLocation, incident->locationIndex);

        std::cout << "\n================ RECOMMENDED SHORTEST RESPONSE ================\n"
                  << std::left << std::setw(20) << "Recommended Resource" << ": " << selected.id << '\n'
                  << std::setw(20) << "From" << ": " << graph_.node(selected.currentLocation).id
                  << " - " << graph_.node(selected.currentLocation).name << '\n'
                  << std::setw(20) << "To" << ": " << incident->locationId
                  << " - " << graph_.node(incident->locationIndex).name << '\n'
                  << std::setw(20) << "Distance" << ": " << std::fixed << std::setprecision(1)
                  << route.distance << " km\n"
                  << std::setw(20) << "Travel Time" << ": " << route.travelTime << " min\n"
                  << std::setw(20) << "Operational Cost" << ": " << std::setprecision(2) << route.cost << "\n\n"
                  << ">>> SHORTEST DISTANCE PATH: ";

        for (std::size_t i = 0; i < route.nodes.size(); ++i) {
            if (i) std::cout << " -> ";
            std::cout << graph_.node(route.nodes[i]).id;
        }
        std::cout << " <<<\n";
        std::cout.unsetf(std::ios::floatfield);
    }

    int responderAvailableStrength(const std::string& id) const {
        const Responder* responder = responderById(id);
        return responder ? responder->availableStrength : -1;
    }

    std::string responderMeasure(const std::string& id) const {
        const Responder* responder = responderById(id);
        return responder ? responderStrengthLabel(responder->type) : "UNITS";
    }

    bool assignResponse(const std::string& incidentId, const std::string& responderId,
                        int strength, std::string& out) {
        Incident* incident = findIncident(incidentId);
        if (!incident) { out = "Incident not found."; return false; }
        if (incident->status != IncidentStatus::Prioritized) {
            out = "Incident is not ready for assignment. Process it in Incident Center first.";
            return false;
        }

        Responder* responder = responderById(responderId);
        if (!responder) { out = "Responder not found."; return false; }
        if (responder->type != incident->requiredResponder) {
            out = "Selected responder is not compatible with this incident type.";
            return false;
        }
        if (responder->availability == ResponderAvailability::Offline) {
            out = "Selected responder is OFFLINE.";
            return false;
        }
        if (responder->availability == ResponderAvailability::Busy && responder->onOperation() == 0) {
            out = "Selected responder is manually marked BUSY.";
            return false;
        }
        if (strength <= 0 || strength > responder->availableStrength) {
            out = "Requested assignment exceeds available response strength.";
            return false;
        }
        if (responder->type != "POLICE_UNIT" && strength != 1) {
            out = "This resource is assigned as a single operational unit/team.";
            return false;
        }

        RouteResult route = Dijkstra::shortestDistancePath(graph_, responder->currentLocation, incident->locationIndex);
        if (!route.reachable) {
            out = "Selected responder has no reachable route to the incident.";
            return false;
        }

        responder->availableStrength -= strength;
        if (responder->availableStrength == 0)
            responder->availability = ResponderAvailability::Busy;
        else
            responder->availability = ResponderAvailability::Available;

        if (responder->type != "POLICE_UNIT")
            responder->assignedIncidentId = incident->id;

        incident->assignedResponderId = responder->id;
        incident->assignedStrength = strength;
        incident->routeNodes = route.nodes;
        incident->routeEdges = route.edges;
        incident->routeCost = route.cost;
        incident->routeDistance = route.distance;
        incident->routeTravelTime = route.travelTime;
        incident->status = IncidentStatus::EnRoute;

        Incident* top = highestReadyIncident();
        if (top && top->id == incident->id) priorityHeap_.pop();

        out = "Response assigned successfully.\n"
              "Incident: " + incident->id +
              "\nResource: " + responder->id +
              "\nAssigned " + std::string(responderStrengthLabel(responder->type)) + ": " + std::to_string(strength) +
              "\nRemaining Available: " + std::to_string(responder->availableStrength) +
              "\nShortest Distance: " + std::to_string(route.distance) + " km";
        return true;
    }

    bool recallResponse(const std::string& id, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident || (incident->status != IncidentStatus::EnRoute &&
                          incident->status != IncidentStatus::Assigned &&
                          incident->status != IncidentStatus::Unreachable &&
                          incident->status != IncidentStatus::RerouteRequired)) {
            out = "Incident has no active response to recall.";
            return false;
        }

        Responder* responder = responderById(incident->assignedResponderId);
        if (responder) {
            responder->availableStrength += incident->assignedStrength;
            if (responder->availableStrength > responder->totalStrength)
                responder->availableStrength = responder->totalStrength;

            if (responder->type != "POLICE_UNIT")
                responder->assignedIncidentId.clear();

            if (responder->availability != ResponderAvailability::Offline)
                responder->availability = responder->availableStrength > 0
                    ? ResponderAvailability::Available
                    : ResponderAvailability::Busy;
        }

        incident->assignedResponderId.clear();
        incident->assignedStrength = 0;
        incident->routeNodes.clear();
        incident->routeEdges.clear();
        incident->routeCost = 0.0;
        incident->routeDistance = 0.0;
        incident->routeTravelTime = 0;
        incident->status = IncidentStatus::Prioritized;

        const int* index = incidentIndex_.get(id);
        if (index) priorityHeap_.push({*index, incident->priorityScore, incident->sequence});

        out = "Response recalled successfully. Resource availability restored and incident returned to Incident Analysis.";
        return true;
    }

    bool markResponseCompleted(const std::string& id, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident || (incident->status != IncidentStatus::EnRoute && incident->status != IncidentStatus::Assigned)) {
            out = "Incident is not actively dispatched.";
            return false;
        }

        Responder* responder = responderById(incident->assignedResponderId);
        if (responder) {
            responder->availableStrength += incident->assignedStrength;
            if (responder->availableStrength > responder->totalStrength)
                responder->availableStrength = responder->totalStrength;

            if (responder->type != "POLICE_UNIT") {
                responder->currentLocation = incident->locationIndex;
                responder->assignedIncidentId.clear();
            }

            if (responder->availability != ResponderAvailability::Offline)
                responder->availability = responder->availableStrength > 0
                    ? ResponderAvailability::Available
                    : ResponderAvailability::Busy;
        }

        incident->status = IncidentStatus::AwaitingUserConfirmation;
        out = "Field response completed. Assigned response strength returned to availability. "
              "The reporting user must now confirm YES/NO.";
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
        incident->assignedStrength = 0;
        incident->routeNodes.clear(); incident->routeEdges.clear();
        incident->status = IncidentStatus::Queued;
        const int* index = incidentIndex_.get(id);
        if (index) intakeQueue_.enqueue(*index);
        out = "NO -> ESCALATED -> urgency increased -> requeued in FIFO Queue.";
        return true;
    }

    bool blockRoad(const std::string& roadId, std::string& out) {
        const int edgeIndex = graph_.findEdgeIndex(roadId);
        if (edgeIndex < 0) {
            out = "Road not found.";
            return false;
        }
        if (graph_.edge(edgeIndex).blocked) {
            out = roadId + " is already blocked.";
            return false;
        }
        graph_.blockEdge(edgeIndex);
        roadUndoStack_.push(edgeIndex);
        int rerouted = 0, unreachable = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            Incident& incident = incidents_[i];
            if (incident.status == IncidentStatus::EnRoute && routeUsesEdge(incident, edgeIndex)) {
                incident.status = IncidentStatus::RerouteRequired;
                if (reroute(incident)) ++rerouted; else ++unreachable;
            }
        }
        out = "Road " + roadId + " blocked successfully.\n"
              "Affected reroutes: " + std::to_string(rerouted) +
              " | Unreachable incidents: " + std::to_string(unreachable) +
              " | Block stack depth: " + std::to_string(roadUndoStack_.size());
        return true;
    }

    bool undoRoadBlock(std::string& out) {
        if (roadUndoStack_.empty()) {
            out = "No blocked road is available to undo.";
            return false;
        }
        const int edgeIndex = roadUndoStack_.pop();
        graph_.openEdge(edgeIndex);
        int recovered = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i)
            if (incidents_[i].status == IncidentStatus::Unreachable && !incidents_[i].assignedResponderId.empty() && reroute(incidents_[i])) ++recovered;
        out = "Road " + graph_.edge(edgeIndex).id + " unblocked successfully.\n"
              "Recovered routes: " + std::to_string(recovered) +
              " | Remaining block stack depth: " + std::to_string(roadUndoStack_.size());
        return true;
    }

    bool allocateShelter(const std::string& id, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident) { out = "Incident not found."; return false; }
        if (incident->status == IncidentStatus::Closed ||
            incident->status == IncidentStatus::Resolved ||
            incident->status == IncidentStatus::Cancelled) {
            out = "Shelter allocation is only available for active incidents.";
            return false;
        }
        if (!incident->shelterId.empty()) {
            out = "Shelter is already allocated to this incident: " + incident->shelterId;
            return false;
        }
        int best = -1; double bestDistance = std::numeric_limits<double>::infinity();
        const int people = incident->victimCount > 0 ? incident->victimCount : 1;
        for (std::size_t i = 0; i < shelters_.size(); ++i) {
            if (!shelters_[i].operational || shelters_[i].available() < people) continue;
            RouteResult route = Dijkstra::shortestDistancePath(graph_, incident->locationIndex, shelters_[i].locationIndex);
            if (route.reachable && route.distance < bestDistance) {
                best = static_cast<int>(i);
                bestDistance = route.distance;
            }
        }
        if (best < 0) { out = "No reachable shelter with sufficient capacity."; return false; }
        shelters_[best].occupancy += people;
        incident->shelterId = shelters_[best].id;
        out = "Allocated " + shelters_[best].id + " using Dijkstra + capacity check.";
        return true;
    }

    bool allocateSupply(const std::string& id, const std::string& type, int quantity, std::string& out) {
        Incident* incident = findIncident(id);
        if (!incident) { out = "Incident not found."; return false; }
        if (incident->status == IncidentStatus::Closed ||
            incident->status == IncidentStatus::Resolved ||
            incident->status == IncidentStatus::Cancelled) {
            out = "Supply allocation is only available for active incidents.";
            return false;
        }
        if (quantity <= 0) { out = "Quantity must be greater than zero."; return false; }
        for (std::size_t i = 0; i < resources_.size(); ++i) {
            if (resources_[i].type == type) {
                if (resources_[i].quantity < quantity) {
                    out = "Insufficient stock. Available quantity: " + std::to_string(resources_[i].quantity);
                    return false;
                }
                resources_[i].quantity -= quantity;
                incident->allocatedResourceType = type;
                incident->allocatedResourceQuantity += quantity;
                out = "Supply allocated successfully.\n"
                      "Incident: " + id + " | Resource: " + type +
                      " | Allocated: " + std::to_string(quantity) +
                      " | Remaining: " + std::to_string(resources_[i].quantity);
                return true;
            }
        }
        out = "Resource type not found.";
        return false;
    }

    bool userExists(int id) const {
        return id > 0 && id <= static_cast<int>(users_.size());
    }

    bool sendMessage(int recipientUserId, const std::string& text) {
        if (text.empty()) return false;
        if (recipientUserId != -1 && !userExists(recipientUserId)) return false;
        messages_.pushBack({nextMessageId_++, recipientUserId, text});
        return true;
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
        std::cout << "\n================ REGISTERED USERS ================\n";
        std::cout << std::left << std::setw(8) << "ID"
                  << std::setw(24) << "Name"
                  << std::setw(30) << "Email"
                  << "Username\n";
        std::cout << std::string(78, '-') << '\n';

        for (std::size_t i = 0; i < users_.size(); ++i)
            std::cout << std::left << std::setw(8) << users_[i].id
                      << std::setw(24) << users_[i].name
                      << std::setw(30) << users_[i].email
                      << users_[i].username << '\n';

        if (users_.empty()) std::cout << "No registered users.\n";
    }

    std::size_t pendingIntakeCount() const { return intakeQueue_.size(); }

    std::size_t blockedRoadCount() const {
        std::size_t count = 0;
        for (int i = 0; i < graph_.edgeCount(); ++i) if (graph_.edge(i).blocked) ++count;
        return count;
    }

    bool hasIncidents() const { return !incidents_.empty(); }

    int supplyQuantity(const std::string& type) const {
        for (std::size_t i = 0; i < resources_.size(); ++i)
            if (resources_[i].type == type) return resources_[i].quantity;
        return -1;
    }

    bool updateResponderStatus(const std::string& id, ResponderAvailability status, std::string& out) {
        Responder* responder = responderById(id);
        if (!responder) { out = "Responder not found."; return false; }
        if (responder->onOperation() > 0) {
            out = "Responder has active deployed strength. Complete the active response before changing status.";
            return false;
        }
        if (status == ResponderAvailability::Assigned) {
            out = "ASSIGNED status is controlled automatically by incident assignment.";
            return false;
        }
        responder->availability = status;
        out = "Responder status updated successfully.";
        return true;
    }

    bool updateResponderLocation(const std::string& id, const std::string& locationId, std::string& out) {
        Responder* responder = responderById(id);
        if (!responder) { out = "Responder not found."; return false; }
        if (responder->onOperation() > 0) {
            out = "Responder location cannot be edited while response strength is on operation.";
            return false;
        }
        const int location = findLocationBinary(locationId);
        if (location < 0) { out = "Location not found."; return false; }
        responder->currentLocation = location;
        out = "Responder location updated successfully.";
        return true;
    }

    bool updateShelterCapacity(const std::string& id, int capacity, std::string& out) {
        for (std::size_t i = 0; i < shelters_.size(); ++i) {
            if (shelters_[i].id != id) continue;
            if (capacity < shelters_[i].occupancy) {
                out = "Capacity cannot be lower than current occupancy.";
                return false;
            }
            shelters_[i].capacity = capacity;
            out = "Shelter capacity updated successfully.";
            return true;
        }
        out = "Shelter not found.";
        return false;
    }

    bool updateShelterOccupancy(const std::string& id, int occupancy, std::string& out) {
        for (std::size_t i = 0; i < shelters_.size(); ++i) {
            if (shelters_[i].id != id) continue;
            if (occupancy < 0 || occupancy > shelters_[i].capacity) {
                out = "Occupancy must be between 0 and shelter capacity.";
                return false;
            }
            shelters_[i].occupancy = occupancy;
            out = "Shelter occupancy updated successfully.";
            return true;
        }
        out = "Shelter not found.";
        return false;
    }

    bool updateShelterOperational(const std::string& id, bool operational, std::string& out) {
        for (std::size_t i = 0; i < shelters_.size(); ++i) {
            if (shelters_[i].id != id) continue;
            shelters_[i].operational = operational;
            out = "Shelter status updated successfully.";
            return true;
        }
        out = "Shelter not found.";
        return false;
    }

    bool setSupplyQuantity(const std::string& type, int quantity, std::string& out) {
        if (quantity < 0) { out = "Quantity cannot be negative."; return false; }
        for (std::size_t i = 0; i < resources_.size(); ++i) {
            if (resources_[i].type != type) continue;
            resources_[i].quantity = quantity;
            out = "Supply quantity updated successfully.";
            return true;
        }
        out = "Resource type not found.";
        return false;
    }

    bool adjustSupply(const std::string& type, int delta, std::string& out) {
        for (std::size_t i = 0; i < resources_.size(); ++i) {
            if (resources_[i].type != type) continue;
            if (resources_[i].quantity + delta < 0) {
                out = "Insufficient stock for this update.";
                return false;
            }
            resources_[i].quantity += delta;
            out = "Supply stock updated successfully. Current quantity: " +
                  std::to_string(resources_[i].quantity);
            return true;
        }
        out = "Resource type not found.";
        return false;
    }

    bool updateSupplySource(const std::string& type, const std::string& source, std::string& out) {
        if (source.empty()) { out = "Location cannot be empty."; return false; }
        for (std::size_t i = 0; i < resources_.size(); ++i) {
            if (resources_[i].type != type) continue;
            resources_[i].source = source;
            out = "Supply location updated successfully.";
            return true;
        }
        out = "Resource type not found.";
        return false;
    }

    void showOperationalIncidents() const {
        std::cout << "\nAVAILABLE INCIDENTS\n";
        std::cout << std::left << std::setw(12) << "Incident" << std::setw(13) << "Type"
                  << std::setw(12) << "Location" << std::setw(26) << "Status" << '\n';
        std::cout << std::string(63, '-') << '\n';
        int shown = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            const Incident& in = incidents_[i];
            if (in.status == IncidentStatus::Closed || in.status == IncidentStatus::Cancelled) continue;
            std::cout << std::left << std::setw(12) << in.id << std::setw(13) << toString(in.type)
                      << std::setw(12) << in.locationId << std::setw(26) << toString(in.status) << '\n';
            ++shown;
        }
        if (!shown) std::cout << "No active incident is available.\n";
    }

    void showUserIncidents(int userId, bool closedOnly = false) const {
        int shown = 0;
        std::cout << "\nYOUR INCIDENTS\n";
        std::cout << std::left << std::setw(12) << "Incident" << std::setw(13) << "Type"
                  << std::setw(12) << "Location" << std::setw(10) << "Priority"
                  << std::setw(26) << "Status" << std::setw(16) << "Responder" << '\n';
        std::cout << std::string(89, '-') << '\n';
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            const Incident& in = incidents_[i];
            if (in.reportedByUserId != userId) continue;
            const bool closed = in.status == IncidentStatus::Closed || in.status == IncidentStatus::Resolved;
            if (closedOnly && !closed) continue;
            std::cout << std::left << std::setw(12) << in.id << std::setw(13) << toString(in.type)
                      << std::setw(12) << in.locationId << std::setw(10) << in.priorityScore
                      << std::setw(26) << toString(in.status)
                      << std::setw(16) << (in.assignedResponderId.empty() ? "-" : in.assignedResponderId) << '\n';
            ++shown;
        }
        if (!shown) std::cout << "No matching incidents.\n";
    }

    void showAllIncidents() const {
        std::cout << "\nALL INCIDENTS\n";
        std::cout << std::left << std::setw(12) << "Incident" << std::setw(13) << "Type"
                  << std::setw(12) << "Location" << std::setw(10) << "Priority"
                  << std::setw(26) << "Status" << std::setw(16) << "Responder" << '\n';
        std::cout << std::string(89, '-') << '\n';
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            const Incident& in = incidents_[i];
            std::cout << std::left << std::setw(12) << in.id << std::setw(13) << toString(in.type)
                      << std::setw(12) << in.locationId << std::setw(10) << in.priorityScore
                      << std::setw(26) << toString(in.status)
                      << std::setw(16) << (in.assignedResponderId.empty() ? "-" : in.assignedResponderId) << '\n';
        }
        if (incidents_.empty()) std::cout << "No incident occurred.\n";
    }

    void showIncident(const std::string& id) const {
        const Incident* in = findIncident(id);
        if (!in) { std::cout << "\nIncident not found.\n"; return; }
        const std::string locationName = graph_.node(in->locationIndex).name;
        std::cout << "\n================ INCIDENT DETAILS ================\n"
                  << std::left << std::setw(18) << "Incident ID" << ": " << in->id << '\n'
                  << std::setw(18) << "Type" << ": " << toString(in->type) << '\n'
                  << std::setw(18) << "Location" << ": " << in->locationId << " - " << locationName << '\n'
                  << std::setw(18) << "Status" << ": " << toString(in->status) << '\n'
                  << std::setw(18) << "Priority" << ": " << in->priorityScore << '\n'
                  << std::setw(18) << "Responder" << ": " << (in->assignedResponderId.empty() ? "-" : in->assignedResponderId) << '\n'
                  << std::setw(18) << "Assigned Strength" << ": " << in->assignedStrength << '\n'
                  << std::setw(18) << "Route Cost" << ": " << std::fixed << std::setprecision(2) << in->routeCost << '\n'
                  << std::setw(18) << "Distance" << ": " << std::setprecision(1) << in->routeDistance << " km\n"
                  << std::setw(18) << "Travel Time" << ": " << in->routeTravelTime << " min\n"
                  << std::setw(18) << "Description" << ": " << in->description << '\n'
                  << "==================================================\n";
        std::cout.unsetf(std::ios::floatfield);
    }

    void showRespondersResources() const {
        std::cout << "\n===================================== RESPONDERS =====================================\n";
        std::cout << std::left << std::setw(18) << "ID"
                  << std::setw(16) << "Type"
                  << std::setw(11) << "Location"
                  << std::setw(11) << "Measure"
                  << std::setw(8) << "Total"
                  << std::setw(11) << "Available"
                  << std::setw(13) << "Operation"
                  << "Status\n";
        std::cout << std::string(99, '-') << '\n';

        for (std::size_t i = 0; i < responders_.size(); ++i) {
            const Responder& r = responders_[i];
            std::cout << std::left << std::setw(18) << r.id
                      << std::setw(16) << r.type
                      << std::setw(11) << graph_.node(r.currentLocation).id
                      << std::setw(11) << responderStrengthLabel(r.type)
                      << std::setw(8) << r.totalStrength
                      << std::setw(11) << r.availableStrength
                      << std::setw(13) << r.onOperation()
                      << responderOperationalStatus(r) << '\n';
        }

        std::cout << "\n================= SHELTERS ==================\n";
        std::cout << std::left << std::setw(14) << "ID" << std::setw(11) << "Capacity"
                  << std::setw(12) << "Occupancy" << std::setw(11) << "Available" << std::setw(12) << "Status" << '\n';
        std::cout << std::string(60, '-') << '\n';
        for (std::size_t i = 0; i < shelters_.size(); ++i)
            std::cout << std::left << std::setw(14) << shelters_[i].id << std::setw(11) << shelters_[i].capacity
                      << std::setw(12) << shelters_[i].occupancy << std::setw(11) << shelters_[i].available()
                      << std::setw(12) << (shelters_[i].operational ? "ACTIVE" : "CLOSED") << '\n';

        std::cout << "\n================== SUPPLIES =================\n";
        std::cout << std::left << std::setw(18) << "Resource" << std::setw(12) << "Quantity" << "Location\n";
        std::cout << std::string(58, '-') << '\n';
        for (std::size_t i = 0; i < resources_.size(); ++i)
            std::cout << std::left << std::setw(18) << resources_[i].type << std::setw(12) << resources_[i].quantity
                      << resources_[i].source << '\n';
    }

    void showEmergencyContacts() const {
        std::cout << "\n================ EMERGENCY CONTACTS ================\n";
        std::cout << std::left << std::setw(18) << "Response Unit"
                  << std::setw(16) << "Type"
                  << std::setw(12) << "Location"
                  << std::setw(30) << "Facility"
                  << "Hotline\n";
        std::cout << std::string(86, '-') << '\n';

        for (std::size_t i = 0; i < responders_.size(); ++i) {
            const Responder& r = responders_[i];
            std::cout << std::left << std::setw(18) << r.id
                      << std::setw(16) << r.type
                      << std::setw(12) << graph_.node(r.currentLocation).id
                      << std::setw(30) << r.baseFacility
                      << r.contactNumber << '\n';
        }
    }

    void showActiveDispatches() const {
        std::cout << "\n================ ACTIVE DISPATCHES ================\n";
        std::cout << std::left << std::setw(12) << "Incident"
                  << std::setw(13) << "Type"
                  << std::setw(12) << "Location"
                  << std::setw(18) << "Responder"
                  << std::setw(11) << "Assigned"
                  << "Status\n";
        std::cout << std::string(82, '-') << '\n';

        int shown = 0;
        for (std::size_t i = 0; i < incidents_.size(); ++i) {
            const Incident& in = incidents_[i];
            if (in.status != IncidentStatus::EnRoute && in.status != IncidentStatus::Assigned) continue;
            std::cout << std::left << std::setw(12) << in.id
                      << std::setw(13) << toString(in.type)
                      << std::setw(12) << in.locationId
                      << std::setw(18) << in.assignedResponderId
                      << std::setw(11) << in.assignedStrength
                      << toString(in.status) << '\n';
            ++shown;
        }
        if (!shown) std::cout << "No active dispatch is currently running.\n";
    }

    void showRoads() const {
        std::cout << "\n========================== ROAD NETWORK ==========================\n";
        std::cout << std::left << std::setw(9) << "Road ID" << std::setw(11) << "From"
                  << std::setw(11) << "To" << std::setw(11) << "Distance" << std::setw(9) << "Time"
                  << std::setw(7) << "Risk" << std::setw(10) << "Traffic" << "Status\n";
        std::cout << std::string(78, '-') << '\n';
        for (int i = 0; i < graph_.edgeCount(); ++i) {
            const auto& e = graph_.edge(i);
            std::cout << std::left << std::setw(9) << e.id << std::setw(11) << graph_.node(e.from).id
                      << std::setw(11) << graph_.node(e.to).id
                      << std::setw(11) << (std::to_string(e.distance).substr(0,3) + " km")
                      << std::setw(9) << (std::to_string(e.travelTime) + " min")
                      << std::setw(7) << e.risk << std::setw(10) << e.congestion
                      << (e.blocked ? "BLOCKED" : "OPEN") << '\n';
        }
        std::cout << std::string(78, '-') << '\n'
                  << "Total Roads: " << graph_.edgeCount() << " | Blocked: " << blockedRoadCount() << '\n';
    }

    void showBlockedRoads() const {
        std::cout << "\n================ BLOCKED ROADS ================\n";
        std::cout << std::left << std::setw(9) << "Road ID" << std::setw(11) << "From"
                  << std::setw(11) << "To" << std::setw(11) << "Distance" << "Status\n";
        std::cout << std::string(55, '-') << '\n';
        int shown = 0;
        for (int i = 0; i < graph_.edgeCount(); ++i) {
            const auto& e = graph_.edge(i);
            if (!e.blocked) continue;
            std::cout << std::left << std::setw(9) << e.id << std::setw(11) << graph_.node(e.from).id
                      << std::setw(11) << graph_.node(e.to).id
                      << std::setw(11) << (std::to_string(e.distance).substr(0,3) + " km") << "BLOCKED\n";
            ++shown;
        }
        if (!shown) std::cout << "No roads are currently blocked.\n";
    }

    void runBfs(const std::string& startId) const {
        const int start = findLocationBinary(startId);
        if (start < 0) { std::cout << "Invalid location.\n"; return; }
        DynamicArray<int> order = BFS::traverse(graph_, start);
        std::cout << "\n================ BFS ANALYSIS ================\n"
                  << "Start Location : " << graph_.node(start).id << " - " << graph_.node(start).name << "\n\n"
                  << std::left << std::setw(8) << "Step" << std::setw(13) << "Location ID" << "Location Name\n"
                  << std::string(52, '-') << '\n';
        for (std::size_t i = 0; i < order.size(); ++i)
            std::cout << std::left << std::setw(8) << (i + 1) << std::setw(13) << graph_.node(order[i]).id
                      << graph_.node(order[i]).name << '\n';
        std::cout << std::string(52, '-') << '\n' << "Total Visited Nodes: " << order.size() << '\n';
    }

    void runDfs(const std::string& startId) const {
        const int start = findLocationBinary(startId);
        if (start < 0) { std::cout << "Invalid location.\n"; return; }
        DynamicArray<int> order = DFS::traverse(graph_, start);
        std::cout << "\n================ DFS ANALYSIS ================\n"
                  << "Start Location : " << graph_.node(start).id << " - " << graph_.node(start).name << "\n\n"
                  << std::left << std::setw(8) << "Step" << std::setw(13) << "Location ID" << "Location Name\n"
                  << std::string(52, '-') << '\n';
        for (std::size_t i = 0; i < order.size(); ++i)
            std::cout << std::left << std::setw(8) << (i + 1) << std::setw(13) << graph_.node(order[i]).id
                      << graph_.node(order[i]).name << '\n';
        std::cout << std::string(52, '-') << '\n' << "Total Visited Nodes: " << order.size() << '\n';
    }

    void runDijkstra(const std::string& fromId, const std::string& toId) const {
        const int from = findLocationBinary(fromId);
        const int to = findLocationBinary(toId);
        if (from < 0 || to < 0) { std::cout << "Invalid source or destination location.\n"; return; }
        RouteResult route = Dijkstra::shortestDistancePath(graph_, from, to);
        if (!route.reachable) { std::cout << "No reachable path.\n"; return; }
        std::cout << "\n================ SHORTEST-DISTANCE ROUTE ================\n"
                  << std::left << std::setw(16) << "From" << ": " << graph_.node(from).id << " - " << graph_.node(from).name << '\n'
                  << std::setw(16) << "To" << ": " << graph_.node(to).id << " - " << graph_.node(to).name << '\n'
                  << std::setw(16) << "Total Distance" << ": " << std::fixed << std::setprecision(1) << route.distance << " km\n"
                  << std::setw(16) << "Travel Time" << ": " << route.travelTime << " min\n"
                  << std::setw(16) << "Operational Cost" << ": " << std::setprecision(2) << route.cost << "\n\nRoute:\n";
        for (std::size_t i = 0; i < route.nodes.size(); ++i) {
            if (i) std::cout << " -> ";
            std::cout << graph_.node(route.nodes[i]).id;
        }
        std::cout << '\n';
        std::cout.unsetf(std::ios::floatfield);
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
            if (responders_[i].availableStrength > 0 &&
                responders_[i].availability != ResponderAvailability::Offline) ++available;
        for (int i=0;i<graph_.edgeCount();++i) if (graph_.edge(i).blocked) ++blocked;
        std::cout << "\n===== CRISISMESH COMMAND DASHBOARD =====\n"
                  << "Users: " << users_.size() << " | Incidents: " << incidents_.size() << " | Active: " << active << " | Closed: " << closed << '\n'
                  << "FIFO Queue: " << intakeQueue_.size() << " | Max Heap: " << priorityHeap_.size() << " | AVL Archive: " << archive_.size() << '\n'
                  << "Available Responders: " << available << "/" << responders_.size() << " | Blocked Roads: " << blocked << "/" << graph_.edgeCount()
                  << " | Undo Stack: " << roadUndoStack_.size() << '\n';
    }
};

} // namespace crisismesh