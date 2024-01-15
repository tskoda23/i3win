#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "config.h"
#include "logger.h"
#include "environment.h"

void Config::loadConfigurationData() {  
    std::ifstream inputFile(storagePath);

    getAppStoragePath();

    if (!inputFile.is_open()) {
        logError("Error opening the config file! " + storagePath);
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
    this->storagePath = getAppStoragePath() + "i3win.conf";
    loadConfigurationData();
}

int Config::getNumericValue(const std::string &key) {
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

std::string Config::getValue(const std::string &key) {
    if (key.empty()) {
        logError("Empty key provided for value");
        throw;
    }

    auto value = configValues[key];

     if (value.empty()) {
        return defaultConfigValues[key];
     }

     if (value[0] == '"') {
         auto err = "Config values shouldn't be wrapped in quotes.";
         logError(err);
         throw err;
     }

     return value;
}

