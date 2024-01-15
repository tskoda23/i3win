#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <future>

#include "window.h"
#include "screen.h"
#include "logger.h"
#include "hotkey.h"
#include "environment.h"
#include "state.h"
#include "layouts.h"
#include "workspace_manager.h"
#include "workspace.h"

using namespace std;

HHOOK g_keyboardHook;

Hotkey hotkey;
WorkspaceManager workspaceManager = WorkspaceManager();

mutex windowStateMutex;

bool debug = true;
bool showRealTimeState = false;

BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,&isCloaked, sizeof(isCloaked))) && isCloaked;
}

BOOL ShouldRegisterWindow(Window& window) {
    if (!IsWindowVisible(window.hwnd) || !IsWindow(window.hwnd) || IsIconic(window.hwnd)
        || IsWindowCloaked(window.hwnd) || window.isHidden())
    {
        return false;
    }

    bool canWindowBeResized = (GetWindowLong(window.hwnd, GWL_STYLE) & WS_THICKFRAME) != 0;

    return canWindowBeResized
        && window.className != "Progman"; // Some Windows OS thing
}

void registerNewWindow(Window& window) {
    // pass WM_DISPLAYCHANGE if resolution changed
    if (ShouldRegisterWindow(window))
    {
        workspaceManager.getActiveWorkspace()->addWindow(window);
    }
}

bool IsMainWindow(HWND hwnd) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    return ((style & WS_CHILD) == 0) && (GetWindow(hwnd, GW_OWNER) == nullptr) && IsWindowVisible(hwnd);
}

bool hasTitle(HWND hwnd){
    int length = ::GetWindowTextLength(hwnd);
    return length > 0;
}

BOOL CALLBACK handleWindow(HWND hwnd, LPARAM lParam) {
    HWND fWinH = GetForegroundWindow();
    Window newFwin = getWindowFromHWND(fWinH);
    newFwin.isInitialized = 1;

    // Exit early if window is popup or something not a real window
    if (!IsMainWindow(hwnd) || !hasTitle(hwnd)) {
        return TRUE;
    }

    Window window = getWindowFromHWND(hwnd);

    registerNewWindow(window);

    auto activeWorkspace = workspaceManager.getActiveWorkspace();

    if(activeWorkspace->focusedWindow.isInitialized == 0
        || activeWorkspace->focusedWindow.hwnd != newFwin.hwnd) {
        activeWorkspace->setFocusedWindow(newFwin);
    }

    return TRUE;
}

int CATCH_KEYBOARD_EVENT = -1;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            bool hotkeyPressed = false;

            // Lock guard's destructor will unlock the mutex automatically at the end of block

            lock_guard<mutex> lock(windowStateMutex);
            DWORD keycode = pKeyInfo->vkCode;

            hotkeyPressed = hotkey.handleKeyPress(keycode, workspaceManager);

            if(hotkeyPressed){
                return CATCH_KEYBOARD_EVENT;
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void printWindowState(const Workspace &workspace) {
    system("cls");

    auto workspaces = workspaceManager.getAllWorkspaces();

    for_each(workspaces.begin(), workspaces.end(), [](Workspace* workspace) {
         if (workspace->windows.size() == 0) {
             return;
         }

        cout << "Workspace " << workspace->workspaceIndex << endl;

        int position = 0;
        for (auto window : workspace->windows) {
            cout << "  " << position++ << " - " << window.title;

            if (window.hwnd == workspace->focusedWindow.hwnd) {
                cout << " - FOCUSED";
            }

            cout << endl;
        }
    });
}

void checkWindowState() {
    while (true) {
        unique_lock<mutex> lock(windowStateMutex);

        workspaceManager.getActiveWorkspace()->onBeforeWindowsRegistered();

        EnumWindows(handleWindow, NULL);

        workspaceManager.getActiveWorkspace()->onAfterWindowsRegistered();

        buildLayout(*workspaceManager.getActiveWorkspace());

        if (showRealTimeState) {
            printWindowState(*workspaceManager.getActiveWorkspace());
        }

        lock.unlock();

        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    if (g_keyboardHook == NULL) {
        cerr << "Error setting up low level keyboard hook" << endl;
        return 1;
    }

    logInfo("***    Window manager started ***");

    // Check window state each second and update layout
    future<void> asyncResult = async(launch::async, checkWindowState);

    // Message loop or other application logic goes here
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove hooks after done
    UnhookWindowsHookEx(g_keyboardHook);

    return 0;
}