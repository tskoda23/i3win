#include <unordered_map>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

class Config {
    public: 
        Config();
        void reload();
        int getNumericConfigValue(std::string key);
        std::string getConfigValue(std::string key);
    private:
        std::unordered_map<std::string, std::string> configValues;
        void loadConfigurationData();
};

#endif