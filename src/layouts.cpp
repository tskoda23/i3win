#include "layouts.h"
#include "screen.h"
#include "window.h"
#include "logger.h"

#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <vector>
#include <iostream>

const int taskbarSize = 50;

int getMainWindowWidthPercentage(Screen screen) {
    int centerWindowWidthFromConfig = screen.config.getNumericValue(MAIN_WINDOW_WIDTH_PERCENTAGE);
    int centerWindowWidthChange = screen.state.getNumericValue(MAIN_WINDOW_PERCENTAGE_CHANGE);

    int percentage = centerWindowWidthFromConfig + centerWindowWidthChange;

    if (percentage < 0) {
        logError("Main window width percentage can't be less than 0");
        percentage = 10;
    } else if (percentage > 100) {
        logError("Main window width percentage can't be more than 100");
        percentage = 100;
    }

    return percentage;
}

void buildStackedLayout(Screen screen) {
    int windowsRendered = 0;
    int margin = 50;

    for (auto window : screen.windows) {
        int startMargin = margin * windowsRendered;
        int endMargin = margin * (screen.windows.size() - windowsRendered - 1);

        bool isWindowMovedSuccessfully = window.move(
            startMargin,
            0,
            screen.screenWidth - startMargin - endMargin,
            screen.screenHeight - taskbarSize);

        if (isWindowMovedSuccessfully) {
            windowsRendered++;
        }
    }
}

void buildSplitLayout(Screen screen) {
    int additionalPadding = screen.config.getNumericValue(ADDITIONAL_WINDOW_PADDING);
    int totalPadding = additionalPadding * (screen.windows.size() - 1);

    int mainWindowWidth = screen.screenWidth * getMainWindowWidthPercentage(screen) / 100;
    int otherWindowsWidth = (screen.screenWidth - mainWindowWidth - totalPadding) / (screen.windows.size() - 1);

    int windowCount = 0;

    for (auto window : screen.windows) {
        bool isMainWindow = windowCount == 0;

        int xPosition = isMainWindow
            ? 0
            : mainWindowWidth 
                // width of all windows before this one
                + (windowCount - 1) * otherWindowsWidth
                // plus all the padding between windows
                + (windowCount * additionalPadding);

        int width = isMainWindow
            ? mainWindowWidth
            : otherWindowsWidth;

        bool isWindowMovedSuccessfully = window.move(
            xPosition,
            0,
            width,
            screen.screenHeight - taskbarSize);

        if (isWindowMovedSuccessfully) {
            windowCount++;
        }
    }
}

void buildCenteredLayout(Screen screen) {
    int windowsCount = screen.windows.size();

    int centerWindowWidth = screen.screenWidth * getMainWindowWidthPercentage(screen) / 100;

    int windowCount = 0;

    for (auto window : screen.windows) {

        int xPosition, yPosition, width, height;

        int sideWindowWidth = (screen.screenWidth - centerWindowWidth) / 2;
        int centerWindowHeight = screen.screenHeight - taskbarSize;

        if (windowCount == 0) {
            window.move(
                sideWindowWidth,
                0,
                centerWindowWidth,
                centerWindowHeight);
        } else {
            int numberOfSideWindows = windowsCount - 1;
            int isLeftSideWindow = windowCount <= (screen.windows.size() / 2);

            int numberOfWindowsOnThisSide = isLeftSideWindow
                ? windowsCount / 2
                // We have -1 here because we want this side to have 
                // one window less than the other one in case of uneven number
                : (windowsCount - 1) / 2;

            int indexOfWindowOnASide = (windowCount - 1) % numberOfWindowsOnThisSide;
            int sideWindowHeight = centerWindowHeight / numberOfWindowsOnThisSide; 

            float yPosition = indexOfWindowOnASide * sideWindowHeight;

            int xPosition = isLeftSideWindow
                ? 0 
                : sideWindowWidth + centerWindowWidth;

            window.move(
                xPosition,
                yPosition,
                sideWindowWidth,
                sideWindowHeight);
        }

        windowCount++;
    }
}

void buildLayout(Screen screen) {
    switch (screen.layoutType)
    {
    case LAYOUT_TYPE_NONE:
        break;
    case LAYOUT_TYPE_STACKED:
        buildStackedLayout(screen);
        break;
    case LAYOUT_TYPE_SPLIT:
        buildSplitLayout(screen);
        break;
    case LAYOUT_TYPE_CENTERED:
        buildCenteredLayout(screen);
        break;
    default:
        logError("Unknown layout type, doing nothing.");
        break;
    }
}