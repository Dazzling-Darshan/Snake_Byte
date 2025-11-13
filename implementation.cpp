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
#include <stdexcept> // Added for exception handling with stoi
#include <iomanip>   // For formatted output

using namespace std;

// Initialize global variables
string SNAKE_HEAD = "🐍";
string SNAKE_HEAD_DEAD = "💥";
string SNAKE_BODY = "🟢";
string SNAKE_BODY_DEAD = "🔴";
string SNAKE_BODY_SHIELD = "🟣"; // New: Purple body for shield
string FOOD_EMOJI = "🍎";
string SPECIAL_FOOD_EMOJI = "🍇";
string POISON_FOOD_EMOJI = "🍄"; // New: Poison food emoji
string SHIELD_EMOJI = "🛡️"; // New: Shield power-up emoji
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

// Helper function to safely create padded strings
string createPaddedString(const string& content, int totalLength) {
    int contentLength = content.length();
    if (contentLength >= totalLength) {
        return content;
    }
    return content + string(totalLength - contentLength, ' ');
}

// Snake implementation
Snake::Snake(int startX, int startY) {
    body.push_back({startX, startY});
    dir = RIGHT;
    nextDir = RIGHT;
    grow = false;
    growAmount = 1;
    shieldActive = false;
}

void Snake::changeDirection(Direction newDir) {
    if ((dir == LEFT && newDir != RIGHT) ||
        (dir == RIGHT && newDir != LEFT) ||
        (dir == UP && newDir != DOWN) ||
        (dir == DOWN && newDir != UP)) {
        nextDir = newDir;
    }
}

void Snake::move() {
    dir = nextDir;
    
    pair<int, int> newHead = body[0];
    
    switch (dir) {
        case LEFT:  newHead.first--; break;
        case RIGHT: newHead.first++; break;
        case UP:    newHead.second--; break;
        case DOWN:  newHead.second++; break;
        case STOP:  // Do nothing when stopped
            break;
    }

    body.insert(body.begin(), newHead);
    
    if (grow) {
        growAmount--;
        if (growAmount <= 0) {
            grow = false;
            growAmount = 1;
        }
    } else {
        body.pop_back();
    }
}

void Snake::setGrow(bool shouldGrow, int amount) {
    grow = shouldGrow;
    growAmount = amount;
}

// New: Method to shrink snake by removing tail segments
void Snake::shrink(int amount) {
    for (int i = 0; i < amount && body.size() > 1; i++) {
        body.pop_back();
    }
}

const vector<pair<int, int>>& Snake::getBody() const {
    return body;
}

pair<int, int> Snake::getHead() const {
    return body[0];
}

int Snake::getLength() const {
    return body.size();
}

// New: Shield methods implementation
void Snake::activateShield() {
    shieldActive = true;
    shieldStartTime = chrono::steady_clock::now();
}

void Snake::deactivateShield() {
    shieldActive = false;
}

bool Snake::hasShield() const {
    return shieldActive;
}

int Snake::getShieldTimeRemaining() const {
    if (!shieldActive) return 0;
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - shieldStartTime).count();
    return max(0, 10 - (int)elapsed); // 10 seconds shield duration
}

bool Snake::shouldBlink() const {
    if (!shieldActive) return false;
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - shieldStartTime).count();
    // Blink every 500ms
    return (elapsed / 500) % 2 == 0;
}

// MODIFIED: Adjust shield timer after a pause
// The parameter now uses the correct duration type (steady_clock::duration)
void Snake::adjustShieldStartTime(chrono::steady_clock::duration pauseDuration) {
    if (shieldActive) {
        // When resuming, add the pause duration to the start time. 
        // This shifts the 'start' further into the past, compensating for the time that elapsed while paused.
        shieldStartTime += pauseDuration; 
    }
}

// Game implementation
Game::Game() : snake(WIDTH / 4, HEIGHT / 2), score(0), gameOver(false), quit(false), 
             paused(false), foodEaten(0), specialFoodEaten(0), poisonFoodEaten(0), 
             specialFoodActive(false), poisonFoodActive(false), shieldActive(false),
             wallCrash(false), obstaclesActive(false), highScore(0) {
    srand(static_cast<unsigned int>(time(0)));
    
    setupConsole();
    screen.hideCursor();
    
    if (loadCustomGraphics()) {
        // Custom graphics loaded silently
    }
    
    // Initialize last shield spawn time
    lastShieldSpawnTime = chrono::steady_clock::now();
    
    // --- HIGH SCORE ADDITION: Load the score upon starting the game
    loadHighScore();
    
    spawnFood();
}

