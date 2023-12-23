#include <iostream>
#include <windows.h>
#include <list>
#include "window.h"
#include "config.h"
#include "state.h"

#ifndef SCREEN_H
#define SCREEN_H

// TODO: This was part of layouts.h, but had to move it here to avoid circular reference
enum LayoutType {
    LAYOUT_TYPE_NONE = 1,
    LAYOUT_TYPE_STACKED,
    LAYOUT_TYPE_SPLIT,
    LAYOUT_TYPE_CENTERED,
};

struct Screen {
    LayoutType layoutType;
    Config config;
    State state;
    std::list<HWND> windowIndexes;
    std::list<Window> windows;
    Window focusedWindow;
    int screenWidth;
    int screenHeight;

    void initialize(int screenWidth, int screenHeight);
    void addWindow(Window window);  
    void onBeforeWindowsRegistered();
    void setFocusedWindow(Window window);
    void onAfterWindowsRegistered();
    void setActiveLayout(LayoutType layout);
    
    void moveFocusLeft();
    void moveFocusRight();

    void moveFocusedWindowLeft();
    void moveFocusedWindowRight();

    Window getWindow(HWND hwnd);

    void closeFocusedWindow();
};

#endif