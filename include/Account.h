#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Person.h"
#include <string>

// Virtual Base Class for Polymorphism
// This demonstrates Abstraction (hiding implementation) and Inheritance.
class Account : public Person {
protected:
    std::string accountNumber;
    std::string pinHash; // We store the SHA-256 hash, not the plaintext PIN
    double balance;
    bool isLocked;
    int failedAttempts;

public:
    Account(const std::string& accName, const std::string& accNum, const std::string& pHash, double initBalance, bool locked = false, int failed = 0);
    virtual ~Account() = default;

    // Getters
    std::string getAccountNumber() const;
    double getBalance() const;
    std::string getPinHash() const;
    bool getIsLocked() const;
    int getFailedAttempts() const;

    // Pure virtual function (makes Account an Abstract Class)
    // Every derived account type MUST implement how it withdraws money
    virtual bool withdraw(double amount) = 0;
    
    // Abstract function to get the account type as a string
    virtual std::string getAccountType() const = 0;

    // Common operations
    void deposit(double amount);
    void setLocked(bool status);
    void setFailedAttempts(int attempts);
};

// ==========================================
// Savings Account - Inherits from Account
// ==========================================
class SavingsAccount : public Account {
public:
    SavingsAccount(const std::string& accName, const std::string& accNum, const std::string& pHash, double initBalance, bool locked = false, int failed = 0);
    
    // Savings accounts cannot withdraw more than their balance
    bool withdraw(double amount) override;
    
    std::string getAccountType() const override { return "SAVINGS"; }
};

// ==========================================
// Current Account - Inherits from Account
// ==========================================
class CurrentAccount : public Account {
private:
    double overdraftLimit = 500.0; // Current accounts can go negative up to $500
public:
    CurrentAccount(const std::string& accName, const std::string& accNum, const std::string& pHash, double initBalance, bool locked = false, int failed = 0);
    
    // Current accounts can withdraw beyond balance (up to overdraft limit)
    bool withdraw(double amount) override;

    std::string getAccountType() const override { return "CURRENT"; }
};

#endif // ACCOUNT_H
