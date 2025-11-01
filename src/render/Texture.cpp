#include "render/Texture.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

Texture::Texture()
    : mTextureID(0)
    , mWidth(0)
    , mHeight(0)
{
}

Texture::~Texture() {
    Unload();
}

bool Texture::Load(const std::string& fileName) {
    // Initialize SDL_image if needed
    static bool sdlImageInitialized = false;
    if (!sdlImageInitialized) {
        int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
            return false;
        }
        sdlImageInitialized = true;
    }

    // Load image using SDL_Image
    SDL_Surface* surface = IMG_Load(fileName.c_str());
    if (!surface) {
        std::cerr << "Failed to load texture: " << fileName << " - " << IMG_GetError() << std::endl;
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    // Determine format
    GLenum format = GL_RGB;
    if (surface->format->BytesPerPixel == 4) {
        format = GL_RGBA;
    }

    // Generate and bind texture
    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free SDL surface
    SDL_FreeSurface(surface);

    std::cout << "Loaded texture: " << fileName << " (" << mWidth << "x" << mHeight << ")" << std::endl;

    return true;
}

void Texture::Unload() {
    if (mTextureID != 0) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
    }
}

void Texture::Bind(unsigned int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
