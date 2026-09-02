#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "Account.h"
#include <vector>
#include <string>

// Forward declaration for SQLite
struct sqlite3;

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbFile;

    // Helper to execute simple SQL commands
    bool executeQuery(const std::string& sql);

public:
    DatabaseManager(const std::string& filename);
    ~DatabaseManager();

    // Account CRUD operations
    std::vector<Account*> loadAccounts();
    bool saveAccount(Account* acc);
    bool updateAccount(Account* acc);
    
    // Transaction History
    bool logTransaction(const std::string& accNum, const std::string& type, double amount);
    std::vector<std::string> getMiniStatement(const std::string& accNum);

    // Admin function
    bool exportToCSV(const std::string& csvFilename);
};

#endif // DATABASEMANAGER_H
