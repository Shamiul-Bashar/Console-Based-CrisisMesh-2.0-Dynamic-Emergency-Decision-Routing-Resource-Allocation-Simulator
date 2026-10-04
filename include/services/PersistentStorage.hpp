#pragma once

#include "dsa/Array.hpp"
#include "models/Models.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <system_error>

namespace crisismesh {

// =============================================================================
// PERSISTENCE HELPERS
// =============================================================================
// Runtime data is written through a temporary file and then atomically replaced.
// This avoids leaving a half-written text file if the application is interrupted.
namespace persistence_detail {

inline bool replaceFileAtomically(const std::filesystem::path& target,
                                  const std::filesystem::path& temp) {
    try {
        const std::filesystem::path backup = target.string() + ".bak";
        std::error_code error;

        std::filesystem::remove(backup, error);
        error.clear();

        const bool hadExistingFile = std::filesystem::exists(target);
        if (hadExistingFile) {
            std::filesystem::rename(target, backup, error);
            if (error) {
                std::filesystem::remove(temp, error);
                return false;
            }
        }

        error.clear();
        std::filesystem::rename(temp, target, error);
        if (error) {
            std::error_code restoreError;
            if (hadExistingFile && std::filesystem::exists(backup))
                std::filesystem::rename(backup, target, restoreError);
            std::filesystem::remove(temp, restoreError);
            return false;
        }

        std::filesystem::remove(backup, error);
        return true;
    } catch (...) {
        return false;
    }
}

inline int incidentNumber(const std::string& id) {
    if (id.rfind("INC-", 0) != 0 || id.size() <= 4) return 0;
    try {
        return std::stoi(id.substr(4));
    } catch (...) {
        return 0;
    }
}

} // namespace persistence_detail

// =============================================================================
// INCIDENT SNAPSHOT STORAGE
// =============================================================================
// Saves all user incidents so My Incidents / Closed History still work after a
// complete program restart. Queue/heap/archive runtime structures are rebuilt
// from these persisted incident snapshots by CrisisMeshSystem at startup.
class IncidentStorage {
public:
    static std::string defaultPath() {
        return "data/incidents.txt";
    }

    template <std::size_t Capacity>
    static bool load(StaticArray<Incident, Capacity>& incidents,
                     int& nextIncidentNumber,
                     long long& sequence,
                     const std::string& path = defaultPath()) {
        incidents.clear();
        nextIncidentNumber = 201;
        sequence = 0;

        std::ifstream input(path);
        if (!input.is_open()) {
            return true; // First run: no incident file yet.
        }

        int maxIncidentNumber = 200;
        long long maxSequence = 0;
        std::string line;

        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream row(line);
            std::string tag;
            row >> tag;

            if (tag == "NEXT_INCIDENT") {
                int storedNext = 201;
                if (row >> storedNext && storedNext >= 201)
                    nextIncidentNumber = storedNext;
                continue;
            }

            if (tag == "SEQUENCE") {
                long long storedSequence = 0;
                if (row >> storedSequence && storedSequence >= 0)
                    sequence = storedSequence;
                continue;
            }

            if (tag != "INCIDENT" || incidents.full()) continue;

            Incident incident;
            int typeValue = 0;
            int statusValue = 0;
            int confirmed = 0;
            std::size_t nodeCount = 0;
            std::size_t edgeCount = 0;

            if (!(row >> std::quoted(incident.id)
                      >> typeValue
                      >> incident.locationIndex
                      >> std::quoted(incident.locationId)
                      >> incident.severity
                      >> incident.urgency
                      >> incident.victimCount
                      >> incident.priorityScore
                      >> incident.sequence
                      >> statusValue
                      >> incident.reportedByUserId
                      >> std::quoted(incident.description)
                      >> std::quoted(incident.requiredResponder)
                      >> std::quoted(incident.assignedResponderId)
                      >> incident.assignedStrength
                      >> confirmed
                      >> std::quoted(incident.escalationReason)
                      >> std::quoted(incident.shelterId)
                      >> std::quoted(incident.allocatedResourceType)
                      >> incident.allocatedResourceQuantity
                      >> incident.routeCost
                      >> incident.routeDistance
                      >> incident.routeTravelTime
                      >> nodeCount)) {
                continue;
            }

            if (typeValue < static_cast<int>(IncidentType::Medical) ||
                typeValue > static_cast<int>(IncidentType::Structural) ||
                statusValue < static_cast<int>(IncidentStatus::Reported) ||
                statusValue > static_cast<int>(IncidentStatus::Cancelled) ||
                incident.id.empty() || incident.reportedByUserId <= 0 ||
                nodeCount > 512) {
                continue;
            }

            bool valid = true;
            for (std::size_t i = 0; i < nodeCount; ++i) {
                int node = -1;
                if (!(row >> node)) {
                    valid = false;
                    break;
                }
                incident.routeNodes.pushBack(node);
            }

            if (!valid || !(row >> edgeCount) || edgeCount > 512) continue;

            for (std::size_t i = 0; i < edgeCount; ++i) {
                int edge = -1;
                if (!(row >> edge)) {
                    valid = false;
                    break;
                }
                incident.routeEdges.pushBack(edge);
            }
            if (!valid) continue;

            incident.type = static_cast<IncidentType>(typeValue);
            incident.status = static_cast<IncidentStatus>(statusValue);
            incident.userConfirmedResolved = confirmed != 0;

            incidents.pushBack(incident);

            const int number = persistence_detail::incidentNumber(incident.id);
            if (number > maxIncidentNumber) maxIncidentNumber = number;
            if (incident.sequence > maxSequence) maxSequence = incident.sequence;
        }

