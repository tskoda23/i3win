#include <windows.h>
#include <iostream>
#include "hotkey.h"
#include "state.h"
#include "layouts.h"
#include "screen.h"
#include "workspace_manager.h"

using namespace std;

bool Hotkey::handleKeyPress(DWORD keycode, WorkspaceManager &workspaceManager) {
    bool hotkeyPressed = false;
    int keycodenum = keycode - '0';

    auto activeWorkspace = workspaceManager.getActiveWorkspace();

    if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000))) {
        hotkeyPressed = true;
        switch(keycode) {
            case 'Q':
                activeWorkspace->closeFocusedWindow();
                break;
            case VK_LEFT:
                activeWorkspace->moveFocusedWindowLeft();
                buildLayout(*activeWorkspace);
                break;
            case VK_RIGHT:
                activeWorkspace->moveFocusedWindowRight();
                buildLayout(*activeWorkspace);
                break;
            case '0': {
                workspaceManager.moveActiveWindowToWorkspace(9);
                buildLayout(*activeWorkspace);
                break;
            }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': 
                workspaceManager.moveActiveWindowToWorkspace(keycodenum - 1);
                buildLayout(*activeWorkspace);
                break;
            default:
                hotkeyPressed = false;
                break;
        }
    } else if (((GetAsyncKeyState(VK_RMENU) & 0x8000))) {
        hotkeyPressed = true;
        switch(keycode){
            case 'A':
                activeWorkspace->setActiveLayout(LAYOUT_TYPE_NONE);
                buildLayout(*activeWorkspace);
                break;
            case 'B':
                activeWorkspace->setActiveLayout(LAYOUT_TYPE_SPLIT);
                buildLayout(*activeWorkspace);
                break;
            case 'C':
                activeWorkspace->setActiveLayout(LAYOUT_TYPE_CENTERED);
                buildLayout(*activeWorkspace);
                break;
            case VK_LEFT:
                activeWorkspace->moveFocusLeft();
                break;
            case VK_RIGHT:
                activeWorkspace->moveFocusRight();
                break;
            case '0': {
                int workspaceIndex = 9;
                printf("Switch to workspace %d\n", workspaceIndex);
                workspaceManager.setActiveWorkspace(workspaceIndex);
                break;
            }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                int workspaceIndex = keycodenum - 1;
                workspaceManager.setActiveWorkspace(workspaceIndex);
                break;
            }
            default:
                hotkeyPressed = false;
                break;
        }
    }

    return hotkeyPressed;
}
