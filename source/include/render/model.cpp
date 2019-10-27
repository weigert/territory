#include "model.h"
//Dependencies
#include "../world/chunk.h"

void Model::setup(){
  //Reset the Model Matrix
  reset();

  //Setup VAO and VBOs
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(3, vbo);
  glGenBuffers(1, &ibo);

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

  //Index Buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Model::update(){
  glBindVertexArray(vao);
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

  //Index Buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Model::cleanup(){
  glDisableVertexAttribArray(0);
  glDeleteBuffers(3, vbo);
  glDeleteVertexArrays(1, &vao);
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
  glBindVertexArray(vao);
  //Bind the Index Buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  //glDrawArrays(GL_TRIANGLES, 0, positions.size()/3);
}

void Model::fromChunkGreedy(Chunk chunk){
  //Clear the Containers
  positions.clear();
  colors.clear();
  normals.clear();

  //Loop over all 6 possible orientations...
  for(int d = 0; d < 6; d++){

    //Our actual dimensional indices
    int u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2
    int v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    int w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    int n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    //Slice Mask
    std::array<BlockType, 16*16> mask = {BLOCK_AIR};

    //Loop over the Positions
    for(x[u] = 0; x[u] < chunk.size; x[u]++){

      //Loop over a slice... (prepare mask)
      for(x[v] = 0; x[v] < chunk.size; x[v]++){
        for(x[w] = 0; x[w] < chunk.size; x[w]++){

          int s = x[w] + x[v]*chunk.size;
          BlockType atPos = chunk.getPosition(glm::vec3(x[0],x[1],x[2]));
          mask[s] = BLOCK_AIR;

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= chunk.size){
            mask[s] = atPos;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          BlockType facing = chunk.getPosition(glm::vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));
          if(facing == BLOCK_AIR){
            mask[s] = atPos;
          }
        }
      }

      //Loop AGAIN over the slice (evaluate mask)
      for(x[v] = 0; x[v] < chunk.size; x[v]++){
        for(x[w] = 0; x[w] < chunk.size; x[w]++){

          int width = 1;
          int height = 1;

          //Get the index of the current slice object we are observing!
          int s = x[w] + x[v]*chunk.size;

          //Get current blocktype, ignore air.
          BlockType current = mask[s];

          //Ignore this surface if it is not part of the mask.
          if(current == BLOCK_AIR) continue;

          //Compute the Width
          while(mask[s+width] == current && x[w] + width < chunk.size) width++;

          bool done = false;
          for(height = 1; x[v] + height < chunk.size; height++) {
            //Loop over the width guy
            for(int k = 0; k < width; k++) {
              if(mask[s+k+height*chunk.size] != current) {
                done = true;
                break;
              }
            }
            if(done) {
              break;
            }
          }

          //Zero Mask
          for(int l = x[v]; l < x[v] + height; l++){
            for(int k = x[w]; k < x[w] + width; k++){
              mask[k+l*chunk.size] = BLOCK_AIR;
            }
          }

          int du[3] = {0}; du[v] = height;
          int dv[3] = {0}; dv[w] = width;

          if(n < 0){
            //Add Indices
            indices.push_back(positions.size()/3+0);
            indices.push_back(positions.size()/3+1);
            indices.push_back(positions.size()/3+2);
            indices.push_back(positions.size()/3+2);
            indices.push_back(positions.size()/3+3);
            indices.push_back(positions.size()/3+0);
            //Some need to go clock-wise, others need to go counterclockwise.
            positions.push_back(x[0]-0.5);
            positions.push_back(x[1]-0.5);
            positions.push_back(x[2]-0.5);
            //Vertex 2
            positions.push_back(x[0]+du[0]-0.5);
            positions.push_back(x[1]+du[1]-0.5);
            positions.push_back(x[2]+du[2]-0.5);
            //Vertex 3
            positions.push_back(x[0]+du[0]+dv[0]-0.5);
            positions.push_back(x[1]+du[1]+dv[1]-0.5);
            positions.push_back(x[2]+du[2]+dv[2]-0.5);
            //Vertex 4
            positions.push_back(x[0]+dv[0]-0.5);
            positions.push_back(x[1]+dv[1]-0.5);
            positions.push_back(x[2]+dv[2]-0.5);
          }
          else{
            //Add Indices
            indices.push_back(positions.size()/3+0);
            indices.push_back(positions.size()/3+1);
            indices.push_back(positions.size()/3+2);
            indices.push_back(positions.size()/3+1);
            indices.push_back(positions.size()/3+0);
            indices.push_back(positions.size()/3+3);

            //Vertex 0
            positions.push_back(x[0]-0.5+y[0]);
            positions.push_back(x[1]-0.5+y[1]);
            positions.push_back(x[2]-0.5+y[2]);
            //Vertex 1
            positions.push_back(x[0]+du[0]+dv[0]-0.5+y[0]);
            positions.push_back(x[1]+du[1]+dv[1]-0.5+y[1]);
            positions.push_back(x[2]+du[2]+dv[2]-0.5+y[2]);
            //Vertex 2
            positions.push_back(x[0]+du[0]-0.5+y[0]);
            positions.push_back(x[1]+du[1]-0.5+y[1]);
            positions.push_back(x[2]+du[2]-0.5+y[2]);
            //Vertex 3
            positions.push_back(x[0]+dv[0]-0.5+y[0]);
            positions.push_back(x[1]+dv[1]-0.5+y[1]);
            positions.push_back(x[2]+dv[2]-0.5+y[2]);
          }

          //Add Colors and Normals to all vertices.
          glm::vec4 color = chunk.getColorByID(current);

          for(int m = 0; m < 4; m++){
            //Add Colors
            colors.push_back(color.x);
            colors.push_back(color.y);
            colors.push_back(color.z);
            colors.push_back(color.w);

            //Add Normals
            normals.push_back(q[0]);
            normals.push_back(q[1]);
            normals.push_back(q[2]);
          }
          //Next Quad
        }
      }
      //Next Slice
    }
    //Next Surface Orientation
  }
  //Finish!
}
