#include "System.h"
#include "UI.h"
#include "sha256.h"
#include <iostream>
#include <limits>
#include <random>
#include <iomanip>

using namespace std;

System::System(const string& dbFilename) 
    : dbManager(dbFilename), loggedInUser(nullptr) {
    accounts = dbManager.loadAccounts();
}

System::~System() {
    for (Account* acc : accounts) {
        delete acc;
    }
}

Account* System::findAccount(const string& accNum) {
    for (auto acc : accounts) {
        if (acc->getAccountNumber() == accNum) {
            return acc;
        }
    }
    return nullptr;
}

void System::run() {
    while (true) {
        UI::printHeader("WELCOME TO PREMIUM BANK ATM");
        UI::printMenuOption(1, "User Login");
        UI::printMenuOption(2, "Create New Account");
        UI::printMenuOption(3, "Admin Login");
        UI::printMenuOption(4, "Exit");

        int choice = UI::getMenuChoice(4);

        switch (choice) {
            case 1: userLogin(); break;
            case 2: createAccount(); break;
            case 3: adminLogin(); break;
            case 4: 
                UI::printMessage("\nThank you for using Premium Bank. Goodbye!", UI::GREEN);
                return;
        }
    }
}

void System::createAccount() {
    UI::printHeader("CREATE NEW ACCOUNT");
    
    string name, pin, pinConfirm;
    double initialDeposit;

    cout << UI::CYAN << " Enter Full Name: " << UI::RESET;
    getline(cin, name);

    // Ask for Account Type (Demonstrating Polymorphism)
    UI::printMessage("\n Select Account Type:", UI::YELLOW);
    UI::printMenuOption(1, "Savings Account (Cannot overdraft)");
    UI::printMenuOption(2, "Current Account (Allows $500 overdraft)");
    int typeChoice = UI::getMenuChoice(2);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(100000, 999999);
    string accNum = to_string(distr(gen));

    while (findAccount(accNum) != nullptr) {
        accNum = to_string(distr(gen));
    }

    cout << UI::CYAN << " Enter 4-digit PIN: " << UI::RESET;
    getline(cin, pin);
    cout << UI::CYAN << " Confirm PIN: " << UI::RESET;
    getline(cin, pinConfirm);

    if (pin != pinConfirm || pin.length() != 4) {
        UI::printError("Invalid PIN setup. Must be 4 digits and match.");
        UI::pause();
        return;
    }

    // Securely hash the PIN using SHA-256
    string pinHash = SHA256::hashString(pin);

    cout << UI::CYAN << " Enter Initial Deposit Amount: " << UI::RESET;
    if (!(cin >> initialDeposit) || initialDeposit < 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        UI::printError("Invalid amount.");
        UI::pause();
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Account* newAcc = nullptr;
    if (typeChoice == 1) {
        newAcc = new SavingsAccount(name, accNum, pinHash, initialDeposit);
    } else {
        newAcc = new CurrentAccount(name, accNum, pinHash, initialDeposit);
    }

    accounts.push_back(newAcc);
    dbManager.saveAccount(newAcc);
    dbManager.logTransaction(accNum, "INITIAL DEPOSIT", initialDeposit);
    
    UI::printSuccess(newAcc->getAccountType() + " Account created successfully!");
    UI::printMessage("Your Account Number is: " + UI::YELLOW + UI::BOLD + accNum, UI::CYAN);
    UI::pause();
}

void System::userLogin() {
    UI::printHeader("USER LOGIN");
    
    string accNum, pin;
    cout << UI::CYAN << " Enter Account Number: " << UI::RESET;
    getline(cin, accNum);
    
    Account* acc = findAccount(accNum);
    if (!acc) {
        UI::printError("Account not found.");
        UI::pause();
        return;
    }

    if (acc->getIsLocked()) {
        UI::printError("Account is LOCKED due to too many failed attempts. Contact Admin.");
        UI::pause();
        return;
    }

    cout << UI::CYAN << " Enter PIN: " << UI::RESET;
    getline(cin, pin);

    // Check hashed PIN
    if (acc->getPinHash() == SHA256::hashString(pin)) {
        acc->setFailedAttempts(0);
        dbManager.updateAccount(acc);
        loggedInUser = acc;
        userMenu();
    } else {
        int attempts = acc->getFailedAttempts() + 1;
        acc->setFailedAttempts(attempts);
        if (attempts >= 3) {
            acc->setLocked(true);
            UI::printError("INCORRECT PIN 3 TIMES. ACCOUNT LOCKED.");
        } else {
            UI::printError("Incorrect PIN. Attempt " + to_string(attempts) + " of 3.");
        }
        dbManager.updateAccount(acc);
        UI::pause();
    }
}

void System::userMenu() {
    while (loggedInUser != nullptr) {
        UI::printHeader("USER MODE - Welcome, " + loggedInUser->getName());
        UI::printMenuOption(1, "Check Balance (Multi-Currency)");
        UI::printMenuOption(2, "Deposit Money");
        UI::printMenuOption(3, "Withdraw Money");
        UI::printMenuOption(4, "Transfer Money");
        UI::printMenuOption(5, "Mini Statement");
        UI::printMenuOption(6, "Logout");

        int choice = UI::getMenuChoice(6);

        switch (choice) {
            case 1: checkBalance(); break;
            case 2: depositMoney(); break;
            case 3: withdrawMoney(); break;
            case 4: transferMoney(); break;
            case 5: viewMiniStatement(); break;
            case 6: 
                loggedInUser = nullptr;
                UI::printSuccess("Logged out successfully.");
                UI::pause();
                break;
        }
    }
}

void System::checkBalance() {
    UI::printHeader("BALANCE INQUIRY");
    double bal = loggedInUser->getBalance();
    
    // Multi-currency display
    cout << "\n" << UI::CYAN << " Base Currency (USD): " << UI::GREEN << "$" << fixed << setprecision(2) << bal << "\n";
    cout << UI::CYAN << " Euro (EUR): " << UI::YELLOW << "€" << fixed << setprecision(2) << (bal * 0.92) << "\n";
    cout << UI::CYAN << " Indian Rupee (INR): " << UI::MAGENTA << "₹" << fixed << setprecision(2) << (bal * 83.15) << "\n";
    
    UI::pause();
}

void System::depositMoney() {
    UI::printHeader("DEPOSIT MONEY");
    double amount;
    cout << UI::CYAN << " Enter amount to deposit: $" << UI::RESET;
    
    if (cin >> amount && amount > 0) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        loggedInUser->deposit(amount);
        dbManager.updateAccount(loggedInUser);
        dbManager.logTransaction(loggedInUser->getAccountNumber(), "DEPOSIT", amount);
        UI::printSuccess("Deposited $" + to_string(amount) + " successfully.");
    } else {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        UI::printError("Invalid deposit amount.");
    }
    UI::pause();
}

