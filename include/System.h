#ifndef SYSTEM_H
#define SYSTEM_H

#include "DatabaseManager.h"
#include <vector>

class System {
private:
    std::vector<Account*> accounts;
    DatabaseManager dbManager;
    Account* loggedInUser;

    // Helper functions
    void userMenu();
    void adminMenu();
    
    // Core features
    void createAccount();
    void userLogin();
    void adminLogin();
    
    void depositMoney();
    void withdrawMoney();
    void checkBalance();
    void transferMoney();
    void changePin();
    void viewMiniStatement();
    
    // Admin specific
    void unlockAccounts();

    // Search helper
    Account* findAccount(const std::string& accNum);

public:
    System(const std::string& dbFilename);
    ~System();
    
    // Main entry point
    void run();
};

#endif // SYSTEM_H
