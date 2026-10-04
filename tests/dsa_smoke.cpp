#include "algorithms/BFS.hpp"
#include "algorithms/DFS.hpp"
#include "algorithms/Dijkstra.hpp"
#include "dsa/AVLTree.hpp"
#include "dsa/HashTable.hpp"
#include "dsa/LinkedList.hpp"
#include "dsa/Queue.hpp"
#include "dsa/Stack.hpp"
#include "graph/Graph.hpp"

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
    return 0;
}
