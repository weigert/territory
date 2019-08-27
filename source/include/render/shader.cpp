#include "shader.h"

void Shader::setup(std::string vs, std::string fs){
  //Generate our Shader
  shaderProgram = glCreateProgram();

  //Current Path
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "source/include/render/shader";

  //Add the two shaders
  vertexShader = addShader((data_dir/vs).string(), GL_VERTEX_SHADER);
  fragmentShader = addShader((data_dir/fs).string(), GL_FRAGMENT_SHADER);

  //Link the Shader
  linkProgram();
}

void Shader::addAttribute(int pos, std::string attrName){
  glBindAttribLocation(shaderProgram, pos, attrName.c_str());
}

void Shader::compileShader(GLuint shader){
  glCompileShader(shader);

  //Bug Checking
  int wasCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &wasCompiled );

  if (wasCompiled == 0){
    std::cout<<"Error"<<std::endl;
    int maxLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char* shaderInfoLog = new char[maxLength];
    glGetShaderInfoLog(shader, maxLength, &maxLength, shaderInfoLog);
    std::cout << "Linker error message : " << shaderInfoLog << std::endl;
  }
  else{
    glAttachShader(shaderProgram, shader);
  }
}

void Shader::linkProgram(){
  glLinkProgram(shaderProgram);
  int isLinked;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == false){
    int maxLength;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

    // Get shader info log
    char* shaderProgramInfoLog = new char[maxLength];
    glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);
    std::cout << "Linker error message : " << shaderProgramInfoLog << std::endl;

    delete shaderProgramInfoLog;
  }
}

void Shader::useProgram(){
  glUseProgram(shaderProgram);
}

int Shader::addShader(std::string fileName, GLenum shaderType){
  char* src;
  int32_t size;
  std::string result = readGLSLFile(fileName, size);
  src = const_cast<char*>(result.c_str());
  int shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &src, &size);
  compileShader(shaderID);
  return shaderID;
}

std::string Shader::readGLSLFile(std::string fileName, int32_t &size){
  //Load the stuff
  //Fragment Shader
  std::ifstream t;
  std::string fileContent;
  //Open the file
  t.open(fileName);
  if(t.is_open()){
    std::stringstream buffer;
    buffer << t.rdbuf();
    fileContent = buffer.str();
  }
  else{
    std::cout<<"File opening failed"<<std::endl;
  }
  t.close();
  //Load into the variables
  size = fileContent.length();
  return fileContent;
}

void Shader::cleanup(){
  //Delete the Objects
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
}

void Shader::setMat4(std::string name, const glm::mat4 mat){
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(std::string name, const glm::vec3 vec){
  glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}

void Shader::setInt(std::string name, int value){
  ///Set the Uniform
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setFloat(std::string name, float value){
  ///Set the Uniform
  glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
