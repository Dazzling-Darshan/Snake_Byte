#include <iostream>
#include <thread>
#include <chrono>
#include "game.h"   // preparing for InputHandler and Game classes

using namespace std;

int main() {
    cout << "Starting Cross-Platform Snake Game..." << endl;
    cout << "Make sure your terminal supports emojis!" << endl;
    cout << "Starting in 2 seconds..." << endl;
    
    this_thread::sleep_for(chrono::seconds(2));

    // --- Clear screen properly at start ---
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    // --- Enable raw input ---
    InputHandler::enableRawInput();

    cout << "Raw input enabled successfully!" << endl;
    cout << "Press any key to exit setup test..." << endl;

    // Wait for a single keypress to test input
    while (!InputHandler::isKeyPressed()) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    InputHandler::getChar();  // consume key press

    // --- Disable raw input before exit ---
    InputHandler::disableRawInput();

    cout << "Exiting setup..." << endl;
    return 0;
}
