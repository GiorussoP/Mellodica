#include "render/TextureAtlas.hpp"
#include "Json.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

TextureAtlas::TextureAtlas(unsigned int textureIndex)
    :  mAtlasWidth(0)
    , mAtlasHeight(0)
    , mTileWidth(0)
    , mTileHeight(0)
    , mColumns(0)
    , mRows(0)
    , mTextureIndex(textureIndex)
{
}

TextureAtlas::~TextureAtlas()
{
}

bool TextureAtlas::Load(const std::string& jsonPath)
{
    // Parse JSON file
    std::ifstream jsonFile(jsonPath);
    if (!jsonFile.is_open()) {
        std::cerr << "Failed to open atlas JSON: " << jsonPath << std::endl;
        return false;
    }
    
    json atlasData;
    try {
        jsonFile >> atlasData;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse atlas JSON: " << e.what() << std::endl;
        jsonFile.close();
        return false;
    }
    jsonFile.close();
    
    // Read atlas metadata
    if (atlasData.contains("meta") && atlasData["meta"].contains("size")) {
        mAtlasWidth = atlasData["meta"]["size"]["w"];
        mAtlasHeight = atlasData["meta"]["size"]["h"];
    } else {
        std::cerr << "Atlas JSON missing meta/size information" << std::endl;
        return false;
    }
    
    // Read frames/tiles
    if (!atlasData.contains("frames") || !atlasData["frames"].is_array()) {
        std::cerr << "Atlas JSON missing frames array" << std::endl;
        return false;
    }
    
    int tileIndex = 0;
    for (const auto& frame : atlasData["frames"]) {
        AtlasTile tile;
        tile.name = frame["filename"];
        tile.x = frame["frame"]["x"];
        tile.y = frame["frame"]["y"];
        tile.width = frame["frame"]["w"];
        tile.height = frame["frame"]["h"];
        tile.tileIndex = tileIndex;

        // Debug print
        std::cout << "Atlas tile: index=" << tileIndex << ", name='" << tile.name << "'" << std::endl;

        // Assume uniform tile sizes (use first tile as reference)
        if (tileIndex == 0) {
            mTileWidth = tile.width;
            mTileHeight = tile.height;
        }

        mTiles[tile.name] = tile;
        mTileList.push_back(tile);
        tileIndex++;
    }
    
    // Calculate columns and rows
    if (mTileWidth > 0 && mTileHeight > 0) {
        mColumns = mAtlasWidth / mTileWidth;
        mRows = mAtlasHeight / mTileHeight;
    }
    
    std::cout << "Loaded texture atlas: " << mTileList.size() << " tiles, "
              << mColumns << "x" << mRows << " grid, "
              << mTileWidth << "x" << mTileHeight << " px per tile" << std::endl;
    
    return true;
}

int TextureAtlas::GetTileIndex(const std::string& tileName) const
{
    auto it = mTiles.find(tileName);
    if (it != mTiles.end()) {
        return it->second.tileIndex;
    }
    return -1; // Not found
}
