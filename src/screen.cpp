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

Screen::Screen(Config &config, State &state, int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->config = config;
    this->state = state;
}

void Screen::setActiveWorkspace(Workspace &workspace) {
    activeWorkspace = &workspace;
}

void Screen::reloadConfig() {
    config.reload();
    logInfo("Config reloaded.");
}
