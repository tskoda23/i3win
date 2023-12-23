#include <unordered_map>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

static std::string MAIN_WINDOW_WIDTH_PERCENTAGE = "MAIN_WINDOW_WIDTH_PERCENTAGE";

static std::unordered_map<std::string, std::string> defaultConfigValues = {
    {MAIN_WINDOW_WIDTH_PERCENTAGE, "50"}
};

class Config {
    public: 
        Config();
        void reload();
        int getNumericValue(std::string key);
        std::string getValue(std::string key);
    private:
        std::unordered_map<std::string, std::string> configValues;
        void loadConfigurationData();
};

#endif