#include <iostream>
#include <windows.h>
#include <unordered_map>
#include "window.h"

#ifndef SCREEN_H
#define SCREEN_H

// TODO: This was part of layouts.h, but had to move it here to avoid circular reference
enum LayoutType {
    LAYOUT_TYPE_NONE,
    LAYOUT_TYPE_STACKED,
    LAYOUT_TYPE_SPLIT
};

struct Screen {
    LayoutType layoutType;
    std::unordered_map<int, HWND> positionToWindowMap;
    std::unordered_map<HWND, int> windowToPositionMap;
    std::unordered_map<HWND, Window> windows;
    Window focusedWindow;
    int screenWidth;
    int screenHeight;

    void initialize(LayoutType layoutType, int screenWidth, int screenHeight);
    void addWindow(Window window);  
    void reset();
    void setFocusedWindow(Window window);
    void setActiveLayout(LayoutType layout);
    
    void moveFocusLeft();
    void moveFocusRight();

    void moveFocusedWindowLeft();
    void moveFocusedWindowRight();

    Window getWindowAtPosition(int position);
    void normalizeIndexes();

};

#endif