        if (nextIncidentNumber <= maxIncidentNumber)
            nextIncidentNumber = maxIncidentNumber + 1;
        if (sequence < maxSequence)
            sequence = maxSequence;

        return true;
    }

    template <std::size_t Capacity>
    static bool save(const StaticArray<Incident, Capacity>& incidents,
                     int nextIncidentNumber,
                     long long sequence,
                     const std::string& path = defaultPath()) {
        try {
            const std::filesystem::path target(path);
            if (target.has_parent_path())
                std::filesystem::create_directories(target.parent_path());

            const std::filesystem::path temp = target.string() + ".tmp";
            std::ofstream output(temp, std::ios::trunc);
            if (!output.is_open()) return false;

            output << "# CrisisMesh 2.0 persistent incident snapshots\n";
            output << "# Internal format; edit only when the program is closed.\n";
            output << "NEXT_INCIDENT " << nextIncidentNumber << "\n";
            output << "SEQUENCE " << sequence << "\n";
            output << std::setprecision(17);

            for (std::size_t i = 0; i < incidents.size(); ++i) {
                const Incident& incident = incidents[i];

                output << "INCIDENT "
                       << std::quoted(incident.id) << ' '
                       << static_cast<int>(incident.type) << ' '
                       << incident.locationIndex << ' '
                       << std::quoted(incident.locationId) << ' '
                       << incident.severity << ' '
                       << incident.urgency << ' '
                       << incident.victimCount << ' '
                       << incident.priorityScore << ' '
                       << incident.sequence << ' '
                       << static_cast<int>(incident.status) << ' '
                       << incident.reportedByUserId << ' '
                       << std::quoted(incident.description) << ' '
                       << std::quoted(incident.requiredResponder) << ' '
                       << std::quoted(incident.assignedResponderId) << ' '
                       << incident.assignedStrength << ' '
                       << (incident.userConfirmedResolved ? 1 : 0) << ' '
                       << std::quoted(incident.escalationReason) << ' '
                       << std::quoted(incident.shelterId) << ' '
                       << std::quoted(incident.allocatedResourceType) << ' '
                       << incident.allocatedResourceQuantity << ' '
                       << incident.routeCost << ' '
                       << incident.routeDistance << ' '
                       << incident.routeTravelTime << ' '
                       << incident.routeNodes.size();

                for (std::size_t n = 0; n < incident.routeNodes.size(); ++n)
                    output << ' ' << incident.routeNodes[n];

                output << ' ' << incident.routeEdges.size();
                for (std::size_t e = 0; e < incident.routeEdges.size(); ++e)
                    output << ' ' << incident.routeEdges[e];

                output << "\n";
            }

            output.flush();
            if (!output.good()) {
                output.close();
                std::error_code ignore;
                std::filesystem::remove(temp, ignore);
                return false;
            }
            output.close();

            return persistence_detail::replaceFileAtomically(target, temp);
        } catch (...) {
            return false;
        }
    }
};

