#include "layouts.h"
#include "logger.h"
#include "workspace.h"

const int taskbarSize = 50;

int getMainWindowWidthPercentage(Workspace &workspace) {
    int centerWindowWidthFromConfig = workspace.config.getNumericValue(MAIN_WINDOW_WIDTH_PERCENTAGE);
    int centerWindowWidthChange = workspace.state.getNumericValue(MAIN_WINDOW_PERCENTAGE_CHANGE);

    int percentage = centerWindowWidthFromConfig + centerWindowWidthChange;

    if (percentage < 0) {
        logError("Main window width percentage can't be less than 0");
        percentage = 10;
    } else if (percentage > 100) {
        logError("Main window width percentage can't be more than 100");
        percentage = 100;
    }

    return percentage;
}

void buildStackedLayout(Workspace &workspace) {
    int windowsRendered = 0;
    int margin = 50;

    for (auto window : workspace.windows) {
        int startMargin = margin * windowsRendered;
        int endMargin = margin * (workspace.windows.size() - windowsRendered - 1);

        bool isWindowMovedSuccessfully = window.move(
            startMargin,
            0,
            workspace.screenWidth - startMargin - endMargin,
            workspace.screenHeight - taskbarSize);

        if (isWindowMovedSuccessfully) {
            windowsRendered++;
        }
    }
}

void buildSplitLayout(Workspace &workspace) {
    int additionalPadding = workspace.config.getNumericValue(ADDITIONAL_WINDOW_PADDING);
    int totalPadding = additionalPadding * (workspace.windows.size() - 1);

    int mainWindowWidth = workspace.screenWidth * getMainWindowWidthPercentage(workspace) / 100;
    int a = workspace.windows.size();
    int otherWindowsWidth = (workspace.screenWidth - mainWindowWidth - totalPadding) / (a - 1);

    int windowCount = 0;

    for (auto window : workspace.windows) {
        bool isMainWindow = windowCount == 0;

        int xPosition = isMainWindow
            ? 0
            : mainWindowWidth 
                // width of all windows before this one
                + (windowCount - 1) * otherWindowsWidth
                // plus all the padding between windows
                + (windowCount * additionalPadding);

        int width = isMainWindow
            ? mainWindowWidth
            : otherWindowsWidth;

        bool isWindowMovedSuccessfully = window.move(
            xPosition,
            0,
            width,
            workspace.screenHeight - taskbarSize);

        if (isWindowMovedSuccessfully) {
            windowCount++;
        }
    }
    
}

void buildCenteredLayout(Workspace &workspace) {
    int windowsCount = workspace.windows.size();

    int centerWindowWidth = workspace.screenWidth * getMainWindowWidthPercentage(workspace) / 100;

    int windowCount = 0;

    for (auto window : workspace.windows) {

        int xPosition, yPosition, width, height;

        int sideWindowWidth = (workspace.screenWidth - centerWindowWidth) / 2;
        int centerWindowHeight = workspace.screenHeight - taskbarSize;

        if (windowCount == 0) {
            window.move(
                sideWindowWidth,
                0,
                centerWindowWidth,
                centerWindowHeight);
        } else {
            int numberOfSideWindows = windowsCount - 1;
            int isLeftSideWindow = windowCount <= (workspace.windows.size() / 2);

            int numberOfWindowsOnThisSide = isLeftSideWindow
                ? windowsCount / 2
                // We have -1 here because we want this side to have 
                // one window less than the other one in case of uneven number
                : (windowsCount - 1) / 2;

            int indexOfWindowOnASide = (windowCount - 1) % numberOfWindowsOnThisSide;
            int sideWindowHeight = centerWindowHeight / numberOfWindowsOnThisSide; 

            float yPosition = indexOfWindowOnASide * sideWindowHeight;

            int xPosition = isLeftSideWindow
                ? 0 
                : sideWindowWidth + centerWindowWidth;

            window.move(
                xPosition,
                yPosition,
                sideWindowWidth,
                sideWindowHeight);
        }

        windowCount++;
    }
}

void buildLayout(Workspace &workspace) {
    if(workspace.windows.size() > 1){
        switch (workspace.layoutType)
        {
            case LAYOUT_TYPE_NONE:
                break;
            case LAYOUT_TYPE_STACKED:
                buildStackedLayout(workspace);
                break;
            case LAYOUT_TYPE_SPLIT:
                buildSplitLayout(workspace);
                break;
            case LAYOUT_TYPE_CENTERED:
                buildCenteredLayout(workspace);
                break;
            default:
                logError("Unknown layout type, doing nothing.");
                break;
        }
    }else if(workspace.windows.size() == 1){
        workspace.windows.front().move(
            0,
            0,
            workspace.screenWidth,
            workspace.screenHeight - taskbarSize);
    }
}