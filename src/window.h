#include <iostream>
#include <windows.h>

#ifndef WINDOW_H
#define WINDOW_H

struct Window {
    HWND hwnd;
    std::string title;
    std::string className;
    std::string exeName;
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
    int isInitialized = 0;

    bool isHidden();
    bool move(int x, int y, int width, int height);
    void hide();
    void show();
};

bool operator<(const Window& lhs, const Window& rhs);

Window getWindowFromHWND(HWND hwnd);

#endif