#include "workspace.h"
#include "screen.h"

void Workspace::init(){
    if(!isInit){
        for(int i = 0; i < 10; i++){
            Screen newScreen;
            screens[i] = newScreen;
        }

        activeWsp = 0;
        activeScreen = &screens[0];
        activeScreen->initialize();
        isInit = true;
    }
}

void Workspace::setActiveWsp(int wsp){
    activeWsp = wsp;
    activeScreen = &screens[wsp];

    if(!activeScreen->isInit){
        activeScreen->initialize();
    }

    printf("Switch active screen to %d", wsp);
}
