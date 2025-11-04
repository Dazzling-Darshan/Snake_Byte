#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main() {
    cout << "Starting Cross-Platform Snake Game..." << endl;
    cout << "Make sure your terminal supports emojis!" << endl;
    cout << "Starting in 2 seconds..." << endl;
    
    this_thread::sleep_for(chrono::seconds(2));
    
    return 0;
}
