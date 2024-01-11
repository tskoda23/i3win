#include <windows.h>

#include "workspace_manager.h"

#ifndef HOTKEY_H 
#define HOTKEY_H 

struct Hotkey {
    bool handleKeyPress(DWORD keycode, WorkspaceManager &workspaceManager);
};

#endif