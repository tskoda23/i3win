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
#include "resource.h"

using namespace std;

HHOOK g_keyboardHook;
bool debug = true;
bool showRealTimeState = false;
int g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
int g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

Screen screen;

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
            std:string message = "";

            // Lock guard's destructor will unlock the mutex automatically at the end of block
            std::lock_guard<std::mutex> lock(windowStateMutex);

            if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == 'Q'))) {
                screen.closeFocusedWindow();
                buildLayout(screen);
                return CATCH_KEYBOARD_EVENT;
            }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == VK_LEFT))) {
                screen.moveFocusedWindowLeft();
                buildLayout(screen);
                return CATCH_KEYBOARD_EVENT;
            }else if (((GetAsyncKeyState(VK_RMENU) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (pKeyInfo->vkCode == VK_RIGHT))) {
                screen.moveFocusedWindowRight();
                buildLayout(screen);
                return CATCH_KEYBOARD_EVENT;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '1')) {
                message = "Alt + 1 pressed! buildSplitLayout!\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '2')) {
                message = "Alt + 2 pressed! buildStackLayout!\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '3')) {
                message = "Alt + 3 pressed! Moving focused window left\n";
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '4')) {
                message = "Alt + 4 pressed! Moving focused window right\n";
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
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == 'A')) {
                message = "Alt + 0 pressed! Switch to workspace 0\n";
                screen.setActiveLayout(LAYOUT_TYPE_STACKED);
                buildLayout(screen);
                return CATCH_KEYBOARD_EVENT;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == 'B')) {
                screen.setActiveLayout(LAYOUT_TYPE_SPLIT);
                buildLayout(screen);
                return CATCH_KEYBOARD_EVENT;
            } else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == 'C')) {
                screen.setActiveLayout(LAYOUT_TYPE_CENTERED);
                buildLayout(screen);
                return CATCH_KEYBOARD_EVENT;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == VK_LEFT)) {
                screen.moveFocusLeft();
                return CATCH_KEYBOARD_EVENT;
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == VK_RIGHT)) {
                screen.moveFocusRight();
                return CATCH_KEYBOARD_EVENT;
            }

            if(debug){
                printf("Key Pressed: 0x%X\n", pKeyInfo->vkCode);
                std::cout << message;
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

NOTIFYICONDATA nidApp;
HMENU hContextMenu;

#define IDM_MENU_ITEM1  1001
#define IDM_MENU_ITEM2  1002
#define ID_TRAY_ICON (WM_USER + 1)

void ShowTrayIcon() {
    Shell_NotifyIcon(NIM_ADD, &nidApp);
}

void HideTrayIcon() {
    Shell_NotifyIcon(NIM_DELETE, &nidApp);
}

LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case ID_TRAY_ICON:
            switch (lParam) {
                case WM_RBUTTONDOWN:
                    std::cout << "Opening tray menu" << std::endl;
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hwnd);
                    TrackPopupMenu(hContextMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
                    break;
                default:
                    break;
            }
            break;

            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                    case IDM_MENU_ITEM1:
                        // Perform action for Menu Item 1
                        MessageBox(hwnd, L"Menu Item 1 selected", L"Info", MB_OK | MB_ICONINFORMATION);
                        break;

                    case IDM_MENU_ITEM2:
                        // Perform action for Menu Item 2
                        MessageBox(hwnd, L"Menu Item 2 selected", L"Info", MB_OK | MB_ICONINFORMATION);
                        break;
                }
                break;

        case WM_CLOSE:
            HideTrayIcon();
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            HideTrayIcon();
            DestroyIcon(nidApp.hIcon); // Release the loaded icon
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

int buildTrayIcon(HWND hwnd) {
    // Already set, don't touch it again
    if (nidApp.hWnd != nullptr) {
        return 0;
    }

    nidApp.cbSize = sizeof(NOTIFYICONDATA);
    nidApp.hWnd = hwnd;
    nidApp.uID = ID_TRAY_ICON; // Unique ID for the tray icon
    nidApp.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nidApp.uCallbackMessage = ID_TRAY_ICON; // Custom message to handle tray icon events
    nidApp.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));

    LoadString(GetModuleHandle(nullptr), IDS_STRING102, nidApp.szTip, sizeof(nidApp.szTip) / sizeof(nidApp.szTip[0])); // Replace IDS_APP_TITLE with your application title string resource

    // Create a context menu
    hContextMenu = CreatePopupMenu();
    AppendMenu(hContextMenu, MF_STRING, IDM_MENU_ITEM1, L"Menu Item 1");
    AppendMenu(hContextMenu, MF_STRING, IDM_MENU_ITEM2, L"Menu Item 2");

    ShowTrayIcon();

    return 0;
}

void checkWindowState() {
    while (true) {
        std::unique_lock<std::mutex> lock(windowStateMutex);

        screen.onBeforeWindowsRegistered();

        EnumWindows(handleWindow, NULL);

        screen.onAfterWindowsRegistered();
        
        buildLayout(screen);

        if (showRealTimeState) {
            printWindowState(screen);    
        }

        lock.unlock();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void buildMainWindow(HINSTANCE hInstance) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    auto className = L"FunnyWindowManager";
    wc.lpfnWndProc = MainWindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = className; 
    RegisterClassEx(&wc);

    // Create the i3win main window
    HWND hwnd = CreateWindow(className, L"SomeTitle", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, nullptr, nullptr, hInstance, nullptr);

    if (hwnd) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
        buildTrayIcon(hwnd);
        UpdateWindow(hwnd);
        ShowTrayIcon();
    } else {
        std::cout << "Failed to create window" << std::endl;
    }
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    if (g_keyboardHook == NULL) {
        std::cerr << "Error setting up low level keyboard hook" << std::endl;
        return 1;
    }

    logInfo("***    Window manager started ***");

    buildMainWindow(hInstance);

    screen.initialize(g_screenWidth, g_screenHeight);

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