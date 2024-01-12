#include <unordered_map>
#include <string>
using namespace std;

#ifndef STATE_H
#define STATE_H

static const string MAIN_WINDOW_PERCENTAGE_CHANGE = "MAIN_WINDOW_PERCENTAGE_CHANGE";
static const string ACTIVE_LAYOUT = "ACTIVE_LAYOUT";

static unordered_map<string, string> defaultStateValues = {
    {MAIN_WINDOW_PERCENTAGE_CHANGE, "0"},
    {ACTIVE_LAYOUT, "0"}
};

class State {
    public: 
        State();
        void reload();
        void setValue(string key, string value);
        int getNumericValue(string key);
        string getValue(string key);
    private:
        unordered_map<string, string> values;
        string storagePath;
        void loadStateData();
};

#endif