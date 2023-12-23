#include <unordered_map>
#include <string>

#ifndef STATE_H
#define STATE_H

class State {
    public: 
        State();
        void reload();
        void setValue(std::string key, std::string value);
        int getNumericValue(std::string key);
        std::string getValue(std::string key);
    private:
        std::unordered_map<std::string, std::string> values;
        void loadStateData();
};

#endif