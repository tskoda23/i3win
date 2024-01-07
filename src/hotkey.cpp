#include<windows.h>
#include<iostream>
#include "hotkey.h"
#include "screen.h"
#include "state.h"
#include "layouts.h"
#include "workspace.h"

using namespace std;

void Hotkey::switchToWorkspace(int wsp, Workspace workspace){
    if(workspace.activeWsp != wsp){
        printf("Switch to workspace %d\n", wsp);
        workspace.activeScreen->hideWindows();
        workspace.setActiveWsp(wsp);
        workspace.activeScreen->showWindows();
        buildLayout(workspace.activeScreen);
    }else{
        printf("Already on the same workspace %d\n", wsp);
    }
}

void Hotkey::moveWindowToWorkspace(int wsp, Workspace workspace){
    if(workspace.activeWsp != wsp){
        printf("Move window to workspace %d\n", wsp);
        Window win = workspace.activeScreen->focusedWindow;
        win.hide();
        workspace.activeScreen->removeWindow(win.hwnd);
        workspace.screens[wsp].addWindow(win);
        buildLayout(workspace.activeScreen);
    }else{
        printf("Already on the same workspace %d\n", wsp);
    }
}

bool Hotkey::handleKeyPress(DWORD keycode, Workspace workspace){
    bool hotkeyPressed = false;
    int keycodenum = keycode - '0';

    if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000))) {
        hotkeyPressed = true;
        switch(keycode){
            case 'Q':
                workspace.activeScreen->closeFocusedWindow();
                break;
            case VK_LEFT:
                workspace.activeScreen->moveFocusedWindowLeft();
                buildLayout(workspace.activeScreen);
                break;
            case VK_RIGHT:
                workspace.activeScreen->moveFocusedWindowRight();
                buildLayout(workspace.activeScreen);
                break;
            case '0':
                moveWindowToWorkspace(9, workspace);
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                moveWindowToWorkspace(keycodenum - 1, workspace);
                break;
            default:
                hotkeyPressed = false;
                break;
        }
    } else if (((GetAsyncKeyState(VK_RMENU) & 0x8000))) {
        hotkeyPressed = true;
        switch(keycode){
            case 'A':
                workspace.activeScreen->setActiveLayout(LAYOUT_TYPE_STACKED);
                buildLayout(workspace.activeScreen);
                break;
            case 'B':
                workspace.activeScreen->setActiveLayout(LAYOUT_TYPE_SPLIT);
                buildLayout(workspace.activeScreen);
                break;
            case 'C':
                workspace.activeScreen->setActiveLayout(LAYOUT_TYPE_CENTERED);
                buildLayout(workspace.activeScreen);
                break;
            case VK_LEFT:
                workspace.activeScreen->moveFocusLeft();
                break;
            case VK_RIGHT:
                workspace.activeScreen->moveFocusRight();
                break;
            case '0':
                switchToWorkspace(9, workspace);
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                switchToWorkspace(keycodenum - 1, workspace);
                break;
            default:
                hotkeyPressed = false;
                break;
        }
    }

    if(hotkeyPressed){
        printf("HOTKEY preseed\n");
    }

    return hotkeyPressed;
}
