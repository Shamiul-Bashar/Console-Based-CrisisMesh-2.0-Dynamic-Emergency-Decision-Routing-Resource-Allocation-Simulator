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
#include <string>

using namespace crisismesh;

int main() {
    Queue<int> q; q.enqueue(10); q.enqueue(20); assert(q.dequeue() == 10);
    Stack<int> s; s.push(1); s.push(2); assert(s.pop() == 2);
    LinkedList<std::string> list; list.pushBack("A"); list.pushBack("B"); assert(list.size() == 2);
    HashTable<int> table; table.put("INC-1", 7); assert(table.get("INC-1") && *table.get("INC-1") == 7);
    AVLTree tree; tree.insert(3, "C"); tree.insert(1, "A"); tree.insert(2, "B"); assert(tree.find(2) && *tree.find(2) == "B");

    Graph g; g.seedCrisisMeshCity();
    auto bfs = BFS::traverse(g, 0); assert(bfs.size() == 20);
    auto dfs = DFS::traverse(g, 0); assert(dfs.size() == 20);
    auto route = Dijkstra::shortestPath(g, 0, 18); assert(route.reachable);

    CrisisMeshSystem system;
    User user;
    user.name = "Test User";
    user.email = "test@example.com";
    user.phone = "000";
    user.username = "tester";
    user.password = "Test123";
    const int userId = system.registerUser(user);
    assert(userId == 1);

    const std::string policeIncident = system.createIncident(
        userId, IncidentType::Police, "LOC-014", 5, 5, 2, "Police response test");
    assert(!policeIncident.empty());

    std::string message;
    assert(system.processNextIntake(message));
    assert(system.isReadyForAnalysis(policeIncident));
    assert(system.highestReadyIncidentId() == policeIncident);

    const int policeBefore = system.responderAvailableStrength("POLICE-UNIT-01");
    assert(policeBefore == 30);
    assert(system.assignResponse(policeIncident, "POLICE-UNIT-01", 10, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 20);
    assert(system.markResponseCompleted(policeIncident, message));
    assert(system.responderAvailableStrength("POLICE-UNIT-01") == 30);

    const std::string fireIncident = system.createIncident(
        userId, IncidentType::Fire, "LOC-018", 5, 4, 1, "Fire response test");
    assert(!fireIncident.empty());
    assert(system.processNextIntake(message));
    assert(system.assignResponse(fireIncident, "FIRE-UNIT-03", 1, message));
    assert(system.responderAvailableStrength("FIRE-UNIT-03") == 0);
    assert(system.markResponseCompleted(fireIncident, message));
    assert(system.responderAvailableStrength("FIRE-UNIT-03") == 1);

    return 0;
}
