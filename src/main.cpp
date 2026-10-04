#include "services/AuthService.hpp"
#include "services/CrisisMeshSystem.hpp"

#include <iostream>
#include <limits>
#include <string>

using namespace crisismesh;

namespace {

int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value && value >= minValue && value <= maxValue) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Enter a value from " << minValue << " to " << maxValue << ".\n";
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

void waitForBack() {
    readInt("\nPress 0 to go back: ", 0, 0);
}

void showCityGraph(const CrisisMeshSystem& system) {
    std::cout << "\n================ CITY GRAPH (20 NODES) ================\n"
              << " LOC-001 ----- LOC-002 ----- LOC-003 ----- LOC-004 ----- LOC-005\n"
              << "    |             |             |             |             |\n"
              << " LOC-006 ----- LOC-007 ----- LOC-008 ----- LOC-009 ----- LOC-010\n"
              << "    |             |             |             |             |\n"
              << " LOC-011 ----- LOC-012 ----- LOC-013 ----- LOC-014 ----- LOC-015\n"
              << "    |             |             |             |             |\n"
              << " LOC-016 ----- LOC-017 ----- LOC-018 ----- LOC-019 ----- LOC-020\n"
              << "=========================================================\n"
              << "Nodes: " << system.graph().nodeCount() << " | Roads: " << system.graph().edgeCount() << "\n"
              << "User and Author portals use the same location IDs and names.\n";
}

IncidentType chooseIncidentType() {
    std::cout << "\n1. Medical\n2. Fire\n3. Police\n4. Rescue\n5. Accident\n6. Flood\n7. Structural\n";
    switch (readInt("Type: ", 1, 7)) {
        case 1: return IncidentType::Medical;
        case 2: return IncidentType::Fire;
        case 3: return IncidentType::Police;
        case 4: return IncidentType::Rescue;
        case 5: return IncidentType::Accident;
        case 6: return IncidentType::Flood;
        default: return IncidentType::Structural;
    }
}

bool simulatedOtpVerification(const std::string& purpose) {
    const int otp = AuthService::generateOtp();
    std::cout << "\n[SIMULATED OTP - " << purpose << "] " << otp << "\n";
    std::cout << "Academic console mode: OTP is displayed locally; no email/SMS is sent.\n";
    const int entered = readInt("Enter OTP: ", 100000, 999999);
    return entered == otp;
}

void registerUser(CrisisMeshSystem& system) {
    std::cout << "\n========== USER REGISTRATION ==========\n";
    User user;
    user.name = readLine("Full name: ");
    user.email = readLine("Email: ");
    user.phone = readLine("Phone: ");
    user.username = readLine("Username: ");
    user.password = readLine("Password (min 6 chars, letter + digit): ");

    if (!AuthService::validEmail(user.email)) {
        std::cout << "Invalid email format.\n";
        return;
    }
    if (!AuthService::validPassword(user.password)) {
        std::cout << "Weak password. Use at least 6 characters with a letter and a digit.\n";
        return;
    }
    if (!simulatedOtpVerification("USER REGISTRATION")) {
        std::cout << "OTP verification failed. Registration cancelled.\n";
        return;
    }

    const int id = system.registerUser(user);
    if (id < 0) std::cout << "Registration failed. Username may already exist or capacity is full.\n";
    else std::cout << "Registration successful. User ID: " << id << '\n';
}

void forgotPassword(CrisisMeshSystem& system) {
    std::cout << "\n========== RESET PASSWORD ==========\n";
    const std::string username = readLine("Username: ");
    if (!simulatedOtpVerification("PASSWORD RESET")) {
        std::cout << "OTP verification failed.\n";
        return;
    }
    const std::string password = readLine("New password: ");
    if (!AuthService::validPassword(password)) {
        std::cout << "Weak password.\n";
        return;
    }
    std::cout << (system.resetPassword(username, password) ? "Password reset successful.\n" : "Username not found.\n");
}

