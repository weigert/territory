#include "model.h"
//Dependencies
#include "../world/chunk.h"

void Model::setup(){
  //Reset the Model Matrix
  reset();

  //Setup VAO and VBOs
  glGenVertexArrays(1, vao);
  glBindVertexArray(vao[0]);
  glGenBuffers(3, vbo);

  //Positions Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //Color Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

  //Normal Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
  //Loop over all elements inside the chunk
  for(int i = 0; i < chunk.size; i++){
    for(int j = 0; j < chunk.size; j++){
      for(int k = 0; k < chunk.size; k++){

        //Get the current block type
        BlockType _type = (BlockType)chunk.data[chunk.getIndex(glm::vec3(i,j,k))];
        int width = chunk.size;

        //Make sure it isn't air
        if(_type != 0){
          //Number of Faces we added
          int faces = 0;

          //For higher LOD values, we actually need LESS vertices!
          //In the future, this could be made even more efficient (if deemed necessary)

          //Only exposed surfaces!
          if(k+1 == width || chunk.getPosition(glm::vec3(i,j,k+1), LOD) == 0){
            for(int l = 0; l < 6; l++){
              positions.push_back((front[l*3]+i));
              positions.push_back((front[l*3+1]+j));
              positions.push_back((front[l*3+2]+k));
            }

            //Draw the Front Face
            faces++;
          }

          if(j+1 == width || chunk.getPosition(glm::vec3(i,j+1,k), LOD) == 0){
            //Draw the Back Face
            for(int l = 0; l < 6; l++){
              positions.push_back((top[l*3]+i));
              positions.push_back((top[l*3+1]+j));
              positions.push_back((top[l*3+2]+k));
            }

            faces++;
          }

          if(i+1 == width || chunk.getPosition(glm::vec3(i+1,j,k), LOD) == 0){
            //Draw the Top Face
            for(int l = 0; l < 6; l++){
              positions.push_back((right[l*3]+i));
              positions.push_back((right[l*3+1]+j));
              positions.push_back((right[l*3+2]+k));
            }

            faces++;
          }
          if(k-1 < 0 || chunk.getPosition(glm::vec3(i,j,k-1), LOD) == 0){
            for(int l = 0; l < 6; l++){
              positions.push_back((back[l*3]+i));
              positions.push_back((back[l*3+1]+j));
              positions.push_back((back[l*3+2]+k));
            }

            //Draw the Bottom Face
            faces++;
          }
          /*
          if(chunk.data.getPosition(i,j-1,k, LOD) == 0){
            //Draw the Left Face
            for(int l = 0; l < 6; l++){
              positions.push_back(bottom[l*3]+i);
              positions.push_back(bottom[l*3+1]+j);
              positions.push_back(bottom[l*3+2]+k);
            }
            faces++;
          }*/
          if(i-1 < 0 || chunk.getPosition(glm::vec3(i-1,j,k), LOD) == 0){
            //Draw the Right Face
            for(int l = 0; l < 6; l++){
              positions.push_back((left[l*3]+i));
              positions.push_back((left[l*3+1]+j));
              positions.push_back((left[l*3+2]+k));
            }
            faces++;
          }

          //We need to get a color
          glm::vec4 color = chunk.getColorByID(chunk.getPosition(glm::vec3(i,j,k), LOD));

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
