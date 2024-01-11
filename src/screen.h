#include <iostream>
#include <windows.h>
#include <list>

#include "window.h"
#include "config.h"
#include "state.h"
#include "workspace.h"

#ifndef SCREEN_H
#define SCREEN_H

struct Screen {
    Screen(Config &config, State &state, int screenWidth, int screenHeight);
    
    Config config;
    State state;
    int screenWidth;
    int screenHeight;
    
    Workspace *activeWorkspace;

    void setActiveWorkspace(Workspace &workspace);
    void reloadConfig();

    void hideWindows();
    void showWindows();
};

#endif
