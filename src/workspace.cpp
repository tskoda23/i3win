#include <iostream>
#include <windows.h>
#include <set>
#include <list>
#include <algorithm>
#include <iterator>

#include "logger.h"
#include "config.h"
#include "state.h"
#include "window.h"
#include "workspace.h"

Workspace::Workspace(
    int workspaceIndex, int screenWidth, int screenHeight, LayoutType layoutType, Config &config, State &state) 
    : layoutType(layoutType),
      screenWidth(screenWidth),
      screenHeight(screenHeight),
      windows(std::list<Window>()),
      windowIndexes(std::list<HWND>()),
      config(config),
      state(state),
      workspaceIndex(workspaceIndex) {
}

void Workspace::addWindow(Window& window) {
    this->windows.push_front(window);
}

void Workspace::removeWindow(HWND hwnd) {
    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    window->hide();
    windows.erase(window);
}

bool Workspace::setFocusedWindow(Window window) {
    if (window.hwnd != NULL) {
        focusedWindow = window;
        return true;
    }
    else {
        return false;
    }
}

void Workspace::setFocusedWindowWin(Window window){
    if (window.hwnd != NULL) {
        SetForegroundWindow(window.hwnd); // TODO: Maybe move this out of this class?
    }
}

void Workspace::setActiveLayout(LayoutType layout) {
    layoutType = layout;
    state.setValue(ACTIVE_LAYOUT, std::to_string(layout));
}

void Workspace::moveFocusLeft(){
    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindowWin(windows.front());
    }

    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.begin()) {
        setFocusedWindowWin(*std::prev(window));
        logInfo("Focus moved to left");
    } else {
        logInfo("Can't move to left, already at the edge");
    }
}

void Workspace::moveFocusRight(){
    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindowWin(windows.front());
    }

    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end() && std::next(window) != windows.end()) {
        setFocusedWindowWin(*std::next(window));
        logInfo("Focus moved to right");
    } else {
        logInfo("Can't move to right, already at the edge");
    }
}

void Workspace::moveFocusedWindowRight() {
    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end() && std::next(window) != windows.end()) {
        std::iter_swap(window, std::next(window));
        logInfo("Window moved to right");
    } else {
        logInfo("Can't move to right, already at the edge");
    }
}

void Workspace::moveFocusedWindowLeft() {
    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end() && window != windows.begin()) {
        std::iter_swap(window, std::prev(window));
        logInfo("Window moved to left");
    } else {
        logInfo("Can't move to left, already at the edge");
    }
}

void Workspace::closeFocusedWindow() {
    LRESULT res = ::SendMessage(focusedWindow.hwnd, WM_CLOSE, NULL, NULL);

    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end()) {
        windows.erase(window);
        logInfo("Window closed");
    } else {
        logError("Can't erase window from the list");
    }
}

void Workspace::changeMainWindowSize(int screenPercentageChange) {
    int existingChange = state.getNumericValue(MAIN_WINDOW_PERCENTAGE_CHANGE);
    int newChange = existingChange + screenPercentageChange;

    state.setValue(MAIN_WINDOW_PERCENTAGE_CHANGE, std::to_string(newChange));
}

void Workspace::setAsMainWindow() {
    bool isAlreadyMainWindow = focusedWindow.hwnd == windows.front().hwnd;
    bool hasLastWindowSwappedWithMainWindow = lastWindowSwappedWithMainWindow != NULL;

    HWND hwnd = isAlreadyMainWindow && hasLastWindowSwappedWithMainWindow
        ? lastWindowSwappedWithMainWindow
        : focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](auto obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end()) {
        lastWindowSwappedWithMainWindow = windows.front().hwnd;
        std::iter_swap(window, windows.begin());
        setFocusedWindowWin(windows.front());
        logInfo("Window moved to main");
    } else {
        logInfo("Can't set window as main");
    }

}

void Workspace::hideWindows() {
    for (auto window : windows) {
        window.hide();
    }   
}

void Workspace::showWindows() {
    for (auto window : windows) {
        window.show();
    }   
}

void Workspace::onBeforeWindowsRegistered() {
    // Remember indexes of windows to restore their position later
    windowIndexes.clear();
    std::transform(windows.begin(), windows.end(), std::front_inserter(windowIndexes),
        [](auto obj) {
            return obj.hwnd;
        }
    );

    // Delete current windows to prepare to receive new ones
    windows.clear();
}

void Workspace::onAfterWindowsRegistered() {
    // If there are no saved windows, build the positions list from scratch
    if (windowIndexes.empty()) {
        std::transform(windows.begin(), windows.end(), std::front_inserter(windowIndexes),
            [](auto obj) {
                return obj.hwnd;
            }
        );
        return;
    }

    std::list<Window> newList;

    // Map previously saved windows indexes to the new ordered list of windows
    for (auto hwnd : windowIndexes) {
        auto window = std::find_if(windows.begin(), windows.end(),
            [hwnd](auto obj) {
                return obj.hwnd == hwnd;
            }
        );

        if (window == windows.end()) {
            logError("Can't find window with hwnd: " + std::to_string((long)hwnd));
            continue;
        }

        newList.push_front(*window);
    }

    // If some windows are new, we have no saved indexes for them,
    // so just add them to the end of the list
    for (auto window : windows) {
        auto hwnd = window.hwnd;

        auto found = std::find_if(newList.begin(), newList.end(),
            [hwnd](auto obj) {
                return obj.hwnd == hwnd;
            }
        );

        if (found == newList.end()) {
            newList.push_back(window);
        }
    }

    // Assign the newly sorted list to the windows list
    windows = newList;
}
