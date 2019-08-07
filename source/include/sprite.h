class Sprite{
public:
  //Draw Stuff
  GLuint vao[1];
  GLuint vbo[2];
  GLuint texture;
  glm::mat4 model = glm::mat4(1.0f);

  //Load the Texture File
  bool loadImage(std::string fileName);
  void setupBuffer();

  void cleanup();
};
