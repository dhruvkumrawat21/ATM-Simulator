@echo off
echo =======================================
echo Building Advanced ATM Simulator...
echo =======================================

if not exist build mkdir build

echo Compiling SQLite (C code)...
gcc -c src\sqlite3.c -o build\sqlite3.o

if %ERRORLEVEL% NEQ 0 (
    echo SQLite compilation failed.
    exit /b %ERRORLEVEL%
)

echo Compiling C++ code and linking...
g++ -Iinclude src\main.cpp src\Account.cpp src\DatabaseManager.cpp src\System.cpp src\UI.cpp src\sha256.cpp build\sqlite3.o -o build\ATMSimulator.exe -std=c++11

if %ERRORLEVEL% EQU 0 (
    echo =======================================
    echo Build Successful! 
    echo Executable is located at build\ATMSimulator.exe
    echo =======================================
) else (
    echo =======================================
    echo Build Failed. Please check errors above.
    echo =======================================
)
