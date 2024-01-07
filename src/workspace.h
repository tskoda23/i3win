#include "screen.h"

#ifndef WORKSPACE_H 
#define WORKSPACE_H 

struct Workspace{
    Screen screens[10]; 
    int activeWsp;
    Screen *activeScreen; 

    void init();
    void setActiveWsp(int wsp);
};

#endif