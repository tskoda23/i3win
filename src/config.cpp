#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "config.h"
#include "logger.h"

// TODO: This path should be:
// - created if it doesn't exist
// - moved to %APPDATA% or something
static std::string configPath = "..\\..\\config\\i3win.conf";

void Config::loadConfigurationData() {
    std::ifstream inputFile(configPath);

    if (!inputFile.is_open()) {
        logError("Error opening the config file!");
        return;
    }

    std::string line;
    
    while (std::getline(inputFile, line)) {
        if (line.empty() || line.front() == '#') {
            // Empty line or a comment, skip it
            continue;
        }

        // Use an istringstream to split the line by the equals sign
        std::istringstream iss(line);
        std::string key, value;

        if (std::getline(iss, key, '=')) {
            // Read the value part (the rest of the line)
            if (std::getline(iss, value)) {
                logInfo("Reading config value: " + key + ": " + value);
                configValues[key] = value;
            }
        }
    }

    inputFile.close();
}

void Config::reload() {
    loadConfigurationData();
}

Config::Config() {
    loadConfigurationData();
}

int Config::getNumericConfigValue(std::string key) {
    auto value = configValues[key];
    try {
        return std::stoi(value);
    }
    catch (const std::invalid_argument& e) {
        auto message = "Integer conversion failed: " + key; 
        logError(message);
        throw message;
    }
    
}

std::string Config::getConfigValue(std::string key) {
    return configValues[key];
}

