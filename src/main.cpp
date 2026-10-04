#include "services/AuthService.hpp"
#include "services/CrisisMeshSystem.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

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

std::string readPassword(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string password;
#ifdef _WIN32
    while (true) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            std::cout << '\n';
            break;
        }
        if (ch == 8) {
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b" << std::flush;
            }
            continue;
        }
        if (ch == 0 || ch == 224) {
            _getch();
            continue;
        }
        if (std::isprint(static_cast<unsigned char>(ch))) {
            password.push_back(static_cast<char>(ch));
            std::cout << '*' << std::flush;
        }
    }
#else
    termios oldState{};
    if (tcgetattr(STDIN_FILENO, &oldState) == 0) {
        termios hidden = oldState;
        hidden.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &hidden);
        std::getline(std::cin, password);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldState);
        std::cout << std::string(password.size(), '*') << '\n';
    } else {
        std::getline(std::cin, password);
    }
#endif
    return password;
}

void waitForBack() {
    readInt("\nPress 0 to go back: ", 0, 0);
}

std::string upperCopy(std::string value) {
    for (char& c : value) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return value;
}

bool allDigits(const std::string& value) {
    if (value.empty()) return false;
    for (char c : value) if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

std::string normalizeId(const std::string& raw, const std::string& prefix) {
    std::string value = upperCopy(raw);
    if (value == "0") return "0";
    if (value.rfind(prefix, 0) == 0) return value;
    if (allDigits(value)) {
        std::ostringstream out;
        out << prefix << std::setw(3) << std::setfill('0') << std::stoi(value);
        return out.str();
    }
    return value;
}

std::string readLocationId(const std::string& prompt) {
    return normalizeId(readLine(prompt), "LOC-");
}

std::string readRoadId(const std::string& prompt) {
    return normalizeId(readLine(prompt), "R-");
}

std::string readIncidentId(const std::string& prompt) {
    return normalizeId(readLine(prompt), "INC-");
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
              << "Nodes: " << system.graph().nodeCount() << " | Roads: " << system.graph().edgeCount() << "\n";
}

IncidentType chooseIncidentType() {
    std::cout << "\nSelect Incident Type\n"
              << "1. Medical\n2. Fire\n3. Police\n4. Rescue\n5. Accident\n6. Flood\n7. Structural\n";
    switch (readInt("Select: ", 1, 7)) {
        case 1: return IncidentType::Medical;
        case 2: return IncidentType::Fire;
        case 3: return IncidentType::Police;
        case 4: return IncidentType::Rescue;
        case 5: return IncidentType::Accident;
        case 6: return IncidentType::Flood;
        default: return IncidentType::Structural;
    }
}

std::string chooseResourceType() {
    std::cout << "\nSelect Resource Type\n"
              << "1. Water\n"
              << "2. Food Pack\n"
              << "3. Medical Kit\n"
              << "4. Rescue Kit\n"
              << "0. Back\n";
    const int choice = readInt("Select: ", 0, 4);
    if (choice == 0) return "";
    if (choice == 1) return "WATER";
    if (choice == 2) return "FOOD_PACK";
    if (choice == 3) return "MEDICAL_KIT";
    return "RESCUE_KIT";
}

std::string chooseShelterId() {
    std::cout << "\nSelect Shelter\n1. SHELTER-01\n2. SHELTER-02\n0. Back\n";
    const int choice = readInt("Select: ", 0, 2);
    if (choice == 0) return "";
    return choice == 1 ? "SHELTER-01" : "SHELTER-02";
}

bool simulatedOtpVerification(const std::string& purpose) {
    const int otp = AuthService::generateOtp();
    std::cout << "\n========== " << purpose << " VERIFICATION ==========\n"
              << "Verification Code: " << otp << "\n";
    const int entered = readInt("Enter verification code: ", 100000, 999999);
    return entered == otp;
}

void registerUser(CrisisMeshSystem& system) {
    std::cout << "\n========== USER REGISTRATION ==========\n";
    User user;
    user.name = readLine("Full name: ");
    user.email = readLine("Email: ");
    user.phone = readLine("Phone: ");
    user.username = readLine("Username: ");
    user.password = readPassword("Password: ");

    if (user.name.empty() || user.username.empty()) {
        std::cout << "Name and username cannot be empty.\n";
        return;
    }
    if (!AuthService::validEmail(user.email)) {
        std::cout << "Invalid email format.\n";
        return;
    }
    if (!AuthService::validPassword(user.password)) {
        std::cout << "Password must contain at least 6 characters with a letter and a digit.\n";
        return;
    }
    if (!simulatedOtpVerification("REGISTRATION")) {
        std::cout << "Verification failed. Registration cancelled.\n";
        return;
    }

    const int id = system.registerUser(user);
    if (id == -2) {
        std::cout << "Registration could not be saved to persistent storage. Please check file permissions.\n";
    } else if (id < 0) {
        std::cout << "Registration failed. Username may already exist or capacity is full.\n";
    } else {
        std::cout << "Registration successful. User ID: " << id << '\n';
    }
}

void forgotPassword(CrisisMeshSystem& system) {
    std::cout << "\n========== RESET PASSWORD ==========\n";
    const std::string username = readLine("Username: ");
    if (!system.usernameExists(username)) {
        std::cout << "Username not found.\n";
        return;
    }
    if (!simulatedOtpVerification("PASSWORD RESET")) {
        std::cout << "Verification failed.\n";
        return;
    }
    const std::string password = readPassword("New password: ");
    if (!AuthService::validPassword(password)) {
        std::cout << "Password must contain at least 6 characters with a letter and a digit.\n";
        return;
    }
    std::cout << (system.resetPassword(username, password) ? "Password reset successful.\n" : "Username not found.\n");
}

void reportIncident(CrisisMeshSystem& system, int userId) {
    std::cout << "\n========== REPORT EMERGENCY ==========\n";
    showCityGraph(system);
    system.listLocations();
    const IncidentType type = chooseIncidentType();
    const std::string location = readLocationId("Location ID (example LOC-014 or 014): ");
    const int severity = readInt("Severity (1-5): ", 1, 5);
    const int urgency = readInt("Urgency (1-5): ", 1, 5);
    const int victims = readInt("Victim count (0-999): ", 0, 999);
    const std::string description = readLine("Short description: ");
    const std::string id = system.createIncident(userId, type, location, severity, urgency, victims, description);
    if (id.empty()) std::cout << "Could not create incident. Check the location and input values.\n";
    else std::cout << "Emergency reported successfully. Incident ID: " << id << "\n";
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
                  << "8. Emergency Contacts\n"
                  << "9. Delete Account\n"
                  << "0. Logout\n";
        const int choice = readInt("Select: ", 0, 9);
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
            const std::string id = readIncidentId("Incident ID (example INC-201, 0 to cancel): ");
            if (id == "0") continue;
            std::cout << "1. YES - Problem solved\n2. NO - Still need help\n";
            const bool solved = readInt("Select: ", 1, 2) == 1;
            std::string reason;
            if (!solved) reason = readLine("Reason: ");
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
                std::cout << std::left << std::setw(12) << "User ID" << ": " << user->id
                          << "\n" << std::setw(12) << "Name" << ": " << user->name
                          << "\n" << std::setw(12) << "Email" << ": " << user->email
                          << "\n" << std::setw(12) << "Phone" << ": " << user->phone
                          << "\n" << std::setw(12) << "Username" << ": " << user->username << "\n";
            }
            waitForBack();
        } else if (choice == 7) {
            std::cout << "\n========== CITY GRAPH & LOCATIONS ==========\n";
            showCityGraph(system);
            system.listLocations();
            waitForBack();
        } else if (choice == 8) {
            system.showEmergencyContacts();
            waitForBack();
        } else if (choice == 9) {
            std::cout << "\n================ DELETE ACCOUNT ================\n"
                      << "This action permanently removes your saved account data.\n"
                      << "An account with an active emergency cannot be deleted.\n\n"
                      << "1. Continue\n"
                      << "0. Cancel\n";

            if (readInt("Select: ", 0, 1) == 0) continue;

            const std::string password = readPassword("Confirm password: ");
            std::cout << "\n1. Permanently Delete Account\n"
                      << "0. Cancel\n";

            if (readInt("Select: ", 0, 1) == 0) continue;

            std::string message;
            const bool deleted = system.deleteUserAccount(userId, password, message);
            std::cout << "\n" << message << '\n';

            if (deleted) {
                std::cout << "Returning to User Access...\n";
                return;
            }

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
            const std::string password = readPassword("Password: ");
            const int userId = system.authenticateUser(username, password);
            if (userId < 0) std::cout << "Invalid username or password.\n";
            else userPortal(system, userId);
        }
    }
}

