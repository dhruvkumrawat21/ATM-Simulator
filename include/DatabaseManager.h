#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "Account.h"
#include <vector>
#include <string>

class DatabaseManager {
private:
    std::string dbFile;
    char encryptionKey;

    // Helper for basic encryption/decryption (XOR)
    std::string encryptDecrypt(const std::string& input) const;

public:
    DatabaseManager(const std::string& filename, char key = 'K');

    // Load and Save all accounts
    std::vector<Account> loadAccounts();
    void saveAccounts(const std::vector<Account>& accounts);

    // Admin function
    bool exportToCSV(const std::vector<Account>& accounts, const std::string& csvFilename);
};

#endif // DATABASEMANAGER_H
