#pragma once

#include <string>
#include <map>
#include <vector>
#include "render/Texture.hpp"

// Represents a single tile/frame in the atlas
struct AtlasTile {
    std::string name;
    int x, y;          // Position in pixels
    int width, height; // Size in pixels
    int tileIndex;     // Calculated tile index for shader
};

class TextureAtlas {
public:
    TextureAtlas(unsigned int textureIndex);
    ~TextureAtlas();
    
    // Load atlas metadata from JSON file (doesn't load the texture itself)
    bool Load(const std::string& jsonPath);
    
    // Get tile index by name
    int GetTileIndex(const std::string& tileName) const;
    
    // Get atlas dimensions
    int GetAtlasWidth() const { return mAtlasWidth; }
    int GetAtlasHeight() const { return mAtlasHeight; }
    
    // Get tile size (assumes uniform tiles)
    int GetTileWidth() const { return mTileWidth; }
    int GetTileHeight() const { return mTileHeight; }
    
    // Get number of columns in the atlas
    int GetColumns() const { return mColumns; }
    
    // Get UV tile size (for shader)
    float GetUVTileSizeX() const { 
        return mTileWidth > 0 ? static_cast<float>(mTileWidth) / mAtlasWidth : 0.0f; 
    }
    float GetUVTileSizeY() const { 
        return mTileHeight > 0 ? static_cast<float>(mTileHeight) / mAtlasHeight : 0.0f; 
    }
    
    // Get UV offset for a specific tile index (actual position from JSON, not grid-based)
    bool GetTileUVOffset(int index, float& outX, float& outY) const {
        if (index >= 0 && index < static_cast<int>(mTileList.size())) {
            outX = static_cast<float>(mTileList[index].x) / mAtlasWidth;
            outY = static_cast<float>(mTileList[index].y) / mAtlasHeight;
            return true;
        }
        return false;
    }

    unsigned int GetTextureIndex() const { return mTextureIndex; }
    void SetTextureIndex(unsigned int index) { mTextureIndex = index; }

private:
    std::map<std::string, AtlasTile> mTiles;
    std::vector<AtlasTile> mTileList;  // Ordered list for index lookup
    
    int mAtlasWidth;
    int mAtlasHeight;
    int mTileWidth;
    int mTileHeight;
    int mColumns;
    int mRows;
    unsigned int mTextureIndex;
};
