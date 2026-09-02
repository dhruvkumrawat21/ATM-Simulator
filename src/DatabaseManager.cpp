#include "DatabaseManager.h"
#include <fstream>
#include <sstream>

DatabaseManager::DatabaseManager(const std::string& filename, char key) 
    : dbFile(filename), encryptionKey(key) {}

std::string DatabaseManager::encryptDecrypt(const std::string& input) const {
    std::string output = input;
    for (size_t i = 0; i < input.length(); ++i) {
        output[i] = input[i] ^ encryptionKey;
    }
    return output;
}

std::vector<Account> DatabaseManager::loadAccounts() {
    std::vector<Account> accounts;
    std::ifstream file(dbFile);

    if (!file.is_open()) {
        return accounts; // Return empty list if file doesn't exist yet
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Decrypt line first
        std::string decryptedLine = encryptDecrypt(line);
        std::stringstream ss(decryptedLine);
        
        std::string name, accNum, pin, balanceStr;
        std::getline(ss, name, '|');
        std::getline(ss, accNum, '|');
        std::getline(ss, pin, '|');
        std::getline(ss, balanceStr, '|');

        if (!name.empty() && !accNum.empty() && !pin.empty() && !balanceStr.empty()) {
            double balance = std::stod(balanceStr);
            accounts.emplace_back(name, accNum, pin, balance);
        }
    }
    file.close();
    return accounts;
}

void DatabaseManager::saveAccounts(const std::vector<Account>& accounts) {
    std::ofstream file(dbFile, std::ios::trunc); // Overwrite completely
    if (!file.is_open()) return;

    for (const auto& acc : accounts) {
        std::string line = acc.getName() + "|" + 
                           acc.getAccountNumber() + "|" + 
                           acc.getPin() + "|" + 
                           std::to_string(acc.getBalance());
        
        // Encrypt line before saving
        std::string encryptedLine = encryptDecrypt(line);
        file << encryptedLine << "\n";
    }
    file.close();
}

bool DatabaseManager::exportToCSV(const std::vector<Account>& accounts, const std::string& csvFilename) {
    std::ofstream file(csvFilename, std::ios::trunc);
    if (!file.is_open()) return false;

    // Write CSV header
    file << "Account Number,Name,Balance\n";

    for (const auto& acc : accounts) {
        file << acc.toCSV() << "\n";
    }
    
    file.close();
    return true;
}
