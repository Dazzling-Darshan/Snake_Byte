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