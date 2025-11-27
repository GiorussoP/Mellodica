#include "CSV.h"
#include <fstream>
#include <SDL_log.h>
#include <sstream>
#include <algorithm>

unsigned numberOfLines(const std::string& filePath) {
    std::ifstream inFile(filePath);
    return std::count(std::istreambuf_iterator<char>(inFile),
               std::istreambuf_iterator<char>(), '\n');
}

MapReader::MapReader(const std::string& filePath) {
    height = numberOfLines(filePath);
    data.assign(height, {});
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    SDL_Log("[MapReader] Loading level %s\n", filePath.c_str());
    width = 0;
    for (unsigned i = 0; i < height; i++) {
        std::string line; buffer >> line;
        auto entries = CSVHelper::Split(line);
        if (width == 0) width = entries.size();
        for (int entry : entries) {
            data[i].push_back(entry);
        }
    }
}
