#include "workspace.h"
#include "screen.h"

void Workspace::init(){
    if(!this->isInit){

        for(int i = 0; i < 10; i++){
            Screen newScreen;
            newScreen.initialize();
            screens[i] = newScreen;
        }

        activeWsp = 0;
        activeScreen = &screens[0];
        isInit = true;
    }
}

void Workspace::setActiveWsp(int wsp){
    this->activeWsp = wsp;
    this->activeScreen = &screens[wsp];
}
