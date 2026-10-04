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
  |
  +-- Author Portal
        +-- operations dashboard
        +-- FIFO intake processing
        +-- priority scheduling
        +-- dispatch/routing/rerouting
        +-- road control
        +-- graph analysis
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