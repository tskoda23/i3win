#include "window.h"
#include "logger.h"
#include <iostream>
#include <windows.h>
#include <psapi.h>

bool Window::isHidden() {
    return left < 0 && right < 0 && top < 0 && bottom < 0;
}

bool Window::move(int x, int y, int width, int height) {
    // Unmaximize if it it's maximized, because otherwise it won't move
    ShowWindow(hwnd, SW_RESTORE);

    bool isWindowMovedSuccessfully = MoveWindow(hwnd, x, y, width, height, TRUE);

    if (!isWindowMovedSuccessfully) {
        DWORD error = GetLastError();
        logError("Can't move window, error code " + error);
    }

    return isWindowMovedSuccessfully;
}

// We need to override this for set ordering and uniqueness
bool operator<(const Window& lhs, const Window& rhs)
{
    return lhs.hwnd < rhs.hwnd;
}

std::string getWindowTitle(HWND hwnd) {
    char buffer[256];

    GetWindowTextA(hwnd, buffer, sizeof(buffer));

    return std::string(buffer);
}

std::string getWindowClassName(HWND hwnd) {
    char className[256];

    GetClassNameA(hwnd, className, sizeof(className));

    return std::string(className);
}

std::string getWindowExeName(HWND hwnd) {
    char buffer[MAX_PATH] = { 0 };
    DWORD dwProcId = 0;

    GetWindowThreadProcessId(hwnd, &dwProcId);

    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
    GetModuleFileNameExA((HMODULE)hProc, (HMODULE)NULL, buffer, MAX_PATH);
    CloseHandle(hProc);
    return std::string(buffer);
}

Window getWindowFromHWND(HWND hwnd) {
    WINDOWINFO info;
    GetWindowInfo(hwnd, &info);

    RECT rect = info.rcWindow;

    std::string title = getWindowTitle(hwnd);
    std::string className = getWindowClassName(hwnd);
    std::string exeName = getWindowExeName(hwnd);

    return Window {
            hwnd,
            title,
            className,
            exeName,
            rect.left,
            rect.top,
            rect.right,
            rect.bottom
    };
}

void Window::hide(){
    // ShowWindow(hwnd, SW_HIDE);
    ShowWindow(hwnd, SW_MINIMIZE);
}

void Window::show(){
    ShowWindow(hwnd, SW_RESTORE);
}