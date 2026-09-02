#ifndef UI_H
#define UI_H

#include <iostream>
#include <string>

namespace UI {
    // ANSI Escape Codes for Colors
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    
    // Background colors
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_RED = "\033[41m";

    // Box drawing characters
    const std::string TL = "╔"; // Top Left
    const std::string TR = "╗"; // Top Right
    const std::string BL = "╚"; // Bottom Left
    const std::string BR = "╝"; // Bottom Right
    const std::string H = "═";  // Horizontal
    const std::string V = "║";  // Vertical
    
    // Utility functions for UI
    void clearScreen();
    void printHeader(const std::string& title);
    void printMessage(const std::string& msg, const std::string& color = WHITE);
    void printError(const std::string& msg);
    void printSuccess(const std::string& msg);
    void printMenuOption(int num, const std::string& text);
    int getMenuChoice(int maxChoice);
    void pause();
}

#endif // UI_H
