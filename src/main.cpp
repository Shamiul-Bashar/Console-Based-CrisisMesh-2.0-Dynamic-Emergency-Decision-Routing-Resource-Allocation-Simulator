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
    std::cout << "(Academic console mode: OTP is generated randomly and displayed locally; no email/SMS is sent.)\n";
    const int entered = readInt("Enter OTP: ", 100000, 999999);
    return entered == otp;
}

void registerUser(CrisisMeshSystem& system) {
    std::cout << "\n========== USER REGISTRATION ==========" << '\n';
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
    std::cout << "\n========== RESET PASSWORD ==========" << '\n';
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
    std::cout << "\n========== REPORT EMERGENCY ==========" << '\n';
    system.listLocations();
    const IncidentType type = chooseIncidentType();
    const std::string location = readLine("Location ID (example LOC-014): ");
    const int severity = readInt("Severity (1-5): ", 1, 5);
    const int urgency = readInt("Urgency (1-5): ", 1, 5);
    const int victims = readInt("Victim count (0-999): ", 0, 999);
    const std::string description = readLine("Short description: ");
    const std::string id = system.createIncident(userId, type, location, severity, urgency, victims, description);
    if (id.empty()) std::cout << "Could not create incident. Check location/inputs.\n";
    else std::cout << "Emergency reported successfully as " << id << ". It is now in the manual FIFO intake Queue.\n";
}

void userPortal(CrisisMeshSystem& system, int userId) {
    while (true) {
        const User* user = system.getUser(userId);
        std::cout << "\n============================================================\n"
                  << " CRISISMESH 2.0 - USER PORTAL | " << (user ? user->name : "User") << "\n"
                  << "============================================================\n"
                  << "1. Report Emergency\n2. Track My Incidents\n3. Confirm Problem Solved / Still Need Help\n"
                  << "4. Messages\n5. Closed History\n6. My Profile\n7. View City Locations\n0. Logout\n";
        const int choice = readInt("Select: ", 0, 7);
        if (choice == 0) return;
        if (choice == 1) reportIncident(system, userId);
        else if (choice == 2) system.showUserIncidents(userId);
        else if (choice == 3) {
            system.showUserIncidents(userId);
            const std::string id = readLine("Incident ID: ");
            std::cout << "1. YES - Problem solved\n2. NO - Still need help\n";
            const bool solved = readInt("Choice: ", 1, 2) == 1;
            std::string reason;
            if (!solved) reason = readLine("Escalation reason: ");
            std::string message;
            system.confirmResolution(userId, id, solved, reason, message);
            std::cout << message << '\n';
        }
        else if (choice == 4) system.showMessagesForUser(userId);
        else if (choice == 5) system.showUserIncidents(userId, true);
        else if (choice == 6) {
            if (user) std::cout << "\nID: " << user->id << "\nName: " << user->name << "\nEmail: " << user->email
                                << "\nPhone: " << user->phone << "\nUsername: " << user->username << "\n";
        }
        else if (choice == 7) system.listLocations();
    }
}

