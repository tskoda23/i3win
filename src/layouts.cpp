#include "layouts.h"
#include "screen.h"
#include "window.h"

#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <vector>
#include <unordered_map>
#include <iostream>

const int padding = 50;
const int taskbarSize = 50;

void buildStackedLayout(Screen screen) {
    int windowCount = 0;

    for (auto item : screen.positionToWindowMap) {
        int position = item.first;
        auto window = screen.windows[item.second];

        int spacing = padding * windowCount;

        MoveWindow(window.hwnd,
            0,                                // X
            spacing,                          // Y
            screen.screenWidth,                    // Width
            screen.screenHeight - spacing,         // Height
            TRUE);

        windowCount++;
    }
}

void buildSplitLayout(Screen screen) {
    int windowsCount = screen.windows.size();

    int windowWidth = screen.screenWidth / windowsCount;

    int windowCount = 0;

    for (auto item : screen.positionToWindowMap) {
        int position = item.first;
        auto window = screen.windows[item.second];

        bool isWindowMovedSuccessfully = MoveWindow(
            window.hwnd,
            windowCount * windowWidth,          // X
            0,                                  // Y
            windowWidth,                        // Width
            screen.screenHeight - taskbarSize,     // Height
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

void buildLayout(Screen screen) {
    switch (screen.layoutType)
    {
    case LAYOUT_TYPE_STACKED:
        buildStackedLayout(screen);
        break;
    case LAYOUT_TYPE_SPLIT:
        buildSplitLayout(screen);
        break;
    default:
        printf("Unknown layout type");
        break;
    }
}