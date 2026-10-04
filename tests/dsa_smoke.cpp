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

    CrisisMeshSystem system;
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

    assert(system.markResponseCompleted(policeIncidentA, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 25);
    assert(system.markResponseCompleted(policeIncidentB, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 30);

    // Shelter allocation cannot be counted twice for the same incident.
    assert(system.allocateShelter(policeIncidentA, message));
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

    return 0;
}