void System::withdrawMoney() {
    UI::printHeader("WITHDRAW MONEY");
    double amount;
    cout << UI::CYAN << " Enter amount to withdraw: $" << UI::RESET;
    
    if (cin >> amount && amount > 0) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Polymorphic call: Works differently for Savings vs Current
        if (loggedInUser->withdraw(amount)) {
            dbManager.updateAccount(loggedInUser);
            dbManager.logTransaction(loggedInUser->getAccountNumber(), "WITHDRAWAL", amount);
            UI::printSuccess("Withdrew $" + to_string(amount) + " successfully.");
        } else {
            UI::printError("Insufficient balance or over draft limit.");
        }
    } else {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        UI::printError("Invalid withdrawal amount.");
    }
    UI::pause();
}

void System::transferMoney() {
    UI::printHeader("MONEY TRANSFER");
    
    string targetAccNum;
    cout << UI::CYAN << " Enter receiver's Account Number: " << UI::RESET;
    getline(cin, targetAccNum);

    if (targetAccNum == loggedInUser->getAccountNumber()) {
        UI::printError("Cannot transfer to your own account.");
        UI::pause();
        return;
    }

    Account* receiver = findAccount(targetAccNum);
    if (!receiver) {
        UI::printError("Receiver account not found.");
        UI::pause();
        return;
    }

    double amount;
    cout << UI::CYAN << " Enter amount to transfer: $" << UI::RESET;
    
    if (cin >> amount && amount > 0) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Polymorphic call for withdrawal
        if (loggedInUser->withdraw(amount)) {
            receiver->deposit(amount);
            
            dbManager.updateAccount(loggedInUser);
            dbManager.updateAccount(receiver);
            
            dbManager.logTransaction(loggedInUser->getAccountNumber(), "TRANSFER OUT", amount);
            dbManager.logTransaction(receiver->getAccountNumber(), "TRANSFER IN", amount);

            UI::printSuccess("Transferred $" + to_string(amount) + " to " + receiver->getName() + " successfully.");
        } else {
            UI::printError("Insufficient balance for transfer.");
        }
    } else {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        UI::printError("Invalid transfer amount.");
    }
    UI::pause();
}

