#include "DatabaseManager.h"
#include "sqlite3.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

DatabaseManager::DatabaseManager(const string& filename) : dbFile(filename), db(nullptr) {
    // Open connection to SQLite Database
    if (sqlite3_open(dbFile.c_str(), &db) != SQLITE_OK) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
    }

    // Create Accounts table if it doesn't exist
    string createAccountsSql = 
        "CREATE TABLE IF NOT EXISTS Accounts ("
        "account_number TEXT PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "pin_hash TEXT NOT NULL, "
        "balance REAL NOT NULL, "
        "account_type TEXT NOT NULL, "
        "is_locked INTEGER NOT NULL, "
        "failed_attempts INTEGER NOT NULL);";
    executeQuery(createAccountsSql);

    // Create Transactions table if it doesn't exist
    string createTransSql = 
        "CREATE TABLE IF NOT EXISTS Transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "account_number TEXT NOT NULL, "
        "type TEXT NOT NULL, "
        "amount REAL NOT NULL, "
        "timestamp TEXT NOT NULL, "
        "FOREIGN KEY(account_number) REFERENCES Accounts(account_number));";
    executeQuery(createTransSql);
}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

bool DatabaseManager::executeQuery(const string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

vector<Account*> DatabaseManager::loadAccounts() {
    vector<Account*> accounts;
    const char* sql = "SELECT account_number, name, pin_hash, balance, account_type, is_locked, failed_attempts FROM Accounts;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string accNum = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string pinHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            double balance = sqlite3_column_double(stmt, 3);
            string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            bool isLocked = sqlite3_column_int(stmt, 5) != 0;
            int failedAttempts = sqlite3_column_int(stmt, 6);

            Account* acc = nullptr;
            if (type == "SAVINGS") {
                acc = new SavingsAccount(name, accNum, pinHash, balance, isLocked, failedAttempts);
            } else {
                acc = new CurrentAccount(name, accNum, pinHash, balance, isLocked, failedAttempts);
            }
            accounts.push_back(acc);
        }
    }
    sqlite3_finalize(stmt);
    return accounts;
}

bool DatabaseManager::saveAccount(Account* acc) {
    string sql = "INSERT INTO Accounts (account_number, name, pin_hash, balance, account_type, is_locked, failed_attempts) VALUES ('" + 
                 acc->getAccountNumber() + "', '" + 
                 acc->getName() + "', '" + 
                 acc->getPinHash() + "', " + 
                 to_string(acc->getBalance()) + ", '" + 
                 acc->getAccountType() + "', " + 
                 to_string(acc->getIsLocked()) + ", " + 
                 to_string(acc->getFailedAttempts()) + ");";
    return executeQuery(sql);
}

bool DatabaseManager::updateAccount(Account* acc) {
    string sql = "UPDATE Accounts SET pin_hash = '" + acc->getPinHash() + 
                 "', balance = " + to_string(acc->getBalance()) + 
                 ", is_locked = " + to_string(acc->getIsLocked()) + 
                 ", failed_attempts = " + to_string(acc->getFailedAttempts()) + 
                 " WHERE account_number = '" + acc->getAccountNumber() + "';";
    return executeQuery(sql);
}

bool DatabaseManager::logTransaction(const string& accNum, const string& type, double amount) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    stringstream timeStream;
    timeStream << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday << " "
               << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec;
               
    string sql = "INSERT INTO Transactions (account_number, type, amount, timestamp) VALUES ('" + 
                 accNum + "', '" + type + "', " + to_string(amount) + ", '" + timeStream.str() + "');";
    return executeQuery(sql);
}

vector<string> DatabaseManager::getMiniStatement(const string& accNum) {
    vector<string> statement;
    string sql = "SELECT type, amount, timestamp FROM Transactions WHERE account_number = '" + accNum + "' ORDER BY id DESC LIMIT 5;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double amount = sqlite3_column_double(stmt, 1);
            string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            
            stringstream line;
            line << "[" << timestamp << "] " << type << ": $" << fixed << setprecision(2) << amount;
            statement.push_back(line.str());
        }
    }
    sqlite3_finalize(stmt);
    return statement;
}

bool DatabaseManager::exportToCSV(const string& csvFilename) {
    ofstream file(csvFilename, ios::trunc);
    if (!file.is_open()) return false;

    file << "Account Number,Name,Type,Balance,Locked\n";

    const char* sql = "SELECT account_number, name, account_type, balance, is_locked FROM Accounts;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string accNum = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            double balance = sqlite3_column_double(stmt, 3);
            bool isLocked = sqlite3_column_int(stmt, 4) != 0;
            
            file << accNum << "," << name << "," << type << "," << fixed << setprecision(2) << balance << "," << (isLocked ? "YES" : "NO") << "\n";
        }
    }
    sqlite3_finalize(stmt);
    file.close();
    return true;
}