// =============================================================================
// PER-USER ACTIVITY / AUDIT HISTORY STORAGE
// =============================================================================
// Stores a chronological, human-readable trail of important User/System/Author
// actions. The audit survives account deletion so operational history is not
// silently erased, while deleted credentials are still removed from users.txt.
class UserHistoryStorage {
public:
    static std::string defaultPath() {
        return "data/user_history.txt";
    }

    template <std::size_t Capacity>
    static bool load(StaticArray<UserActivityEntry, Capacity>& history,
                     long long& nextSequence,
                     const std::string& path = defaultPath()) {
        history.clear();
        nextSequence = 1;

        std::ifstream input(path);
        if (!input.is_open()) {
            return true; // First run: no history file yet.
        }

        long long maxSequence = 0;
        std::string line;

        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream row(line);
            std::string tag;
            row >> tag;

            if (tag == "NEXT_ACTIVITY") {
                long long storedNext = 1;
                if (row >> storedNext && storedNext > 0)
                    nextSequence = storedNext;
                continue;
            }

            if (tag != "ACTIVITY") continue;

            UserActivityEntry entry;
            if (!(row >> entry.sequence
                      >> entry.userId
                      >> std::quoted(entry.timestamp)
                      >> std::quoted(entry.actor)
                      >> std::quoted(entry.action)
                      >> std::quoted(entry.incidentId)
                      >> std::quoted(entry.details))) {
                continue;
            }

            if (entry.sequence <= 0 || entry.userId <= 0) continue;

            // Keep the newest Capacity records if a very old log grows large.
            if (history.full()) {
                for (std::size_t i = 1; i < history.size(); ++i)
                    history[i - 1] = history[i];
                history.popBack();
            }

            history.pushBack(entry);
            if (entry.sequence > maxSequence) maxSequence = entry.sequence;
        }

        if (nextSequence <= maxSequence)
            nextSequence = maxSequence + 1;

        return true;
    }

    template <std::size_t Capacity>
    static bool save(const StaticArray<UserActivityEntry, Capacity>& history,
                     long long nextSequence,
                     const std::string& path = defaultPath()) {
        try {
            const std::filesystem::path target(path);
            if (target.has_parent_path())
                std::filesystem::create_directories(target.parent_path());

            const std::filesystem::path temp = target.string() + ".tmp";
            std::ofstream output(temp, std::ios::trunc);
            if (!output.is_open()) return false;

            output << "# CrisisMesh 2.0 persistent per-user activity history\n";
            output << "# Format: ACTIVITY <seq> <userId> <time> <actor> <action> <incidentId> <details>\n";
            output << "NEXT_ACTIVITY " << nextSequence << "\n";

            for (std::size_t i = 0; i < history.size(); ++i) {
                const UserActivityEntry& entry = history[i];
                output << "ACTIVITY "
                       << entry.sequence << ' '
                       << entry.userId << ' '
                       << std::quoted(entry.timestamp) << ' '
                       << std::quoted(entry.actor) << ' '
                       << std::quoted(entry.action) << ' '
                       << std::quoted(entry.incidentId) << ' '
                       << std::quoted(entry.details) << "\n";
            }

            output.flush();
            if (!output.good()) {
                output.close();
                std::error_code ignore;
                std::filesystem::remove(temp, ignore);
                return false;
            }
            output.close();

            return persistence_detail::replaceFileAtomically(target, temp);
        } catch (...) {
            return false;
        }
    }
};

} // namespace crisismesh
