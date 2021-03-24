/*
================================================================================
                          Chunk Meshing Algorithms
================================================================================
*/

#include <functional>

namespace chunkmesh {
using namespace std;


function<void(Model* , Chunk*)> greedy = [](Model* m, Chunk* c){

//  Blueprint temp;
  int LOD = scene::LOD;
  int CHLOD = CHUNKSIZE/LOD;

  for(int d = 0; d < 6; d++){

    int u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2      //Dimension m->indices
    int v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    int w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    int n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    BlockType* mask = new BlockType[16*16/LOD/LOD];
    BlockType current, facing;
    int s;

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth
      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_AIR;                                                  //Set to Air

          current = c->getPosition((float)LOD*vec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_AIR) continue;

          //Skip Non-Cubic Elements in the Chunk! (every iteration)
        //  if(!block::isCubic(atPos)){
            //If it is our first pass, also add it to the meshing editBuffer.
            //IGNORE THIS FOR LARGE LOD
        //    if(d == 0) temp.add(vec3(x[0], x[1], x[2]), atPos, false);
      //      continue;
        //  }

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->getPosition((float)LOD*vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_AIR || !block::isCubic(facing))
            mask[s] = current;

        }
      }

      int width = 1, height = 1;
      bool quaddone;
      vec4 color;

      for(x[v] = 0; x[v] < CHLOD; x[v]++){            //Evaluate Mask
        for(x[w] = 0; x[w] < CHLOD; x[w] += width){   //Permissible Skip

          width = height = 1;       //Current Quad Dimensions

          s = x[w] + x[v]*CHLOD;    //Current Slice Index
          current = mask[s];        //Current Block Type

          if(current == BLOCK_AIR)  //We don't mesh air
            continue;

          while(mask[s+width] == current && x[w] + width < CHLOD)
            width++;

          quaddone = false;
          for(height = 1; x[v] + height < CHLOD; height++){   //Find Height

            for(int k = 0; k < width; k++){                   //Iterate Over Width
              if(mask[s+k+height*CHLOD] != current) {
                quaddone = true;
                break;
              }
            }
            if(quaddone) break;
          }

          for(int l = x[v]; l < x[v] + height; l++)   //Zero the Mask
          for(int k = x[w]; k < x[w] + width; k++)
            mask[k+l*CHLOD] = BLOCK_AIR;

          int du[3] = {0}; du[v] = height;
          int dv[3] = {0}; dv[w] = width;

          //Add Quad to Model

          if(n < 0){
            //Add m->indices
            m->indices.push_back(m->positions.size()/3+0);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+3);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+0);

            //Some need to go clock-wise, others need to go counterclockwise.
            m->positions.push_back((x[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]-0.5)*(float)LOD);
            //Vertex 2
            m->positions.push_back((x[0]+du[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]+du[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]+du[2]-0.5)*(float)LOD);
            //Vertex 3
            m->positions.push_back((x[0]+du[0]+dv[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]+du[1]+dv[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]+du[2]+dv[2]-0.5)*(float)LOD);
            //Vertex 4
            m->positions.push_back((x[0]+dv[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]+dv[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]+dv[2]-0.5)*(float)LOD);
          }
          else{
            //Add m->indices
            m->indices.push_back(m->positions.size()/3+0);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+3);
            m->indices.push_back(m->positions.size()/3+0);

            //Vertex 0
            m->positions.push_back((x[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]-0.5+y[2])*(float)LOD);
            //Vertex 1
            m->positions.push_back((x[0]+du[0]+dv[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]+du[1]+dv[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]+du[2]+dv[2]-0.5+y[2])*(float)LOD);
            //Vertex 2
            m->positions.push_back((x[0]+du[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]+du[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]+du[2]-0.5+y[2])*(float)LOD);
            //Vertex 3
            m->positions.push_back((x[0]+dv[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]+dv[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]+dv[2]-0.5+y[2])*(float)LOD);
          }

          color = block::getColor(current, color::hashrand(helper::getIndex(glm::vec3(x[0], x[1], x[2]), vec3(16))));

          for(int l = 0; l < 4; l++){
            m->add(m->colors, color);
            m->add(m->normals, vec3(q[0], q[1], q[2]));
          }

          //Next Quad
        }
      }
      //Next Slice
    }
    //Next Surface Orientation
    delete[] mask;
  }

/*
  //Mesh all the guys that could not be meshed greedily with large quads.
  while(!temp.edits.empty()){
    //For now, we add cubes
  //  addIrregular(temp.edits.back().pos, temp.edits.back().type);
    temp.edits.pop_back();
  }
  */
  //Finish!
};

