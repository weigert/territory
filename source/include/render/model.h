#pragma once
#include "../forward/model.fwd.h"
//Dependencies
#include "../forward/chunk.fwd.h"

class Model{
public:
  //Model Information
  std::vector<GLfloat> positions;
  std::vector<GLfloat> normals;
  std::vector<GLfloat> colors;
  std::vector<GLuint> indices;

  // Create variables for storing the ID of our VAO and VBO
	GLuint vbo[3], vao, ibo;

  //Model Matrix (helps translate vertices from model space to real space)
  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 pos = glm::vec3(0.0f);  //Chunkspace Position!
  glm::vec3 cpos = glm::vec3(0);

  //Read File Functions
  void setup();
  void update();
  void cleanup();

  //Model Generation Functions
  void fromChunkNaive(Chunk _chunk);
  void fromChunkGreedy(Chunk _chunk);
  void addCube(glm::vec3 pos, BlockType type);
  //void fromOctree(Octree _octree, int LOD, glm::vec3 translate);

  //Position Handling Functions
  void reset();
  void translate(const glm::vec3 &axis);
  void rotate(const glm::vec3 &axis, float angle);

  //Render Function to Render Stuff with the shader, so we can do this to each model individually
  void render();
};
