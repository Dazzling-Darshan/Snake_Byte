#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

// Platform-specific includes
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif
