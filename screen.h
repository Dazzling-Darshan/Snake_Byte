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
extern string FOOD_EMOJI;
extern string SPECIAL_FOOD_EMOJI;
extern string WALL;
extern string EMPTY_SPACE;
