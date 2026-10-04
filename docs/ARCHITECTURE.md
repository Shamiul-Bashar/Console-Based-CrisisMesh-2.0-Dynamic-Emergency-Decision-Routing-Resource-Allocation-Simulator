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

The program is deliberately in-memory and deterministic except for generated OTP values. Restarting the program resets users, incidents, road state, messages and operational state.

## Resource lifecycle

A responder stores total strength and available strength; on-operation strength is calculated as total minus available. Police strength is measured in officers, Fire/Ambulance in units, and Rescue in teams. Manual assignment reduces availability. Response completion restores the assigned strength.
