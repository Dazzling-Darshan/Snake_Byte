#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <string>
#include <vector>

// Platform-specific includes
#ifdef _WIN32
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

using namespace std;

// Console dimensions
const int WIDTH = 40;
const int HEIGHT = 20;

// Default emojis (fallback)
extern string SNAKE_HEAD;
extern string SNAKE_HEAD_DEAD;
extern string SNAKE_BODY;
extern string SNAKE_BODY_DEAD;
extern string SNAKE_BODY_SHIELD; // New: Shielded snake body
extern string FOOD_EMOJI;
extern string SPECIAL_FOOD_EMOJI;
extern string POISON_FOOD_EMOJI; // New: Poison food emoji
extern string SHIELD_EMOJI; // New: Shield power-up emoji
extern string WALL;
extern string EMPTY_SPACE;

// Special food types
extern vector<string> SPECIAL_FOODS;
extern int currentSpecialFoodIndex;

class Screen {
private:
    string screenBuffer;

public:
    void clear();
    void addToBuffer(const string& content);
    void draw();
    void hideCursor();
    void showCursor();
};

// Cross-platform console setup
void setupConsole();

// Function to load custom graphics from files
bool loadCustomGraphics();

#endif