Game::~Game() {
    screen.showCursor();
}

// --- HIGH SCORE IMPLEMENTATIONS ---

/**
 * Loads the high score from the file, setting to 0 if the file doesn't exist
 * or contains invalid data.
 */
void Game::loadHighScore() {
    ifstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        string line;
        if (getline(file, line)) {
            try {
                // Attempt to convert string to integer
                highScore = stoi(line);
            } catch (const std::invalid_argument& e) {
                // Non-numeric data found
                highScore = 0; 
            } catch (const std::out_of_range& e) {
                // Number too large/small
                highScore = 0;
            }
        }
        file.close();
    } else {
        highScore = 0;
    }
}

/**
 * Saves the current score as the new high score if it is greater than the
 * currently loaded high score.
 */
void Game::saveHighScore() {
    if (score > highScore) {
        highScore = score;
        ofstream file(HIGHSCORE_FILE);
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }
}

// --- END HIGH SCORE IMPLEMENTATIONS ---


bool Game::isObstacle(int x, int y) const {
    if (!obstaclesActive) return false;
    for (const auto& obs : obstacles) {
        if (x == obs.first && y == obs.second) {
            return true;
        }
    }
    return false;
}

void Game::spawnFood() {
    bool occupied;
    do {
        occupied = false;
        food.first = rand() % (WIDTH - 2) + 1;
        food.second = rand() % (HEIGHT - 2) + 1;
        
        for (const auto& segment : snake.getBody()) {
            if (segment.first == food.first && segment.second == food.second) {
                occupied = true;
                break;
            }
        }
        if (specialFoodActive && food.first == specialFood.first && food.second == specialFood.second) {
            occupied = true;
        }
        if (poisonFoodActive && food.first == poisonFood.first && food.second == poisonFood.second) {
            occupied = true;
        }
        if (shieldActive && food.first == shield.first && food.second == shield.second) {
            occupied = true;
        }
        if (isObstacle(food.first, food.second)) {
            occupied = true;
        }
    } while (occupied);
}

void Game::spawnSpecialFood() {
    bool occupied;
    do {
        occupied = false;
        specialFood.first = rand() % (WIDTH - 2) + 1;
        specialFood.second = rand() % (HEIGHT - 2) + 1;
        
        for (const auto& segment : snake.getBody()) {
            if (segment.first == specialFood.first && specialFood.second == specialFood.second) {
                occupied = true;
                break;
            }
        }
        if (specialFood.first == food.first && specialFood.second == food.second) {
            occupied = true;
        }
        if (poisonFoodActive && specialFood.first == poisonFood.first && specialFood.second == poisonFood.second) {
            occupied = true;
        }
        if (shieldActive && specialFood.first == shield.first && specialFood.second == shield.second) {
            occupied = true;
        }
        if (isObstacle(specialFood.first, specialFood.second)) {
            occupied = true;
        }
    } while (occupied);
    specialFoodActive = true;
    specialFoodSpawnTime = chrono::steady_clock::now();
    
    SPECIAL_FOOD_EMOJI = SPECIAL_FOODS[currentSpecialFoodIndex];
    currentSpecialFoodIndex = (currentSpecialFoodIndex + 1) % SPECIAL_FOODS.size();
}

// New: Spawn poison food
void Game::spawnPoisonFood() {
    bool occupied;
    do {
        occupied = false;
        poisonFood.first = rand() % (WIDTH - 2) + 1;
        poisonFood.second = rand() % (HEIGHT - 2) + 1;
        
        for (const auto& segment : snake.getBody()) {
            if (segment.first == poisonFood.first && poisonFood.second == poisonFood.second) {
                occupied = true;
                break;
            }
        }
        if (poisonFood.first == food.first && poisonFood.second == food.second) {
            occupied = true;
        }
        if (specialFoodActive && poisonFood.first == specialFood.first && poisonFood.second == specialFood.second) {
            occupied = true;
        }
        if (shieldActive && poisonFood.first == shield.first && poisonFood.second == shield.second) {
            occupied = true;
        }
        if (isObstacle(poisonFood.first, poisonFood.second)) {
            occupied = true;
        }
    } while (occupied);
    poisonFoodActive = true;
    poisonFoodSpawnTime = chrono::steady_clock::now();
}

// New: Spawn shield power-up
void Game::spawnShield() {
    bool occupied;
    do {
        occupied = false;
        shield.first = rand() % (WIDTH - 2) + 1;
        shield.second = rand() % (HEIGHT - 2) + 1;
        
        for (const auto& segment : snake.getBody()) {
            if (segment.first == shield.first && shield.second == shield.second) {
                occupied = true;
                break;
            }
        }
        if (shield.first == food.first && shield.second == food.second) {
            occupied = true;
        }
        if (specialFoodActive && shield.first == specialFood.first && shield.second == specialFood.second) {
            occupied = true;
        }
        if (poisonFoodActive && shield.first == poisonFood.first && shield.second == poisonFood.second) {
            occupied = true;
        }
        if (isObstacle(shield.first, shield.second)) {
            occupied = true;
        }
    } while (occupied);
    shieldActive = true;
    shieldSpawnTime = chrono::steady_clock::now();
}

void Game::spawnObstacles() {
    obstacles.clear();
    bool occupied;

    for (int i = 0; i < OBSTACLE_COUNT; ++i) {
        pair<int, int> obs;
        do {
            occupied = false;
            obs.first = rand() % WIDTH;
            obs.second = rand() % HEIGHT;

            for (const auto& segment : snake.getBody()) {
                if (segment.first == obs.first && segment.second == obs.second) {
                    occupied = true;
                    break;
                }
            }

            if (obs.first == food.first && obs.second == food.second) {
                occupied = true;
            }
            if (specialFoodActive && obs.first == specialFood.first && obs.second == specialFood.second) {
                occupied = true;
            }
            if (poisonFoodActive && obs.first == poisonFood.first && obs.second == poisonFood.second) {
                occupied = true;
            }
            if (shieldActive && obs.first == shield.first && obs.second == shield.second) {
                occupied = true;
            }
        } while (occupied);
        obstacles.push_back(obs);
    }
    obstaclesActive = true;
    obstacleSpawnTime = chrono::steady_clock::now();
}

void Game::updateSpecialFood() {
    if (specialFoodActive) {
        auto now = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(now - specialFoodSpawnTime);
        if (duration.count() >= SPECIAL_FOOD_DURATION) {
            specialFoodActive = false;
        }
    }
}

// New: Update poison food
void Game::updatePoisonFood() {
    if (poisonFoodActive) {
        auto now = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(now - poisonFoodSpawnTime);
        if (duration.count() >= POISON_FOOD_DURATION) {
            poisonFoodActive = false;
        }
    }
}

// New: Update shield power-up
void Game::updateShield() {
    // Check if it's time to spawn a new shield
    auto now = chrono::steady_clock::now();
    auto timeSinceLastSpawn = chrono::duration_cast<chrono::seconds>(now - lastShieldSpawnTime);
    
    if (!shieldActive && timeSinceLastSpawn.count() >= SHIELD_SPAWN_INTERVAL) {
        spawnShield();
        lastShieldSpawnTime = now;
    }
    
    // Update existing shield duration
    if (shieldActive) {
        auto shieldDuration = chrono::duration_cast<chrono::seconds>(now - shieldSpawnTime);
        if (shieldDuration.count() >= SHIELD_DURATION) {
            shieldActive = false;
        }
    }
    
    // Update snake's shield status timer
    if (snake.hasShield()) {
        if (snake.getShieldTimeRemaining() <= 0) {
            snake.deactivateShield();
        }
    }
}

void Game::updateObstacles() {
    if (obstaclesActive) {
        auto now = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(now - obstacleSpawnTime);
        if (duration.count() >= OBSTACLE_DURATION) {
            obstaclesActive = false;
            obstacles.clear();
        }
    }
}

int Game::getGameSpeed() {
    int speedReduction = min(snake.getLength() * speedDecrement, baseSpeed - minSpeed);
    int calculatedSpeed = baseSpeed - speedReduction;
    return max(calculatedSpeed, minSpeed);
}

// --- TIMER REMAINING HELPERS (For Draw) ---

// Calculates remaining time for active obstacles
int Game::getObstacleTimeRemaining() const {
    if (!obstaclesActive) return 0;
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - obstacleSpawnTime).count();
    return max(0, OBSTACLE_DURATION - (int)elapsed);
}

// ADDED: Calculates remaining time for active special food
int Game::getSpecialFoodTimeRemaining() const {
    if (!specialFoodActive) return 0;
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - specialFoodSpawnTime).count();
    return max(0, SPECIAL_FOOD_DURATION - (int)elapsed);
}

