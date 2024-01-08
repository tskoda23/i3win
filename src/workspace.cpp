#include "workspace.h"
#include "screen.h"

void Workspace::init(){
    for(int i = 0; i < 10; i++){
        Screen newScreen;
        newScreen.initialize();
        screens[i] = newScreen;
    }

    activeWsp = 0;
    activeScreen = &screens[0];
}

void Workspace::setActiveWsp(int wsp){
    activeWsp = wsp;
    activeScreen = &screens[wsp];
}
