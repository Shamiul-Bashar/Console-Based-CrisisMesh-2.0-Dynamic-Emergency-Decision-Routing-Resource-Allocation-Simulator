#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "dsa/AVLTree.hpp"
#include "dsa/HashTable.hpp"
#include "dsa/LinkedList.hpp"
#include "dsa/Queue.hpp"
#include "dsa/Stack.hpp"
#include "graph/Graph.hpp"
#include "services/CrisisMeshSystem.hpp"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <string>

using namespace crisismesh;

int main() {
    Queue<int> q;
    q.enqueue(10);
    q.enqueue(20);
    assert(q.dequeue() == 10);

    Stack<int> s;
    s.push(1);
    s.push(2);
    assert(s.pop() == 2);

    LinkedList<std::string> list;
    list.pushBack("A");
    list.pushBack("B");
    assert(list.size() == 2);

    HashTable<int> table;
    table.put("INC-1", 7);
    assert(table.get("INC-1") && *table.get("INC-1") == 7);

    AVLTree tree;
    tree.insert(3, "C");
    tree.insert(1, "A");
    tree.insert(2, "B");
    assert(tree.find(2) && *tree.find(2) == "B");

    Graph g;
    g.seedCrisisMeshCity();
    auto bfs = BFS::traverse(g, 0);
    auto dfs = DFS::traverse(g, 0);
    assert(bfs.size() == 20);
    assert(dfs.size() == 20);

    const auto weightedRoute = Dijkstra::shortestPath(g, 0, 18);
    const auto distanceRoute = Dijkstra::shortestDistancePath(g, 0, 18);
    assert(weightedRoute.reachable);
    assert(distanceRoute.reachable);
    assert(distanceRoute.distance <= weightedRoute.distance + 1e-9);

    const std::string testDir = "test_runtime_data";
    const std::string persistenceFile = testDir + "/users_persistence.txt";
    const std::string persistenceIncidents = testDir + "/incidents_persistence.txt";
    const std::string persistenceHistory = testDir + "/history_persistence.txt";
    const std::string workflowFile = testDir + "/users_workflow.txt";
    const std::string workflowIncidents = testDir + "/incidents_workflow.txt";
    const std::string workflowHistory = testDir + "/history_workflow.txt";
    std::filesystem::remove_all(testDir);

    // Persistent user-account lifecycle.
    User persistentUser;
    persistentUser.name = "Persistent User";
    persistentUser.email = "persistent@example.com";
    persistentUser.phone = "111";
    persistentUser.username = "persistent";
    persistentUser.password = "Keep123";

    int persistentUserId = -1;
    {
        CrisisMeshSystem firstRun(persistenceFile, persistenceIncidents, persistenceHistory);
        persistentUserId = firstRun.registerUser(persistentUser);
        assert(persistentUserId == 1);
        assert(firstRun.authenticateUser("persistent", "Keep123") == persistentUserId);
        assert(std::filesystem::exists(persistenceFile));
    }

    {
        CrisisMeshSystem secondRun(persistenceFile, persistenceIncidents, persistenceHistory);
        assert(secondRun.authenticateUser("persistent", "Keep123") == persistentUserId);
        assert(secondRun.resetPassword("persistent", "New456"));
    }

    {
        CrisisMeshSystem thirdRun(persistenceFile, persistenceIncidents, persistenceHistory);
        assert(thirdRun.authenticateUser("persistent", "New456") == persistentUserId);
        std::string deleteMessage;
        assert(thirdRun.deleteUserAccount(persistentUserId, "New456", deleteMessage));
        assert(!thirdRun.usernameExists("persistent"));
        assert(thirdRun.hasRetainedUserData(persistentUserId));
    }

    {
        CrisisMeshSystem fourthRun(persistenceFile, persistenceIncidents, persistenceHistory);
        assert(!fourthRun.usernameExists("persistent"));
        assert(fourthRun.hasUserHistory(persistentUserId));
        assert(fourthRun.hasRetainedUserData(persistentUserId));
        User nextUser = persistentUser;
        nextUser.username = "nextuser";
        nextUser.email = "next@example.com";
        nextUser.password = "Next123";
        assert(fourthRun.registerUser(nextUser) == 2); // Deleted IDs are not reused.
    }

    CrisisMeshSystem system(workflowFile, workflowIncidents, workflowHistory);
    User user;
    user.name = "Test User";
    user.email = "test@example.com";
    user.phone = "000";
    user.username = "tester";
    user.password = "Test123";
    const int userId = system.registerUser(user);
    assert(userId == 1);
    assert(system.userExists(userId));
    assert(!system.userExists(99));

    // Direct messaging must reject non-existent recipients.
    assert(!system.sendMessage(99, "Invalid recipient"));
    assert(system.sendMessage(userId, "Valid direct message"));
    assert(!system.sendMessage(userId, ""));

    // Police incident: manual officer pool allocation.
    const std::string policeIncidentA = system.createIncident(
        userId, IncidentType::Police, "LOC-014", 5, 5, 2, "Police response A");
    assert(!policeIncidentA.empty());

    std::string message;
    assert(!system.deleteUserAccount(userId, "Test123", message)); // Active emergency protects account integrity.
    assert(system.processNextIntake(message));
    assert(system.isReadyForAnalysis(policeIncidentA));
    assert(system.highestReadyIncidentId() == policeIncidentA);

    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 30);
    assert(system.assignResponse(policeIncidentA, "POLICE-UNIT-01", 10, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 20);

    // Same police station can support a second incident using remaining officers.
    const std::string policeIncidentB = system.createIncident(
        userId, IncidentType::Police, "LOC-019", 4, 4, 1, "Police response B");
    assert(!policeIncidentB.empty());
    assert(system.processNextIntake(message));
    assert(system.assignResponse(policeIncidentB, "POLICE-UNIT-01", 5, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 15);

    // Shelter allocation is valid while response is active and cannot be double-counted.
    assert(system.allocateShelter(policeIncidentA, message));
    assert(!system.allocateShelter(policeIncidentA, message));

    assert(system.markResponseCompleted(policeIncidentA, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 25);
    assert(system.markResponseCompleted(policeIncidentB, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 30);
    assert(!system.allocateShelter(policeIncidentA, message));

    // Fire unit: discrete unit, no double assignment.
    const std::string fireIncidentA = system.createIncident(
        userId, IncidentType::Fire, "LOC-018", 5, 4, 1, "Fire response A");
    assert(!fireIncidentA.empty());
    assert(system.processNextIntake(message));
    assert(system.assignResponse(fireIncidentA, "FIRE-UNIT-03", 1, message));
    assert(system.responderAvailableStrength("FIRE-UNIT-03") == 0);

    const std::string fireIncidentB = system.createIncident(
        userId, IncidentType::Fire, "LOC-019", 4, 4, 1, "Fire response B");
    assert(!fireIncidentB.empty());
    assert(system.processNextIntake(message));
    assert(!system.assignResponse(fireIncidentB, "FIRE-UNIT-03", 1, message));

    // Recall returns the unit and sends the incident back to Incident Analysis.
    assert(system.recallResponse(fireIncidentA, message));
    assert(system.responderAvailableStrength("FIRE-UNIT-03") == 1);
    assert(system.isReadyForAnalysis(fireIncidentA));

    assert(system.assignResponse(fireIncidentA, "FIRE-UNIT-03", 1, message));
    assert(system.markResponseCompleted(fireIncidentA, message));
    assert(system.responderAvailableStrength("FIRE-UNIT-03") == 1);
    assert(system.confirmResolution(userId, fireIncidentA, true, "", message));

    // Keep one dispatch active so responder usage can be reconstructed after restart.
    const std::string activeMedicalIncident = system.createIncident(
        userId, IncidentType::Medical, "LOC-017", 4, 4, 1, "Active restart check");
    assert(!activeMedicalIncident.empty());
    assert(system.processNextIntake(message));
    assert(system.assignResponse(activeMedicalIncident, "AMB-UNIT-04", 1, message));
    assert(system.responderAvailableStrength("AMB-UNIT-04") == 0);

    // Incident-linked supply consumption must also survive the restart.
    assert(system.supplyQuantity("WATER") == 500);
    assert(system.allocateSupply(activeMedicalIncident, "WATER", 15, message));
    assert(system.supplyQuantity("WATER") == 485);

    // Leave one additional incident queued so FIFO reconstruction can be verified.
    const std::string restartQueuedIncident = system.createIncident(
        userId, IncidentType::Medical, "LOC-017", 3, 3, 1, "Restart persistence check");
    assert(!restartQueuedIncident.empty());
    assert(std::filesystem::exists(workflowIncidents));
    assert(std::filesystem::exists(workflowHistory));

    {
        CrisisMeshSystem restarted(workflowFile, workflowIncidents, workflowHistory);
        assert(restarted.authenticateUser("tester", "Test123") == userId);
        assert(restarted.hasUserHistory(userId));

        const Incident* closed = restarted.findIncident(fireIncidentA);
        assert(closed != nullptr);
        assert(closed->status == IncidentStatus::Closed);

        const Incident* active = restarted.findIncident(activeMedicalIncident);
        assert(active != nullptr);
        assert(active->status == IncidentStatus::EnRoute);
        assert(restarted.responderAvailableStrength("AMB-UNIT-04") == 0);
        assert(restarted.supplyQuantity("WATER") == 485);

        const Incident* queued = restarted.findIncident(restartQueuedIncident);
        assert(queued != nullptr);
        assert(queued->status == IncidentStatus::Queued);
        assert(restarted.pendingIntakeCount() >= 1);
        assert(restarted.isReadyForAnalysis(fireIncidentB));
    }

    // Multiple road block + LIFO undo.
    assert(system.blockRoad("R-001", message));
    assert(system.blockRoad("R-002", message));
    assert(system.graph().edge(system.graph().findEdgeIndex("R-001")).blocked);
    assert(system.graph().edge(system.graph().findEdgeIndex("R-002")).blocked);
    assert(system.undoRoadBlock(message));
    assert(!system.graph().edge(system.graph().findEdgeIndex("R-002")).blocked);
    assert(system.graph().edge(system.graph().findEdgeIndex("R-001")).blocked);
    assert(system.undoRoadBlock(message));
    assert(!system.graph().edge(system.graph().findEdgeIndex("R-001")).blocked);

    std::filesystem::remove_all(testDir);
    return 0;
}
