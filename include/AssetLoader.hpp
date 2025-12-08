#pragma once
#include <string>

inline std::string getAssetPath(const std::string& relative) {
    const char* appdir = getenv("APPDIR");
    if (appdir) {
        return std::string(appdir) + "/usr/bin/assets/" + relative;
    }
    // Fallback when running from build/ or source directory
    return "./assets/" + relative;
}