/*
function<void(Model* , Chunk*)> greedynaive = [](Model* m, Chunk* c){

  Blueprint temp;
  int LOD = scene::LOD;
  int CHLOD = CHUNKSIZE/LOD;

  for(int d = 0; d < 6; d++){

    int u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2      //Dimension m->indices
    int v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    int w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    int n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    BlockType* mask = new BlockType[16*16/LOD/LOD];

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth
      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          int s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_AIR;                                                  //Set to Air

          BlockType atPos = c->getPosition(vec3(x[0]*LOD,x[1]*LOD,x[2]*LOD));   //Position at one corner!

          //Skip Non-Cubic Elements in the Chunk! (every iteration)
          if(!block::isCubic(atPos)){
            //If it is our first pass, also add it to the meshing editBuffer.
            //IGNORE THIS FOR LARGE LOD
            if(d == 0) temp.add(vec3(x[0], x[1], x[2]), atPos, false);
            continue;
          }

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = atPos;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          BlockType facing = c->getPosition((float)LOD*glm::vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));
          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_AIR || !block::isCubic(facing)){
            mask[s] = atPos;
          }
        }
      }

      //Loop AGAIN over the slice (evaluate mask)
      for(x[v] = 0; x[v] < CHLOD; x[v]++){
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          int width = 1;
          int height = 1;

          //Get the index of the current slice object we are observing!
          int s = x[w] + x[v]*CHLOD;

          //Get current blocktype, ignore air.
          BlockType current = mask[s];

          //Ignore this surface if it is not part of the mask.
          if(current == BLOCK_AIR) continue;

          //Compute the Width
          while(mask[s+width] == current && x[w] + width < CHLOD) width++;

          bool done = false;
          for(height = 1; x[v] + height < CHLOD; height++) {
            //Loop over the width guy
            for(int k = 0; k < width; k++) {
              if(mask[s+k+height*CHLOD] != current) {
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
              mask[k+l*CHLOD] = BLOCK_AIR;
            }
          }

          int du[3] = {0}; du[v] = height;
          int dv[3] = {0}; dv[w] = width;

          if(n < 0){
            //Add m->indices
            m->indices.push_back(m->positions.size()/3+0);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+3);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+0);

            //Some need to go clock-wise, others need to go counterclockwise.
            m->positions.push_back((x[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]-0.5)*(float)LOD);
            //Vertex 2
            m->positions.push_back((x[0]+du[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]+du[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]+du[2]-0.5)*(float)LOD);
            //Vertex 3
            m->positions.push_back((x[0]+du[0]+dv[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]+du[1]+dv[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]+du[2]+dv[2]-0.5)*(float)LOD);
            //Vertex 4
            m->positions.push_back((x[0]+dv[0]-0.5)*(float)LOD);
            m->positions.push_back((x[1]+dv[1]-0.5)*(float)LOD);
            m->positions.push_back((x[2]+dv[2]-0.5)*(float)LOD);
          }
          else{
            //Add m->indices
            m->indices.push_back(m->positions.size()/3+0);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+3);
            m->indices.push_back(m->positions.size()/3+0);

            //Vertex 0
            m->positions.push_back((x[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]-0.5+y[2])*(float)LOD);
            //Vertex 1
            m->positions.push_back((x[0]+du[0]+dv[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]+du[1]+dv[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]+du[2]+dv[2]-0.5+y[2])*(float)LOD);
            //Vertex 2
            m->positions.push_back((x[0]+du[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]+du[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]+du[2]-0.5+y[2])*(float)LOD);
            //Vertex 3
            m->positions.push_back((x[0]+dv[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((x[1]+dv[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((x[2]+dv[2]-0.5+y[2])*(float)LOD);
          }

          //Add m->colors and m->normals to all vertices.
          vec4 color = block::getColor(current, color::hashrand(helper::getIndex(glm::vec3(x[0], x[1], x[2]), glm::vec3(16))));

          for(int l = 0; l < 4; l++){
            m->add(m->colors, color);
            m->add(m->normals, vec3(q[0], q[1], q[2]));
          }
          //Next Quad
        }
      }
      //Next Slice
    }
    //Next Surface Orientation
    delete[] mask;
  }

  //Mesh all the guys that could not be meshed greedily with large quads.
  while(!temp.edits.empty()){
    //For now, we add cubes
  //  addIrregular(temp.edits.back().pos, temp.edits.back().type);
    temp.edits.pop_back();
  }
  //Finish!
};
*/

}
