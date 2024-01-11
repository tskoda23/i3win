#include "screen.h"
#include "workspace.h"
#include "config.h"
#include "state.h"

#ifndef WORKSPACE_MANAGER_H 
#define WORKSPACE_MANAGER_H 

class WorkspaceManager {
    public:
        WorkspaceManager();

        Workspace* getActiveWorkspace();
        std::vector<Workspace*> getAllWorkspaces();

        void setActiveWorkspace(int workspaceIndex);
        void moveActiveWindowToWorkspace(int workspaceIndex);
        void reloadConfig();
    private:
        int screenWidth;
        int screenHeight;
        std::vector<Workspace*> workspaces;
        Config config;
        State state;
        Screen screen;
};

#endif