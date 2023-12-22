#include "environment.h"
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <shlobj.h>

std::string path;
std::string storageFolderName = "\\i3win\\";

std::string getAppStoragePath() {
    if (!path.empty()) {
        return path;
    }

    wchar_t appDataPath[MAX_PATH];

    if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::cerr << "Failed to get the APPDATA folder path." << std::endl;
        return std::string();
    }

    // Convert wide string to std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    path = converter.to_bytes(appDataPath) + "\\i3win\\";
    
    std::cout << "AppData folder found: " << path << std::endl;

    return path;
}