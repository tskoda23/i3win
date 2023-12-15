#include <iostream>
#include <windows.h>
#include <unordered_map>
#include "window.h"
#include "screen.h"

void Screen::initialize(LayoutType layoutType, int screenWidth, int screenHeight) {
    this->layoutType = layoutType;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

void Screen::addWindow(Window window) {
    bool doesWindowAlreadyExist = windowToPositionMap.find(window.hwnd) != windowToPositionMap.end();

    // if (doesWindowAlreadyExist) {
    //     /*std::cout << "Exists:" << std::endl;*/
    // } else {
        windowToPositionMap[window.hwnd] = windows.size();
        positionToWindowMap[windows.size()] = window.hwnd;
    // }

    windows[window.hwnd] = window;
}

void Screen::reset() {
     /*positionToWindowMap.clear();
    windowToPositionMap.clear();*/
    windows.clear();
}

void Screen::setFocusedWindow(Window window) {
    focusedWindow = window;
    SetForegroundWindow(focusedWindow.hwnd); // TODO: Maybe move this out of this struct?
}

void Screen::setActiveLayout(LayoutType layout) {
    layoutType = layout;
}

void Screen::moveFocusLeft(){
    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex > 0) {
        setFocusedWindow(
            getWindowAtPosition(focusedWindowIndex - 1)
        );
    } else {
        printf("Can't go further.");
    }
}

void Screen::moveFocusRight(){
    auto focusedWindowIndex = windowToPositionMap[focusedWindow.hwnd];

    if (focusedWindowIndex < windows.size() - 1) {
        setFocusedWindow(
            getWindowAtPosition(focusedWindowIndex + 1)
        );
    } else {
        printf("Can't go further.");
    }
}

Window Screen::getWindowAtPosition(int position) {
    HWND hwnd = positionToWindowMap[position];

    return windows[hwnd];
}

void Screen::moveFocusedWindowRight() {

    std::cout << "Focused is " << focusedWindow.title << std::endl;

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
    std::cout << "Focused is " << focusedWindow.title << std::endl;

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