#include <iostream>
#include <thread>
#include <chrono>
#include "game.h"

using namespace std;

int main() {
    cout << "Starting Cross-Platform Snake Game..." << endl;
    cout << "Make sure your terminal supports emojis!" << endl;
    cout << "Starting in 2 seconds..." << endl;
    
    this_thread::sleep_for(chrono::seconds(2));
    
    // Clear screen properly at start
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    
    // Enable raw input
    InputHandler::enableRawInput();
    
    Game game;
    
    // Main game loop
    while (!game.shouldQuit()) {
        game.draw();
        game.handleInput();
        
        if (!game.isGameOver()) {
            game.update();
        }
        
        // Control game speed with dynamic speed based on snake length
        int gameSpeed = game.getGameSpeed();
        this_thread::sleep_for(chrono::milliseconds(gameSpeed));
    }
    
    // If game over, show the final screen
    if (game.isGameOver()) {
        game.draw();
        // Wait for any key press before exiting
        while (!InputHandler::isKeyPressed()) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        InputHandler::getChar(); // Clear the key press
    }
    
    // Disable raw input
    InputHandler::disableRawInput();
    
    return 0;
}
