#include <iostream>
#include <windows.h>
#include <set>
#include <list>
#include <algorithm>
#include <iterator>
#include "window.h"
#include "screen.h"
#include "logger.h"
#include "state.h"

void Screen::initialize(int screenWidth, int screenHeight) {
    this->layoutType = layoutType;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->config = Config();
    this->state = State();

    this->layoutType = (LayoutType) state.getNumericValue(ACTIVE_LAYOUT);
}

void Screen::addWindow(Window window) {
    windows.push_front(window);
}

void Screen::onBeforeWindowsRegistered() {
    // Remember indexes of windows to restore their position later
    windowIndexes.clear();
    std::transform(windows.begin(), windows.end(), std::front_inserter(windowIndexes),
        [](const Window& obj) {
            return obj.hwnd;
        }
    );

    // Delete current windows to prepare to receive new ones
    windows.clear();
}

void Screen::onAfterWindowsRegistered() {
    // If there are no saved windows, build the positions list from scratch
    if (windowIndexes.empty()) {
        std::transform(windows.begin(), windows.end(), std::front_inserter(windowIndexes),
            [](const Window& obj) {
                return obj.hwnd;
            }
        );
        return;
    }

    std::list<Window> newList;

    // Map previously saved windows indexes to the new ordered list of windows
    for (auto hwnd : windowIndexes) {
         auto window = std::find_if(windows.begin(), windows.end(),
            [hwnd](const Window& obj) {
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
            [hwnd](const Window& obj) {
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

void Screen::setFocusedWindow(Window window) {
    if (window.hwnd != NULL) {
        focusedWindow = window;
        SetForegroundWindow(focusedWindow.hwnd); // TODO: Maybe move this out of this struct?
    }
}

void Screen::setActiveLayout(LayoutType layout) {
    layoutType = layout;
    state.setValue(ACTIVE_LAYOUT, std::to_string(layout));
}

void Screen::moveFocusLeft(){
    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindow(windows.front());
    }

    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](const Window& obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.begin()) {
        setFocusedWindow(*std::prev(window));
        logInfo("Focus moved to left");
    } else {
        logInfo("Can't move to left, already at the edge");
    }
}

void Screen::moveFocusRight(){
    if (focusedWindow.hwnd == NULL && !windows.empty()) {
        setFocusedWindow(windows.front());
    }

    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](const Window& obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end() && std::next(window) != windows.end()) {
        setFocusedWindow(*std::next(window));
        logInfo("Focus moved to right");
    } else {
        logInfo("Can't move to right, already at the edge");
    }
}

void Screen::moveFocusedWindowRight() {
    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](const Window& obj) {
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

void Screen::moveFocusedWindowLeft() {
    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](const Window& obj) {
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

void Screen::closeFocusedWindow() {
    LRESULT res = ::SendMessage(focusedWindow.hwnd, WM_CLOSE, NULL, NULL);

    HWND hwnd = focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](const Window& obj) {
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

void Screen::changeMainWindowSize(int screenPercentageChange) {
    int existingChange = state.getNumericValue(MAIN_WINDOW_PERCENTAGE_CHANGE);
    int newChange = existingChange + screenPercentageChange;

    state.setValue(MAIN_WINDOW_PERCENTAGE_CHANGE, std::to_string(newChange));
}

void Screen::setAsMainWindow() {

    bool isAlreadyMainWindow = focusedWindow.hwnd == windows.front().hwnd;
    bool hasLastWindowSwappedWithMainWindow = lastWindowSwappedWithMainWindow != NULL;

    HWND hwnd = isAlreadyMainWindow && hasLastWindowSwappedWithMainWindow
        ? lastWindowSwappedWithMainWindow
        : focusedWindow.hwnd;

    auto window = std::find_if(windows.begin(), windows.end(),
        [hwnd](const Window& obj) {
            return obj.hwnd == hwnd;
        }
    );

    if (window != windows.end()) {
        lastWindowSwappedWithMainWindow = windows.front().hwnd;
        std::iter_swap(window, windows.begin());
        setFocusedWindow(windows.front());
        logInfo("Window moved to main");
    } else {
        logInfo("Can't set window as main");
    }

}

void Screen::reloadConfig() {
    config.reload();
    logInfo("Config reloaded.");
}
