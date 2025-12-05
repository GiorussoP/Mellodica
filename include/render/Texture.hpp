#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>

class Texture {
public:
  Texture();
  ~Texture();

  // Load a texture from file using SDL_Image
  bool Load(const std::string &fileName);

  // Load texture from SDL_Surface (for text rendering)
  bool LoadFromSurface(SDL_Surface *surface);

  // Unload the texture
  void Unload();

  // Bind this texture to a texture unit (0-31)
  void Bind(unsigned int textureUnit = 0);

  // Unbind texture
  void Unbind();

  // Get the OpenGL texture ID
  unsigned int GetTextureID() const { return mTextureID; }

  // Get texture dimensions
  int GetWidth() const { return mWidth; }
  int GetHeight() const { return mHeight; }

private:
  unsigned int mTextureID;
  int mWidth;
  int mHeight;
};