void reportIncident(CrisisMeshSystem& system, int userId) {
    std::cout << "\n========== REPORT EMERGENCY ==========\n";
    showCityGraph(system);
    system.listLocations();
    const IncidentType type = chooseIncidentType();
    const std::string location = readLine("Location ID (example LOC-014): ");
    const int severity = readInt("Severity (1-5): ", 1, 5);
    const int urgency = readInt("Urgency (1-5): ", 1, 5);
    const int victims = readInt("Victim count (0-999): ", 0, 999);
    const std::string description = readLine("Short description: ");
    const std::string id = system.createIncident(userId, type, location, severity, urgency, victims, description);
    if (id.empty()) std::cout << "Could not create incident. Check location/inputs.\n";
    else std::cout << "Emergency reported successfully as " << id << ". It is now in the FIFO intake Queue.\n";
}

void userPortal(CrisisMeshSystem& system, int userId) {
    while (true) {
        const User* user = system.getUser(userId);
        std::cout << "\n================ USER PORTAL ================\n"
                  << "Logged in as: " << (user ? user->name : "User") << "\n"
                  << "1. Report Emergency\n"
                  << "2. My Incidents\n"
                  << "3. Confirm Problem Solved / Still Need Help\n"
                  << "4. Messages\n"
                  << "5. Closed History\n"
                  << "6. My Profile\n"
                  << "7. City Graph & Locations\n"
                  << "0. Logout\n";
        const int choice = readInt("Select: ", 0, 7);
        if (choice == 0) return;

        if (choice == 1) {
            reportIncident(system, userId);
            waitForBack();
        } else if (choice == 2) {
            std::cout << "\n========== MY INCIDENTS ==========\n";
            system.showUserIncidents(userId);
            waitForBack();
        } else if (choice == 3) {
            std::cout << "\n========== RESOLUTION CONFIRMATION ==========\n";
            system.showUserIncidents(userId);
            const std::string id = readLine("Incident ID: ");
            std::cout << "1. YES - Problem solved\n2. NO - Still need help\n";
            const bool solved = readInt("Choice: ", 1, 2) == 1;
            std::string reason;
            if (!solved) reason = readLine("Escalation reason: ");
            std::string message;
            system.confirmResolution(userId, id, solved, reason, message);
            std::cout << message << '\n';
            waitForBack();
        } else if (choice == 4) {
            std::cout << "\n========== MESSAGES ==========\n";
            system.showMessagesForUser(userId);
            waitForBack();
        } else if (choice == 5) {
            std::cout << "\n========== CLOSED HISTORY ==========\n";
            system.showUserIncidents(userId, true);
            waitForBack();
        } else if (choice == 6) {
            std::cout << "\n========== MY PROFILE ==========\n";
            if (user) {
                std::cout << "ID: " << user->id << "\nName: " << user->name << "\nEmail: " << user->email
                          << "\nPhone: " << user->phone << "\nUsername: " << user->username << "\n";
            }
            waitForBack();
        } else if (choice == 7) {
            std::cout << "\n========== CITY GRAPH & LOCATIONS ==========\n";
            showCityGraph(system);
            system.listLocations();
            waitForBack();
        }
    }
}

void userEntry(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== USER ACCESS ==========\n"
                  << "1. Register\n2. Login\n3. Forgot Password\n0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) registerUser(system);
        else if (choice == 3) forgotPassword(system);
        else {
            const std::string username = readLine("Username: ");
            const std::string password = readLine("Password: ");
            const int userId = system.authenticateUser(username, password);
            if (userId < 0) std::cout << "Invalid username or password.\n";
            else userPortal(system, userId);
        }
    }
}

