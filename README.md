# Snake_Byte

🐍 Cross-Platform Snake Game
A feature-rich, cross-platform Snake game built with C++ that supports both Windows and Linux/macOS systems with emoji graphics and dynamic gameplay elements.

# 👥 Contributors

Prajapati Darshankumar (202512026) 

Jayswal Mayank (202512093) 

Harsh Abhichandani (202512027)

# 🎮 Features

Core Gameplay
Classic Snake mechanics with smooth controls

Dynamic speed system - game gets faster as snake grows

Score tracking with bonus points for special food

Wall collision detection with visual crash effects

Self-collision detection

# Enhanced Features

Special Food System 🍇🍌🍋

Spawns every 4 regular foods

Grants 3x growth and 30 points

Disappears after 10 seconds

Rotating special food emojis

# Obstacle System 🚧

Spawns every 5 foods

5 randomly placed obstacles

Lasts for 8 seconds with countdown timer

Adds strategic challenge

Custom Graphics Support

Load custom emojis/text from files

Fallback to default emoji graphics

Supports: head.txt, body.txt, special_food.txt

# Technical Features

Cross-platform compatibility (Windows/Linux/macOS)

Raw input handling for responsive controls

Terminal/Console graphics using emojis

Dynamic game speed based on snake length

Clean object-oriented architecture

# 🛠️ Installation & Compilation

Prerequisites
C++17 compatible compiler

Terminal/Console that supports UTF-8 and emojis

# Compilation Instructions
Windows
bash

g++ -std=c++17 main.cpp implementation.cpp -o snake_game.exe

Linux/macOS
bash

g++ -std=c++17 main.cpp implementation.cpp -o snake_game

Recommended Compilers
Windows: MinGW-w64, Visual Studio 2019+

Linux: GCC 7+, Clang 5+

macOS: Xcode Command Line Tools

# 🎯 How to Play
Controls
W or ↑ - Move Up

A or ← - Move Left

S or ↓ - Move Down

D or → - Move Right

Q - Quit Game

# Game Rules

Objective: Eat food to grow longer and score points

Regular Food 🍎: +10 points, +1 length

Special Food 🍇: +30 points, +3 length (spawns every 4 foods)

Avoid: Walls, your own body, and temporary obstacles

# Game Over conditions:

Hit the wall

Collide with yourself

Run into obstacles

# Scoring System

Regular food: 10 points

Special food: 30 points

Score display updates in real-time

# 🏗️ Project Structure

``` 
snake_game/
├── main.cpp              # Entry point and game loop
├── game.h               # Game and Snake class declarations
├── input_handler.h      # Cross-platform input handling
├── screen.h            # Console display management
├── implementation.cpp   # All class implementations
├── head.txt            # Custom snake head graphic (optional)
├── body.txt            # Custom snake body graphic (optional)
└── special_food.txt    # Custom special food graphic (optional)

```

# 🎨 Customization

Custom Graphics
Create text files to customize game appearance:

head.txt - Snake head emoji/text

text
🐍
body.txt - Snake body segments

text
🟢
special_food.txt - Special food appearance

text
🍇

# Game Configuration

Modify constants in game.h:

cpp
const int SPECIAL_FOOD_DURATION = 10;  // Seconds special food lasts
const int OBSTACLE_DURATION = 8;       // Seconds obstacles last
const int OBSTACLE_COUNT = 5;          // Number of obstacles
const int baseSpeed = 200;             // Initial game speed (ms)
const int minSpeed = 75;               // Minimum game speed (ms)

# 🚀 Game Mechanics

Speed System
Base Speed: 200ms per frame

Speed Reduction: 9ms per segment

Minimum Speed: 75ms per frame

Formula: max(baseSpeed - (length * speedDecrement), minSpeed)

Food Spawning
Regular Food: Always available

Special Food: Spawns after every 4 regular foods eaten

Obstacles: Spawn after every 5 regular foods eaten

Smart spawning avoids walls, snake body, and other objects

Collision Detection
Wall collisions with visual crash indicator

Self-collision detection

Obstacle collision during active periods

Precise coordinate-based detection system

# 🔧 Technical Details

Cross-Platform Implementation
Windows: Uses conio.h and Windows API

Linux/macOS: Uses termios for raw input mode

Unicode Support: UTF-8 encoding for emoji display

Terminal Control: ANSI escape sequences and platform-specific APIs

# Class Architecture

Game: Main game logic and state management

Snake: Snake behavior and movement

Screen: Console output and display management

InputHandler: Cross-platform input processing

# 🐛 Known Issues & Solutions

Common Problems
Emojis not displaying:

Ensure terminal supports UTF-8

On Windows: Use Windows Terminal or modern console

On Linux: Install proper font packages

# Compilation errors:

Ensure C++17 support is enabled

Link required libraries for your platform

