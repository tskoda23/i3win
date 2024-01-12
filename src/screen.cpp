#include <iostream>
#include <windows.h>
#include <set>
#include <list>
#include <algorithm>
#include <iterator>

#include "window.h"
#include "screen.h"
#include "logger.h"
#include "state.h"

Screen::Screen(int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

void Screen::setActiveWorkspace(Workspace &workspace) {
    activeWorkspace = &workspace;
}
