#include <unordered_map>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

static std::string MAIN_WINDOW_WIDTH_PERCENTAGE = "MAIN_WINDOW_WIDTH_PERCENTAGE";

static std::string COMMAND_MODIFIER_KEY = "COMMAND_MODIFIER_KEY";

static std::string COMMAND_SWITCH_FOCUS_LEFT = "COMMAND_SWITCH_FOCUS_LEFT";
static std::string COMMAND_SWITCH_FOCUS_RIGHT = "COMMAND_SWITCH_FOCUS_RIGHT";
static std::string COMMAND_MOVE_WINDOW_LEFT = "COMMAND_MOVE_WINDOW_LEFT";
static std::string COMMAND_MOVE_WINDOW_RIGHT = "COMMAND_MOVE_WINDOW_RIGHT";

static std::string COMMAND_SET_LAYOUT_NONE = "COMMAND_SET_LAYOUT_NONE";
static std::string COMMAND_SET_LAYOUT_SPLIT = "COMMAND_SET_LAYOUT_SPLIT";
static std::string COMMAND_SET_LAYOUT_STACKED = "COMMAND_SET_LAYOUT_STACKED";
static std::string COMMAND_SET_LAYOUT_CENTERED = "COMMAND_SET_LAYOUT_CENTERED";

static std::string COMMAND_CLOSE_WINDOW = "COMMAND_CLOSE_WINDOW";
static std::string COMMAND_SET_AS_MAIN_WINDOW = "COMMAND_SET_AS_MAIN_WINDOW";

static std::string COMMAND_MAIN_WINDOW_SIZE_DECREASE = "COMMAND_MAIN_WINDOW_SIZE_DECREASE";
static std::string COMMAND_MAIN_WINDOW_SIZE_INCREASE = "COMMAND_MAIN_WINDOW_SIZE_INCREASE";

static std::string COMMAND_RELOAD_CONFIG = "COMMAND_RELOAD_CONFIG";

static std::unordered_map<std::string, std::string> defaultConfigValues = {
    {MAIN_WINDOW_WIDTH_PERCENTAGE, "50"},
    {COMMAND_MODIFIER_KEY, "alt"},

    {COMMAND_SWITCH_FOCUS_LEFT, "j"},
    {COMMAND_SWITCH_FOCUS_RIGHT, "k"},
    {COMMAND_MOVE_WINDOW_LEFT, "h"},
    {COMMAND_MOVE_WINDOW_RIGHT, "l"},

    {COMMAND_SET_LAYOUT_NONE, "a"},
    {COMMAND_SET_LAYOUT_SPLIT, "s"},
    {COMMAND_SET_LAYOUT_STACKED, "d"},
    {COMMAND_SET_LAYOUT_CENTERED, "f"},

    {COMMAND_CLOSE_WINDOW, "q"},
    {COMMAND_SET_AS_MAIN_WINDOW, "w"},
    {COMMAND_MAIN_WINDOW_SIZE_DECREASE, "e"},
    {COMMAND_MAIN_WINDOW_SIZE_INCREASE, "r"},

    {COMMAND_RELOAD_CONFIG, "t"}
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