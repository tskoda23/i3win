#include "layouts.h"
#include "screen.h"
#include "window.h"

#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <iostream>

// TODO: Write this in some log file

void logError(std::string error) {
    std::cout << "ERROR: " << error << std::endl;
}

void logInfo(std::string error) {
    std::cout << "INFO: " << error << std::endl;
}