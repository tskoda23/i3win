#include "layouts.h"
#include "window.h"

#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <set>
#include <iostream>

void buildStackedLayout(std::set<Window> windows, int screenWidth, int screenHeight) {
    int windowCount = 0;
    int padding = 50;

    for (const Window& window : windows)
    {
        int spacing = padding * windowCount;

        MoveWindow(window.hwnd,
            0,                                // X
            spacing,                          // Y
            screenWidth,                    // Width
            screenHeight - spacing,         // Height
            TRUE);

        windowCount++;
    }
}

void buildSplitLayout(std::set<Window> windows, int screenWidth, int screenHeight) {
    int windowsCount = windows.size();

    int windowWidth = screenWidth / windowsCount;

    int windowCount = 0;

    for (const Window& window : windows)
    {
        std::cout << "Moving ===> " << window.exeName << std::endl;

        bool isWindowMovedSuccessfully = MoveWindow(
            window.hwnd,
            windowCount * windowWidth,      // X
            0,                              // Y
            windowWidth,                    // Width
            screenHeight,                 // Height
            TRUE);

        if (!isWindowMovedSuccessfully) {
            DWORD error = GetLastError();
            printf("SYSTEM ERROR CODE %d", error);
        }
        else {
            windowCount++;
        }
    }
}