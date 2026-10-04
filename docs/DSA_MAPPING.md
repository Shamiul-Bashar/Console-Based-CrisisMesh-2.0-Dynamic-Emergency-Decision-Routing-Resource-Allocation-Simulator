# DSA Marks Mapping — CrisisMesh Console Edition

This file maps the tentative marking distribution directly to executable project features.

## 1) Array, Linked List, Stack, Queue — 5 marks

- **Array**: manual `StaticArray` and `DynamicArray` in `include/dsa/Array.hpp`.
  - Stores users, incidents, responders, shelters and supplies.
  - Responder candidate ranking uses an explicit fixed array buffer.
- **Linked List**: manual singly linked list in `include/dsa/LinkedList.hpp`.
  - Stores chronological closed-incident history.
  - Stores Author -> User / broadcast messages.
- **Stack**: manual Stack in `include/dsa/Stack.hpp`.
  - Stores blocked roads for `Undo Last Road Block`.
  - DFS uses the same manual Stack.
- **Queue**: manual circular-array Queue in `include/dsa/Queue.hpp`.
  - New incidents enter the FIFO intake Queue.
  - BFS uses the same manual Queue.

## 2) Tree — 5 marks

- **AVL Tree**: `include/dsa/AVLTree.hpp`.
- When a reporting user confirms `YES`, the closed incident is inserted into the AVL archive using report sequence as the key.
- Author menu can print the AVL archive in-order.
- Rotations keep the tree balanced.

## 3) BFS, DFS, Searching — 3 marks

- **BFS**: `include/algorithms/BFS.hpp`, traverses only open roads.
- **DFS**: `include/algorithms/DFS.hpp`, traverses only open roads.
- **Searching**:
  - manual Binary Search over sorted `LOC-001 ... LOC-020` IDs for location validation/search;
  - manual separate-chaining Hash Table provides fast incident ID lookup and username lookup.

## 4) Sorting — 2 marks

- **Merge Sort**: `include/algorithms/MergeSort.hpp`.
- During dispatch, all compatible available responders are evaluated with Dijkstra.
- Candidates are Merge-Sorted by:
  1. reachable first,
  2. lower weighted route cost,
  3. lower travel time,
  4. lower distance,
  5. responder ID tie-break.

## Additional DSA Evidence

- **Max Heap** schedules the highest-priority triaged incident.
- **Min Heap** drives Dijkstra.
- **Hash Table** indexes incidents/users.
- **Graph** models the 20-node, 31-road city.
- **Dijkstra** powers dispatch, rerouting, and shelter selection.

## STL Policy

The assessed structures above are implemented manually. `std::vector` is used only for the graph adjacency representation, because STL is explicitly permitted for graph implementation in the project instruction.