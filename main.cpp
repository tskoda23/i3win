#include <windows.h>
#include <iostream>

// Global hook handle
HHOOK g_hHook;
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


// Keyboard hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD keyCode = pKeyStruct->vkCode;

            char keyChar = MapVirtualKey(keyCode, 2);
            
            if (isprint(keyChar) && isRaltPressed && isRshiftPressed) {
                printf("Alt + Shift + %c\n", keyChar);
            }else 
                
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

    // Call the next hook in the hook chain
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

int main() {
    // Set the keyboard hook
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

    if (g_hHook == NULL) {
        // Hook installation failed
        // Handle the error here
        return 1;
    }

    // Message loop or other application logic goes here
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Unhook the keyboard hook when you're done
    UnhookWindowsHookEx(g_hHook);

    return 0;
}
