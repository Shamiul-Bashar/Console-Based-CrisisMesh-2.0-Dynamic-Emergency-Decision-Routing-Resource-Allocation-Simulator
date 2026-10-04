# CrisisMesh 2.0 — Console Edition

A professional **C++17 console-based emergency decision, dispatch, routing, and resource-allocation simulator** derived from the same academic concept as CrisisMesh 2.0, but intentionally redesigned with **no GUI, no web frontend, no Node server, and no real email/SMS OTP delivery**.

This edition is built for a Data Structures & Algorithms project demonstration. The console explicitly exposes the structures and algorithms used in each operational step so they are easy to explain during presentation and viva.

## Portals

### User Portal
- Registration with random **simulated console OTP** verification
- Login and password reset with random console OTP
- Report emergency incidents
- Track only the user's own incidents
- Confirm `YES` when the problem is solved
- Confirm `NO` with an escalation reason when more help is needed
- Read direct and broadcast Author messages
- View closed incident history and profile

### Author Portal
- Author login + random simulated console OTP
- Operations dashboard
- Process the FIFO incident intake Queue
- Dispatch highest-priority incident from the Max Heap
- View/search incidents
- Mark field response complete
- Inspect responders, shelters, and supplies
- Block roads and automatically reroute active incidents
- Undo the latest road block using a Stack
- Run BFS, DFS, and Dijkstra on the current road graph
- Binary-search locations
- Allocate shelters and supplies
- View registered users without passwords
- Send direct messages or broadcast announcements
- Inspect AVL archive and Linked List history

## Core Operational Workflow

```text
User emergency report
  -> manual FIFO Queue
  -> deterministic priority calculation
  -> manual Max Heap
  -> compatible responder filtering in Array
  -> manual Merge Sort candidate ranking
  -> Graph + manual Min Heap Dijkstra
  -> responder dispatch / EN_ROUTE
  -> optional road block + Stack undo + reroute
  -> Author marks field response complete
  -> User confirms YES or NO
     -> YES: CLOSED -> Linked List history + AVL archive
     -> NO : ESCALATED -> FIFO Queue again
```

## DSA Coverage

| Requirement | Manual implementation | Operational use |
|---|---|---|
| Array | `include/dsa/Array.hpp` | Users, incidents, responders, shelters, resources, candidate buffers |
| Linked List | `include/dsa/LinkedList.hpp` | Closed-incident history and Author messages |
| Stack | `include/dsa/Stack.hpp` | Road-block undo and DFS traversal |
| Queue | `include/dsa/Queue.hpp` | FIFO incident intake and BFS frontier |
| Tree | `include/dsa/AVLTree.hpp` | Balanced archive of closed incidents |
| BFS | `include/algorithms/BFS.hpp` | Open-road reachability / level traversal |
| DFS | `include/algorithms/DFS.hpp` | Open-road depth traversal |
| Searching | manual binary search in `CrisisMeshSystem` | Location validation/search |
| Sorting | `include/algorithms/MergeSort.hpp` | Responder candidate ranking |
| Max Heap | `include/dsa/MaxHeap.hpp` | Highest-priority emergency scheduling |
| Min Heap | `include/dsa/MinHeap.hpp` | Dijkstra frontier |
| Hash Table | `include/dsa/HashTable.hpp` | Fast incident and username lookup |
| Graph | `include/graph/Graph.hpp` | 24-node, 42-road city network |
| Dijkstra | `include/algorithms/Dijkstra.hpp` | Dispatch, rerouting, shelter selection |

> STL containers are intentionally avoided for the assessed Array/Linked List/Stack/Queue/Tree/Heap/Hash structures. `std::vector` is used only inside the graph adjacency representation, matching the project instruction that STL is acceptable for graph work.

## Routing Model

The console preserves the CrisisMesh weighted road cost idea:

```text
cost = distance
     + 0.35 * travel_time
     + 0.75 * risk
     + 0.45 * congestion
     + 0.03 * max(0, 70 - capacity)
```

Blocked roads are ignored by BFS, DFS, and Dijkstra. If a blocked road belongs to an active dispatch route, the system attempts a new Dijkstra route automatically.

## Incident Lifecycle

```text
QUEUED
 -> TRIAGED
 -> PRIORITIZED
 -> ASSIGNED
 -> EN_ROUTE
 -> RESPONSE_COMPLETED
 -> AWAITING_USER_CONFIRMATION
    -> YES -> RESOLVED -> CLOSED
    -> NO  -> ESCALATED -> QUEUED
```

Additional states include `WAITING_FOR_RESOURCE`, `REROUTE_REQUIRED`, `UNREACHABLE`, and `CANCELLED`.

## Simulated OTP

This console edition does **not** send email or SMS. Every verification generates a random 6-digit OTP and displays it in the terminal:

```text
[SIMULATED OTP - AUTHOR LOGIN] 483921
```

The user types that value back into the console. This keeps the verification workflow without requiring external services.

## Demo Author Credentials

```text
Username: author
Password: Crisis@2026
```

An additional random console OTP is required after the password is accepted.

## Build

### CMake

```bash
cmake -S . -B build
cmake --build build
```

Run:

```bash
./build/crisismesh_console
```

On Windows with a multi-config generator, the executable may be under `build/Debug/` or `build/Release/`.

### Direct g++

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp -o crisismesh_console
./crisismesh_console
```

## Tests

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

`tests/dsa_smoke.cpp` checks the manual Queue, Stack, Linked List, Hash Table, AVL Tree and Graph/BFS/DFS/Dijkstra basics.

## Suggested Presentation Demo

1. Register a User and complete simulated OTP verification.
2. Submit a FIRE or MEDICAL incident.
3. Login as Author and explain the FIFO Queue.
4. Process intake and show priority insertion into the Max Heap.
5. Dispatch the highest-priority incident and explain Array -> Merge Sort -> Dijkstra.
6. Block one road, show automatic rerouting, then use Stack undo.
7. Run BFS and DFS from a selected location.
8. Mark response complete.
9. Login as the reporting User and answer `YES` to close the incident.
10. Return to Author and show the Linked List history + AVL Tree archive.
11. Repeat with `NO` to demonstrate escalation and re-queuing.
12. Demonstrate direct/broadcast messaging and shelter/resource allocation.

## Academic Scope

This is a deterministic academic simulator. It does not connect to real emergency services, GPS, maps, municipal systems, production identity providers, email gateways, or SMS gateways.