#include "model.h"

std::vector<GLfloat> Model::ReadFile(const char* file){
  // Open file
  std::ifstream t(file);
	std::vector<GLfloat> result;
	std::cout << "Reading : " << file << std::endl;

	while (t.good())
	{
		std::string str;
		t >> str;
		GLfloat f = std::atof(str.c_str());
		result.push_back(f);
	}

	return result;
}

void Model::setup(){
  //Reset the Model Matrix
  reset();

  //Setup VAO and VBOs
  glGenVertexArrays(1, vao);
  glBindVertexArray(vao[0]);
  glGenBuffers(3, vbo);

  //Positions Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[positionAttributeIndex]);
  glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(positionAttributeIndex);
  glVertexAttribPointer(positionAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //Color Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[colorAttributeIndex]);
  glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(colorAttributeIndex);
  glVertexAttribPointer(colorAttributeIndex, 4, GL_FLOAT, GL_FALSE, 0, 0);

  //Normal Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[normalAttributeIndex]);
  glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(normalAttributeIndex);
  glVertexAttribPointer(normalAttributeIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Model::cleanup(){
  glDisableVertexAttribArray(0);
  glDeleteBuffers(3, vbo);
  glDeleteVertexArrays(1, vao);
}

//Position Functions
void Model::reset(){
  //Construct a unit matrix
  model = glm::mat4(1.0f);
}

void Model::translate(const glm::vec3 &axis){
  //Construct the Translation Matrix
  pos += axis;
  model = glm::translate(model, axis);
}

void Model::rotate(const glm::vec3 &axis, float angle){
  //Add the rotation operation to the model matrix
  model = glm::translate(glm::rotate(glm::translate(model, -pos), angle, axis), pos);
}

void Model::render(){
  //Render the Object, using the shader it has!
  glBindVertexArray(vao[0]);
  glDrawArrays(GL_TRIANGLES, 0, positions.size()/3);
}