bool authorLogin() {
    std::cout << "\n========== AUTHOR LOGIN ==========\n";
    const std::string username = readLine("Username: ");
    const std::string password = readPassword("Password: ");
    if (username != "author" || password != "Crisis@2026") {
        std::cout << "Invalid Author credentials.\n";
        return false;
    }
    if (!simulatedOtpVerification("AUTHOR LOGIN")) {
        std::cout << "Verification failed.\n";
        return false;
    }
    return true;
}

void incidentCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ INCIDENT CENTER ================\n";
        system.showAllIncidents();
        std::cout << "\nPending in FIFO Queue: " << system.pendingIntakeCount() << "\n"
                  << "Ready for Incident Analysis: " << system.analysisReadyCount() << "\n"
                  << "1. Process Next Pending Incident (FIFO)\n"
                  << "2. Search Incident\n"
                  << "0. Back\n";

        const int choice = readInt("Select: ", 0, 2);
        if (choice == 0) return;

        if (choice == 1) {
            std::string message;
            system.processNextIntake(message);
            std::cout << "\n" << message << '\n';
            waitForBack();
        } else {
            const std::string id = readIncidentId("Incident ID (example INC-201 or 201): ");
            if (id != "0") system.showIncidentDetails(id);
            waitForBack();
        }
    }
}

void dispatchCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ DISPATCH CENTER ================\n";
        system.showActiveDispatches();
        std::cout << "\n1. Refresh Active Dispatches\n"
                  << "2. Mark Response Completed\n"
                  << "3. Recall Response / Return to Analysis\n"
                  << "0. Back\n";

        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) continue;

        const std::string id = readIncidentId("Incident ID (example INC-201 or 201): ");
        if (id == "0") continue;

        std::string message;
        if (choice == 2) system.markResponseCompleted(id, message);
        else system.recallResponse(id, message);
        std::cout << "\n" << message << '\n';
        waitForBack();
    }
}

void cityGraphCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ CITY GRAPH & ROADS ================\n"
                  << "1. View City Graph & Locations\n"
                  << "2. View Road Network\n"
                  << "3. Block Road + Auto Reroute\n"
                  << "4. View Blocked Roads\n"
                  << "5. Undo Last Road Block\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 5);
        if (choice == 0) return;

        if (choice == 1) {
            showCityGraph(system);
            system.listLocations();
            waitForBack();
        } else if (choice == 2) {
            system.showRoads();
            waitForBack();
        } else if (choice == 3) {
            std::cout << "\n========== BLOCK ROAD & AUTO REROUTE ==========\n";
            system.showRoads();
            const std::string roadId = readRoadId("\nRoad ID to block (R-001 or 001, 0 to cancel): ");
            if (roadId != "0") {
                std::string message;
                system.blockRoad(roadId, message);
                std::cout << "\n" << message << '\n';
            }
            waitForBack();
        } else if (choice == 4) {
            system.showBlockedRoads();
            waitForBack();
        } else {
            std::cout << "\n========== UNDO LAST ROAD BLOCK ==========\n";
            std::string message;
            system.undoLastRoadBlock(message);
            std::cout << message << '\n';
            system.showBlockedRoads();
            waitForBack();
        }
    }
}

void routeSearchCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ ROUTE & LOCATION SEARCH ================\n"
                  << "1. Dijkstra Shortest-Distance Route\n"
                  << "2. Binary Search Location\n"
                  << "3. View Locations\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        if (choice == 1) {
            system.listLocations();
            const std::string from = readLocationId("From location (LOC-001 or 001): ");
            const std::string to = readLocationId("To location (LOC-002 or 002): ");
            system.runDijkstra(from, to);
            waitForBack();
        } else if (choice == 2) {
            system.listLocations();
            const std::string id = readLocationId("Location ID (LOC-001 or 001): ");
            const int index = system.findLocationBinary(id);
            if (index < 0) std::cout << "\nLocation not found.\n";
            else std::cout << "\nFound: " << system.graph().node(index).id << " - "
                           << system.graph().node(index).name << "\n";
            waitForBack();
        } else {
            system.listLocations();
            waitForBack();
        }
    }
}

void manageResponders(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ MANAGE RESPONDERS ================\n";
        system.showRespondersAndResources();
        std::cout << "\n1. Update Responder Status\n"
                  << "2. Update Responder Location\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 2);
        if (choice == 0) return;
        const std::string id = upperCopy(readLine("Responder ID: "));
        std::string message;
        if (choice == 1) {
            std::cout << "\n1. AVAILABLE\n2. BUSY\n3. OFFLINE\n0. Cancel\n";
            const int status = readInt("Select status: ", 0, 3);
            if (status == 0) continue;
            ResponderAvailability value = ResponderAvailability::Available;
            if (status == 2) value = ResponderAvailability::Busy;
            if (status == 3) value = ResponderAvailability::Offline;
            system.updateResponderStatus(id, value, message);
        } else {
            system.listLocations();
            const std::string location = readLocationId("New location (LOC-001 or 001): ");
            system.updateResponderLocation(id, location, message);
        }
        std::cout << "\n" << message << '\n';
        waitForBack();
    }
}

void manageShelters(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ MANAGE SHELTERS ================\n";
        system.showRespondersAndResources();
        std::cout << "\n1. Update Capacity\n"
                  << "2. Update Occupancy\n"
                  << "3. Change Operational Status\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 3);
        if (choice == 0) return;
        const std::string shelterId = chooseShelterId();
        if (shelterId.empty()) continue;
        std::string message;
        if (choice == 1) {
            system.updateShelterCapacity(shelterId, readInt("New capacity: ", 0, 100000), message);
        } else if (choice == 2) {
            system.updateShelterOccupancy(shelterId, readInt("New occupancy: ", 0, 100000), message);
        } else {
            std::cout << "1. ACTIVE\n2. CLOSED\n";
            system.updateShelterOperational(shelterId, readInt("Select status: ", 1, 2) == 1, message);
        }
        std::cout << "\n" << message << '\n';
        waitForBack();
    }
}

void manageSupplies(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ MANAGE SUPPLIES ================\n";
        system.showRespondersAndResources();
        std::cout << "\n1. Set Quantity\n"
                  << "2. Add Stock\n"
                  << "3. Remove Stock\n"
                  << "4. Update Resource Location\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 4);
        if (choice == 0) return;
        const std::string type = chooseResourceType();
        if (type.empty()) continue;
        std::string message;
        if (choice == 1) {
            system.setSupplyQuantity(type, readInt("New quantity: ", 0, 1000000), message);
        } else if (choice == 2) {
            system.adjustSupply(type, readInt("Quantity to add: ", 1, 1000000), message);
        } else if (choice == 3) {
            system.adjustSupply(type, -readInt("Quantity to remove: ", 1, 1000000), message);
        } else {
            system.updateSupplySource(type, readLine("New resource location: "), message);
        }
        std::cout << "\n" << message << '\n';
        waitForBack();
    }
}

void allocateShelterScreen(CrisisMeshSystem& system) {
    std::cout << "\n================ ALLOCATE SHELTER ================\n";
    system.showOperationalIncidents();
    const std::string id = readIncidentId("\nIncident ID (INC-201 or 201, 0 to cancel): ");
    if (id == "0") return;
    std::string message;
    system.allocateShelter(id, message);
    std::cout << "\n" << message << '\n';
    waitForBack();
}

