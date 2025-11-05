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

// ==========================
// Commit 2 additions below
// ==========================

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

    // Obstacle handling
    const int OBSTACLE_DURATION = 8;
    const int OBSTACLE_COUNT = 5;
    vector<pair<int, int>> obstacles;
    chrono::steady_clock::time_point obstacleSpawnTime;
    bool obstaclesActive;

    // Speed control
    int baseSpeed = 200;
    int minSpeed = 75;
    int speedDecrement = 9;

    // Helper methods (to be implemented later)
    bool isObstacle(int x, int y) const;
    void spawnFood();
    void spawnSpecialFood();
    void spawnObstacles();
    bool checkCollision();
    void resetGame();
    void updateSpecialFood();
    void updateObstacles();
    void saveHighScore();
    int loadHighScore();

public:
    Game();  // Constructor
};

#endif
