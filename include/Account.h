#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Person.h"
#include <string>

class Account : public Person {
private:
    std::string accountNumber;
    std::string pin;
    double balance;

public:
    Account(const std::string& accName, const std::string& accNum, const std::string& accPin, double initBalance);

    // Getters
    std::string getAccountNumber() const;
    double getBalance() const;
    std::string getPin() const; // Only used for saving/validating

    // Encapsulated operations
    bool verifyPin(const std::string& enteredPin) const;
    bool changePin(const std::string& oldPin, const std::string& newPin);
    void deposit(double amount);
    bool withdraw(double amount);
    bool transfer(Account& receiver, double amount);

    // For file persistence
    std::string toCSV() const;
};

#endif // ACCOUNT_H
