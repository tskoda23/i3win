#include "environment.h"
#include "logger.h"

#include <windows.h>
#include <psapi.h>
#include <winerror.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

std::string infoFilePath = getAppStoragePath() + "info.txt";
std::string errorFilePath = getAppStoragePath() + "error.txt";

std::string getTimestamp() {
    char buffer[26]; 

    time_t currentTime;
    time(&currentTime);

    if (ctime_s(buffer, sizeof(buffer), &currentTime) != 0) {
        std::cerr << "Error converting time to string" << std::endl;
    }

    // Remove newline from timestamp
    buffer[strlen(buffer) - 1] = '\0';

    return std::string(buffer);
}

void writeToFile(std::string filePath, std::string text) {
   std::ofstream outFile(filePath, std::ios::app);

    if (!outFile.is_open()) {
        outFile.open(filePath);
    }

    if (outFile.is_open()) {
        outFile << getTimestamp() << " - " << text << std::endl;
        outFile.close();
    }
}

void logError(std::string error) {
    writeToFile(errorFilePath, error);

    std::cout << "ERROR: " << error << std::endl;
}

void logInfo(std::string info) {
    writeToFile(infoFilePath, info);

    std::cout << "INFO: " << info << std::endl;
}