void System::viewMiniStatement() {
    UI::printHeader("MINI STATEMENT");
    vector<string> statement = dbManager.getMiniStatement(loggedInUser->getAccountNumber());
    
    if (statement.empty()) {
        UI::printMessage("No transactions found.", UI::YELLOW);
    } else {
        for (const string& record : statement) {
            cout << UI::WHITE << " " << record << "\n";
        }
    }
    UI::pause();
}

void System::adminLogin() {
    UI::printHeader("ADMIN LOGIN");
    
    const string ADMIN_ID = "admin";
    const string ADMIN_PASS = "admin123";

    string id, pass;
    cout << UI::CYAN << " Enter Admin ID: " << UI::RESET;
    getline(cin, id);
    cout << UI::CYAN << " Enter Admin Password: " << UI::RESET;
    getline(cin, pass);

    if (id == ADMIN_ID && pass == ADMIN_PASS) {
        adminMenu();
    } else {
        UI::printError("Invalid Admin credentials.");
        UI::pause();
    }
}

void System::adminMenu() {
    while (true) {
        UI::printHeader("ADMIN MODE");
        UI::printMenuOption(1, "View All Accounts");
        UI::printMenuOption(2, "Export Accounts to CSV");
        UI::printMenuOption(3, "Unlock a Locked Account");
        UI::printMenuOption(4, "Logout");

        int choice = UI::getMenuChoice(4);

        switch (choice) {
            case 1: {
                UI::printHeader("ALL USER ACCOUNTS");
                if (accounts.empty()) {
                    UI::printMessage("No accounts found in the system.", UI::YELLOW);
                } else {
                    for (Account* acc : accounts) {
                        cout << UI::CYAN << " Acc No: " << UI::YELLOW << acc->getAccountNumber() 
                                  << UI::CYAN << " | Name: " << UI::WHITE << acc->getName() 
                                  << UI::CYAN << " | Type: " << UI::MAGENTA << acc->getAccountType()
                                  << UI::CYAN << " | Bal: " << UI::GREEN << "$" << fixed << setprecision(2) << acc->getBalance() 
                                  << UI::CYAN << " | Locked: " << (acc->getIsLocked() ? (UI::RED + "YES") : (UI::GREEN + "NO"))
                                  << UI::RESET << "\n";
                    }
                }
                UI::pause();
                break;
            }
            case 2: {
                string csvFile = "data/exported_accounts.csv";
                if (dbManager.exportToCSV(csvFile)) {
                    UI::printSuccess("Exported all accounts to " + csvFile);
                } else {
                    UI::printError("Failed to export accounts.");
                }
                UI::pause();
                break;
            }
            case 3:
                unlockAccounts();
                break;
            case 4:
                UI::printSuccess("Admin logged out.");
                UI::pause();
                return;
        }
    }
}

void System::unlockAccounts() {
    UI::printHeader("UNLOCK ACCOUNT");
    string accNum;
    cout << UI::CYAN << " Enter Account Number to unlock: " << UI::RESET;
    getline(cin, accNum);
    
    Account* acc = findAccount(accNum);
    if (!acc) {
        UI::printError("Account not found.");
    } else if (!acc->getIsLocked()) {
        UI::printMessage("Account is already unlocked.", UI::YELLOW);
    } else {
        acc->setLocked(false);
        acc->setFailedAttempts(0);
        dbManager.updateAccount(acc);
        UI::printSuccess("Account " + accNum + " has been UNLOCKED successfully.");
    }
    UI::pause();
}