bool authorLogin() {
    std::cout << "\n========== AUTHOR LOGIN ==========\n";
    const std::string username = readLine("Username: ");
    const std::string password = readLine("Password: ");
    if (username != "author" || password != "Crisis@2026") {
        std::cout << "Invalid Author credentials.\n";
        return false;
    }
    if (!simulatedOtpVerification("AUTHOR LOGIN")) {
        std::cout << "OTP verification failed.\n";
        return false;
    }
    return true;
}

void incidentCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== INCIDENT CENTER ==========\n";
        system.showAllIncidents();
        std::cout << "\n1. Process Next FIFO Intake\n"
                  << "2. Search Incident\n"
                  << "3. Mark Field Response Completed\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        std::string message;
        if (choice == 1) {
            system.processNextIntake(message);
            std::cout << message << '\n';
        } else if (choice == 2) {
            system.showIncidentDetails(readLine("Incident ID: "));
        } else {
            system.markResponseCompleted(readLine("Incident ID: "), message);
            std::cout << message << '\n';
        }
    }
}

void dispatchCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== DISPATCH CENTER ==========\n"
                  << "1. Dispatch Highest Priority Incident\n"
                  << "2. View Responders\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) {
            std::string message;
            system.dispatchHighest(message);
            std::cout << message << '\n';
        } else {
            system.showRespondersAndResources();
        }
    }
}

void cityGraphCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== CITY GRAPH & ROADS ==========\n";
        showCityGraph(system);
        std::cout << "\n1. View Location Names\n"
                  << "2. View Road Details\n"
                  << "3. Block Road + Auto Reroute\n"
                  << "4. Undo Last Road Block\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 4);
        if (choice == 0) return;
        std::string message;
        if (choice == 1) system.listLocations();
        else if (choice == 2) system.showRoads();
        else if (choice == 3) {
            system.showRoads();
            system.blockRoad(readLine("Road ID (example R-001): "), message);
            std::cout << message << '\n';
        } else {
            system.undoLastRoadBlock(message);
            std::cout << message << '\n';
        }
    }
}

void routeSearchCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== ROUTE & LOCATION SEARCH ==========\n"
                  << "1. Dijkstra Shortest Route\n"
                  << "2. Binary Search Location\n"
                  << "3. View Locations\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) {
            const std::string from = readLine("From location ID: ");
            const std::string to = readLine("To location ID: ");
            system.runDijkstra(from, to);
        } else if (choice == 2) {
            const std::string id = readLine("Location ID: ");
            const int index = system.findLocationBinary(id);
            if (index < 0) std::cout << "Location not found.\n";
            else std::cout << "Found: " << system.graph().node(index).id << " | "
                           << system.graph().node(index).name << "\n";
        } else {
            system.listLocations();
        }
    }
}

void resourceCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== RESPONDERS & RESOURCES ==========\n"
                  << "1. View Responders / Shelters / Supplies\n"
                  << "2. Allocate Shelter\n"
                  << "3. Allocate Supplies\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) {
            system.showRespondersAndResources();
        } else if (choice == 2) {
            std::string message;
            system.allocateShelter(readLine("Incident ID: "), message);
            std::cout << message << '\n';
        } else {
            const std::string id = readLine("Incident ID: ");
            const std::string type = readLine("Resource type (WATER/FOOD_PACK/MEDICAL_KIT/RESCUE_KIT): ");
            const int qty = readInt("Quantity: ", 1, 10000);
            std::string message;
            system.allocateSupply(id, type, qty, message);
            std::cout << message << '\n';
        }
    }
}

void userDirectoryCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== USER DIRECTORY ==========\n";
        system.listUsers();
        std::cout << "\n1. Refresh User List\n0. Back\n";
        const int choice = readInt("Select: ", 0, 1);
        if (choice == 0) return;
    }
}

void messageCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== MESSAGE CENTER ==========\n"
                  << "1. Send Direct Message\n"
                  << "2. Broadcast Message\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) {
            system.listUsers();
            const int userId = readInt("Recipient User ID: ", 1, 100);
            system.sendMessage(userId, readLine("Message: "));
            std::cout << "Direct message stored.\n";
        } else {
            system.sendMessage(-1, readLine("Broadcast message: "));
            std::cout << "Broadcast stored for all registered users.\n";
        }
    }
}

void traversalCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== BFS / DFS ANALYSIS ==========\n"
                  << "1. Run BFS\n"
                  << "2. Run DFS\n"
                  << "3. View City Graph\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) system.runBfs(readLine("Start location ID: "));
        else if (choice == 2) system.runDfs(readLine("Start location ID: "));
        else showCityGraph(system);
    }
}

void archiveCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== ARCHIVE & HISTORY ==========\n";
        system.showArchive();
        std::cout << "\n1. Refresh Archive\n0. Back\n";
        const int choice = readInt("Select: ", 0, 1);
        if (choice == 0) return;
    }
}

void dsaSummaryCenter() {
    std::cout << "\n========== DSA SUMMARY ==========\n"
              << "Array       : users, incidents, responders, resources\n"
              << "Linked List : messages and closed-history records\n"
              << "Stack       : road-block undo and DFS\n"
              << "Queue       : FIFO emergency intake and BFS\n"
              << "AVL Tree    : closed incident archive\n"
              << "Max Heap    : emergency priority scheduling\n"
              << "Hash Table  : incident and username lookup\n"
              << "Merge Sort  : responder ranking\n"
              << "Binary Search: location lookup\n"
              << "Graph       : shared 20-node city\n"
              << "Dijkstra    : shortest emergency route\n";
    waitForBack();
}

void authorPortal(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n---------------- DASHBOARD ----------------\n"
                  << "1. Incident Center\n"
                  << "2. Dispatch Center\n"
                  << "3. City Graph & Roads\n"
                  << "4. Route & Location Search\n"
                  << "5. Responders & Resources\n"
                  << "6. User Directory\n"
                  << "7. Message Center\n"
                  << "8. BFS / DFS Analysis\n"
                  << "9. Archive & History\n"
                  << "10. DSA Summary\n"
                  << "0. Logout\n";

        const int choice = readInt("Select: ", 0, 10);
        if (choice == 0) return;
        if (choice == 1) incidentCenter(system);
        else if (choice == 2) dispatchCenter(system);
        else if (choice == 3) cityGraphCenter(system);
        else if (choice == 4) routeSearchCenter(system);
        else if (choice == 5) resourceCenter(system);
        else if (choice == 6) userDirectoryCenter(system);
        else if (choice == 7) messageCenter(system);
        else if (choice == 8) traversalCenter(system);
        else if (choice == 9) archiveCenter(system);
        else dsaSummaryCenter();
    }
}

} // namespace

int main() {
    CrisisMeshSystem system;

    std::cout << "\n============================================================\n"
              << "          CRISISMESH 2.0 - CONSOLE EDITION (C++17)\n"
              << " Dynamic Emergency Decision, Routing & Resource Allocation\n"
              << "============================================================\n"
              << "Academic simulation only - no real emergency service connection.\n";

    while (true) {
        std::cout << "\nMAIN PORTAL\n"
                  << "1. User Portal\n"
                  << "2. Author Portal\n"
                  << "3. About DSA Architecture\n"
                  << "0. Exit\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) break;
        if (choice == 1) userEntry(system);
        else if (choice == 2) {
            if (authorLogin()) authorPortal(system);
        } else {
            std::cout << "\nOperational DSA path:\n"
                      << "User Report -> Queue -> Max Heap -> responder Array -> Merge Sort -> Dijkstra -> dispatch.\n"
                      << "Supporting DSA: Stack, Linked List, AVL Tree, Hash Table, Binary Search, BFS and DFS.\n";
            waitForBack();
        }
    }

    std::cout << "CrisisMesh Console Edition closed.\n";
    return 0;
}
