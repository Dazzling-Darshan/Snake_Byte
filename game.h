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

class Game {
private:
    Snake snake;
    pair<int, int> food;
    pair<int, int> specialFood;
    int score;
    bool gameOver;
    bool quit;
    Screen screen;
    int foodEaten;
    bool specialFoodActive;
    chrono::steady_clock::time_point specialFoodSpawnTime;
    const int SPECIAL_FOOD_DURATION = 10;
    pair<int, int> crashPosition;
    bool wallCrash;

    // Obstacle members
    const int OBSTACLE_DURATION = 10;
    const int OBSTACLE_COUNT = 7;
    vector<pair<int, int>> obstacles;
    chrono::steady_clock::time_point obstacleSpawnTime;
    bool obstaclesActive;

    // Speed control
    int baseSpeed = 200;
    int minSpeed = 75;
    int speedDecrement = 9;

    // Helper methods
    bool isObstacle(int x, int y) const;
    void spawnFood();
    void spawnSpecialFood();
    void spawnObstacles();
    void updateSpecialFood();
    void updateObstacles();
    int getObstacleTimeRemaining() const;

public:
    Game();
    ~Game();
    void draw();
    void update();
    void handleInput();
    bool isGameOver() const;
    bool shouldQuit() const;
    int getGameSpeed();
};

#endif
