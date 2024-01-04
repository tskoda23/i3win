#include<windows.h>
#include<iostream>
#include "hotkey.h"
#include "screen.h"
#include "state.h"
#include "layouts.h"
#include "workspace.h"

using namespace std;

void Hotkey::switchToWorkspace(int wsp, Workspace workspace){
    workspace.activeScreen->hideWindows();

    workspace.setActiveWsp(wsp);

    workspace.activeScreen->showWindows();

    buildLayout(workspace.activeScreen);
}

void Hotkey::moveWindowToWorkspace(int wsp, Workspace workspace){
    if(workspace.activeWsp != wsp){
        Window win = workspace.activeScreen->focusedWindow;
        win.hide();
        workspace.activeScreen->removeWindow(win.hwnd);
        Window newWin = getWindowFromHWND(win.hwnd);
        workspace.screens[wsp].addWindow(newWin);
        buildLayout(workspace.activeScreen);
    }
}

bool Hotkey::handleKeyPress(DWORD keycode, Workspace workspace){
    string message;
    bool hotkeyPressed = false;
    int keycodenum = keycode - '0';

    if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {// Mod + Shift + Key
        
        printf("Switch to workspace %c", keycode);

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
                break;
        }
        message = "Alt + Shift + " + keycode;
        hotkeyPressed = true;
    }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000)) { // Mod + key
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
                workspace.activeScreen->moveFocusLeft();
                workspace.activeScreen->moveFocusRight();
                break;
            case '0':
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
                break;
        }
        message = "Alt + " + keycode;
        hotkeyPressed = true;
    }

    if(hotkeyPressed){
        if(message != ""){
            cout << message << endl;
        }
    }

    //printf("Key Pressed: 0x%X\n", keycode);
    printf("Key Pressed: %c\n", keycode);


    return hotkeyPressed;
}