@echo off
echo =======================================
echo Building ATM Simulator with g++...
echo =======================================

if not exist build mkdir build

g++ -Iinclude src\main.cpp src\Account.cpp src\DatabaseManager.cpp src\System.cpp src\UI.cpp -o build\ATMSimulator.exe -std=c++11

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