// ADDED: Calculates remaining time until the next shield spawns
int Game::getShieldSpawnRemaining() const {
    if (shieldActive || snake.hasShield()) return 0; // Don't show spawn countdown if a shield is already on the map or active on snake
    auto now = chrono::steady_clock::now();
    auto timeSinceLastSpawn = chrono::duration_cast<chrono::seconds>(now - lastShieldSpawnTime);
    return max(0, SHIELD_SPAWN_INTERVAL - (int)timeSinceLastSpawn.count());
}

// --- END TIMER REMAINING HELPERS ---

// New: Get pause state
bool Game::isPaused() const {
    return paused;
}

// MODIFIED: Toggle pause state with time compensation
void Game::togglePause() {
    paused = !paused;
    
    if (paused) {
        pauseStartTime = chrono::steady_clock::now();
    } else {
        auto now = chrono::steady_clock::now();
        // Calculate the duration of the pause. This results in steady_clock::duration.
        auto pauseDuration = now - pauseStartTime; 
        
        // Compensate all active and spawning timers by ADDING the pause duration to their start times
        
        // 1. Special Food
        if (specialFoodActive) {
            specialFoodSpawnTime += pauseDuration;
        }
        
        // 2. Poison Food (if active)
        if (poisonFoodActive) {
             poisonFoodSpawnTime += pauseDuration;
        }
        
        // 3. Obstacles
        if (obstaclesActive) {
            obstacleSpawnTime += pauseDuration;
        }

        // 4. Shield Spawn Timer
        lastShieldSpawnTime += pauseDuration;
        
        // 5. Shield Power-up Active on Snake
        snake.adjustShieldStartTime(pauseDuration);
        
        // 6. Shield Power-up on map (if it hasn't been picked up)
        if (shieldActive) {
            shieldSpawnTime += pauseDuration;
        }
    }
}

