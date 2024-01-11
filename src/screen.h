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
    Screen(int screenWidth, int screenHeight);
    
    int screenWidth;
    int screenHeight;
    
    Workspace *activeWorkspace;

    void setActiveWorkspace(Workspace &workspace);
};

#endif
