#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "layouts.h"
#include "window.h"
#include "screen.h"

using namespace std;

HHOOK g_keyboardHook;
bool debug = true;
int g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
int g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

Screen screen;

// Filter out internal windows OS stuff that's always shown
bool isWindowsClassName(std::string className) {
    return className == "Progman";
}

BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,&isCloaked, sizeof(isCloaked))) && isCloaked;
}

void registerNewWindow(Window window) {
    // pass WM_DISPLAYCHANGE if resolution changed
    // check if it has title
    // 
    if (IsWindowVisible(window.hwnd) && IsWindow(window.hwnd) && !IsWindowCloaked(window.hwnd) && !IsIconic(window.hwnd) && !isWindowsClassName(window.className))
    {
        if (!window.isHidden()) {
            screen.addWindow(window);
        }
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

    if(screen.focusedWindow.isInitialized == 0 || screen.focusedWindow.hwnd != newFwin.hwnd){
        screen.setFocusedWindow(newFwin);
        if(debug){
            std::cout << "FOCUSED IS INIT? " << screen.focusedWindow.isInitialized << " - " << screen.focusedWindow.title << std::endl;
        }
    }

    // Exit early if window is popup or something not a real window
    if (!IsMainWindow(hwnd) || !hasTitle(hwnd)) {
        return TRUE;    
    }

    Window Window = getWindowFromHWND(hwnd);

    registerNewWindow(Window);

    return TRUE;
}

// Listens for any window changes (create/destroy/update), updates the `activeWindows` list
// and prints out the latest state.
void CALLBACK WindowWatcherHookProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD         event,
    HWND          hwnd,
    LONG          idObject,
    LONG          idChild,
    DWORD         dwEventThread,
    DWORD         dwmsEventTime
)
{
    /*if (!IsMainWindow(hwnd) || !IsWindow(hwnd) || IsIconic(hwnd)) {
        return;
    }*/

    /*switch (event)
    {
        case EVENT_OBJECT_CREATE:
            std::cout << "new " << std::endl;
            break;
        case EVENT_OBJECT_HIDE:
            std::cout << "hide " << std::endl;
            break;
        case EVENT_OBJECT_SHOW:
            std::cout << "show " << std::endl;
            break;
        case EVENT_OBJECT_DESTROY:
            std::cout << "destroy " << std::endl;
            break;
        default:
            printf("UNKNOWN EVENT \n");
        break;
    }*/

    screen.reset();
    EnumWindows(handleWindow, NULL);

   if(debug){
        // std::system("cls");
        // std::cout << "Currently active windows: " << activeWindows.size() << std::endl;
        // for (const Window& window : activeWindows)
        // {
        //     std::cout << "  " << window.hwnd << " - " << window.className << " - " << window.title <<  " - " << window.exeName << std::endl;
        //     printf("    %d %d %d %d\n", window.left, window.top, window.right, window.bottom);
        // }

        printf("Focused: %d %d %d %d\n", screen.focusedWindow.left, screen.focusedWindow.top, screen.focusedWindow.right, screen.focusedWindow.bottom);
    }
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std:string message = "";

            if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '1')) {
                message = "Alt + 1 pressed! buildSplitLayout!\n";
                screen.setActiveLayout(LAYOUT_TYPE_SPLIT);
                buildLayout(screen);
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '2')) {
               // message = "Alt + 2 pressed! buildStackLayout!\n";
                screen.setActiveLayout(LAYOUT_TYPE_STACKED);
                buildLayout(screen);
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '3')) {
                //message = "Alt + 3 pressed! Moving focused window left\n";
                screen.moveFocusedWindowLeft();
                buildLayout(screen);
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '4')) {
                //message = "Alt + 4 pressed! Moving focused window right\n";
                screen.moveFocusedWindowRight();
                buildLayout(screen);
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '5')) {
                message = "Alt + 5 pressed! Switch to workspace 5\n";
                screen.moveFocusLeft();
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '6')) {
                message = "Alt + 6 pressed! Switch to workspace 6\n";
                screen.moveFocusRight();
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '7')) {
                message = "Alt + 7 pressed! Switch to workspace 7\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '8')) {
                message = "Alt + 8 pressed! Switch to workspace 8\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '9')) {
                message = "Alt + 9 pressed! Switch to workspace 9\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '0')) {
                message = "Alt + 0 pressed! Switch to workspace 0\n";
            }

            if(debug){
                printf("Key Pressed: 0x%X\n", pKeyInfo->vkCode);
                std::cout << message;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    if (g_keyboardHook == NULL) {
        std::cerr << "Error setting up low level keyboard hook" << std::endl;
        return 1;
    }

    HWINEVENTHOOK hWindowWatcherHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_FOCUS, nullptr, WindowWatcherHookProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (hWindowWatcherHook == nullptr)
    {
        std::cerr << "Error setting up WinEventHook" << std::endl;
        return 1;
    }

    screen.initialize(LAYOUT_TYPE_NONE, g_screenWidth, g_screenHeight);

    // Message loop or other application logic goes here
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Remove hooks after done
    UnhookWindowsHookEx(g_keyboardHook);
    UnhookWinEvent(hWindowWatcherHook);

    return 0;
}
