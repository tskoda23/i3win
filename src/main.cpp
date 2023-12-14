#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <set>
#include <iostream>

#include "layouts.h"
#include "window.h"

using namespace std;

HHOOK g_keyboardHook;
bool debug = true;
int g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
int g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

std::set<Window> activeWindows;
Window focusedWindow;

// Filter out internal windows OS stuff that's always shown
bool isWindowsClassName(std::string className) {
    return className == "Progman";
}

BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,&isCloaked, sizeof(isCloaked))) && isCloaked;
}

void registerNewWindow(Window Window) {
    // pass WM_DISPLAYCHANGE if resolution changed
    // check if it has title
    // 
    if (IsWindowVisible(Window.hwnd) && IsWindow(Window.hwnd) && !IsWindowCloaked(Window.hwnd) && !IsIconic(Window.hwnd) && !isWindowsClassName(Window.className))
    {
        if (!Window.isHidden()) {
            activeWindows.insert(Window);
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

    if(focusedWindow.isInitialized == 0 || focusedWindow.hwnd != newFwin.hwnd){
        focusedWindow = newFwin;
        if(debug){
            std::cout << "FOCUSED IS INIT? " << focusedWindow.isInitialized << " - " << focusedWindow.hwnd << " - " << focusedWindow.className << " - " << focusedWindow.title << std::endl;
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
    // Exit early if the event is coming from something that is not a real window, like a dialog or popup
    if (!IsMainWindow(hwnd)) {
        return;
    }

    switch (event)
    {
        case EVENT_OBJECT_CREATE:
            // std::cout << "new " << title << " " << exeName << std::endl;
            break;
        case EVENT_OBJECT_HIDE:
            // std::cout << "hide " << title << " " << exeName << std::endl;
            break;
        case EVENT_OBJECT_SHOW:
            // std::cout << "show " << title << " " << exeName << std::endl;
            break;
        case EVENT_OBJECT_DESTROY:
            // std::cout << "destroy " << title << " " << exeName << std::endl;
            break;
        default:
            printf("UNKNOWN EVENT \n");
        break;
    }

    activeWindows.clear();
    EnumWindows(handleWindow, NULL);

   if(debug){
        // std::system("cls");
        // std::cout << "Currently active windows: " << activeWindows.size() << std::endl;
        // for (const Window& window : activeWindows)
        // {
        //     std::cout << "  " << window.hwnd << " - " << window.className << " - " << window.title <<  " - " << window.exeName << std::endl;
        //     printf("    %d %d %d %d\n", window.left, window.top, window.right, window.bottom);
        // }

        printf("Focused: %d %d %d %d\n", focusedWindow.left, focusedWindow.top, focusedWindow.right, focusedWindow.bottom);
    }
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std:string message = "";

            if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '1')) {
                message = "Alt + 1 pressed! buildSplitLayout! 1\n";
                buildSplitLayout(activeWindows, g_screenWidth, g_screenHeight);
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '2')) {
                message = "Alt + 2 pressed! buildStackLayout! 1\n";
                buildStackedLayout(activeWindows, g_screenWidth, g_screenHeight);
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '3')) {
                message = "Alt + 3 pressed! Switch to workspace 3\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '4')) {
                message = "Alt + 4 pressed! Switch to workspace 4\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '5')) {
                message = "Alt + 5 pressed! Switch to workspace 5\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '6')) {
                message = "Alt + 6 pressed! Switch to workspace 6\n";
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

    HWINEVENTHOOK hWindowWatcherHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr, WindowWatcherHookProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (hWindowWatcherHook == nullptr)
    {
        std::cerr << "Error setting up WinEventHook" << std::endl;
        return 1;
    }

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
