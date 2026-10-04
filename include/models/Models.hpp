#pragma once

#include "dsa/Array.hpp"
#include <string>

namespace crisismesh {

enum class IncidentType { Medical, Fire, Police, Rescue, Accident, Flood, Structural };
enum class IncidentStatus {
    Reported, Queued, Triaged, Prioritized, Assigned, EnRoute, RerouteRequired,
    ResponseCompleted, AwaitingUserConfirmation, Resolved, Closed,
    WaitingForResource, Unreachable, Escalated, Cancelled
};
enum class ResponderAvailability { Available, Assigned, Busy, Offline };

inline const char* toString(IncidentType type) {
    switch (type) {
        case IncidentType::Medical: return "MEDICAL";
        case IncidentType::Fire: return "FIRE";
        case IncidentType::Police: return "POLICE";
        case IncidentType::Rescue: return "RESCUE";
        case IncidentType::Accident: return "ACCIDENT";
        case IncidentType::Flood: return "FLOOD";
        case IncidentType::Structural: return "STRUCTURAL";
    }
    return "UNKNOWN";
}

inline const char* toString(IncidentStatus status) {
    switch (status) {
        case IncidentStatus::Reported: return "REPORTED";
        case IncidentStatus::Queued: return "QUEUED";
        case IncidentStatus::Triaged: return "TRIAGED";
        case IncidentStatus::Prioritized: return "PRIORITIZED";
        case IncidentStatus::Assigned: return "ASSIGNED";
        case IncidentStatus::EnRoute: return "EN_ROUTE";
        case IncidentStatus::RerouteRequired: return "REROUTE_REQUIRED";
        case IncidentStatus::ResponseCompleted: return "RESPONSE_COMPLETED";
        case IncidentStatus::AwaitingUserConfirmation: return "AWAITING_USER_CONFIRMATION";
        case IncidentStatus::Resolved: return "RESOLVED";
        case IncidentStatus::Closed: return "CLOSED";
        case IncidentStatus::WaitingForResource: return "WAITING_FOR_RESOURCE";
        case IncidentStatus::Unreachable: return "UNREACHABLE";
        case IncidentStatus::Escalated: return "ESCALATED";
        case IncidentStatus::Cancelled: return "CANCELLED";
    }
    return "UNKNOWN";
}

inline const char* toString(ResponderAvailability status) {
    switch (status) {
        case ResponderAvailability::Available: return "AVAILABLE";
        case ResponderAvailability::Assigned: return "ASSIGNED";
        case ResponderAvailability::Busy: return "BUSY";
        case ResponderAvailability::Offline: return "OFFLINE";
    }
    return "UNKNOWN";
}

inline std::string requiredResponderType(IncidentType type) {
    switch (type) {
        case IncidentType::Medical:
        case IncidentType::Accident: return "AMBULANCE";
        case IncidentType::Fire: return "FIRE_TRUCK";
        case IncidentType::Police: return "POLICE_UNIT";
        case IncidentType::Rescue:
        case IncidentType::Flood:
        case IncidentType::Structural: return "RESCUE_TEAM";
    }
    return "RESCUE_TEAM";
}

inline const char* responseCategoryName(IncidentType type) {
    switch (type) {
        case IncidentType::Police: return "LAW ENFORCEMENT RESPONSE";
        case IncidentType::Fire: return "FIRE RESPONSE";
        case IncidentType::Medical:
        case IncidentType::Accident: return "MEDICAL RESPONSE";
        case IncidentType::Rescue:
        case IncidentType::Flood:
        case IncidentType::Structural: return "RESCUE RESPONSE";
    }
    return "EMERGENCY RESPONSE";
}

inline const char* responderStrengthLabel(const std::string& type) {
    if (type == "POLICE_UNIT") return "OFFICERS";
    if (type == "RESCUE_TEAM") return "TEAMS";
    return "UNITS";
}

struct User {
    int id{0};
    std::string name;
    std::string email;
    std::string phone;
    std::string username;
    std::string password;
};

struct Incident {
    std::string id;
    IncidentType type{IncidentType::Rescue};
    int locationIndex{-1};
    std::string locationId;
    int severity{1};
    int urgency{1};
    int victimCount{0};
    int priorityScore{0};
    long long sequence{0};
    IncidentStatus status{IncidentStatus::Reported};
    int reportedByUserId{0};
    std::string description;
    std::string requiredResponder;
    std::string assignedResponderId;
    int assignedStrength{0};
    bool userConfirmedResolved{false};
    std::string escalationReason;
    std::string shelterId;
    std::string allocatedResourceType;
    int allocatedResourceQuantity{0};
    DynamicArray<int> routeNodes;
    DynamicArray<int> routeEdges;
    double routeCost{0.0};
    double routeDistance{0.0};
    int routeTravelTime{0};
};

struct Responder {
    std::string id;
    std::string type;
    int currentLocation{-1};
    ResponderAvailability availability{ResponderAvailability::Available};
    int capacity{1};
    std::string baseFacility;
    std::string assignedIncidentId;
    int totalStrength{1};
    int availableStrength{1};
    std::string contactNumber;

    int onOperation() const { return totalStrength - availableStrength; }
};

struct Shelter {
    std::string id;
    int locationIndex{-1};
    int capacity{0};
    int occupancy{0};
    bool operational{true};
    int available() const { return capacity - occupancy; }
};

struct SupplyResource {
    std::string type;
    int quantity{0};
    std::string source;
};

struct Message {
    int id{0};
    int recipientUserId{-1};
    std::string text;
};

struct HistoryEntry {
    long long sequence{0};
    std::string incidentId;
    std::string summary;
};

struct IncidentHeapEntry {
    int incidentIndex{-1};
    int priority{0};
    long long sequence{0};
};

struct IncidentHigherPriority {
    bool operator()(const IncidentHeapEntry& a, const IncidentHeapEntry& b) const {
        if (a.priority != b.priority) return a.priority > b.priority;
        return a.sequence < b.sequence;
    }
};

struct Candidate {
    int responderIndex{-1};
    std::string responderId;
    bool reachable{false};
    double weightedCost{0.0};
    double distance{0.0};
    int travelTime{0};
};

inline bool candidateComesBefore(const Candidate& a, const Candidate& b) {
    if (a.reachable != b.reachable) return a.reachable && !b.reachable;
    if (a.weightedCost != b.weightedCost) return a.weightedCost < b.weightedCost;
    if (a.travelTime != b.travelTime) return a.travelTime < b.travelTime;
    if (a.distance != b.distance) return a.distance < b.distance;
    return a.responderId < b.responderId;
}

inline int calculatePriority(int severity, int urgency, int victims, IncidentType type) {
    int typeWeight = 4;
    if (type == IncidentType::Fire || type == IncidentType::Medical) typeWeight = 8;
    else if (type == IncidentType::Accident || type == IncidentType::Rescue) typeWeight = 7;
    else if (type == IncidentType::Flood || type == IncidentType::Structural) typeWeight = 6;
    else if (type == IncidentType::Police) typeWeight = 5;
    const int victimWeight = victims > 10 ? 10 : victims;
    return severity * 12 + urgency * 10 + victimWeight * 2 + typeWeight;
}

} // namespace crisismesh
