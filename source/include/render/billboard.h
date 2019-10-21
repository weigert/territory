#pragma once
#include "../forward/billboard.fwd.h"

//Class
class Billboard{
public:
  GLuint fbo;       //We need an FBO to render scene to screen
  GLuint texture;
  GLuint depthTexture;
  GLuint vao;
  GLuint vbo[2];

  //Load the Texture from some surface
  bool fromRaw(SDL_Surface* TextureImage);
  bool fromImage(std::string file);

  //Render the Billboard to a specific quad
  bool setup(int width, int height);
  bool setup2(int width, int height);
  void render();
  void renderDepth();
  void cleanup();
};