void Game::draw() {
    screen.clear();
    
    screen.addToBuffer("====== 🐍 SNAKE GAME 🐍 ======\n");
    
    for (int i = 0; i < WIDTH + 2; i++) {
        screen.addToBuffer(WALL);
    }
    screen.addToBuffer("\n");

    for (int y = 0; y < HEIGHT; y++) {
        if (wallCrash && crashPosition.first == -1 && crashPosition.second == y) {
            screen.addToBuffer(SNAKE_HEAD_DEAD);
        } else {
            screen.addToBuffer(WALL);
        }
        
        for (int x = 0; x < WIDTH; x++) {
            if (gameOver && y == HEIGHT/2 - 4 && x == WIDTH/2 - 4) {
                screen.addToBuffer("G A M E  O V E R");
                x += 7;
                continue;
            }
            
            if (paused && y == HEIGHT/2 - 3 && x == WIDTH/2 - 3) {
                screen.addToBuffer("P A U S E D");
                x += 5;
                continue;
            }
            
            string cellContent = EMPTY_SPACE;
            bool isOccupied = false;
            
            if (obstaclesActive) {
                for (const auto& obs : obstacles) {
                    if (x == obs.first && y == obs.second) {
                        cellContent = WALL;
                        isOccupied = true;
                        break;
                    }
                }
            }
            
            if (!isOccupied) {
                if (x == snake.getHead().first && y == snake.getHead().second) {
                    if (gameOver && !wallCrash) {
                        cellContent = SNAKE_HEAD_DEAD;
                    } else {
                        cellContent = SNAKE_HEAD;
                    }
                }
                else if (specialFoodActive && x == specialFood.first && y == specialFood.second && !gameOver) {
                    cellContent = SPECIAL_FOOD_EMOJI;
                }
                else if (poisonFoodActive && x == poisonFood.first && y == poisonFood.second && !gameOver) {
                    cellContent = POISON_FOOD_EMOJI;
                }
                else if (shieldActive && x == shield.first && y == shield.second && !gameOver) {
                    cellContent = SHIELD_EMOJI;
                }
                else if (x == food.first && y == food.second && !gameOver) {
                    cellContent = FOOD_EMOJI;
                }
                else {
                    for (size_t i = 1; i < snake.getBody().size(); i++) {
                        if (x == snake.getBody()[i].first && y == snake.getBody()[i].second) {
                            if (gameOver) {
                                cellContent = SNAKE_BODY_DEAD;
                            } else if (snake.hasShield() && snake.shouldBlink()) {
                                cellContent = SNAKE_BODY_SHIELD; // Blinking purple when shield active
                            } else {
                                cellContent = SNAKE_BODY;
                            }
                            break;
                        }
                    }
                }
            }
            
            screen.addToBuffer(cellContent);
        }
        
        if (wallCrash && crashPosition.first == WIDTH && crashPosition.second == y) {
            screen.addToBuffer(SNAKE_HEAD_DEAD);
        } else {
            screen.addToBuffer(WALL);
        }
        screen.addToBuffer("\n");
    }

    for (int i = 0; i < WIDTH + 2; i++) {
        if (wallCrash && crashPosition.first == i && crashPosition.second == HEIGHT) {
            screen.addToBuffer(SNAKE_HEAD_DEAD);
        } else {
            screen.addToBuffer(WALL);
        }
    }
    screen.addToBuffer("\n");

    // --- FIXED: Clean ASCII interface without box drawing characters ---
    screen.addToBuffer("==============================================\n");
    screen.addToBuffer("             GAME STATISTICS                 \n");
    screen.addToBuffer("----------------------------------------------\n");
    
    // Left Column
    string leftCol = "Score: " + to_string(score) + " 🏆";
    leftCol += string(25 - leftCol.length(), ' ');
    screen.addToBuffer(leftCol);
    
    // Right Column: Special Food Status
    string specialFoodStatus;
    if (paused && specialFoodActive) {
        specialFoodStatus = "PAUSED            ";
    } else if (specialFoodActive) {
        int remaining = getSpecialFoodTimeRemaining();
        specialFoodStatus = "Active " + to_string(remaining) + " s";
    } else {
        specialFoodStatus = to_string(specialFoodEaten) + "             ";
    }
    string rightCol = "Special: " + specialFoodStatus + " ";
    screen.addToBuffer(rightCol + "\n");
    
    // Left Column
    leftCol = "High Score: " + to_string(highScore) + " ⭐";
    leftCol += string(25 - leftCol.length(), ' ');
    screen.addToBuffer(leftCol);
    
    // Right Column
    rightCol = "Speed: " + to_string(getGameSpeed()) + "ms 🚀";
    screen.addToBuffer(rightCol + "\n");
    
    // Left Column
    leftCol = "Length: " + to_string(snake.getLength()) + " 📏";
    leftCol += string(25 - leftCol.length(), ' ');
    screen.addToBuffer(leftCol);
    
    // 
    
    
    // Shield status (Combined logic for active shield and spawn timer)
    string shieldStatus;
    if (paused) {
        // Paused state overrides everything
        if (snake.hasShield()) {
             shieldStatus = "Shield: PAUSED           "; 
        } else {
             shieldStatus = "Shield: PAUSED           ";
        }
    } else if (snake.hasShield()) {
        // Snake has active shield
        shieldStatus = "Shield: Active " + to_string(snake.getShieldTimeRemaining()) + " s 🛡️";
    } else if (shieldActive) {
        // Shield power-up is on the map (Time on map is SHIELD_DURATION)
        auto now = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(now - shieldSpawnTime).count();
        int remaining = max(0, SHIELD_DURATION - (int)elapsed); 
        shieldStatus = "Shield: Available (" + to_string(remaining) + "s) " + SHIELD_EMOJI;
    } else {
        // Waiting for next shield spawn
        int nextSpawn = getShieldSpawnRemaining();
        shieldStatus = "Shield: Available in " + to_string(nextSpawn) + " s";
    }
    screen.addToBuffer(shieldStatus + "\n");
    
    // Obstacles status - full width
    string obstacleStr;
    if (paused && obstaclesActive) {
        obstacleStr = "Obstacles: PAUSED           ";
    } else if (obstaclesActive) {
        int remaining = getObstacleTimeRemaining();
        obstacleStr = "Obstacles: Active " + to_string(remaining) + " s 🚧";
    } else {
        obstacleStr = "Obstacles: Clear            ";
    }
    screen.addToBuffer(obstacleStr + "\n");
    
    screen.addToBuffer("----------------------------------------------\n");
    
    // Controls
    screen.addToBuffer("Controls: WASD/Arrows | SPACE: Pause | Q: Quit\n");
    
    if (paused) {
        screen.addToBuffer("             *** GAME PAUSED ***                 \n");
    }
    
    if (gameOver) {
        screen.addToBuffer("                 💀 GAME OVER! 💀                \n");
    }
    
    screen.addToBuffer("==============================================\n");

    screen.draw();
}

