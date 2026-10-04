# Architecture

```text
main.cpp
  |
  +-- User Portal
  |     +-- registration/login/reset + simulated OTP
  |     +-- report incident
  |     +-- track status
  |     +-- YES/NO resolution confirmation
  |     +-- messages/profile
  |     +-- My History & Activity Log
  |     +-- persistent account storage / delete account
  |     +-- professional emergency contact directory
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
        +-- individual persistent User history
        +-- archive/history

Persistent Storage
  |
  +-- UserStorage
  |     +-- data/users.txt
  |     +-- account ID + credentials/profile data
  |     +-- save on register/reset/delete
  |
  +-- IncidentStorage
  |     +-- data/incidents.txt
  |     +-- incident lifecycle/status
  |     +-- responder assignment + route snapshot
  |     +-- shelter/supply allocation
  |
  +-- UserHistoryStorage
        +-- data/user_history.txt
        +-- timestamp + User ID + actor + action
        +-- incident reference + human-readable detail

Program Startup
  |
  +-- seed city graph / responders / resources
  +-- load users.txt
  +-- load incidents.txt
  +-- load user_history.txt
  +-- rebuild username Hash Table
  +-- rebuild incident Hash Table
  +-- rebuild pending FIFO Queue
  +-- rebuild prioritized Max Heap
  +-- rebuild closed Linked List + AVL archive
  +-- restore active responder strength usage

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

## Persistence model

Registered User accounts are persisted in `data/users.txt`. Incident snapshots are
persisted in `data/incidents.txt`, and chronological per-user audit events are
persisted in `data/user_history.txt`.

The incident loader reconstructs the runtime data structures required by the DSA
workflow. A queued incident returns to the FIFO Queue, a prioritized incident
returns to the Max Heap, closed records rebuild the Linked List and AVL archive,
and an active dispatch restores responder usage. This keeps persistence separate
from the assessed data-structure implementations rather than replacing them with
a database or STL container.

Messages and temporary road/resource administration changes remain session
runtime state. Incident-linked route, shelter, supply, assignment, and lifecycle
information is persistent.

## Account deletion model

A User cannot delete an account while an emergency is active. When deletion is
allowed, login/account credentials are removed from `users.txt`; incident and
audit history remain available for operational integrity. Deleted User IDs are
not reused.

## Resource lifecycle

A responder stores total strength and available strength; on-operation strength
is calculated as total minus available. Police strength is measured in officers,
Fire/Ambulance in units, and Rescue in teams. Manual assignment reduces
availability. Response completion restores the assigned strength.

## Dispatch confirmation lifecycle

```text
AUTHOR: Mark Response Completed
          |
          v
Responder strength restored
          |
          v
AWAITING_USER_CONFIRMATION
          |
          +---- User YES ----> RESOLVED -> CLOSED -> persistent history
          |
          +---- User NO -----> urgency update -> FIFO Queue -> re-analysis
```

The Dispatch Center does not accept YES/NO itself. The reporting User confirms
from the User Portal, which removes the earlier ambiguous console interaction.
