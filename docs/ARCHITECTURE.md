# Architecture

```text
main.cpp
  |
  +-- User Portal
  |     +-- registration/login/reset + simulated OTP
  |     +-- report incident
  |     +-- track status
  |     +-- YES/NO resolution confirmation
  |     +-- messages/history/profile
  |     +-- persistent account storage / delete account
  |     +-- emergency contacts
  |
  +-- Author Portal
        +-- operations dashboard
        +-- FIFO intake processing
        +-- priority scheduling / Max Heap
        +-- Incident Analysis
        |     +-- type-specific response category
        |     +-- compatible responder availability
        |     +-- BFS / DFS reachability
        |     +-- Dijkstra responder comparison
        |     +-- shortest-route recommendation
        |     +-- manual responder/strength assignment
        +-- Dispatch Center / response completion
        +-- road control / rerouting
        +-- responder/shelter/supply allocation
        +-- user messaging
        +-- archive/history

UserStorage
  +-- data/users.txt
  +-- load on startup
  +-- save on register/reset/delete

CrisisMeshSystem
  |
  +-- Manual DSA layer
  |     Array / Linked List / Stack / Queue / AVL
  |     Max Heap / Min Heap / Hash Table
  |
  +-- Algorithms
  |     BFS / DFS / Binary Search / Merge Sort / Dijkstra
  |
  +-- Graph
        20 nodes / 31 roads
```

Registered User accounts are persisted in `data/users.txt`. Operational state remains in-memory: restarting resets incidents, road state, messages, deployments, shelters and resource changes.

## Resource lifecycle

A responder stores total strength and available strength; on-operation strength is calculated as total minus available. Police strength is measured in officers, Fire/Ambulance in units, and Rescue in teams. Manual assignment reduces availability. Response completion restores the assigned strength.
