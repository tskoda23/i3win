#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "state.h"
#include "logger.h"
#include "environment.h"

void State::loadStateData() {
    std::ifstream inputFile(storagePath);

    if (!inputFile.is_open()) {
        logError("Error opening the state file!");
        return;
    }

    std::string line;
    
    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }

        // Use an istringstream to split the line by the equals sign
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=')) {
            // Read the value part (the rest of the line)
            if (std::getline(iss, value)) {
                logInfo("Reading state value: " + key + ": " + value);
                values[key] = value;
            }
        }
    }

    inputFile.close();
}

void State::setValue(std::string key, std::string value) {
    values[key] = value;

    std::ofstream outFile(storagePath, std::ofstream::trunc);

    if (!outFile.is_open()) {
        outFile.open(storagePath);
    }

    if (outFile.is_open()) {
        outFile.clear();
        for (auto item : values) {
            outFile << item.first<< "=" << item.second << std::endl;
        }
        outFile.close();
    }

    loadStateData();
}

void State::reload() {
    loadStateData();
}

State::State() {
    this->storagePath = getAppStoragePath() + "i3win.state";
    loadStateData();
}

int State::getNumericValue(std::string key) {
    auto value = getValue(key);

    try {
        return std::stoi(value);
    }
    catch (const std::invalid_argument& e) {
        auto message = "Integer conversion failed: " + key; 
        logError(message);
        throw message;
    }
}

std::string State::getValue(std::string key) {
    if (key.empty()) {
        logError("Empty key provided for value");
        throw;
    }

    auto value = values[key];

     if (value.empty()) {
        return defaultStateValues[key];
    }

     return value;
}
