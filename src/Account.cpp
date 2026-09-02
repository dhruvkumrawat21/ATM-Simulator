#include "Account.h"

Account::Account(const std::string& accName, const std::string& accNum, const std::string& accPin, double initBalance)
    : Person(accName), accountNumber(accNum), pin(accPin), balance(initBalance) {
}

std::string Account::getAccountNumber() const {
    return accountNumber;
}

double Account::getBalance() const {
    return balance;
}

std::string Account::getPin() const {
    return pin;
}

bool Account::verifyPin(const std::string& enteredPin) const {
    return pin == enteredPin;
}

bool Account::changePin(const std::string& oldPin, const std::string& newPin) {
    if (verifyPin(oldPin)) {
        pin = newPin;
        return true;
    }
    return false;
}

void Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
    }
}

bool Account::withdraw(double amount) {
    if (amount > 0 && balance >= amount) {
        balance -= amount;
        return true;
    }
    return false;
}

bool Account::transfer(Account& receiver, double amount) {
    if (withdraw(amount)) {
        receiver.deposit(amount);
        return true;
    }
    return false;
}

std::string Account::toCSV() const {
    return accountNumber + "," + name + "," + std::to_string(balance);
}
