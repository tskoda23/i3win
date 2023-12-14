#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <dwmapi.h>
#include <set>
#include <iostream>
using namespace std;

HHOOK g_keyboardHook;
bool debug = true;
int g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
int g_screenHeight = GetSystemMetrics(SM_CYSCREEN);

struct WindowInfo
{
    HWND hwnd;
    std::string title;
    std::string className;
    std::string exeName;
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
    int isInitialized = 0;

    bool isHidden() {
        return left < 0 && right < 0 && top < 0 && bottom < 0;
    }

    bool isHwndEqualToNumericValue(DWORD_PTR numericValue) const {
        return reinterpret_cast<DWORD_PTR>(hwnd) == numericValue;
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

std::string getWindowClassName(HWND hwnd) {
    char className[256];

    GetClassNameA(hwnd, className, sizeof(className));

    return std::string(className);
}

std::string getWindowExeName(HWND hwnd){
    char buffer[MAX_PATH] = {0};
    DWORD dwProcId = 0; 

    GetWindowThreadProcessId(hwnd, &dwProcId);   

    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, dwProcId);    
    GetModuleFileNameExA((HMODULE)hProc, (HMODULE)NULL, buffer, MAX_PATH);
    CloseHandle(hProc);
    return std::string(buffer);
}

// Filter out internal windows OS stuff that's always shown
bool isWindowsClassName(std::string className) {
    return className == "Progman";
}

BOOL IsWindowCloaked(HWND hwnd)
{
 BOOL isCloaked = FALSE;
 return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED,&isCloaked, sizeof(isCloaked))) && isCloaked;
}

void registerNewWindow(WindowInfo windowInfo) {
    // pass WM_DISPLAYCHANGE if resolution changed
    // check if it has title
    // 
    if (IsWindowVisible(windowInfo.hwnd) && IsWindow(windowInfo.hwnd) && !IsWindowCloaked(windowInfo.hwnd) && !IsIconic(windowInfo.hwnd) && !isWindowsClassName(windowInfo.className))
    {
        if (!windowInfo.isHidden()) {
            activeWindows.insert(windowInfo);
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

WindowInfo getWindowInfo(HWND hwnd){
    WINDOWINFO info;
    GetWindowInfo(hwnd, &info);

    RECT rect = info.rcWindow;

    std::string title = getWindowName(hwnd);
    std::string className = getWindowClassName(hwnd);
    std::string exeName = getWindowExeName(hwnd);

    WindowInfo windowInfo = {
            hwnd,
            title,
            className,
            exeName,
            rect.left,
            rect.top,
            rect.right,
            rect.bottom
    };

    return windowInfo;
}


BOOL CALLBACK handleWindow(HWND hwnd, LPARAM lParam) {
    HWND fWinH= GetForegroundWindow();
    WindowInfo newFwin = getWindowInfo(fWinH);
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

    WindowInfo windowInfo = getWindowInfo(hwnd);

    registerNewWindow(windowInfo);

    return TRUE;
}

void buildStackedLayout() {
    int windowCount = 0;
    int padding = 50;

    for (const WindowInfo& window : activeWindows)
    {
        int spacing = padding * windowCount;

        MoveWindow(window.hwnd,
            0,                                // X
            spacing,                          // Y
            g_screenWidth,                    // Width
            g_screenHeight - spacing,         // Height
            TRUE);

        windowCount++;
    }
}

void buildSplitLayout() {
    int windowsCount = activeWindows.size();

    int windowWidth = g_screenWidth / windowsCount;
    
    int windowCount = 0;

    for (const WindowInfo& window : activeWindows)
    {
        if (!MoveWindow(window.hwnd,
            windowCount * windowWidth,      // X
            0,                              // Y
            windowWidth,                    // Width
            g_screenHeight,                 // Height
            TRUE)) {
            DWORD error = GetLastError();
            printf("SYSTEM ERROR CODE %d", error);
        }
        else {
            windowCount++;
        }
    }
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

    if(debug){
        std::system("cls");
        std::cout << "Currently active windows: " << activeWindows.size() << std::endl;
        for (const WindowInfo& window : activeWindows)
        {
            std::cout << "  " << window.hwnd << " - " << window.className << " - " << window.title <<  " - " << window.exeName << std::endl;
            printf("    %d %d %d %d\n", window.left, window.top, window.right, window.bottom);
        }

        printf("    %d %d %d %d\n", focusedWindow.left, focusedWindow.top, focusedWindow.right, focusedWindow.bottom);
    }
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            std:string message = "";

            if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '1')) {
                message = "Alt + 1 pressed! buildSplitLayout! 1\n";
                buildSplitLayout();
            }else if ((GetAsyncKeyState(VK_RMENU) & 0x8000) && (pKeyInfo->vkCode == '2')) {
                message = "Alt + 2 pressed! Switch to workspace 2\n";
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
