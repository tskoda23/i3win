#include "config.h"
#include "state.h"
#include "window.h"

#ifndef WORKSPACE_H 
#define WORKSPACE_H 

// TODO: This was part of layouts.h, but had to move it here to avoid circular reference
enum LayoutType {
    LAYOUT_TYPE_NONE = 0,
    LAYOUT_TYPE_STACKED,
    LAYOUT_TYPE_SPLIT,
    LAYOUT_TYPE_CENTERED,
};

class Workspace {
    public:
        Workspace(int workspaceIndex, int screenWidth, int screenHeight, LayoutType layoutType, Config &config, State &state);

        std::list<Window> windows;
        Window focusedWindow;
        LayoutType layoutType;
        Config &config;
        State &state;
        int screenWidth;
        int screenHeight;
        int workspaceIndex;

        void addWindow(Window& window);
        void removeWindow(HWND hwnd);
        void setFocusedWindow(Window window);
        void setActiveLayout(LayoutType layout);
        void moveFocusLeft();
        void moveFocusRight();
        void moveFocusedWindowRight();
        void moveFocusedWindowLeft();
        void closeFocusedWindow();
        void changeMainWindowSize(int screenPercentageChange);
        void setAsMainWindow();
        void hideWindows();
        void showWindows();

        void onBeforeWindowsRegistered();
        void onAfterWindowsRegistered();

    private:
        std::list<HWND> windowIndexes;
        HWND lastWindowSwappedWithMainWindow;
};

#endif
