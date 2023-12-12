#include <windows.h>
#include <set>
#include <iostream>
using namespace std;

HHOOK g_keyboardHook;
const DWORD ralt = 165;
const DWORD rshift = 161;

const DWORD rarrow = 39;
const DWORD darrow = 40;
const DWORD uarrow = 38;
const DWORD larrow = 37;

const DWORD num0 = 47;
const DWORD num1 = 48;
const DWORD num2 = 49;
const DWORD num3 = 50;
const DWORD num4 = 51;
const DWORD num5 = 52;
const DWORD num6 = 53;
const DWORD num7 = 54;
const DWORD num8 = 55;
const DWORD num9 = 56;

bool isRaltPressed = false;
bool isRshiftPressed = false;
bool debug = true;

struct WindowInfo
{
    HWND hwnd;
    std::string name;
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;

    bool isHidden() {
        return left < 0 && right < 0 && top < 0 && bottom < 0;
    }
};

// We need to override this for set ordering and uniqueness
bool operator<(const WindowInfo& lhs, const WindowInfo& rhs)
{
    return lhs.hwnd < rhs.hwnd;
}

std::set<WindowInfo> activeWindows;

WindowInfo focusedWindow;

std::string getWindowName(HWND hwnd) {
    char nameBuffer[256];

    GetWindowTextA(hwnd, nameBuffer, sizeof(nameBuffer));

    return std::string(nameBuffer);
}

void registerNewWindow(WindowInfo windowInfo) {
    if (IsWindowVisible(windowInfo.hwnd) && !IsIconic(windowInfo.hwnd))
    {
        if (!windowInfo.isHidden()) {
            activeWindows.insert(windowInfo);
        }
    }
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD keyCode = pKeyStruct->vkCode;

            char keyChar = MapVirtualKey(keyCode, 2);

            if (isprint(keyChar) && isRaltPressed && isRshiftPressed) {
                printf("Alt + Shift + %c\n", keyChar);
            }
            else

                if (isprint(keyChar) && isRaltPressed) {
                    printf("Alt + %c\n", keyChar);
                }

            if (debug) {
                printf("PRESSED: %d\n", keyCode);
            }

            switch (keyCode) {
            case ralt:
                isRaltPressed = true;
                break;
            case rshift:
                isRshiftPressed = true;
                break;
            default:
                break;
            }
        }

        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD keyCode = pKeyStruct->vkCode;

            if (debug) {
                printf("RELEASED: %d\n", keyCode);
            }
            switch (keyCode) {
            case ralt:
                isRaltPressed = false;
                break;
            case rshift:
                isRshiftPressed = false;
                break;
            default:
                break;
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

bool IsMainWindow(HWND hwnd) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    return ((style & WS_CHILD) == 0) && (GetWindow(hwnd, GW_OWNER) == nullptr) && IsWindowVisible(hwnd);
}

bool hasTitle(HWND hwnd){
    int length = ::GetWindowTextLength(hwnd);
    return length > 0;
}

WindowInfo getWindowInfo(HWND hwnd){
    WINDOWINFO info;
    GetWindowInfo(hwnd, &info);

    RECT rect = info.rcWindow;

    std::string title = getWindowName(hwnd);

    WindowInfo windowInfo = {
            hwnd,
            title,
            rect.left,
            rect.top,
            rect.right,
            rect.bottom
    };

    return windowInfo;
}


BOOL CALLBACK handleWindow(HWND hwnd, LPARAM lParam) {
    HWND fWinH= GetForegroundWindow();
    focusedWindow = getWindowInfo(fWinH);

    // Exit early if window is popup or something not a real window
    if (!IsMainWindow(hwnd) || !hasTitle(hwnd)) {
        return TRUE;    
    }

    WindowInfo windowInfo = getWindowInfo(hwnd);

    registerNewWindow(windowInfo);

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
    // Exit early if window is popup or something not a real window
    if (!IsMainWindow(hwnd) || !hasTitle(hwnd)) {
        return;
    }

    activeWindows.clear();
    EnumWindows(handleWindow, NULL);

    std::system("cls");
    std::cout << "Currently active windows: " << activeWindows.size() << std::endl;
    for (const WindowInfo& window : activeWindows)
    {
        std::cout << "  " << window.hwnd << " - " << window.name << std::endl;
        printf("    %d %d %d %d\n", window.left, window.top, window.right, window.bottom);
    }

    printf("    %d %d %d %d\n", focusedWindow.left, focusedWindow.top, focusedWindow.right, focusedWindow.bottom);
}

int main() {
    g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);

    if (g_keyboardHook == NULL) {
        std::cerr << "Error setting up low level keyboard hook" << std::endl;
        return 1;
    }

    HWINEVENTHOOK hWindowWatcherHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_REORDER, nullptr, WindowWatcherHookProc, 0, 0, WINEVENT_OUTOFCONTEXT);

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
