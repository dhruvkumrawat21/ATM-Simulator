#include "System.h"
#include "UI.h"
#include <iostream>
#include <limits>
#include <random>

System::System(const std::string& dbFilename) 
    : dbManager(dbFilename), loggedInUser(nullptr) {
    accounts = dbManager.loadAccounts();
}

System::~System() {
    dbManager.saveAccounts(accounts);
}

Account* System::findAccount(const std::string& accNum) {
    for (auto& acc : accounts) {
        if (acc.getAccountNumber() == accNum) {
            return &acc;
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
    
    std::string name, pin, pinConfirm;
    double initialDeposit;

    std::cout << UI::CYAN << " Enter Full Name: " << UI::RESET;
    std::getline(std::cin, name);

    // Generate a random 6-digit account number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(100000, 999999);
    std::string accNum = std::to_string(distr(gen));

    // Ensure it's unique
    while (findAccount(accNum) != nullptr) {
        accNum = std::to_string(distr(gen));
    }

    std::cout << UI::CYAN << " Enter 4-digit PIN: " << UI::RESET;
    std::getline(std::cin, pin);
    std::cout << UI::CYAN << " Confirm PIN: " << UI::RESET;
    std::getline(std::cin, pinConfirm);

    if (pin != pinConfirm) {
        UI::printError("PINs do not match. Account creation failed.");
        UI::pause();
        return;
    }

    if (pin.length() != 4) {
        UI::printError("PIN must be exactly 4 characters.");
        UI::pause();
        return;
    }

    std::cout << UI::CYAN << " Enter Initial Deposit Amount: " << UI::RESET;
    if (!(std::cin >> initialDeposit) || initialDeposit < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        UI::printError("Invalid amount.");
        UI::pause();
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    accounts.emplace_back(name, accNum, pin, initialDeposit);
    dbManager.saveAccounts(accounts); // Save immediately
    
    UI::printSuccess("Account created successfully!");
    UI::printMessage("Your Account Number is: " + UI::YELLOW + UI::BOLD + accNum, UI::CYAN);
    UI::pause();
}

void System::userLogin() {
    UI::printHeader("USER LOGIN");
    
    std::string accNum, pin;
    std::cout << UI::CYAN << " Enter Account Number: " << UI::RESET;
    std::getline(std::cin, accNum);
    
    Account* acc = findAccount(accNum);
    if (!acc) {
        UI::printError("Account not found.");
        UI::pause();
        return;
    }

    std::cout << UI::CYAN << " Enter PIN: " << UI::RESET;
    std::getline(std::cin, pin);

    if (acc->verifyPin(pin)) {
        loggedInUser = acc;
        userMenu();
    } else {
        UI::printError("Incorrect PIN.");
        UI::pause();
    }
}

void System::userMenu() {
    while (loggedInUser != nullptr) {
        UI::printHeader("USER MODE - Welcome, " + loggedInUser->getName());
        UI::printMenuOption(1, "Check Balance");
        UI::printMenuOption(2, "Deposit Money");
        UI::printMenuOption(3, "Withdraw Money");
        UI::printMenuOption(4, "Transfer Money");
        UI::printMenuOption(5, "Change PIN");
        UI::printMenuOption(6, "Logout");

        int choice = UI::getMenuChoice(6);

        switch (choice) {
            case 1: checkBalance(); break;
            case 2: depositMoney(); break;
            case 3: withdrawMoney(); break;
            case 4: transferMoney(); break;
            case 5: changePin(); break;
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
    std::cout << "\n" << UI::CYAN << " Current Balance: " << UI::GREEN << "$" << loggedInUser->getBalance() << "\n";
    UI::pause();
}

void System::depositMoney() {
    UI::printHeader("DEPOSIT MONEY");
    double amount;
    std::cout << UI::CYAN << " Enter amount to deposit: $" << UI::RESET;
    
    if (std::cin >> amount && amount > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        loggedInUser->deposit(amount);
        dbManager.saveAccounts(accounts);
        UI::printSuccess("Deposited $" + std::to_string(amount) + " successfully.");
    } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        UI::printError("Invalid deposit amount.");
    }
    UI::pause();
}

void System::withdrawMoney() {
    UI::printHeader("WITHDRAW MONEY");
    double amount;
    std::cout << UI::CYAN << " Enter amount to withdraw: $" << UI::RESET;
    
    if (std::cin >> amount && amount > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (loggedInUser->withdraw(amount)) {
            dbManager.saveAccounts(accounts);
            UI::printSuccess("Withdrew $" + std::to_string(amount) + " successfully.");
        } else {
            UI::printError("Insufficient balance.");
        }
    } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        UI::printError("Invalid withdrawal amount.");
    }
    UI::pause();
}

void System::transferMoney() {
    UI::printHeader("MONEY TRANSFER");
    
    std::string targetAccNum;
    std::cout << UI::CYAN << " Enter receiver's Account Number: " << UI::RESET;
    std::getline(std::cin, targetAccNum);

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
    std::cout << UI::CYAN << " Enter amount to transfer: $" << UI::RESET;
    
    if (std::cin >> amount && amount > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (loggedInUser->transfer(*receiver, amount)) {
            dbManager.saveAccounts(accounts);
            UI::printSuccess("Transferred $" + std::to_string(amount) + " to " + receiver->getName() + " successfully.");
        } else {
            UI::printError("Insufficient balance for transfer.");
        }
    } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        UI::printError("Invalid transfer amount.");
    }
    UI::pause();
}

void System::changePin() {
    UI::printHeader("PIN MANAGEMENT");
    
    std::string oldPin, newPin;
    std::cout << UI::CYAN << " Enter current PIN: " << UI::RESET;
    std::getline(std::cin, oldPin);

    if (!loggedInUser->verifyPin(oldPin)) {
        UI::printError("Incorrect current PIN.");
        UI::pause();
        return;
    }

    std::cout << UI::CYAN << " Enter new 4-digit PIN: " << UI::RESET;
    std::getline(std::cin, newPin);

    if (newPin.length() != 4) {
        UI::printError("PIN must be exactly 4 characters.");
        UI::pause();
        return;
    }

    if (loggedInUser->changePin(oldPin, newPin)) {
        dbManager.saveAccounts(accounts);
        UI::printSuccess("PIN changed successfully!");
    } else {
        UI::printError("Failed to change PIN.");
    }
    UI::pause();
}

void System::adminLogin() {
    UI::printHeader("ADMIN LOGIN");
    
    // Hardcoded Admin credentials for simulation
    const std::string ADMIN_ID = "admin";
    const std::string ADMIN_PASS = "admin123";

    std::string id, pass;
    std::cout << UI::CYAN << " Enter Admin ID: " << UI::RESET;
    std::getline(std::cin, id);
    std::cout << UI::CYAN << " Enter Admin Password: " << UI::RESET;
    std::getline(std::cin, pass);

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
        UI::printMenuOption(3, "Logout");

        int choice = UI::getMenuChoice(3);

        switch (choice) {
            case 1: {
                UI::printHeader("ALL USER ACCOUNTS");
                if (accounts.empty()) {
                    UI::printMessage("No accounts found in the system.", UI::YELLOW);
                } else {
                    for (const auto& acc : accounts) {
                        std::cout << UI::CYAN << " Acc No: " << UI::YELLOW << acc.getAccountNumber() 
                                  << UI::CYAN << " | Name: " << UI::WHITE << acc.getName() 
                                  << UI::CYAN << " | Balance: " << UI::GREEN << "$" << acc.getBalance() 
                                  << UI::RESET << "\n";
                    }
                }
                UI::pause();
                break;
            }
            case 2: {
                std::string csvFile = "data/exported_accounts.csv";
                if (dbManager.exportToCSV(accounts, csvFile)) {
                    UI::printSuccess("Exported all accounts to " + csvFile);
                } else {
                    UI::printError("Failed to export accounts.");
                }
                UI::pause();
                break;
            }
            case 3:
                UI::printSuccess("Admin logged out.");
                UI::pause();
                return;
        }
    }
}
