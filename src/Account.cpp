#include "Account.h"
#include <iostream>

using namespace std; // Using standard namespace as requested to make code easier to read

// ==========================================
// Base Account Implementation
// ==========================================
Account::Account(const string& accName, const string& accNum, const string& pHash, double initBalance, bool locked, int failed)
    : Person(accName), accountNumber(accNum), pinHash(pHash), balance(initBalance), isLocked(locked), failedAttempts(failed) {
}

string Account::getAccountNumber() const {
    return accountNumber;
}

double Account::getBalance() const {
    return balance;
}

string Account::getPinHash() const {
    return pinHash;
}

bool Account::getIsLocked() const {
    return isLocked;
}

int Account::getFailedAttempts() const {
    return failedAttempts;
}

void Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
    }
}

void Account::setLocked(bool status) {
    isLocked = status;
}

void Account::setFailedAttempts(int attempts) {
    failedAttempts = attempts;
}

// ==========================================
// Savings Account Implementation
// ==========================================
SavingsAccount::SavingsAccount(const string& accName, const string& accNum, const string& pHash, double initBalance, bool locked, int failed)
    : Account(accName, accNum, pHash, initBalance, locked, failed) {}

bool SavingsAccount::withdraw(double amount) {
    // Savings accounts enforce strict balance limits
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        return true;
    }
    return false; // Insufficient funds
}

// ==========================================
// Current Account Implementation
// ==========================================
CurrentAccount::CurrentAccount(const string& accName, const string& accNum, const string& pHash, double initBalance, bool locked, int failed)
    : Account(accName, accNum, pHash, initBalance, locked, failed) {}

bool CurrentAccount::withdraw(double amount) {
    // Current accounts allow overdraft up to the limit
    if (amount > 0 && (balance + overdraftLimit) >= amount) {
        balance -= amount;
        return true;
    }
    return false; // Exceeded overdraft limit
}
