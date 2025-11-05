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

// Snake implementation
Snake::Snake(int startX, int startY) {
    body.push_back({startX, startY});
    dir = RIGHT;
    nextDir = RIGHT;
    grow = false;
    growAmount = 1;
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

const vector<pair<int, int>>& Snake::getBody() const {
    return body;
}

pair<int, int> Snake::getHead() const {
    return body[0];
}

int Snake::getLength() const {
    return body.size();
}

// Game implementation
Game::Game() : snake(WIDTH / 4, HEIGHT / 2), score(0), gameOver(false), quit(false), 
             foodEaten(0), specialFoodActive(false), wallCrash(false),
             obstaclesActive(false) {
    srand(static_cast<unsigned int>(time(0)));
    
    setupConsole();
    screen.hideCursor();
    
    if (loadCustomGraphics()) {
        // Custom graphics loaded silently
    }
    
    spawnFood();
}

Game::~Game() {
    screen.showCursor();
}

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
            if (segment.first == specialFood.first && segment.second == specialFood.second) {
                occupied = true;
                break;
            }
        }
        if (specialFood.first == food.first && specialFood.second == food.second) {
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

int Game::getObstacleTimeRemaining() const {
    if (!obstaclesActive) return 0;
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(now - obstacleSpawnTime).count();
    return max(0, OBSTACLE_DURATION - (int)elapsed);
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
            if (gameOver && y == HEIGHT/2 - 1 && x == WIDTH/2 - 4) {
                screen.addToBuffer("G A M E  O V E R");
                x += 7;
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
                else if (x == food.first && y == food.second && !gameOver) {
                    cellContent = FOOD_EMOJI;
                }
                else {
                    for (size_t i = 1; i < snake.getBody().size(); i++) {
                        if (x == snake.getBody()[i].first && y == snake.getBody()[i].second) {
                            if (gameOver) {
                                cellContent = SNAKE_BODY_DEAD;
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

    screen.addToBuffer("Score: " + to_string(score) + " 🏆 | Snake Length: " + to_string(snake.getLength()) + "\n");
    screen.addToBuffer("Speed: " + to_string(getGameSpeed()) + "ms | Obstacles: " + (obstaclesActive ? to_string(getObstacleTimeRemaining()) + "s 🚧" : "Clear ") + " | Controls: WASD/Arrows | Q to Quit\n");
    
    if (gameOver) {
        screen.addToBuffer("💀 GAME OVER! Press any key to exit... 💀\n");
    }

    screen.draw();
}

void Game::update() {
    if (gameOver) return;

    snake.move();
    updateSpecialFood();
    updateObstacles();

    pair<int, int> head = snake.getHead();

    if (head.first < 0 || head.first >= WIDTH || 
        head.second < 0 || head.second >= HEIGHT) {
        gameOver = true;
        wallCrash = true;
        crashPosition = head;
        
        if (head.first < 0) crashPosition = make_pair(-1, head.second);
        else if (head.first >= WIDTH) crashPosition = make_pair(WIDTH, head.second);
        else if (head.second < 0) crashPosition = make_pair(head.first, -1);
        else if (head.second >= HEIGHT) crashPosition = make_pair(head.first, HEIGHT);
        
        return;
    }

    for (size_t i = 1; i < snake.getBody().size(); i++) {
        if (head.first == snake.getBody()[i].first && 
            head.second == snake.getBody()[i].second) {
            gameOver = true;
            return;
        }
    }
    
    if (obstaclesActive) {
        for (const auto& obs : obstacles) {
            if (head.first == obs.first && head.second == obs.second) {
                gameOver = true;
                return;
            }
        }
    }

    if (head.first == food.first && head.second == food.second) {
        score += 10;
        snake.setGrow(true, 1);
        foodEaten++;
        spawnFood();
        
        if (foodEaten % 4 == 0) {
            spawnSpecialFood();
        }
        if (foodEaten % 5 == 0) {
            spawnObstacles();
        }
    }

    if (specialFoodActive && head.first == specialFood.first && head.second == specialFood.second) {
        score += 30;
        snake.setGrow(true, 3);
        specialFoodActive = false;
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
