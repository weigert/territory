#pragma once
#include "../forward/sprite.fwd.h"
//Dependencies

class Animation{
public:
  //Metadata
  int ID = 0;
  int nframe = 0;
  int frames = 1;

  //Animation Image Parameters
  int w = 4;
  int h = 4;

  //Translation of Model during animation
  glm::vec3 translate = glm::vec3(0);
};

class Sprite{
public:
  //Draw Stuff
  GLuint vao;
  GLuint vbo[2];
  GLuint texture;
  glm::mat4 model = glm::mat4(1.0f);

  //Load the Texture File
  bool loadImage(std::string fileName);
  void resetModel();
  void setupBuffer(bool small);

  //Animation Stuff
  Animation animation;
  bool doAnimationFrame();
  void setAnimation(int id, glm::vec3 _translate);

  //Render and Cleanup
  void render();
  void cleanup();
};
