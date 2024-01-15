#include <windows.h>
#include <iostream>
#include "hotkey.h"
#include "state.h"
#include "layouts.h"
#include "screen.h"
#include "logger.h"
#include "workspace_manager.h"

using namespace std;

int KEY_PRESSED = 0x8000;
bool HOTKEY_DETECTED = true;

std::unordered_map<std::string, int> keys = {
    {"left", VK_LEFT},
    {"right", VK_RIGHT},
    {"up", VK_UP},
    {"down", VK_DOWN},

    {"alt", VK_MENU},
    {"ralt", VK_RMENU},
    {"lalt", VK_LMENU},

    {"shift", VK_SHIFT},
    {"rshift", VK_RSHIFT},
    {"lshift", VK_LSHIFT},
    
    {"ctrl", VK_CONTROL},
    {"rctrl", VK_RCONTROL},
    {"lctrl", VK_LCONTROL},

    {"caps", VK_CAPITAL}
};

bool keyPressed(DWORD keyCode, string key) {
    if (key.empty()) {
        logError("Key length is 0");
        throw "Key length is 0";
    }

    // If it's one-char letter/number just convert it to char, otherwise it has to be special key, 
    // so take it from the keys map
    char c = key.size() == 1 
        ? key[0]
        : keys[key];

    return keyCode == toupper(c);
}

bool Hotkey::handleKeyPress(DWORD keycode, WorkspaceManager &workspaceManager) {
    int keycodenum = keycode - '0';
    bool hotkeyDetected = true;

    auto activeWorkspace = workspaceManager.getActiveWorkspace();
    auto config = workspaceManager.config;

    int mainKey = keys[config.getValue(COMMAND_MAIN_KEY)];
    int modifierKey = keys[config.getValue(COMMAND_MODIFIER_KEY)];

    if (GetAsyncKeyState(mainKey) && GetAsyncKeyState(modifierKey) & KEY_PRESSED) {
        // Moving windows across workspaces
        if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_1))) {
            workspaceManager.moveActiveWindowToWorkspace(1);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_2))) {
            workspaceManager.moveActiveWindowToWorkspace(2);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_3))) {
            workspaceManager.moveActiveWindowToWorkspace(3);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_4))) {
            workspaceManager.moveActiveWindowToWorkspace(4);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_5))) {
            workspaceManager.moveActiveWindowToWorkspace(5);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_6))) {
            workspaceManager.moveActiveWindowToWorkspace(6);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_7))) {
            workspaceManager.moveActiveWindowToWorkspace(7);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_8))) {
            workspaceManager.moveActiveWindowToWorkspace(8);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_9))) {
            workspaceManager.moveActiveWindowToWorkspace(9);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_TO_WORKSPACE_10))) {
            workspaceManager.moveActiveWindowToWorkspace(0);
            buildLayout(*activeWorkspace);
        } else {
           return !HOTKEY_DETECTED;
        }

        return HOTKEY_DETECTED;

    } else if (GetAsyncKeyState(mainKey) & KEY_PRESSED) {

        // Window management
        if (keyPressed(keycode, config.getValue(COMMAND_SWITCH_FOCUS_LEFT))) {
            activeWorkspace->moveFocusLeft();
        } else if (keyPressed(keycode, config.getValue(COMMAND_SWITCH_FOCUS_RIGHT))) {
            activeWorkspace->moveFocusRight();
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_LEFT))) {
            activeWorkspace->moveFocusedWindowLeft();
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MOVE_WINDOW_RIGHT))) {
            activeWorkspace->moveFocusedWindowRight();
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_CLOSE_WINDOW))) {
            activeWorkspace->closeFocusedWindow();
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_AS_MAIN_WINDOW))) {
            activeWorkspace->setAsMainWindow();
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MAIN_WINDOW_SIZE_INCREASE))) {
            activeWorkspace->changeMainWindowSize(+5);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_MAIN_WINDOW_SIZE_DECREASE))) {
            activeWorkspace->changeMainWindowSize(-5);
            buildLayout(*activeWorkspace);
        }

        // Layouts
        else if (keyPressed(keycode, config.getValue(COMMAND_SET_LAYOUT_NONE))) {
            activeWorkspace->setActiveLayout(LAYOUT_TYPE_NONE);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_LAYOUT_SPLIT))) {
            activeWorkspace->setActiveLayout(LAYOUT_TYPE_SPLIT);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_LAYOUT_STACKED))) {
            activeWorkspace->setActiveLayout(LAYOUT_TYPE_STACKED);
            buildLayout(*activeWorkspace);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_LAYOUT_CENTERED))) {
            activeWorkspace->setActiveLayout(LAYOUT_TYPE_CENTERED);
            buildLayout(*activeWorkspace);
        }
       
        // Workspace management
        else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_1))) {
            workspaceManager.setActiveWorkspace(1);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_2))) {
            workspaceManager.setActiveWorkspace(2);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_3))) {
            workspaceManager.setActiveWorkspace(3);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_4))) {
            workspaceManager.setActiveWorkspace(4);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_5))) {
            workspaceManager.setActiveWorkspace(5);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_6))) {
            workspaceManager.setActiveWorkspace(6);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_7))) {
            workspaceManager.setActiveWorkspace(7);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_8))) {
            workspaceManager.setActiveWorkspace(8);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_9))) {
            workspaceManager.setActiveWorkspace(9);
        } else if (keyPressed(keycode, config.getValue(COMMAND_SET_WORKSPACE_10))) {
            workspaceManager.setActiveWorkspace(0);
        } 

        // Misc
        else if (keyPressed(keycode, config.getValue(COMMAND_RELOAD_CONFIG))) {
            workspaceManager.reloadConfig();
        }

        else {
            return !HOTKEY_DETECTED;
        }

        return HOTKEY_DETECTED;
    }
    return !HOTKEY_DETECTED;
}
