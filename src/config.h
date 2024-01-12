#include <unordered_map>
#include <string>

using namespace std;

#ifndef CONFIG_H
#define CONFIG_H

static const string COMMAND_MAIN_KEY = "COMMAND_MAIN_KEY";
static const string COMMAND_MODIFIER_KEY = "COMMAND_MODIFIER_KEY";

// Window operations
static const string COMMAND_SWITCH_FOCUS_LEFT = "COMMAND_SWITCH_FOCUS_LEFT";
static const string COMMAND_SWITCH_FOCUS_RIGHT = "COMMAND_SWITCH_FOCUS_RIGHT";
static const string COMMAND_MOVE_WINDOW_LEFT = "COMMAND_MOVE_WINDOW_LEFT";
static const string COMMAND_MOVE_WINDOW_RIGHT = "COMMAND_MOVE_WINDOW_RIGHT";
static const string COMMAND_CLOSE_WINDOW = "COMMAND_CLOSE_WINDOW";
static const string COMMAND_SET_AS_MAIN_WINDOW = "COMMAND_SET_AS_MAIN_WINDOW";
static const string COMMAND_MAIN_WINDOW_SIZE_DECREASE = "COMMAND_MAIN_WINDOW_SIZE_DECREASE";
static const string COMMAND_MAIN_WINDOW_SIZE_INCREASE = "COMMAND_MAIN_WINDOW_SIZE_INCREASE";
static const string MAIN_WINDOW_WIDTH_PERCENTAGE = "MAIN_WINDOW_WIDTH_PERCENTAGE";
static const string ADDITIONAL_WINDOW_PADDING = "ADDITIONAL_WINDOW_PADDING";

// Layouts
static const string COMMAND_SET_LAYOUT_NONE = "COMMAND_SET_LAYOUT_NONE";
static const string COMMAND_SET_LAYOUT_SPLIT = "COMMAND_SET_LAYOUT_SPLIT";
static const string COMMAND_SET_LAYOUT_STACKED = "COMMAND_SET_LAYOUT_STACKED";
static const string COMMAND_SET_LAYOUT_CENTERED = "COMMAND_SET_LAYOUT_CENTERED";

// Workspaces
static const string COMMAND_SET_WORKSPACE_1 = "COMMAND_SET_WORKSPACE_1";
static const string COMMAND_SET_WORKSPACE_2 = "COMMAND_SET_WORKSPACE_2";
static const string COMMAND_SET_WORKSPACE_3 = "COMMAND_SET_WORKSPACE_3";
static const string COMMAND_SET_WORKSPACE_4 = "COMMAND_SET_WORKSPACE_4";
static const string COMMAND_SET_WORKSPACE_5 = "COMMAND_SET_WORKSPACE_5";
static const string COMMAND_SET_WORKSPACE_6 = "COMMAND_SET_WORKSPACE_6";
static const string COMMAND_SET_WORKSPACE_7 = "COMMAND_SET_WORKSPACE_7";
static const string COMMAND_SET_WORKSPACE_8 = "COMMAND_SET_WORKSPACE_8";
static const string COMMAND_SET_WORKSPACE_9 = "COMMAND_SET_WORKSPACE_9";
static const string COMMAND_SET_WORKSPACE_10 = "COMMAND_SET_WORKSPACE_10";

static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_1 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_1";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_2 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_2";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_3 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_3";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_4 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_4";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_5 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_5";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_6 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_6";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_7 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_7";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_8 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_8";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_9 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_9";
static const string COMMAND_MOVE_WINDOW_TO_WORKSPACE_10 = "COMMAND_MOVE_WINDOW_TO_WORKSPACE_10";

// Misc
static string COMMAND_RELOAD_CONFIG = "COMMAND_RELOAD_CONFIG";

static unordered_map<string, string> defaultConfigValues = {
    {COMMAND_MAIN_KEY, "ralt"},
    {COMMAND_MODIFIER_KEY, "rshift"},

    // Window operations
    {COMMAND_SWITCH_FOCUS_LEFT, "j"},
    {COMMAND_SWITCH_FOCUS_RIGHT, "k"},
    {COMMAND_MOVE_WINDOW_LEFT, "h"},
    {COMMAND_MOVE_WINDOW_RIGHT, "l"},
    {COMMAND_CLOSE_WINDOW, "y"},
    {COMMAND_SET_AS_MAIN_WINDOW, "b"},
    {COMMAND_MAIN_WINDOW_SIZE_DECREASE, "n"},
    {COMMAND_MAIN_WINDOW_SIZE_INCREASE, "m"},
    {MAIN_WINDOW_WIDTH_PERCENTAGE, "50"},
    {ADDITIONAL_WINDOW_PADDING, "0"},

    // Layouts
    {COMMAND_SET_LAYOUT_NONE, "a"},
    {COMMAND_SET_LAYOUT_SPLIT, "s"},
    {COMMAND_SET_LAYOUT_STACKED, "d"},
    {COMMAND_SET_LAYOUT_CENTERED, "f"},

    // Workspace operations
    { COMMAND_SET_WORKSPACE_1, "1"},
    { COMMAND_SET_WORKSPACE_2, "2"},
    { COMMAND_SET_WORKSPACE_3, "3"},
    { COMMAND_SET_WORKSPACE_4, "4"},
    { COMMAND_SET_WORKSPACE_5, "5"},
    { COMMAND_SET_WORKSPACE_6, "6"},
    { COMMAND_SET_WORKSPACE_7, "7"},
    { COMMAND_SET_WORKSPACE_8, "8"},
    { COMMAND_SET_WORKSPACE_9, "9"},
    { COMMAND_SET_WORKSPACE_10, "0"},

    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_1, "1"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_2, "2"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_3, "3"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_4, "4"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_5, "5"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_6, "6"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_7, "7"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_8, "8"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_9, "9"},
    { COMMAND_MOVE_WINDOW_TO_WORKSPACE_10, "0"},

    // Misc
    {COMMAND_RELOAD_CONFIG, "v"},
};

class Config {
    public: 
        Config();
        void reload();
        int getNumericValue(const string &key);
        string getValue(const string &key);
    private:
        unordered_map<string, string> configValues;
        string storagePath;
        void loadConfigurationData();
};

#endif