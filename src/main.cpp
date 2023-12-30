#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <future>

#include "layouts.h"
#include "window.h"
#include "screen.h"
#include "logger.h"

using namespace std;

HHOOK g_keyboardHook;
bool debug = true;
bool showRealTimeState = false;
int g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
int g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

int ac = 0;
Screen screen0;
Screen screen1;
Screen screen2;
Screen screen3;
Screen screen4;
Screen screen5;
Screen screen6;
Screen screen7;
Screen screen8;
Screen screen9;

Screen screens[10] = {screen0, screen1, screen2, screen3, screen4, screen5, screen6, screen7, screen8, screen9};

std::mutex windowStateMutex;

// Filter out internal windows OS stuff that's always shown
BOOL IsWindowsClassName(std::string className) {
    return className == "Progman" ||
    className == "MultitaskingViewFrame";
}

BOOL IsWindowCloaked(HWND hwnd)
{
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,&isCloaked, sizeof(isCloaked))) && isCloaked;
}

void registerNewWindow(Window window) {
    // pass WM_DISPLAYCHANGE if resolution changed
    // check if it has title
    if (IsWindowVisible(window.hwnd) && IsWindow(window.hwnd) && !IsWindowCloaked(window.hwnd) && !IsIconic(window.hwnd) && !IsWindowsClassName(window.className))
    {
        if (!window.isHidden()) {
            screens[ac].addWindow(window);
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

    if(screens[ac].focusedWindow.isInitialized == 0 || screens[ac].focusedWindow.hwnd != newFwin.hwnd){
        screens[ac].setFocusedWindow(newFwin);
    }

    // Exit early if window is popup or something not a real window
    if (!IsMainWindow(hwnd) || !hasTitle(hwnd)) {
        return TRUE;    
    }

    Window Window = getWindowFromHWND(hwnd);

    registerNewWindow(Window);

    return TRUE;
}

int CATCH_KEYBOARD_EVENT = -1;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std::string message;
            bool hotkeyPressed = false;

            // Lock guard's destructor will unlock the mutex automatically at the end of block
            std::lock_guard<std::mutex> lock(windowStateMutex);

            if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == 'Q'))) {
                message = "Alt + Shift + Q pressed! Close focused window!\n";
                screens[ac].closeFocusedWindow();
                hotkeyPressed = true;
            }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == VK_LEFT))) {
                message = "Alt + Shift + LEFT pressed! Move focused window right!\n";
                screens[ac].moveFocusedWindowLeft();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == VK_RIGHT))) {
                message = "Alt + Shift + RIGHT pressed! Move focused window right!\n";
                screens[ac].moveFocusedWindowRight();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '1'))) {
                if(ac != 0){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[0].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 1 pressed! Move focused window to workspace 1!\n";
                }else{
                    message = "Alt + Shift + 1 pressed! Already on workspace 1!\n";
                }
                hotkeyPressed = true;
            }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '2'))) {
                if(ac != 1){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[1].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 2 pressed! Move focused window to workspace 2!\n";
                }else{
                    message = "Alt + Shift + 2 pressed! Already on workspace 2!\n";
                }
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '3'))) {
                if(ac != 2){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[2].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 3 pressed! Move focused window to workspace 3!\n";
                }else{
                    message = "Alt + Shift + 3 pressed! Already on workspace 3!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '4'))) {
                if(ac != 3){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[3].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 4 pressed! Move focused window to workspace 4!\n";
                }else{
                    message = "Alt + Shift + 4 pressed! Already on workspace 4!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '5'))) {
                if(ac != 4){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[4].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 5 pressed! Move focused window to workspace 5!\n";
                }else{
                    message = "Alt + Shift + 5 pressed! Already on workspace 5!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '6'))) {
                if(ac != 5){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[5].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 6 pressed! Move focused window to workspace 6!\n";
                }else{
                    message = "Alt + Shift + 6 pressed! Already on workspace 6!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '7'))) {
                if(ac != 6){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[6].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 7 pressed! Move focused window to workspace 7!\n";
                }else{
                    message = "Alt + Shift + 7 pressed! Already on workspace 7!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '8'))) {
                if(ac != 7){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[7].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 8 pressed! Move focused window to workspace 8!\n";
                }else{
                    message = "Alt + Shift + 8 pressed! Already on workspace 8!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '9'))) {
                if(ac != 8){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[8].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 9 pressed! Move focused window to workspace 9!\n";
                }else{
                    message = "Alt + Shift + 9 pressed! Already on workspace 9!\n";
                }
                hotkeyPressed = true;
             }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == '0'))) {
                if(ac != 9){
                    Window win = screens[ac].focusedWindow;
                    win.hide();
                    screens[ac].removeWindow(win.hwnd);
                    Window newWin = getWindowFromHWND(win.hwnd);
                    screens[9].addWindow(newWin);
                    buildLayout(screens[ac]);
                    message = "Alt + Shift + 0 pressed! Move focused window to workspace 10!\n";
                }else{
                    message = "Alt + Shift + 0 pressed! Already on workspace 10!\n";
                }
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '1')) {
                message = "Alt + 1 pressed! Switch to workspace 1!\n";
                screens[ac].hideWindows();
                ac = 0;
                screens[ac].showWindows();
                // screens[ac].initialize(g_screenWidth, g_screenHeight);
                // NO NEED TO INITIALIZE IT'S ALREADY INIT
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '2')) {
                message = "Alt + 2 pressed! Switch to workspace 2!\n";
                screens[ac].hideWindows();
                ac = 1;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '3')) {
                message = "Alt + 3 pressed! Switch to workspace 3\n";
                screens[ac].hideWindows();
                ac = 2;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '4')) {
                message = "Alt + 4 pressed! Switch to workspace 4\n";
                screens[ac].hideWindows();
                ac = 3;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '5')) {
                message = "Alt + 5 pressed! Switch to workspace 5\n";
                screens[ac].hideWindows();
                ac = 4;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '6')) {
                message = "Alt + 6 pressed! Switch to workspace 6\n";
                screens[ac].hideWindows();
                ac = 5;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '7')) {
                message = "Alt + 7 pressed! Switch to workspace 7\n";
                screens[ac].hideWindows();
                ac = 6;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '8')) {
                message = "Alt + 8 pressed! Switch to workspace 8\n";
                screens[ac].hideWindows();
                ac = 7;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '9')) {
                message = "Alt + 9 pressed! Switch to workspace 9\n";
                screens[ac].hideWindows();
                ac = 8;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '0')) {
                message = "Alt + 0 pressed! Switch to workspace 0\n";
                screens[ac].hideWindows();
                ac = 9;
                screens[ac].initialize(g_screenWidth, g_screenHeight);
                screens[ac].showWindows();
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == 'A')) {
                message = "Alt + A pressed! Build stacked layout\n";
                screens[ac].setActiveLayout(LAYOUT_TYPE_STACKED);
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == 'B')) {
                message = "Alt + B pressed! Build split layout\n";
                screens[ac].setActiveLayout(LAYOUT_TYPE_SPLIT);
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            } else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == 'C')) {
                message = "Alt + C pressed! Build centered layout\n";
                screens[ac].setActiveLayout(LAYOUT_TYPE_CENTERED);
                buildLayout(screens[ac]);
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == VK_LEFT)) {
                message = "Alt + LEFT pressed! Move focus left\n";
                screens[ac].moveFocusLeft();
                hotkeyPressed = true;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == VK_RIGHT)) {
                message = "Alt + RIGHT pressed! Move focus right\n";
                screens[ac].moveFocusRight();
                hotkeyPressed = true;
            }

            if(hotkeyPressed){
                if(debug && message != ""){
                    std::cout << message << std::endl;
                }
                return CATCH_KEYBOARD_EVENT;
            }

            if(debug){
                printf("Key Pressed: 0x%X\n", pKeyInfo->vkCode);
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void printWindowState(Screen screen) {
    std::system("cls");
    std::cout << "Currently active windows: " << screen.windows.size() << std::endl;
        
    int position = 0;
    for (auto window : screen.windows) {
        std::cout << position++ << " - " << window.title;

        if (window.hwnd == screen.focusedWindow.hwnd) {
            std::cout << " - FOCUSED";
        }

        std::cout << std::endl;
    }
}

void checkWindowState() {
    while (true) {
        std::unique_lock<std::mutex> lock(windowStateMutex);

        screens[ac].onBeforeWindowsRegistered();

        EnumWindows(handleWindow, NULL);

        screens[ac].onAfterWindowsRegistered();
        
        buildLayout(screens[ac]);

        if (showRealTimeState) {
            printWindowState(screens[ac]);    
        }

        lock.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    if (g_keyboardHook == NULL) {
        std::cerr << "Error setting up low level keyboard hook" << std::endl;
        return 1;
    }

    logInfo("***    Window manager started ***");

    screens[ac].initialize(g_screenWidth, g_screenHeight);

    // Chechk window state each second and update layout
    std::future<void> asyncResult = std::async(std::launch::async, checkWindowState);

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
