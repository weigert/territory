class Model{
public:
  //Model Information
  std::vector<GLfloat> positions;
  std::vector<GLfloat> normals;
	std::vector<GLfloat> colors;

  //Model Matrix (helps translate vertices from model space to real space)
  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 pos = glm::vec3(0.0f);

  // Create variables for storing the ID of our VAO and VBO
	GLuint vbo[3], vao[1];

  //Read File Functions
  std::vector<GLfloat> ReadFile(const char* file);
  void setup();
  void cleanup();

  //Model Generation Functions
  void fromChunk(Chunk _chunk, int LOD);
  void fromOctree(Octree _octree, int LOD, glm::vec3 translate);

  //Position Handling Functions
  void reset();
  void translate(const glm::vec3 &axis);
  void rotate(const glm::vec3 &axis, float angle);

  //Render Function to Render Stuff with the shader, so we can do this to each model individually
  void render();

  //Face Stuff
  //Construct the Coordinates
  GLfloat front[18] = { -0.5,  0.5, 0.5,
                       0.5,  0.5, 0.5,
                       0.5, -0.5, 0.5,
                      -0.5,  0.5, 0.5,
                       0.5, -0.5, 0.5,
                      -0.5, -0.5, 0.5};

  GLfloat top[18] = {   -0.5,  0.5,  0.5,
                       0.5,  0.5, -0.5,
                       0.5,  0.5,  0.5,
                      -0.5,  0.5,  0.5,
                      -0.5,  0.5, -0.5,
                       0.5,  0.5, -0.5};

  GLfloat right[18] = {  0.5, -0.5,  0.5,
                       0.5,  0.5,  0.5,
                       0.5,  0.5, -0.5,
                       0.5, -0.5,  0.5,
                       0.5,  0.5, -0.5,
                       0.5, -0.5, -0.5};

  GLfloat back[18] = {  -0.5,  0.5, -0.5,
                       0.5, -0.5, -0.5,
                       0.5,  0.5, -0.5,
                      -0.5,  0.5, -0.5,
                      -0.5, -0.5, -0.5,
                       0.5, -0.5, -0.5};

  GLfloat bottom[18] = {-0.5, -0.5,  0.5,
                       0.5, -0.5,  0.5,
                       0.5, -0.5, -0.5,
                      -0.5, -0.5,  0.5,
                       0.5, -0.5, -0.5,
                      -0.5, -0.5, -0.5};

   GLfloat left[18] = { -0.5, -0.5,  0.5,
                      -0.5,  0.5, -0.5,
                      -0.5,  0.5,  0.5,
                      -0.5, -0.5,  0.5,
                      -0.5, -0.5, -0.5,
                      -0.5,  0.5, -0.5};

};
