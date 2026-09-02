# ATM Simulator - Master Visual Guide

This document contains visual diagrams mapping out the entire architecture, class relationships, logic flows, and database structure of the ATM Simulator. You can use these diagrams to easily explain how the system works to your professor, interviewer, or teammates.

*(Note: These diagrams use Mermaid.js syntax. GitHub will automatically render them into beautiful images when viewed in your repository.)*

---

## 1. High-Level Architecture Diagram
This shows how the different modules of the system connect. The User interacts with the UI, which talks to the System logic, which securely hashes data and talks to the Database Manager to save it.

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

---

## 2. Complete Class Diagram (Object-Oriented Design)
This diagram proves your knowledge of OOP. It shows Inheritance (arrows with hollow triangles) and Abstraction (italicized abstract classes).

```mermaid
classDiagram
    %% Inheritance Structure
    class Person {
        #string name
        +getName()
    }

    class Account {
        <<Abstract Base Class>>
        #string accountNumber
        #string pinHash
        #double balance
        #bool isLocked
        #int failedAttempts
        +withdraw(double amount)*
        +deposit(double amount)
        +getAccountType()*
    }

    class SavingsAccount {
        +withdraw(double amount)
        +getAccountType()
    }

    class CurrentAccount {
        -double overdraftLimit
        +withdraw(double amount)
        +getAccountType()
    }

    Person <|-- Account : Inherits
    Account <|-- SavingsAccount : Inherits
    Account <|-- CurrentAccount : Inherits

    %% System Composition
    class System {
        -DatabaseManager dbManager
        -Account* loggedInUser
        +run()
        -userLogin()
        -transferMoney()
    }
    
    class DatabaseManager {
        -sqlite3* db
        +saveAccount()
        +logTransaction()
    }

    System "1" *-- "1" DatabaseManager : Contains
    System "1" o-- "Many" Account : Manages
```

---

## 3. User Login & Security Flow (Sequence Diagram)
This step-by-step sequence diagram explains exactly what happens in the background when a user tries to log in. It highlights the 3-strike locking system and the SHA-256 hashing.

```mermaid
sequenceDiagram
    actor User
    participant UI
    participant System
    participant SHA256
    participant Database

    User->>UI: Enters Account Number & PIN
    UI->>System: Sends Login Request
    System->>Database: Fetch Account Data
    Database-->>System: Returns Account Object
    
    alt Account is Locked
        System-->>UI: Show Error (Account Locked)
    else Account is Active
        System->>SHA256: Hash entered PIN
        SHA256-->>System: Returns 64-char Hash
        
        alt Hashes Match
            System->>Database: Reset failed_attempts = 0
            System-->>UI: Show User Menu (Login Success)
        else Hashes Do Not Match
            System->>Database: Increment failed_attempts (+1)
            alt failed_attempts == 3
                System->>Database: Set isLocked = TRUE
                System-->>UI: Show Error (Account Locked!)
            else
                System-->>UI: Show Error (Incorrect PIN)
            end
        end
    end
```

---

## 4. Money Transfer Flow
This diagram explains how the system ensures money is transferred safely between two accounts, updating the balances and logging the transaction history.

```mermaid
sequenceDiagram
    actor User
    participant System
    participant Sender_Account
    participant Receiver_Account
    participant Database

    User->>System: Transfer $100 to Acc 123456
    System->>Database: Fetch Receiver Account
    Database-->>System: Receiver Found
    
    System->>Sender_Account: withdraw($100)
    alt Insufficient Balance
        Sender_Account-->>System: Returns False
        System-->>User: Show Error (Insufficient Funds)
    else Sufficient Balance
        Sender_Account-->>System: Returns True (Balance Updated)
        System->>Receiver_Account: deposit($100)
        
        %% Database Updates
        System->>Database: UPDATE Sender Balance
        System->>Database: UPDATE Receiver Balance
        System->>Database: INSERT Transaction Log (Sender - Transfer Out)
        System->>Database: INSERT Transaction Log (Receiver - Transfer In)
        
        System-->>User: Show Success Message
    end
```

---

## 5. Database Schema (Entity-Relationship Diagram)
This shows the structure of your SQLite database tables.

```mermaid
erDiagram
    ACCOUNTS ||--o{ TRANSACTIONS : "has many"
    
    ACCOUNTS {
        string account_number PK
        string name
        string pin_hash "SHA-256 string"
        double balance
        string account_type "SAVINGS or CURRENT"
        int is_locked "0 or 1"
        int failed_attempts
    }
    
    TRANSACTIONS {
        int id PK
        string account_number FK "Links to Accounts"
        string type "DEPOSIT, WITHDRAW, etc."
        double amount
        string timestamp
    }
```