void userEntry(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n========== USER ACCESS ==========" << '\n'
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
    std::cout << "\n========== AUTHOR LOGIN ==========" << '\n';
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

void authorPortal(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================================================================\n"
                  << " CRISISMESH 2.0 - AUTHOR / EMERGENCY OPERATIONS CONSOLE\n"
                  << "================================================================\n"
                  << "1. Dashboard\n2. Process Next FIFO Intake\n3. Dispatch Highest Priority Incident\n"
                  << "4. View All Incidents\n5. Search Incident (Hash Table)\n6. Mark Field Response Completed\n"
                  << "7. View Responders / Shelters / Supplies\n8. Block Road + Auto Reroute\n9. Undo Last Road Block (Stack)\n"
                  << "10. BFS Analysis\n11. DFS Analysis\n12. Dijkstra Route Analysis\n13. Search Location (Binary Search)\n"
                  << "14. Allocate Shelter\n15. Allocate Supplies\n16. User Directory\n17. Send Direct Message\n18. Broadcast Message\n"
                  << "19. AVL Archive + Linked List History\n20. View Roads\n0. Logout\n";
        const int choice = readInt("Select: ", 0, 20);
        if (choice == 0) return;

        std::string message;
        if (choice == 1) system.dashboard();
        else if (choice == 2) { system.processNextIntake(message); std::cout << message << '\n'; }
        else if (choice == 3) { system.dispatchHighest(message); std::cout << message << '\n'; }
        else if (choice == 4) system.showAllIncidents();
        else if (choice == 5) system.showIncidentDetails(readLine("Incident ID: "));
        else if (choice == 6) { system.markResponseCompleted(readLine("Incident ID: "), message); std::cout << message << '\n'; }
        else if (choice == 7) system.showRespondersAndResources();
        else if (choice == 8) { system.showRoads(); system.blockRoad(readLine("Road ID (example R-001): "), message); std::cout << message << '\n'; }
        else if (choice == 9) { system.undoLastRoadBlock(message); std::cout << message << '\n'; }
        else if (choice == 10) system.runBfs(readLine("Start location ID: "));
        else if (choice == 11) system.runDfs(readLine("Start location ID: "));
        else if (choice == 12) {
            const std::string from = readLine("From location ID: ");
            const std::string to = readLine("To location ID: ");
            system.runDijkstra(from, to);
        }
        else if (choice == 13) {
            const std::string id = readLine("Location ID: ");
            const int index = system.findLocationBinary(id);
            if (index < 0) std::cout << "Not found.\n";
            else std::cout << "Found: " << system.graph().node(index).id << " | " << system.graph().node(index).name << "\n";
        }
        else if (choice == 14) { system.allocateShelter(readLine("Incident ID: "), message); std::cout << message << '\n'; }
        else if (choice == 15) {
            const std::string id = readLine("Incident ID: ");
            const std::string type = readLine("Resource type (WATER/FOOD_PACK/MEDICAL_KIT/RESCUE_KIT): ");
            const int qty = readInt("Quantity: ", 1, 10000);
            system.allocateSupply(id, type, qty, message); std::cout << message << '\n';
        }
        else if (choice == 16) system.listUsers();
        else if (choice == 17) {
            system.listUsers();
            const int userId = readInt("Recipient User ID: ", 1, 100);
            system.sendMessage(userId, readLine("Message: "));
            std::cout << "Direct message queued in manual Linked List message store.\n";
        }
        else if (choice == 18) {
            system.sendMessage(-1, readLine("Broadcast message: "));
            std::cout << "Broadcast queued for all registered users.\n";
        }
        else if (choice == 19) system.showArchive();
        else if (choice == 20) system.showRoads();
    }
}

} // namespace

int main() {
    CrisisMeshSystem system;

    std::cout << "\n================================================================\n"
              << "          CRISISMESH 2.0 - CONSOLE EDITION (C++17)\n"
              << " Dynamic Emergency Decision, Routing & Resource Allocation\n"
              << "================================================================\n"
              << "Academic simulation only - no real emergency service connection.\n";

    while (true) {
        std::cout << "\nMAIN PORTAL\n1. User Portal\n2. Author Portal\n3. About DSA Architecture\n0. Exit\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) break;
        if (choice == 1) userEntry(system);
        else if (choice == 2) {
            if (authorLogin()) authorPortal(system);
        } else {
            std::cout << "\nOperational DSA path:\n"
                      << "User Report -> manual Queue -> priority calculation -> manual Max Heap -> responder Array ->\n"
                      << "manual Merge Sort -> Graph + manual Min Heap Dijkstra -> Stack road undo ->\n"
                      << "user confirmation -> Linked List history + AVL Tree archive.\n"
                      << "Additional evidence: manual Hash Table incident lookup, Binary Search locations, BFS and DFS.\n";
        }
    }

    std::cout << "CrisisMesh Console Edition closed.\n";
    return 0;
}