void Game::update() {
    // If paused, timers are stopped via togglePause, but game logic must stop
    if (gameOver || paused) return;

    snake.move();
    updateSpecialFood();
    updatePoisonFood(); // New: Update poison food
    updateShield(); // New: Update shield power-up
    updateObstacles();

    pair<int, int> head = snake.getHead();

    // Check Wall Collision (shield doesn't protect from walls)
    if (head.first < 0 || head.first >= WIDTH || 
        head.second < 0 || head.second >= HEIGHT) {
        gameOver = true;
        wallCrash = true;
        
        // Determine crash position for drawing the dead snake head on the wall
        if (head.first < 0) crashPosition = make_pair(-1, head.second);
        else if (head.first >= WIDTH) crashPosition = make_pair(WIDTH, head.second);
        else if (head.second < 0) crashPosition = make_pair(head.first, -1);
        else if (head.second >= HEIGHT) crashPosition = make_pair(head.first, HEIGHT);
        
        saveHighScore(); // --- HIGH SCORE ADDITION: Save on game over
        return;
    }

    // Check Self Collision (skip if shield is active)
    if (!snake.hasShield()) {
        for (size_t i = 1; i < snake.getBody().size(); i++) {
            if (head.first == snake.getBody()[i].first && 
                head.second == snake.getBody()[i].second) {
                gameOver = true;
                saveHighScore(); // --- HIGH SCORE ADDITION: Save on game over
                return;
            }
        }
    }
    
    // Check Obstacle Collision (skip if shield is active)
    if (obstaclesActive && !snake.hasShield()) {
        for (const auto& obs : obstacles) {
            if (head.first == obs.first && head.second == obs.second) {
                gameOver = true;
                saveHighScore(); // --- HIGH SCORE ADDITION: Save on game over
                return;
            }
        }
    }

    // Check Food consumption
    if (head.first == food.first && head.second == food.second) {
        score += 10;
        snake.setGrow(true, 1);
        foodEaten++;
        spawnFood();
        
        if (foodEaten % 4 == 0) {
            spawnSpecialFood();
            spawnPoisonFood(); // Spawn poison food along with special food
        }
        if (foodEaten % 5 == 0) {
            spawnObstacles();
        }
    }

    // Check Special Food consumption
    if (specialFoodActive && head.first == specialFood.first && head.second == specialFood.second) {
        score += 30;
        snake.setGrow(true, 3);
        specialFoodActive = false;
        specialFoodEaten++;
    }

    // New: Check Poison Food consumption
    if (poisonFoodActive && head.first == poisonFood.first && head.second == poisonFood.second) {
        score = max(0, score - 30); // Decrease score, but not below 0
        
        // Decrease length by 3 by removing tail segments
        snake.shrink(3);
        
        poisonFoodActive = false;
        poisonFoodEaten++;
    }

    // New: Check Shield Power-up consumption
    if (shieldActive && head.first == shield.first && head.second == shield.second) {
        snake.activateShield();
        shieldActive = false;
    }
}

void Game::handleInput() {
    if (InputHandler::isKeyPressed()) {
        int ch = InputHandler::getChar();
        
        if (ch == 224) {
            ch = InputHandler::getChar();
            switch (ch) {
                case 72: snake.changeDirection(UP); break;
                case 80: snake.changeDirection(DOWN); break;
                case 75: snake.changeDirection(LEFT); break;
                case 77: snake.changeDirection(RIGHT); break;
            }
        } 
        else if (ch == 27) {
            if (InputHandler::isKeyPressed()) {
                ch = InputHandler::getChar();
                if (ch == 91) {
                    if (InputHandler::isKeyPressed()) {
                        ch = InputHandler::getChar();
                        switch (ch) {
                            case 65: snake.changeDirection(UP); break;
                            case 66: snake.changeDirection(DOWN); break;
                            case 67: snake.changeDirection(RIGHT); break;
                            case 68: snake.changeDirection(LEFT); break;
                        }
                    }
                }
            }
        } else {
            switch (tolower(ch)) {
                case 'w': snake.changeDirection(UP); break;
                case 's': snake.changeDirection(DOWN); break;
                case 'a': snake.changeDirection(LEFT); break;
                case 'd': snake.changeDirection(RIGHT); break;
                case ' ': togglePause(); break; // New: Spacebar toggles pause
                case 'q': quit = true; break;
            }
        }
    }
}

bool Game::isGameOver() const {
    return gameOver;
}

bool Game::shouldQuit() const {
    return quit || gameOver;
}
