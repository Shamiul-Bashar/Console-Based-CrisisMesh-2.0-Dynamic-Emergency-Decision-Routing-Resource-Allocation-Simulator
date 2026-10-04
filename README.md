<div align="center">

# CrisisMesh 2.0 — Console Edition

### Dynamic Emergency Decision, Incident Analysis, Routing & Resource Allocation Simulator

![Language](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Architecture](https://img.shields.io/badge/Architecture-DSA--First-6f42c1?style=for-the-badge)
![Interface](https://img.shields.io/badge/Interface-Console-0f766e?style=for-the-badge)
![City Graph](https://img.shields.io/badge/City_Graph-20_Nodes_%7C_31_Roads-f97316?style=for-the-badge)

[![C++ Build and Tests](https://github.com/Shamiul-Bashar/Console-Based-risisMesh-2.0-Dynamic-Emergency-Decision-Routing-Resource-Allocation-Simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/Shamiul-Bashar/Console-Based-risisMesh-2.0-Dynamic-Emergency-Decision-Routing-Resource-Allocation-Simulator/actions/workflows/ci.yml)

A **C++17, DSA-driven emergency management simulator** that connects emergency reporting, FIFO intake, priority scheduling, graph analysis, shortest-path routing, manual deployment, rerouting, resource allocation, resolution, and archival into one complete console workflow.

</div>

---

## Overview

CrisisMesh 2.0 is an academic **Data Structures & Algorithms simulation project** designed to demonstrate core DSA concepts through a realistic emergency-response domain instead of isolated laboratory examples.

The system contains two operational portals:

- **User Portal** — registration, emergency reporting, incident tracking, messages, resolution confirmation, city locations, and emergency contacts.
- **Author Portal** — FIFO incident processing, priority scheduling, Incident Analysis, graph traversal, Dijkstra response comparison, manual resource assignment, road control, messaging, shelter/supply allocation, and archival.

The city is deliberately compact: **20 shared locations connected by 31 bidirectional roads**. This is large enough to demonstrate graph algorithms but small enough to inspect and explain during a project showcase or viva.

### Final audited baseline

The current implementation includes:

- manual Array, Linked List, Stack, Queue, AVL Tree, Max Heap, Min Heap, and Hash Table;
- BFS, DFS, Binary Search, Merge Sort, and Dijkstra;
- priority-based incident processing through a Max Heap;
- type-specific Incident Analysis;
- shortest-distance comparison across compatible response units;
- manual Police officer-pool assignment;
- Fire / Ambulance / Rescue unit availability tracking;
- multiple road blocking with LIFO undo;
- automatic rerouting for affected active routes;
- response recall and re-analysis;
- shelter and supply allocation;
- user YES/NO confirmation and escalation;
- Linked List history + AVL archive;
- direct/broadcast messaging;
- persistent registered-user accounts in a local text file;
- safe User Portal account deletion;
- masked password input and console verification codes;
- permanent Linux + Windows build/test CI.

---

## Key Specifications

| Feature | Implementation |
|---|---|
| Language | **C++17** |
| Interface | **Console / terminal** |
| Portals | **User + Author** |
| City model | **20 locations / 31 bidirectional roads** |
| Incident types | Medical, Fire, Police, Rescue, Accident, Flood, Structural |
| Response groups | Ambulance, Fire Unit, Police Unit, Rescue Team |
| Incident intake | Manual **FIFO Queue** |
| Priority scheduling | Manual **Max Heap** |
| Graph representation | Adjacency-list Graph |
| Traversal | **BFS + DFS** |
| Route analysis | **Dijkstra shortest-distance path** |
| Dijkstra frontier | Manual **Min Heap** |
| Candidate ordering | Manual **Merge Sort** |
| Fast lookup | Manual **Hash Table** |
| Location lookup | Manual **Binary Search** |
| Road undo | Manual **Stack** |
| Closed history | Manual **Linked List** |
| Closed archive | Manual **AVL Tree** |
| Authentication | Password + random 6-digit console verification |
| Account storage | Persistent local text file: `data/users.txt` |
| Operational runtime | Incidents/messages/road/resource state remain in-memory |
| Build validation | GitHub Actions on **Linux + Windows** |

---

## Repository Structure

~~~text
Console-Based-risisMesh-2.0-Dynamic-Emergency-Decision-Routing-Resource-Allocation-Simulator/
│
├── .github/
│   └── workflows/
│       └── ci.yml
│
├── docs/
│   ├── ARCHITECTURE.md
│   └── DSA_MAPPING.md
│
├── include/
│   ├── algorithms/
│   │   ├── BFS.hpp
│   │   ├── BinarySearch.hpp
│   │   ├── DFS.hpp
│   │   ├── Dijkstra.hpp
│   │   └── MergeSort.hpp
│   │
│   ├── dsa/
│   │   ├── AVLTree.hpp
│   │   ├── Array.hpp
│   │   ├── HashTable.hpp
│   │   ├── LinkedList.hpp
│   │   ├── MaxHeap.hpp
│   │   ├── MinHeap.hpp
│   │   ├── Queue.hpp
│   │   └── Stack.hpp
│   │
│   ├── graph/
│   │   └── Graph.hpp
│   ├── models/
│   │   └── Models.hpp
│   └── services/
│       ├── AuthService.hpp
│       └── CrisisMeshSystem.hpp
│
├── src/
│   └── main.cpp
├── tests/
│   └── dsa_smoke.cpp
├── CMakeLists.txt
├── .gitignore
└── README.md
~~~

| Layer | Responsibility |
|---|---|
| <code>src/main.cpp</code> | Console navigation, User/Author menus, input/output flow |
| <code>services/CrisisMeshSystem.hpp</code> | Main application orchestration and operational state |
| <code>models/Models.hpp</code> | User, Incident, Responder, Shelter, Supply and lifecycle models |
| <code>graph/Graph.hpp</code> | Shared 20-node / 31-road city network |
| <code>algorithms/</code> | BFS, DFS, Dijkstra, Binary Search, Merge Sort |
| <code>dsa/</code> | Manually implemented core data structures |
| <code>tests/</code> | DSA + workflow + resource lifecycle regression tests |
| <code>.github/workflows/ci.yml</code> | Cross-platform build/test validation |

---

# High-Level System Architecture

~~~mermaid
flowchart TB
    USER["User Portal"]
    AUTHOR["Author Portal"]
    MAIN["Console Application<br/>src/main.cpp"]
    AUTH["AuthService<br/>verification + validation"]
    SYS["CrisisMeshSystem<br/>Application / Service Layer"]
    MODEL["Domain Models<br/>User • Incident • Responder<br/>Shelter • Supply • Message"]
    GRAPH["City Graph<br/>20 Nodes • 31 Roads"]
    ALG["Algorithm Layer<br/>BFS • DFS • Dijkstra<br/>Binary Search • Merge Sort"]
    DSA["Manual DSA Layer<br/>Array • Linked List • Stack • Queue<br/>AVL • Max Heap • Min Heap • Hash Table"]
    TEST["CTest / GitHub Actions<br/>Linux + Windows"]

    USER --> MAIN
    AUTHOR --> MAIN
    MAIN --> AUTH
    MAIN --> SYS

    SYS --> MODEL
    SYS --> GRAPH
    SYS --> ALG
    SYS --> DSA

    ALG --> GRAPH
    ALG --> DSA

    TEST --> SYS
    TEST --> ALG
    TEST --> DSA
~~~

The architecture separates **console interaction**, **application logic**, **domain state**, **graph algorithms**, and **manual DSA implementations**. The portals call the central system layer rather than directly modifying internal structures.

---

# Portal Architecture

~~~mermaid
flowchart LR
    START["Main Portal"]

    START --> USER["User Portal"]
    START --> AUTHOR["Author Portal"]

    USER --> U1["Register / Login / Reset"]
    USER --> U2["Report Emergency"]
    USER --> U3["Track Incidents"]
    USER --> U4["YES / NO Resolution"]
    USER --> U5["Messages"]
    USER --> U6["History / Profile"]
    USER --> U7["City Graph & Locations"]
    USER --> U8["Emergency Contacts"]

    AUTHOR --> A1["Incident Center"]
    AUTHOR --> A2["Dispatch Center"]
    AUTHOR --> A3["City Graph & Roads"]
    AUTHOR --> A4["Route & Location Search"]
    AUTHOR --> A5["Responders & Resources"]
    AUTHOR --> A6["User Directory"]
    AUTHOR --> A7["Message Center"]
    AUTHOR --> A8["Incident Analysis"]
    AUTHOR --> A9["Archive & History"]
    AUTHOR --> A10["DSA Summary"]
~~~

## Author Dashboard

The Author portal is intentionally grouped into **10 main sections** so the system remains easy to navigate and demonstrate.

| No. | Section | Purpose |
|---:|---|---|
| 1 | Incident Center | View incidents, process FIFO intake, search incident |
| 2 | Dispatch Center | View active deployment, complete or recall response |
| 3 | City Graph & Roads | View graph/roads, block roads, inspect blocks, Stack undo |
| 4 | Route & Location Search | Dijkstra route analysis, Binary Search, location list |
| 5 | Responders & Resources | Responders, shelters, supplies, updates and allocation |
| 6 | User Directory | Registered-user table |
| 7 | Message Center | Direct and broadcast Author messages |
| 8 | Incident Analysis | BFS / DFS / Dijkstra + manual response assignment |
| 9 | Archive & History | Linked List closed history + AVL archive |
| 10 | DSA Summary | Presentation-oriented DSA mapping |

Every submenu provides a clear **0. Back** path.

---

# End-to-End Emergency Workflow

~~~mermaid
flowchart TD
    REPORT["User reports emergency"]
    QUEUE["Incident enters FIFO Queue"]
    PROCESS["Author processes next intake"]
    PRIORITY["Calculate priority score"]
    HEAP["Insert into Max Heap"]
    READY["Incident becomes PRIORITIZED"]
    ANALYSIS["Open Incident Analysis"]
    TRAVERSE["Run BFS / DFS if required"]
    DIJKSTRA["Run Dijkstra for compatible responders"]
    RANK["Merge Sort response candidates"]
    RECOMMEND["Show shortest-distance recommendation"]
    ASSIGN["Author manually assigns responder / strength"]
    ENROUTE["Incident → EN_ROUTE"]
    ROAD{"Blocked road affects active route?"}
    REROUTE["Recompute route"]
    FIELD["Field response"]
    COMPLETE["Author marks response completed"]
    CONFIRM{"User confirms solved?"}
    CLOSE["RESOLVED → CLOSED"]
    ARCHIVE["Linked List history + AVL archive"]
    ESCALATE["ESCALATED<br/>increase urgency"]
    REQUEUE["Return to FIFO Queue"]

    REPORT --> QUEUE --> PROCESS --> PRIORITY --> HEAP --> READY
    READY --> ANALYSIS --> TRAVERSE --> DIJKSTRA --> RANK --> RECOMMEND --> ASSIGN --> ENROUTE
    ENROUTE --> ROAD
    ROAD -- "No" --> FIELD
    ROAD -- "Yes" --> REROUTE --> FIELD
    FIELD --> COMPLETE --> CONFIRM
    CONFIRM -- "YES" --> CLOSE --> ARCHIVE
    CONFIRM -- "NO" --> ESCALATE --> REQUEUE --> PROCESS
~~~

This workflow is the core of CrisisMesh. The major stages are backed by visible DSA concepts instead of hidden application behavior.

---

# Incident Lifecycle

~~~mermaid
stateDiagram-v2
    [*] --> QUEUED : User reports incident
    QUEUED --> TRIAGED : Author processes FIFO intake
    TRIAGED --> PRIORITIZED : Priority calculated
    PRIORITIZED --> EN_ROUTE : Manual response assignment

    EN_ROUTE --> REROUTE_REQUIRED : Active road blocked
    REROUTE_REQUIRED --> EN_ROUTE : Alternate route found
    REROUTE_REQUIRED --> UNREACHABLE : No alternate route

    EN_ROUTE --> PRIORITIZED : Recall response
    UNREACHABLE --> PRIORITIZED : Recall / re-analysis

    EN_ROUTE --> AWAITING_USER_CONFIRMATION : Response completed

    AWAITING_USER_CONFIRMATION --> CLOSED : User confirms YES
    AWAITING_USER_CONFIRMATION --> ESCALATED : User confirms NO

    ESCALATED --> QUEUED : Urgency increased + requeue
    CLOSED --> [*]
~~~

The model also defines states such as <code>ASSIGNED</code>, <code>RESPONSE_COMPLETED</code>, <code>WAITING_FOR_RESOURCE</code>, <code>RESOLVED</code>, and <code>CANCELLED</code> to keep the domain explicit.

---

# Incident Priority Engine

When an incident is processed, CrisisMesh calculates:

~~~text
Priority Score
= severity × 12
+ urgency × 10
+ min(victim_count, 10) × 2
+ incident_type_weight
~~~

### Incident-type weight

| Type | Weight |
|---|---:|
| Fire | 8 |
| Medical | 8 |
| Accident | 7 |
| Rescue | 7 |
| Flood | 6 |
| Structural | 6 |
| Police | 5 |

### Operational priority label

| Score | Label |
|---:|---|
| >= 120 | **CRITICAL** |
| >= 90 | **HIGH** |
| >= 65 | **ELEVATED** |
| < 65 | **STANDARD** |

Processed incidents are inserted into the manual **Max Heap**. Higher scores are analyzed first; sequence number preserves earlier-report priority when scores are equal.

~~~mermaid
flowchart LR
    INPUT["Severity<br/>Urgency<br/>Victims<br/>Incident Type"]
    SCORE["Priority Formula"]
    LABEL["CRITICAL / HIGH<br/>ELEVATED / STANDARD"]
    HEAP["Manual Max Heap"]
    ANALYZE["Analyze Highest-Priority Incident"]

    INPUT --> SCORE
    SCORE --> LABEL
    SCORE --> HEAP
    HEAP --> ANALYZE
~~~

---

# Incident Analysis Workflow

Incident Analysis is the main decision-support section of the Author portal.

~~~mermaid
flowchart TD
    READY["PRIORITIZED Incident"]
    TYPE["Read incident type"]
    MAP["Map to response category"]
    FILTER["Filter compatible responders"]
    AVAIL["Show Total / Available / On Operation"]
    CHOICE{"Analysis requested"}

    BFS["BFS<br/>Reachability"]
    DFS["DFS<br/>Depth traversal"]
    DIJ["Dijkstra<br/>from every compatible responder"]
    SORT["Merge Sort<br/>reachable → distance → time → ID"]
    TABLE["Distance / time comparison table"]
    BEST["Highlight shortest-distance responder"]
    PATH["Show full path<br/>LOC-xxx → LOC-xxx → ..."]
    MANUAL["Author manually selects responder"]
    DEPLOY["Deploy resource / strength"]

    READY --> TYPE --> MAP --> FILTER --> AVAIL --> CHOICE
    CHOICE --> BFS
    CHOICE --> DFS
    CHOICE --> DIJ --> SORT --> TABLE --> BEST --> PATH --> MANUAL --> DEPLOY
~~~

## Response-category mapping

| Incident type | Professional response category | Required resource |
|---|---|---|
| Police | **Law Enforcement Response** | Police Unit / Officers |
| Fire | **Fire & Rescue Response** | Fire Unit |
| Medical | **Emergency Medical Response** | Ambulance |
| Accident | **Emergency Medical Response** | Ambulance |
| Rescue | **Search & Rescue Response** | Rescue Team |
| Flood | **Search & Rescue Response** | Rescue Team |
| Structural | **Search & Rescue Response** | Rescue Team |

---

# Dijkstra Response Analysis

For a selected incident, CrisisMesh evaluates every compatible response resource against the incident location.

The final recommendation uses **physical shortest distance**. The route also retains an operational cost metric for additional context.

~~~mermaid
flowchart LR
    INCIDENT["Incident Location"]
    R1["Compatible Responder A"]
    R2["Compatible Responder B"]
    R3["Compatible Responder C"]

    D1["Dijkstra"]
    D2["Dijkstra"]
    D3["Dijkstra"]

    C["Candidate Table"]
    S["Merge Sort"]
    BEST["Shortest Available<br/>Reachable Response"]
    ROUTE["Highlighted Route"]

    R1 --> D1
    R2 --> D2
    R3 --> D3

    INCIDENT --> D1
    INCIDENT --> D2
    INCIDENT --> D3

    D1 --> C
    D2 --> C
    D3 --> C

    C --> S --> BEST --> ROUTE
~~~

### Candidate ranking

1. Reachable responders first
2. Shorter physical distance
3. Lower travel time
4. Responder ID tie-break

Example output concept:

~~~text
Responder         Location     Available   Distance     Time
-------------------------------------------------------------
POLICE-UNIT-01    LOC-005      30          3.1 km       7 min
POLICE-UNIT-02    LOC-006      24          4.6 km      10 min
POLICE-UNIT-03    LOC-016      18          6.2 km      14 min

>>> SHORTEST DISTANCE PATH:
LOC-005 -> LOC-010 -> LOC-015 -> LOC-014
~~~

Dijkstra provides **decision support**. The Author still makes the final assignment manually.

---

# Routing Metrics

## Physical shortest-distance route

Used for Incident Analysis, manual response assignment, rerouting, and shelter route selection:

~~~text
Minimize Σ road_distance
~~~

## Operational route cost

Each road also exposes a calculated operational cost:

~~~text
cost = distance
     + 0.35 × travel_time
     + 0.75 × risk
     + 0.45 × congestion
     + 0.03 × max(0, 70 - capacity)
~~~

The operational cost is retained as route-quality information while the current final recommendation is based on shortest physical distance.

Blocked roads are ignored by BFS, DFS, and route calculations.

---

# Resource Deployment Lifecycle

~~~mermaid
flowchart LR
    TOTAL["Total Strength"]
    AVAILABLE["Available"]
    ASSIGN["Manual Assignment"]
    OP["On Operation"]
    COMPLETE["Response Completed"]
    RESTORE["Return Strength"]
    RECALL["Recall Response"]

    TOTAL --> AVAILABLE
    AVAILABLE --> ASSIGN --> OP

    OP --> COMPLETE --> RESTORE --> AVAILABLE
    OP --> RECALL --> RESTORE
~~~

## Police officer-pool example

~~~text
Central Police Station

Total Officers      = 30
Available Before    = 30
Assigned            = 10
On Operation        = 10
Available After     = 20

Response Completed
        ↓
Available returns to 30
~~~

Police resources can support multiple simultaneous incidents while enough officers remain available.

## Fire / Ambulance / Rescue

Fire Units, Ambulance Units, and Rescue Teams are discrete deployable resources. A fully assigned unit cannot be assigned to another incident until the current response is completed or recalled.

---

# Dispatch Recall / Re-analysis

If the wrong response resource was assigned, or conditions change, the deployment can be recalled.

~~~mermaid
flowchart TD
    ACTIVE["Active / Unreachable Dispatch"]
    RECALL["Recall Response"]
    RESTORE["Restore officers / unit / team"]
    CLEAR["Clear route + assignment"]
    PRIORITY["Incident → PRIORITIZED"]
    HEAP["Return to Max Heap"]
    ANALYSIS["Incident Analysis"]
    REASSIGN["Run route analysis + reassign"]

    ACTIVE --> RECALL --> RESTORE --> CLEAR --> PRIORITY --> HEAP --> ANALYSIS --> REASSIGN
~~~

This recovery path prevents the workflow from getting stuck after a bad assignment or route failure.

---

# Road Block, Reroute & Stack Undo

Multiple roads can be blocked. Every successfully blocked road is pushed onto a manual **Stack**.

~~~mermaid
flowchart TD
    BLOCK["Author selects road to block"]
    VALID{"Road valid and open?"}
    SET["Mark road BLOCKED"]
    PUSH["Push road index onto Stack"]
    ACTIVE{"Used by active dispatch?"}
    REROUTE["Run Dijkstra again"]
    FOUND{"Alternate route found?"}
    ENROUTE["Continue EN_ROUTE"]
    UNREACHABLE["Mark UNREACHABLE"]

    UNDO["Undo Last Road Block"]
    POP["Pop Stack"]
    OPEN["Reopen road"]
    RECOVER["Retry unreachable routes"]

    BLOCK --> VALID
    VALID -- "No" --> BLOCK
    VALID -- "Yes" --> SET --> PUSH --> ACTIVE

    ACTIVE -- "No" --> ENROUTE
    ACTIVE -- "Yes" --> REROUTE --> FOUND
    FOUND -- "Yes" --> ENROUTE
    FOUND -- "No" --> UNREACHABLE

    UNDO --> POP --> OPEN --> RECOVER
~~~

LIFO example:

~~~text
Block order:
R-001 → R-008 → R-015

Undo order:
R-015 → R-008 → R-001
~~~

This provides a direct operational use of the Stack data structure.

---

# User Resolution & Escalation Workflow

~~~mermaid
flowchart TD
    COMPLETE["Author marks field response completed"]
    WAIT["AWAITING_USER_CONFIRMATION"]
    USER{"Problem solved?"}

    YES["YES"]
    RESOLVED["RESOLVED"]
    CLOSED["CLOSED"]
    LIST["Append to Linked List history"]
    AVL["Insert into AVL archive"]

    NO["NO"]
    REASON["Store escalation reason"]
    URGENCY["Increase urgency up to 5"]
    SCORE["Recalculate priority"]
    CLEAR["Clear assignment / route"]
    QUEUE["Re-enter FIFO Queue"]

    COMPLETE --> WAIT --> USER
    USER -- "YES" --> YES --> RESOLVED --> CLOSED --> LIST --> AVL
    USER -- "NO" --> NO --> REASON --> URGENCY --> SCORE --> CLEAR --> QUEUE
~~~

The workflow therefore closes the loop with the reporting user instead of assuming deployment automatically solved the emergency.

---

# City Graph

Both portals use the **same graph, location IDs, and location names**.

~~~text
LOC-001 ----- LOC-002 ----- LOC-003 ----- LOC-004 ----- LOC-005
   |             |             |             |             |
LOC-006 ----- LOC-007 ----- LOC-008 ----- LOC-009 ----- LOC-010
   |             |             |             |             |
LOC-011 ----- LOC-012 ----- LOC-013 ----- LOC-014 ----- LOC-015
   |             |             |             |             |
LOC-016 ----- LOC-017 ----- LOC-018 ----- LOC-019 ----- LOC-020
~~~

Location categories include hospitals, fire stations, police stations, markets, schools, shelters, residential zones, City Hall, Bus Terminal, Industrial Zone, and Central Junction.

## Road attributes

| Property | Meaning |
|---|---|
| Road ID | <code>R-001 ... R-031</code> |
| From / To | Connected graph nodes |
| Distance | Physical distance |
| Travel time | Estimated minutes |
| Risk | Route-risk level |
| Congestion | Traffic intensity |
| Capacity | Road handling capacity |
| Blocked | Open / blocked operational state |

---

# DSA Architecture

~~~mermaid
flowchart TB
    SYSTEM["CrisisMeshSystem"]

    subgraph STRUCTURES["Manual Data Structures"]
        ARRAY["Array"]
        LL["Linked List"]
        STACK["Stack"]
        QUEUE["Queue"]
        AVL["AVL Tree"]
        MAXH["Max Heap"]
        MINH["Min Heap"]
        HASH["Hash Table"]
    end

    subgraph ALGORITHMS["Algorithms"]
        BFS["BFS"]
        DFS["DFS"]
        BS["Binary Search"]
        MS["Merge Sort"]
        DIJ["Dijkstra"]
    end

    subgraph USES["Operational Uses"]
        INTAKE["Incident Intake"]
        PRIORITY["Priority Scheduling"]
        ROAD["Road Undo"]
        HISTORY["History / Archive"]
        SEARCH["Incident / User / Location Lookup"]
        GRAPHAN["Graph Analysis"]
        RANK["Responder Ranking"]
        ROUTE["Shortest-Path Routing"]
    end

    SYSTEM --> STRUCTURES
    SYSTEM --> ALGORITHMS

    QUEUE --> INTAKE
    MAXH --> PRIORITY
    STACK --> ROAD
    LL --> HISTORY
    AVL --> HISTORY
    HASH --> SEARCH
    BS --> SEARCH
    BFS --> GRAPHAN
    DFS --> GRAPHAN
    MS --> RANK
    MINH --> DIJ
    DIJ --> ROUTE
    ARRAY --> SYSTEM
~~~

---

# DSA-to-Feature Mapping

| DSA / Algorithm | File | Operational use |
|---|---|---|
| Static / Dynamic Array | <code>include/dsa/Array.hpp</code> | Users, incidents, responders, shelters, resources, route state |
| Linked List | <code>include/dsa/LinkedList.hpp</code> | Messages and chronological closed history |
| Stack | <code>include/dsa/Stack.hpp</code> | Road-block undo and DFS |
| Queue | <code>include/dsa/Queue.hpp</code> | FIFO emergency intake and BFS |
| AVL Tree | <code>include/dsa/AVLTree.hpp</code> | Balanced archive of closed incidents |
| Max Heap | <code>include/dsa/MaxHeap.hpp</code> | Highest-priority processed incident |
| Min Heap | <code>include/dsa/MinHeap.hpp</code> | Dijkstra frontier |
| Hash Table | <code>include/dsa/HashTable.hpp</code> | Incident ID and username lookup |
| BFS | <code>include/algorithms/BFS.hpp</code> | Reachability from incident location |
| DFS | <code>include/algorithms/DFS.hpp</code> | Depth traversal of open roads |
| Binary Search | <code>include/algorithms/BinarySearch.hpp</code> + system lookup | Sorted location-ID search |
| Merge Sort | <code>include/algorithms/MergeSort.hpp</code> | Priority-view ordering and responder ranking |
| Dijkstra | <code>include/algorithms/Dijkstra.hpp</code> | Response comparison, assignment, rerouting, shelter routing |
| Graph | <code>include/graph/Graph.hpp</code> | Shared 20-node / 31-road city |

> Core assessed structures are implemented manually. STL containers are intentionally avoided for the assessed Array / Linked List / Stack / Queue / Tree / Heap / Hash structures. <code>std::vector</code> is used for graph adjacency representation.

---

# Authentication & Console Verification

Registration and Author login use a random **6-digit console verification code**.

~~~text
========== AUTHOR LOGIN VERIFICATION ==========
Verification Code: 483921
Enter verification code:
~~~

Password input is masked in the terminal.

### Author demo credentials

~~~text
Username: author
Password: Crisis@2026
~~~

> This is an academic console simulator. It does not connect to a real email/SMS identity provider.

---

# Persistent User Accounts

Registered User accounts are persisted to a local text file:

~~~text
data/users.txt
~~~

The file is created automatically on the first successful registration. It stores the next User ID and the currently registered accounts so a User can close the program, reopen it, and log in again with the same account.

~~~mermaid
flowchart LR
    START["Program Start"]
    LOAD["Load data/users.txt"]
    INDEX["Rebuild username Hash Table"]
    LOGIN["User Login"]

    REGISTER["Register User"]
    RESET["Reset Password"]
    DELETE["Delete Account"]
    SAVE["Rewrite data/users.txt"]

    START --> LOAD --> INDEX --> LOGIN
    REGISTER --> SAVE
    RESET --> SAVE
    DELETE --> SAVE
~~~

### Persistence rules

- Registration is saved immediately.
- Password reset is saved immediately.
- Account deletion removes the User from persistent storage.
- User IDs are monotonic; a deleted ID is **not reused**.
- Account deletion requires password confirmation.
- An account with an active emergency cannot be deleted, because the reporting User is still required for the resolution workflow.
- The runtime file and its temporary write file are excluded from Git through `.gitignore`.

> The text-file persistence layer is designed for this academic console project. It is not a production identity/database system, and the local account file should be treated as private data.

---

# Professional Emergency List

The User Portal presents reported emergencies in a structured table:

~~~text
Incident    Type        Location    Priority   Level        Status                     Responder
---------------------------------------------------------------------------------------------------------
INC-201     POLICE      LOC-014     115        HIGH         PRIORITIZED                -
INC-202     FIRE        LOC-018     128        CRITICAL     EN_ROUTE                   FIRE-UNIT-03
---------------------------------------------------------------------------------------------------------
Total Records: 2
~~~

The same table style is reused for Closed History, with clear priority level, lifecycle status, and assigned responder information.

---

# Responders, Shelters & Supplies

## Responder state

~~~text
Total Strength
Available Strength
On Operation = Total - Available
Operational Status
Current Location
Base Facility
~~~

## Shelter state

~~~text
Capacity
Current Occupancy
Available Capacity
Operational / Closed Status
Graph Location
~~~

The same incident cannot reserve shelter capacity twice.

## Supply catalog

~~~text
1. WATER
2. FOOD_PACK
3. MEDICAL_KIT
4. RESCUE_KIT
~~~

The Author can set, add, remove, relocate, and allocate stock. Completed/closed emergencies cannot receive new operational allocations.

---

# Message Flow

~~~mermaid
flowchart LR
    AUTHOR["Author"]
    DIRECT["Direct Message"]
    BROADCAST["Broadcast"]
    VALIDATE["Validate recipient / non-empty text"]
    LL["Manual Linked List"]
    USER["User Message Screen"]

    AUTHOR --> DIRECT --> VALIDATE --> LL
    AUTHOR --> BROADCAST --> VALIDATE --> LL
    LL --> USER
~~~

Direct messages require a valid registered User ID. Broadcasts are visible to all registered users.

---

# Build & Run

## Requirements

- C++17-compatible compiler
- GCC/MinGW, Clang, or MSVC
- CMake 3.16+ for the CMake workflow

## Windows PowerShell — Direct g++

~~~powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp -o crisismesh.exe
.\crisismesh.exe
~~~

## Linux / macOS — Direct g++

~~~bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/main.cpp -o crisismesh
./crisismesh
~~~

## CMake

~~~bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
~~~

Typical single-config run:

~~~bash
./build/crisismesh_console
~~~

On Windows with a multi-config generator, the executable may appear under <code>build/Debug/</code> or <code>build/Release/</code>.

---

# Automated Verification

~~~mermaid
flowchart LR
    PUSH["Push / Pull Request"]
    LINUX["Linux Build"]
    WIN["Windows MSVC Build"]
    LT["Linux CTest"]
    WT["Windows CTest"]
    PASS["Cross-platform validation"]

    PUSH --> LINUX --> LT --> PASS
    PUSH --> WIN --> WT --> PASS
~~~

Current automated coverage includes:

- Queue FIFO behavior;
- Stack LIFO behavior;
- Linked List operations;
- Hash Table lookup;
- AVL insertion/search;
- 20-node BFS and DFS reachability;
- weighted and shortest-distance Dijkstra reachability;
- concurrent Police officer deployment and restoration;
- Fire-unit double-assignment rejection;
- recall → restore → re-analysis;
- duplicate shelter-allocation rejection;
- direct-message validation;
- multiple road block;
- LIFO road undo.

---

# Recommended Demonstration Flow

~~~mermaid
flowchart TD
    UREG["1. Register User"]
    UREP["2. Report POLICE / FIRE / MEDICAL incident"]
    ALOGIN["3. Login as Author"]
    IC["4. Incident Center<br/>Process FIFO"]
    HEAP["5. Explain Priority + Max Heap"]
    IA["6. Incident Analysis"]
    BFSD["7. Run BFS / DFS"]
    DIJ["8. Run Dijkstra"]
    PATH["9. Explain distance table + shortest path"]
    ASSIGN["10. Manually assign responder"]
    RESOURCE["11. Show availability decrease"]
    ROAD["12. Optional road block + reroute + Stack undo"]
    COMPLETE["13. Mark response completed"]
    RESTORE["14. Show availability restored"]
    USER["15. User confirms YES / NO"]
    ARCHIVE["16. Show Linked List history + AVL archive"]

    UREG --> UREP --> ALOGIN --> IC --> HEAP --> IA --> BFSD --> DIJ
    DIJ --> PATH --> ASSIGN --> RESOURCE --> ROAD --> COMPLETE --> RESTORE --> USER --> ARCHIVE
~~~

This sequence demonstrates the major DSA requirements through one coherent operational story.

---

# Design Principles

The project intentionally emphasizes:

- **DSA-first architecture** — each assessed structure is tied to a real system behavior.
- **Manual implementation visibility** — core assessed structures are not hidden behind STL.
- **Decision support, not blind automation** — Dijkstra recommends; the Author assigns.
- **Consistent shared graph** — User and Author operate on the same city.
- **Explicit state transitions** — incident lifecycle changes are visible and explainable.
- **Resource accountability** — deployed officers/units reduce availability and later return.
- **Recoverability** — responses can be recalled, roads undone, unreachable routes recovered.
- **Compact demonstration scope** — 20 nodes balance realism with explainability.
- **Cross-platform quality** — Windows and Linux are continuously verified.

---

# Current Scope

Implemented:

- ✅ User registration/login/password reset
- ✅ Persistent User accounts in `data/users.txt`
- ✅ User account deletion with active-emergency protection
- ✅ Professional User emergency-list table
- ✅ Masked password input
- ✅ Console verification codes
- ✅ Emergency reporting
- ✅ FIFO incident intake
- ✅ Priority scoring
- ✅ Max-Heap scheduling
- ✅ Incident Analysis
- ✅ BFS / DFS
- ✅ Dijkstra shortest-distance comparison
- ✅ Merge-Sorted responder analysis
- ✅ Manual Police officer assignment
- ✅ Fire / Ambulance / Rescue deployment
- ✅ Resource availability tracking
- ✅ Dispatch recall / re-analysis
- ✅ Multiple road blocking
- ✅ Stack-based multiple undo
- ✅ Active-route rerouting
- ✅ Shelter allocation
- ✅ Supply management/allocation
- ✅ Direct/broadcast messaging
- ✅ User YES/NO confirmation
- ✅ Escalation and FIFO requeue
- ✅ Linked List history
- ✅ AVL archive
- ✅ Hash-based lookup
- ✅ Binary location search
- ✅ Cross-platform CI

---

# Architectural Boundaries

The console edition intentionally does **not** include:

- persistent incident/dispatch/message/road/resource databases;
- production-grade database-backed account storage;
- GPS or real map APIs;
- real police/fire/hospital integrations;
- real emergency phone routing;
- real email/SMS OTP delivery;
- production-grade identity management;
- machine-learning dispatch;
- global multi-vehicle optimization;
- automatic multi-station deployment to one incident.

Registered User accounts persist in `data/users.txt`. Operational simulation state—incidents, messages, road changes, deployments, shelter allocations, and resource changes—remains in memory and resets when the executable restarts.

These boundaries keep the project focused on **Data Structures, Algorithms, state modeling, and emergency decision logic**.

---

# Possible Future Extensions

Future versions could add:

- persistent incident/history/message storage in a database;
- multi-responder deployment for one incident;
- fastest / safest / balanced route modes;
- richer road-event simulation;
- incident timeline/event log;
- configurable city files;
- exportable incident reports;
- larger graph datasets;
- a separate graphical map interface.

---

# Educational Value

~~~text
Emergency Report
      ↓
Queue
      ↓
Priority Calculation
      ↓
Max Heap
      ↓
Graph Analysis
      ↓
BFS / DFS / Dijkstra
      ↓
Merge Sort + Search
      ↓
Manual Deployment
      ↓
Stack-based Road Recovery
      ↓
Linked List History
      ↓
AVL Archive
~~~

CrisisMesh connects:

**Data Structures → Graph Theory → Searching → Sorting → Priority Scheduling → Shortest Path → Resource State → Incident Lifecycle**

into a single explainable project instead of treating them as disconnected topics.

---

## Additional Documentation

- [Architecture Reference](docs/ARCHITECTURE.md)
- [DSA Mapping](docs/DSA_MAPPING.md)
- [Automated Workflow Tests](tests/dsa_smoke.cpp)

---

<div align="center">

### CrisisMesh 2.0 — DSA-Driven Emergency Decision & Routing Simulator

**C++17 • Manual Data Structures • Graph Algorithms • Incident Analysis • Resource Allocation**

</div>
