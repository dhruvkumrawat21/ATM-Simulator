#include "UI.h"
#include <limits>

namespace UI {
    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void printHeader(const std::string& title) {
        clearScreen();
        int width = 50;
        int padding = (width - title.length()) / 2;
        
        std::cout << CYAN << BOLD;
        std::cout << TL;
        for (int i = 0; i < width; ++i) std::cout << H;
        std::cout << TR << "\n";
        
        std::cout << V;
        for (int i = 0; i < padding; ++i) std::cout << " ";
        std::cout << WHITE << title << CYAN;
        for (int i = 0; i < width - padding - title.length(); ++i) std::cout << " ";
        std::cout << V << "\n";
        
        std::cout << BL;
        for (int i = 0; i < width; ++i) std::cout << H;
        std::cout << BR << "\n";
        std::cout << RESET << "\n";
    }

    void printMessage(const std::string& msg, const std::string& color) {
        std::cout << color << msg << RESET << "\n";
    }

    void printError(const std::string& msg) {
        std::cout << "\n" << RED << BOLD << " [ERROR] " << RESET << RED << msg << RESET << "\n";
    }

    void printSuccess(const std::string& msg) {
        std::cout << "\n" << GREEN << BOLD << " [SUCCESS] " << RESET << GREEN << msg << RESET << "\n";
    }

    void printMenuOption(int num, const std::string& text) {
        std::cout << "  " << YELLOW << "[" << num << "] " << RESET << text << "\n";
    }

    int getMenuChoice(int maxChoice) {
        int choice;
        while (true) {
            std::cout << "\n" << CYAN << " >> " << RESET << "Enter your choice (1-" << maxChoice << "): ";
            if (std::cin >> choice && choice >= 1 && choice <= maxChoice) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            } else {
                std::cin.clear(); // Clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                printError("Invalid input. Please enter a number between 1 and " + std::to_string(maxChoice) + ".");
            }
        }
    }

    void pause() {
        std::cout << "\n" << WHITE << "Press Enter to continue..." << RESET;
        std::cin.get();
    }
}
