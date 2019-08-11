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

void Model::fromChunk(Chunk chunk, int LOD){
  /*
  if LOD = 4, then we load all of it
  if LOD = 3, then we load half of it
  if LOD = 0, then we only do it once
  i < pow(2, LOD);
  */

  //Make sure we don't dive too far
  if(LOD > chunk.data.depth){
    LOD = chunk.data.depth;
  }

  //Really, we should only dive down the Octree!

  //Get the scale multiplier
  int scale = pow(2, chunk.data.depth-LOD);
  int width = pow(2, LOD);

  //Loop over all elements inside the chunk
  for(int i = 0; i < width; i++){
    for(int j = 0; j < width; j++){
      for(int k = 0; k < width; k++){

        //Get the current block type
        BlockType _type = chunk.data.getPosition(scale*i,scale*j,scale*k, LOD);

        //Make sure it isn't air
        if(_type != 0){
          //Number of Faces we added
          int faces = 0;

          //For higher LOD values, we actually need LESS vertices!


          //In the future, this could be made even more efficient (if deemed necessary)

          //Only exposed surfaces!
          if(k+1 == width || chunk.data.getPosition(scale*i,scale*j,scale*(k+1), LOD) == 0){
            GLfloat front[] = { -0.5,  0.5, 0.5,
                                 0.5,  0.5, 0.5,
                                 0.5, -0.5, 0.5,
                                -0.5,  0.5, 0.5,
                                 0.5, -0.5, 0.5,
                                -0.5, -0.5, 0.5};

            for(int l = 0; l < 6; l++){
              positions.push_back(scale*(front[l*3]+i));
              positions.push_back(scale*(front[l*3+1]+j));
              positions.push_back(scale*(front[l*3+2]+k));
            }

            //Draw the Front Face
            faces++;
          }

          if(j+1 == width || chunk.data.getPosition(scale*i,scale*(j+1),scale*k, LOD) == 0){
            GLfloat top[] = {-0.5,  0.5,  0.5,
                              0.5,  0.5, -0.5,
                              0.5,  0.5,  0.5,

                              -0.5,  0.5,  0.5,
                              -0.5,  0.5, -0.5,
                              0.5,  0.5, -0.5};

            //Draw the Back Face
            for(int l = 0; l < 6; l++){
              positions.push_back(scale*(top[l*3]+i));
              positions.push_back(scale*(top[l*3+1]+j));
              positions.push_back(scale*(top[l*3+2]+k));
            }

            faces++;
          }

          if(i+1 == width || chunk.data.getPosition(scale*(i+1),scale*j,scale*k, LOD) == 0){
            GLfloat right[] = { 0.5, -0.5,  0.5,
                                0.5,  0.5,  0.5,
                                0.5,  0.5, -0.5,

                                0.5, -0.5,  0.5,
                                0.5,  0.5, -0.5,
                                0.5, -0.5, -0.5};

            //Draw the Top Face
            for(int l = 0; l < 6; l++){
              positions.push_back(scale*(right[l*3]+i));
              positions.push_back(scale*(right[l*3+1]+j));
              positions.push_back(scale*(right[l*3+2]+k));
            }

            faces++;
          }
          if(k-1 < 0 || chunk.data.getPosition(scale*i,scale*j,scale*(k-1), LOD) == 0){
            GLfloat back[] = {-0.5,  0.5, -0.5,
                               0.5, -0.5, -0.5,
                               0.5,  0.5, -0.5,

                              -0.5,  0.5, -0.5,
                              -0.5, -0.5, -0.5,
                               0.5, -0.5, -0.5};

            for(int l = 0; l < 6; l++){
              positions.push_back(scale*(back[l*3]+i));
              positions.push_back(scale*(back[l*3+1]+j));
              positions.push_back(scale*(back[l*3+2]+k));
            }


            //Draw the Bottom Face
            faces++;
          }
          /*
          if(chunk.data.getPosition(i,j-1,k, LOD) == 0){
            GLfloat bottom[] = {-0.5, -0.5,  0.5,
            0.5, -0.5,  0.5,
            0.5, -0.5, -0.5,

            -0.5, -0.5,  0.5,
            0.5, -0.5, -0.5,
            -0.5, -0.5, -0.5};

            //Draw the Left Face
            for(int l = 0; l < 6; l++){
              positions.push_back(bottom[l*3]+i);
              positions.push_back(bottom[l*3+1]+j);
              positions.push_back(bottom[l*3+2]+k);
            }
            faces++;
          }*/
          if(i-1 < 0 || chunk.data.getPosition(scale*(i-1),scale*j,scale*k, LOD) == 0){
            GLfloat left[] = {-0.5, -0.5,  0.5,
                              -0.5,  0.5, -0.5,
                              -0.5,  0.5,  0.5,

                              -0.5, -0.5,  0.5,
                              -0.5, -0.5, -0.5,
                              -0.5,  0.5, -0.5};

            //Draw the Right Face
            for(int l = 0; l < 6; l++){
              positions.push_back(scale*(left[l*3]+i));
              positions.push_back(scale*(left[l*3+1]+j));
              positions.push_back(scale*(left[l*3+2]+k));
            }
            faces++;
          }

          //We need to get a color
          glm::vec4 color = chunk.getColorByID(chunk.data.getPosition(scale*i, scale*j, scale*k, LOD));
          //For all 36 triangles we add a color (12*3)
          for(int m = 0; m < 6*faces; m++){
            colors.push_back(color.x);
            colors.push_back(color.y);
            colors.push_back(color.z);
            colors.push_back(color.w);
          }
        }
      }
    }
  }

  //Compute all Surface Normals
  //For every surface we compute the surface normal
  for(unsigned int m = 0; m < positions.size()/9; m++){
    glm::vec3 v1 = glm::vec3(positions[m*9], positions[m*9+1], positions[m*9+2]);
    glm::vec3 v2 = glm::vec3(positions[m*9+3], positions[m*9+4], positions[m*9+5]);
    glm::vec3 v3 = glm::vec3(positions[m*9+6], positions[m*9+7], positions[m*9+8]);
    glm::vec3 edge1 = v3-v1;
    glm::vec3 edge2 = v2-v1;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    //We add the x,y,z value 3 times (once for each for every vertex)
    for(int n = 0; n < 3; n++){
      normals.push_back(normal.x);
      normals.push_back(normal.y);
      normals.push_back(normal.z);
    }
  }
}