void allocateSupplyScreen(CrisisMeshSystem& system) {
    std::cout << "\n================ ALLOCATE SUPPLIES ================\n";
    system.showOperationalIncidents();
    const std::string id = readIncidentId("\nIncident ID (INC-201 or 201, 0 to cancel): ");
    if (id == "0") return;
    const std::string type = chooseResourceType();
    if (type.empty()) return;
    const int available = system.supplyQuantity(type);
    std::cout << "Available Quantity: " << available << '\n';
    const int qty = readInt("Quantity to allocate: ", 1, 1000000);
    std::string message;
    system.allocateSupply(id, type, qty, message);
    std::cout << "\n" << message << '\n';
    waitForBack();
}

void resourceCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ RESPONDERS & RESOURCES ================\n"
                  << "1. View All Resources\n"
                  << "2. Manage Responders\n"
                  << "3. Manage Shelters\n"
                  << "4. Manage Supplies\n"
                  << "5. Allocate Shelter\n"
                  << "6. Allocate Supplies\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 6);
        if (choice == 0) return;
        if (choice == 1) {
            system.showRespondersAndResources();
            waitForBack();
        } else if (choice == 2) manageResponders(system);
        else if (choice == 3) manageShelters(system);
        else if (choice == 4) manageSupplies(system);
        else if (choice == 5) allocateShelterScreen(system);
        else allocateSupplyScreen(system);
    }
}

void userDirectoryCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ USER DIRECTORY ================\n";
        system.listUsers();
        std::cout << "\n1. Refresh User List\n0. Back\n";
        const int choice = readInt("Select: ", 0, 1);
        if (choice == 0) return;
    }
}

void messageCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ MESSAGE CENTER ================\n"
                  << "1. Send Direct Message\n"
                  << "2. Broadcast Message\n"
                  << "0. Back\n";
        const int choice = readInt("Select: ", 0, 2);
        if (choice == 0) return;
        if (choice == 1) {
            system.listUsers();
            const int userId = readInt("Recipient User ID: ", 1, 100);
            if (!system.userExists(userId)) {
                std::cout << "User ID not found. Message was not sent.\n";
                waitForBack();
                continue;
            }
            const std::string message = readLine("Message: ");
            std::cout << (system.sendMessage(userId, message)
                          ? "Message sent successfully.\n"
                          : "Message cannot be empty.\n");
        } else {
            const std::string message = readLine("Broadcast message: ");
            std::cout << (system.sendMessage(-1, message)
                          ? "Broadcast sent successfully.\n"
                          : "Broadcast cannot be empty.\n");
        }
    }
}

void analyzeIncident(CrisisMeshSystem& system, const std::string& incidentId) {
    if (!system.isReadyForAnalysis(incidentId)) {
        std::cout << "\nThis incident is not ready for analysis. Process it in Incident Center first.\n";
        waitForBack();
        return;
    }

    while (system.isReadyForAnalysis(incidentId)) {
        system.showIncidentResponseProfile(incidentId);

        std::cout << "\n1. Run BFS Reachability Analysis\n"
                  << "2. Run DFS Reachability Analysis\n"
                  << "3. Run Dijkstra Response Comparison\n"
                  << "4. Assign Response Resource\n"
                  << "0. Back\n";

        const int choice = readInt("Select: ", 0, 4);
        if (choice == 0) return;

        if (choice == 1) {
            system.runIncidentBfs(incidentId);
            waitForBack();
        } else if (choice == 2) {
            system.runIncidentDfs(incidentId);
            waitForBack();
        } else if (choice == 3) {
            system.runIncidentDijkstraAnalysis(incidentId);
            waitForBack();
        } else {
            std::cout << "\n================ MANUAL RESPONSE ASSIGNMENT ================\n";
            system.showIncidentResponseProfile(incidentId);
            const std::string responderId = upperCopy(readLine("\nResponder ID to assign (0 to cancel): "));
            if (responderId == "0") continue;
            const int available = system.responderAvailableStrength(responderId);

            if (available < 0) {
                std::cout << "Responder not found.\n";
                waitForBack();
                continue;
            }
            if (available == 0) {
                std::cout << "Selected responder has no available strength.\n";
                waitForBack();
                continue;
            }

            const std::string measure = system.responderMeasure(responderId);
            std::cout << "Available " << measure << ": " << available << '\n';
            const int amount = readInt("Assignment quantity (0 to cancel): ", 0, available);
            if (amount == 0) continue;

            std::string message;
            const bool assigned = system.assignResponse(incidentId, responderId, amount, message);
            std::cout << "\n" << message << '\n';
            waitForBack();

            if (assigned) return;
        }
    }
}

void incidentAnalysisCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ INCIDENT ANALYSIS ================\n";
        system.showAnalysisReadyIncidents();
        std::cout << "\nReady Incidents: " << system.analysisReadyCount() << "\n"
                  << "1. Analyze Highest-Priority Incident (Max Heap)\n"
                  << "2. Analyze Processed Incident by ID\n"
                  << "0. Back\n";

        const int choice = readInt("Select: ", 0, 2);
        if (choice == 0) return;

        std::string id;
        if (choice == 1) {
            id = system.highestReadyIncidentId();
            if (id.empty()) {
                std::cout << "\nNo processed incident is ready for analysis.\n";
                waitForBack();
                continue;
            }
        } else {
            id = readIncidentId("Incident ID (INC-201 or 201): ");
            if (id == "0") continue;
            if (!system.isReadyForAnalysis(id)) {
                std::cout << "\nIncident is not in PRIORITIZED state. Process it in Incident Center first.\n";
                waitForBack();
                continue;
            }
        }

        analyzeIncident(system, id);
    }
}

void archiveCenter(CrisisMeshSystem& system) {
    while (true) {
        std::cout << "\n================ ARCHIVE & HISTORY ================\n";
        system.showArchive();
        std::cout << "\n1. Refresh Archive\n0. Back\n";
        const int choice = readInt("Select: ", 0, 1);
        if (choice == 0) return;
    }
}

void dsaSummaryCenter() {
    std::cout << "\n========================== DSA SUMMARY ==========================\n"
              << std::left << std::setw(6) << "No." << std::setw(28) << "Data Structure / Algorithm" << "Used For\n"
              << std::string(76, '-') << '\n'
              << std::setw(6) << "1" << std::setw(28) << "Array" << "Users, incidents, responders, resources\n"
              << std::setw(6) << "2" << std::setw(28) << "Linked List" << "Messages and closed incident history\n"
              << std::setw(6) << "3" << std::setw(28) << "Stack" << "Road-block undo and DFS traversal\n"
              << std::setw(6) << "4" << std::setw(28) << "Queue" << "FIFO emergency intake and BFS traversal\n"
              << std::setw(6) << "5" << std::setw(28) << "AVL Tree" << "Closed incident archive\n"
              << std::setw(6) << "6" << std::setw(28) << "Max Heap" << "Emergency priority scheduling\n"
              << std::setw(6) << "7" << std::setw(28) << "Hash Table" << "Incident and username lookup\n"
              << std::setw(6) << "8" << std::setw(28) << "Merge Sort" << "Responder ranking\n"
              << std::setw(6) << "9" << std::setw(28) << "Binary Search" << "Location search\n"
              << std::setw(6) << "10" << std::setw(28) << "Graph" << "Shared 20-node city network\n"
              << std::setw(6) << "11" << std::setw(28) << "Dijkstra" << "Shortest emergency route\n"
              << std::string(76, '-') << '\n'
              << "Total DSA Components: 11\n";
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
                  << "8. Incident Analysis\n"
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
        else if (choice == 8) incidentAnalysisCenter(system);
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
              << "============================================================\n";

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
            dsaSummaryCenter();
        }
    }

    std::cout << "CrisisMesh Console Edition closed.\n";
    return 0;
}
