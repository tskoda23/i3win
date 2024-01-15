#include "workspace.h"
#include "screen.h"
#include "logger.h"
#include "config.h"
#include "state.h"
#include "workspace_manager.h"

WorkspaceManager::WorkspaceManager()
    : config(Config()),
      state(State()),
      screenWidth(GetSystemMetrics(SM_CXSCREEN)),
      screenHeight(GetSystemMetrics(SM_CYSCREEN)),
      screen(screenWidth, screenHeight)
{
    for (int i = 0; i < 10; i++)
    {
        // TODO: Figure out why it doesn't work with a string constant here
        auto activeLayout = (LayoutType) state.getNumericValue("ACTIVE_LAYOUT");

        this->workspaces.push_back(
            new Workspace(i, screenWidth, screenHeight, activeLayout, config, state));
    }

    this->screen.setActiveWorkspace(*workspaces.at(1));
}

std::vector<Workspace *> WorkspaceManager::getAllWorkspaces()
{
    return this->workspaces;
}

Workspace *WorkspaceManager::getActiveWorkspace()
{
    return this->screen.activeWorkspace;
}

void WorkspaceManager::setActiveWorkspace(int workspaceIndex)
{
    if (workspaceIndex == this->screen.activeWorkspace->workspaceIndex)
    {
        logInfo("Already on workspace " + std::to_string(workspaceIndex));
        return;
    }

    Workspace *nextWorkspace = this->workspaces.at(workspaceIndex);

    this->screen.activeWorkspace->hideWindows();
    nextWorkspace->showWindows();

    logInfo("Switch to workspace " + std::to_string(workspaceIndex));
    this->screen.activeWorkspace = nextWorkspace;
}

void WorkspaceManager::moveActiveWindowToWorkspace(int workspaceIndex)
{
    if (workspaceIndex == this->screen.activeWorkspace->workspaceIndex)
    {
        logInfo("Window already on workspace " + std::to_string(workspaceIndex));
        return;
    }

    logInfo("Move window to workspace " + std::to_string(workspaceIndex));

    auto windowToMove = this->getActiveWorkspace()->focusedWindow;
    auto workspaceToMoveTo = this->workspaces.at(workspaceIndex);

    workspaceToMoveTo->addWindow(windowToMove);
    this->getActiveWorkspace()->removeWindow(windowToMove.hwnd);
}

void WorkspaceManager::reloadConfig()
{
    config.reload();
    logInfo("Config reloaded.");
}
