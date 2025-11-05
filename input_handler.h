#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

// Platform-specific include
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif

class InputHandler {
private:
#ifdef _WIN32
    // Windows implementation
#else
    // Linux/macOS implementation
    static struct termios oldt, newt;
    static bool rawModeEnabled;
#endif

public:
    static void enableRawInput();
    static void disableRawInput();
    static bool isKeyPressed();
    static int getChar();
};

#endif
