#include "input_handler.h"
#include "screen.h"
#include "game.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;

// Initialize global variables
string SNAKE_HEAD = "🐍";
string SNAKE_HEAD_DEAD = "💥";
string SNAKE_BODY = "🟢";
string SNAKE_BODY_DEAD = "🔴";
string FOOD_EMOJI = "🍎";
string SPECIAL_FOOD_EMOJI = "🍇";
string WALL = "⬜";
string EMPTY_SPACE = "  ";

vector<string> SPECIAL_FOODS = {"🍇", "🍌", "🍋"};
int currentSpecialFoodIndex = 0;

// Initialize static members for non-Windows systems

#ifndef _WIN32
struct termios InputHandler::oldt;
struct termios InputHandler::newt;
bool InputHandler::rawModeEnabled = false;
#endif

// InputHandler implementation
void InputHandler::enableRawInput() {
#ifdef _WIN32
    // Not needed for Windows with _kbhit and _getch
#else
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    rawModeEnabled = true;
#endif
}

void InputHandler::disableRawInput() {
#ifdef _WIN32
    // Not needed for Windows
#else
    if (rawModeEnabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        rawModeEnabled = false;
    }
#endif
}

bool InputHandler::isKeyPressed() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    struct timeval tv;
    fd_set rdfs;
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    FD_ZERO(&rdfs);
    FD_SET(STDIN_FILENO, &rdfs);
    
    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);
#endif
}

int InputHandler::getChar() {
#ifdef _WIN32
    return _getch();
#else
    char ch = 0;
    read(STDIN_FILENO, &ch, 1);
    return ch;
#endif
}

// Screen implementation
void Screen::clear() {
    screenBuffer.clear();
    screenBuffer += "\033[H";
}

void Screen::addToBuffer(const string& content) {
    screenBuffer += content;
}

void Screen::draw() {
    cout << "\033[H" << screenBuffer;
    cout.flush();
}

void Screen::hideCursor() {
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
#else
    cout << "\033[?25l";
#endif
}

void Screen::showCursor() {
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
#else
    cout << "\033[?25h";
#endif
}

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#else
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
}

bool loadCustomGraphics() {
    bool loaded = false;
    
    ifstream headFile("head.txt");
    if (headFile.is_open()) {
        string line;
        if (getline(headFile, line)) {
            if (!line.empty()) {
                SNAKE_HEAD = line;
                loaded = true;
            }
        }
        headFile.close();
    }
    
    ifstream bodyFile("body.txt");
    if (bodyFile.is_open()) {
        string line;
        if (getline(bodyFile, line)) {
            if (!line.empty()) {
                SNAKE_BODY = line;
                loaded = true;
            }
        }
        bodyFile.close();
    }

    ifstream specialFoodFile("special_food.txt");
    if (specialFoodFile.is_open()) {
        string line;
        if (getline(specialFoodFile, line)) {
            if (!line.empty()) {
                SPECIAL_FOOD_EMOJI = line;
                loaded = true;
            }
        }
        specialFoodFile.close();
    }
    
    return loaded;
}

