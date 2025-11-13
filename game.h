#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <fstream> // Required for file I/O (High Score)
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
    bool shieldActive; // New: Shield status
    chrono::steady_clock::time_point shieldStartTime; // New: Shield timer

public:
    Snake(int startX, int startY);
    void changeDirection(Direction newDir);
    void move();
    void setGrow(bool shouldGrow, int amount = 1);
    void shrink(int amount); // New: Method to shrink snake
    const vector<pair<int, int>>& getBody() const;
    pair<int, int> getHead() const;
    int getLength() const;
    
    // New: Shield methods
    void activateShield();
    void deactivateShield();
    bool hasShield() const;
    int getShieldTimeRemaining() const;
    bool shouldBlink() const; // For blinking effect
    // MODIFIED: Use steady_clock's native duration type
    void adjustShieldStartTime(chrono::steady_clock::duration pauseDuration); 
};

class Game {
private:
    Snake snake;
    pair<int, int> food;
    pair<int, int> specialFood;
    pair<int, int> poisonFood; // New: Poison food
    pair<int, int> shield; // New: Shield power-up
    int score;
    bool gameOver;
    bool quit;
    bool paused; // New: Pause state
    Screen screen;
    int foodEaten;
    int specialFoodEaten; // New: Counter for special food
    int poisonFoodEaten; // New: Counter for poison food
    bool specialFoodActive;
    bool poisonFoodActive; // New: Poison food status
    bool shieldActive; // New: Shield power-up status
    chrono::steady_clock::time_point specialFoodSpawnTime;
    chrono::steady_clock::time_point poisonFoodSpawnTime; // New: Poison food timer
    chrono::steady_clock::time_point shieldSpawnTime; // New: Shield spawn timer
    chrono::steady_clock::time_point lastShieldSpawnTime; // New: Last shield spawn time
    
    // ADDED: Pause tracking variable
    chrono::steady_clock::time_point pauseStartTime;

    const int SPECIAL_FOOD_DURATION = 10;
    const int POISON_FOOD_DURATION = 10; // New: Poison food duration
    const int SHIELD_DURATION = 10; // New: Shield power-up duration
    const int SHIELD_SPAWN_INTERVAL = 60; // Changed: Shield spawn interval to 60 seconds
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

    // --- HIGH SCORE ADDITIONS ---
    // Variable to hold the loaded high score
    int highScore;
    // Constant for the high score filename
    const string HIGHSCORE_FILE = "highscore.txt";
    // ----------------------------

    // Helper methods
    bool isObstacle(int x, int y) const;
    void spawnFood();
    void spawnSpecialFood();
    void spawnPoisonFood(); // New: Spawn poison food
    void spawnShield(); // New: Spawn shield power-up
    void spawnObstacles();
    void updateSpecialFood();
    void updatePoisonFood(); // New: Update poison food
    void updateShield(); // New: Update shield power-up
    void updateObstacles();
    
    // Timer calculation helpers (for drawing stats)
    int getSpecialFoodTimeRemaining() const; // ADDED
    int getShieldSpawnRemaining() const;     // ADDED
    int getObstacleTimeRemaining() const;

public:
    Game();
    ~Game();
    void draw();
    void update();
    void handleInput();
    bool isGameOver() const;
    bool shouldQuit() const;
    bool isPaused() const; // New: Get pause state
    void togglePause(); // MODIFIED: Logic updated to handle timer compensation
    int getGameSpeed();

    // --- HIGH SCORE METHODS ---
    // Loads the score from the file
    void loadHighScore();
    // Saves the current score if it's a new high score
    void saveHighScore();
    // --------------------------
};

#endif
