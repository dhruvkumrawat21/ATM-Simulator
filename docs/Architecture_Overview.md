# Advanced ATM Simulator - Architecture Overview

## Introduction
The ATM Simulator is a robust, object-oriented C++ application designed to simulate real-world banking operations. It bridges the gap between low-level terminal interfaces and high-level database management by using a local SQLite database and cryptographic security.

## Three-Tier Architecture
The project follows a simplified **Three-Tier Architecture** pattern to separate concerns, making the code highly modular and easy to maintain.

```mermaid
graph TD
    A[User / Admin] -->|Interacts| B[Terminal UI Module]
    B -->|Sends Input| C[System Module - Core Logic]
    
    C -->|Uses| D[Account Modules - Savings/Current]
    C -->|Hashes PINs| E[SHA-256 Security Module]
    
    C -->|Requests Data Save/Load| F[Database Manager]
    F -->|Executes SQL Queries| G[(SQLite Database - bank.db)]
    
    classDef ui fill:#3498db,stroke:#2980b9,color:white;
    classDef sys fill:#2ecc71,stroke:#27ae60,color:white;
    classDef sec fill:#e74c3c,stroke:#c0392b,color:white;
    classDef db fill:#f39c12,stroke:#d35400,color:white;
    
    class A,B ui;
    class C,D sys;
    class E sec;
    class F,G db;
```


### 1. Presentation Layer (UI)
- **File:** `UI.cpp`, `UI.h`
- **Purpose:** Handles all interactions with the user. It uses ANSI escape sequences and ASCII box drawing to present a beautiful, colorful, and error-proof command-line interface. It ensures that bad user input (like typing letters instead of numbers) is caught gracefully without crashing the system.

### 2. Business Logic Layer (System & Accounts)
- **Files:** `System.cpp`, `Account.cpp`, `sha256.cpp`
- **Purpose:** The core brain of the application. 
  - The `System` class manages the state of the application (who is logged in, navigating menus).
  - The `Account` hierarchy manages banking rules. Using **Polymorphism**, the system differentiates between `SavingsAccount` (which prevents going into negative balance) and `CurrentAccount` (which allows a $500 overdraft).
  - The `SHA256` module handles security. Before a PIN is sent to the database, it is irreversibly hashed.

### 3. Data Access Layer (DatabaseManager)
- **Files:** `DatabaseManager.cpp`, `sqlite3.c`
- **Purpose:** Completely isolates the database operations from the business logic. The `DatabaseManager` translates C++ objects into SQL queries (`INSERT`, `SELECT`, `UPDATE`) and communicates with the bundled SQLite C-engine. This layer ensures that data persistence is handled securely and efficiently in `bank.db`.

## Key Architectural Decisions
1. **SQLite Amalgamation:** Instead of requiring a separate MySQL server, the SQLite engine is compiled *directly* into the executable. This makes the software highly portable and "plug-and-play".
2. **Abstract Base Classes:** The `Account` class is abstract. This enforces the rule that a user cannot have a generic account; it must be a specific type (Savings or Current).
3. **Transaction Logging:** Instead of just updating a balance, every financial movement generates a unique transaction record in a separate SQL table, mimicking a real bank's ledger.
