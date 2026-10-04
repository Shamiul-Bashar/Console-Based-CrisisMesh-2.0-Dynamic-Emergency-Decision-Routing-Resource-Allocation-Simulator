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
- View Emergency Contacts for every response unit using fixed 3-digit simulation numbers

### Author Portal
- Author login + random simulated console OTP
- Login opens a `---------------- DASHBOARD ----------------` summary
- Dashboard contains only **10 grouped main sections**:
  1. Incident Center
  2. Dispatch Center
  3. City Graph & Roads
  4. Route & Location Search
  5. Responders & Resources
  6. User Directory
  7. Message Center
  8. Incident Analysis
  9. Archive & History
  10. DSA Summary
- Every section opens as its own screen and always provides `0. Back`, even when no data exists.
- The City Graph is a compact **20-node / 31-road** network. The exact same location IDs and names are shared by User and Author portals.

## Core Operational Workflow

```text
User emergency report
  -> manual FIFO Queue
  -> Author processes incident
  -> priority calculation + manual Max Heap
  -> Incident Analysis
     -> type-specific response category
     -> compatible responder availability
     -> BFS / DFS reachability
     -> Dijkstra comparison for every compatible responder
     -> shortest response path recommendation
  -> Author manually assigns responder / response strength
  -> EN_ROUTE
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
| Sorting | `include/algorithms/MergeSort.hpp` | Incident-analysis responder ranking by reachable route, distance and time |
| Max Heap | `include/dsa/MaxHeap.hpp` | Highest-priority processed incident in Incident Analysis |
| Min Heap | `include/dsa/MinHeap.hpp` | Dijkstra frontier |
| Hash Table | `include/dsa/HashTable.hpp` | Fast incident and username lookup |
| Graph | `include/graph/Graph.hpp` | 20-node, 31-road city network |
| Dijkstra | `include/algorithms/Dijkstra.hpp` | Compare compatible responders, recommend shortest response, reroute, shelter selection |

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

## Console Verification

Authentication workflows use a random 6-digit console verification code.

```text
========== AUTHOR LOGIN VERIFICATION ==========
Verification Code: 483921
```

Password input is masked in the terminal.

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
5. Open Incident Analysis and select the highest-priority processed incident from the Max Heap.
6. Show compatible response locations, total/available/on-operation strength.
7. Run BFS and DFS from the incident location.
8. Run Dijkstra and explain the responder distance table and recommended shortest path.
9. Manually assign a responder; for Police, assign an officer count.
10. Show the reduced availability in Responders & Resources.
11. Mark the response completed from Dispatch Center and show the resource strength restored.
12. Login as the reporting User and answer `YES` to close the incident.
13. Show the Linked List history + AVL Tree archive.
14. Demonstrate road blocking/Stack undo, messaging, shelters and supplies.

## Academic Scope

This is a deterministic academic simulator. It does not connect to real emergency services, GPS, maps, municipal systems, production identity providers, email gateways, or SMS gateways.

## Manual Response Strength

Incident Analysis uses professional response categories:
- Police -> **Law Enforcement Response** (officers)
- Fire -> **Fire & Rescue Response** (fire units)
- Medical / Accident -> **Emergency Medical Response** (ambulance units)
- Rescue / Flood / Structural -> **Search & Rescue Response** (rescue teams)

The responder table tracks **Total**, **Available**, and **On Operation** strength. For example, assigning 10 officers from a station with 30 available immediately changes its state to 20 available / 10 on operation. Marking the response completed returns those 10 officers to the available pool.
