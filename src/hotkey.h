#include<windows.h>

#include "screen.h"
#include "workspace.h"

#ifndef HOTKEY_H 
#define HOTKEY_H 

struct Hotkey{
    void switchToWorkspace(int wsp, Workspace *workspace);
    void moveWindowToWorkspace(int wsp, Workspace *workspace);
    bool handleKeyPress(DWORD keycode, Workspace *workspace);
};

#endif