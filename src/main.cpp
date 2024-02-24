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
#include "resource.h"


using namespace std;

// We'll use globals until we figure out something better 

HHOOK g_keyboardHook;
Hotkey hotkey;
WorkspaceManager workspaceManager = WorkspaceManager();
mutex windowStateMutex;

bool debug = true;
bool showRealTimeState = false;
bool Running;

const COLORREF MASK_COLOR = RGB(255, 128, 0);
const COLORREF BORDER_COLOR = RGB(87, 172, 227);

HWND mainWindowHandle;

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


void drawBorder(HWND hwnd) {
        RECT rect;
        if(GetWindowRect(hwnd, &rect))
        {
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            MoveWindow(mainWindowHandle, rect.left, rect.top, width, height, TRUE);
        }
}

void removeBorder() {
    MoveWindow(mainWindowHandle, 0, 0, 0, 0, TRUE);
}

BOOL CALLBACK handleWindow(HWND hwnd, LPARAM lParam) {
    // Exit early if window is popup or something not a real window
    if (!IsMainWindow(hwnd) || !hasTitle(hwnd)) {
        return TRUE;
    }

    Window window = getWindowFromHWND(hwnd);

    registerNewWindow(window);

    HWND fHwnd = GetForegroundWindow();

    auto activeWorkspace = workspaceManager.getActiveWorkspace();

    if (fHwnd == NULL) {
        logInfo("REMOVE BORDER");
        removeBorder();
        return TRUE;
    }

    if(activeWorkspace->focusedWindow.hwnd != fHwnd) {
        Window focusedWindow = getWindowFromHWND(fHwnd);
        if (activeWorkspace->setFocusedWindow(focusedWindow)) {
            drawBorder(focusedWindow.hwnd);
        }
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

        string log = "";

        log += "Workspace " + to_string(workspace->workspaceIndex) + "\n";

        int position = 0;
        for (auto window : workspace->windows) {
            log += " " + to_string(position++) + " - " + window.title;

            if (window.hwnd == workspace->focusedWindow.hwnd) {
                log += " - FOCUSED";
            }
            log += "\n";
        }
        logInfo(log);
    });
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
                        MessageBoxA(hwnd, "Menu Item 1 selected", "Info", MB_OK | MB_ICONINFORMATION);
                        break;

                    case IDM_MENU_ITEM2:
                        Running = false;
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
        case WM_PAINT:
            {
                PAINTSTRUCT ps{};
                HDC hdc = BeginPaint(hwnd, &ps);

                HPEN hPen = CreatePen(PS_SOLID, 5, BORDER_COLOR);
                HBRUSH hBrush = CreateSolidBrush(MASK_COLOR);
                HGDIOBJ hOldPen = SelectObject(hdc, hPen);
                HGDIOBJ hOldBrush = SelectObject(hdc, hBrush);
                RECT rect; 
                GetClientRect(hwnd, &rect);
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

                if (hOldPen)
                    SelectObject(hdc, hOldPen);
                if (hOldBrush)
                    SelectObject(hdc, hOldBrush);
                if (hPen)
                    DeleteObject(hPen);
                if (hBrush)
                    DeleteObject(hBrush);

                EndPaint(hwnd, &ps);
            }

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
    AppendMenuA(hContextMenu, MF_STRING, IDM_MENU_ITEM1, "Menu Item 1");
    AppendMenuA(hContextMenu, MF_STRING, IDM_MENU_ITEM2, "Exit");

    ShowTrayIcon();

    return 0;
}

void checkWindowState() {
    while (Running) {
        unique_lock<mutex> lock(windowStateMutex);

        workspaceManager.getActiveWorkspace()->onBeforeWindowsRegistered();

        EnumWindows(handleWindow, NULL);

        workspaceManager.getActiveWorkspace()->onAfterWindowsRegistered();

        buildLayout(*workspaceManager.getActiveWorkspace());

        if (showRealTimeState) {
            printWindowState(*workspaceManager.getActiveWorkspace());
        }

        lock.unlock();

        this_thread::sleep_for(50ms);
    }
}

int buildMainWindow(HINSTANCE hInstance) {
    WNDCLASSEXA wc = { sizeof(WNDCLASSEX) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FunnyWindowManager"; 
    RegisterClassExA(&wc);

    // Create the i3win main window
    HWND hwnd = CreateWindowExA( WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE, 
                            wc.lpszClassName, "mau5trapi3win", WS_POPUP,
                             0, 0, 0, 0, nullptr, nullptr, hInstance, nullptr);
    if (hwnd) {
        mainWindowHandle = hwnd;
        SetLayeredWindowAttributes(hwnd, MASK_COLOR, 255, LWA_COLORKEY);
        ShowWindow(hwnd, SW_SHOW);
        buildTrayIcon(hwnd);
        UpdateWindow(hwnd);
        ShowTrayIcon();
        return 1;
    } else {
        logError("Failed to create window");
        return 0;
    }
}

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    if (g_keyboardHook == NULL) {
        logError("Error setting up low level keyboard hook");
        return 1;
    }

    logInfo("***    Window manager started ***");

    int win = buildMainWindow(hInstance);

    if(win){
        // Check window state each second and update layout
        future<void> asyncResult = async(launch::async, checkWindowState);

        Running = true;
        while (Running) {
            MSG msg;
            BOOL result = GetMessage(&msg, 0, 0, 0);
            if(result > 0){
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }else{
                Running = false;
                break;
            }
        }
    }
    // Remove hooks after done
    UnhookWindowsHookEx(g_keyboardHook);

    return 0;
}
