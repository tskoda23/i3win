#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <set>
#include "window.h"
#include "screen.h"

void Screen::initialize(LayoutType layoutType, int screenWidth, int screenHeight) {
    this->layoutType = layoutType;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

int counter = 0;

void Screen::addWindow(Window window) {
    bool doesWindowAlreadyExist = windowToPositionMap.find(window.hwnd) != windowToPositionMap.end();

    if (doesWindowAlreadyExist) {
        /*std::cout << "Exists:" << std::endl;*/
    } else {
        int currentPosition = windowToPositionMap.size();
        windowToPositionMap[window.hwnd] = currentPosition;
        positionToWindowMap[currentPosition] = window.hwnd;
    }

    windows[window.hwnd] = window;

    counter++;
}

// After a window is removed, few things happen:
// - focused window might be removed, so we need to set it to another one
// - leftover indexes remain, so we need to remove them
void Screen::normalizeScreenState() {
    bool isFocusedWindowKnown = windows.find(focusedWindow.hwnd) != windows.end();

    if (!isFocusedWindowKnown) {
        int previouslyFocusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];
        focusedWindow = getWindowAtPosition(previouslyFocusedWindowIndex - 1);
        setFocusedWindow(focusedWindow);
    }

    std::set<std::pair<int, HWND>> itemsToRemove;

    for (auto item : positionToWindowMap) {
        int position = item.first;
        HWND hwnd = item.second;

        bool doesWindowStillExist = windows.find(hwnd) != windows.end();

        if (!doesWindowStillExist) {
            itemsToRemove.insert(item);
        }
    }

    for (auto item : itemsToRemove) {
        std::cout << "Removing: " << item.first << " - " << std::endl;

        positionToWindowMap.erase(item.first);
        windowToPositionMap.erase(item.second);
    }
}

void Screen::reset() {
    windows.clear();
}

void Screen::setFocusedWindow(Window window) {
    if (window.hwnd != NULL) {
        focusedWindow = window;
        SetForegroundWindow(focusedWindow.hwnd); // TODO: Maybe move this out of this struct?
    }
    else {
        printf("ERROR: Focused window is null\n");
    }
}

void Screen::setActiveLayout(LayoutType layout) {
    layoutType = layout;
}

void Screen::moveFocusLeft(){
    normalizeScreenState();

    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindow(
            getWindowAtPosition(0)
        );
    }

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex > 0) {
        setFocusedWindow(
            getWindowAtPosition(focusedWindowIndex - 1)
        );
    } else {
        printf("Can't go further left.\n");
    }
}

void Screen::moveFocusRight(){
    normalizeScreenState();

    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindow(
            getWindowAtPosition(0)
        );
    }

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex < windows.size() - 1) {
        setFocusedWindow(
            getWindowAtPosition(focusedWindowIndex + 1)
        );
    } else {
        printf("Can't go further right.\n");
    }
}

Window Screen::getWindowAtPosition(int position) {
    HWND hwnd = positionToWindowMap[position];

    return windows[hwnd];
}

void Screen::moveFocusedWindowRight() {
    normalizeScreenState();

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex == windows.size() - 1) {
        printf(" -- can't move it, already at the edge\n");
        return;
    } else {
        positionToWindowMap[focusedWindowIndex] = positionToWindowMap[focusedWindowIndex + 1];
        positionToWindowMap[focusedWindowIndex + 1] = focusedWindow.hwnd;

        windowToPositionMap[focusedWindow.hwnd] = focusedWindowIndex + 1;
        windowToPositionMap[positionToWindowMap[focusedWindowIndex]] = focusedWindowIndex;
    }
}

void Screen::moveFocusedWindowLeft() {
    normalizeScreenState();

    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex < 1) {
        printf(" -- can't move it, already at the edge\n");
        return;
    } else {
        positionToWindowMap[focusedWindowIndex] = positionToWindowMap[focusedWindowIndex - 1];
        positionToWindowMap[focusedWindowIndex - 1] = focusedWindow.hwnd;

        windowToPositionMap[focusedWindow.hwnd] = focusedWindowIndex - 1;
        windowToPositionMap[positionToWindowMap[focusedWindowIndex]] = focusedWindowIndex;
    } 
}

void Screen::closeFocusedWindow() {
    LRESULT res = ::SendMessage(focusedWindow.hwnd, WM_CLOSE, NULL, NULL);
    int position = windowToPositionMap[focusedWindow.hwnd];

    windowToPositionMap.erase(focusedWindow.hwnd);
    positionToWindowMap.erase(position);
}