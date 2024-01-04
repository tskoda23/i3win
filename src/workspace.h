#include "screen.h"

#ifndef WORKSPACE_H 
#define WORKSPACE_H 

struct Workspace{
    int activeWsp;
    Screen screens[10]; 
    Screen *activeScreen; 
    bool isInit = false;

    void init();
    void setActiveWsp(int wsp);
};

#endif