#ifndef GAME_H
#define GAME_H

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
#include "input_handler.h"
#include "screen.h"

using namespace std;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

class Snake {
private:
    vector<pair<int, int>> body;
    Direction dir;
    Direction nextDir;
    bool grow;
    int growAmount;

public:
    Snake(int startX, int startY);
    void changeDirection(Direction newDir);
    void move();
    void setGrow(bool shouldGrow, int amount = 1);
    const vector<pair<int, int>>& getBody() const;
    pair<int, int> getHead() const;
    int getLength() const;
};

#endif

