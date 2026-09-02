# System Flow and Data Journey

This document explains exactly how data moves through the application when a user performs an action.

## Flow 1: Creating an Account

1. **User Input**: The user selects "Create Account" in the UI and types their name, PIN, and initial deposit. They also choose "Savings" or "Current".
2. **Security**: The `System` passes the plain-text PIN to the `SHA256` class. `SHA256` returns a 64-character encrypted hash.
3. **Object Creation**: The `System` creates a new C++ object (either `SavingsAccount` or `CurrentAccount`) in memory.
4. **Database Storage**: The `System` passes this object to `DatabaseManager::saveAccount()`.
5. **SQL Execution**: The `DatabaseManager` constructs an `INSERT INTO Accounts` SQL query and executes it against `bank.db`.
6. **Transaction Log**: An "INITIAL DEPOSIT" transaction is logged in the `Transactions` table.

---

## Flow 2: User Login and Locking Mechanism

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

1. **Authentication**: User enters their Account Number and PIN.
2. **Verification**: 
   - The system fetches the account from memory.
   - It hashes the entered PIN using `SHA256`.
   - It compares the *new* hash with the *stored* hash.
3. **Success**: If they match, `failed_attempts` resets to 0. `loggedInUser` pointer is set, and the User Menu opens.
4. **Failure & Locking**: If they do not match, `failed_attempts` increments. If `failed_attempts` reaches 3, `isLocked` becomes `true`. An `UPDATE` SQL query is immediately fired to save this locked status to the database. The user is blocked from logging in until an Admin unlocks it.

## Flow 3: Money Transfer (ACID principles simulated)

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

1. **Validation**: User enters a receiver's account number and an amount. The system checks if the receiver exists and if the sender has enough funds (triggering the polymorphic `withdraw()` check).
2. **Execution**:
   - Sender's balance decreases.
   - Receiver's balance increases.
3. **Persistence**: `DatabaseManager::updateAccount()` is called for *both* users.
4. **Logging**: Two transaction logs are created: one for "TRANSFER OUT" (sender) and one for "TRANSFER IN" (receiver).

## Admin Operations Flow

- Admin logs in using hardcoded secure credentials (`admin` / `admin123`).
- **View Accounts**: Queries the `Accounts` table and formats a beautiful console table.
- **Export to CSV**: Queries the `Accounts` table and writes the results to a `.csv` file using standard C++ `<fstream>`, bridging SQL data to standard spreadsheet formats.
- **Unlock Account**: Sets an account's `isLocked` flag to `false` and `failed_attempts` to 0, updating the SQL database immediately.
