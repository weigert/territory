#include "chunk.h"

void Chunk::fillVolume(glm::vec3 start, glm::vec3 end, BlockType _type){
  //Fills the volume
  for(int i = start.x; i <= end.x; i++){
    for(int j = start.y; j <= end.y; j++){
      for(int k = start.y; k <= end.z; k++){
        data.setPosition(i, j, k, _type);
      }
    }
  }
}

void Chunk::hollowBox(glm::vec3 start, glm::vec3 end, BlockType _type){
  //Fills the volume
  for(int i = start.x; i <= end.x; i++){
    for(int j = start.y; j <= end.y; j++){
      for(int k = start.y; k <= end.z; k++){
        if(i == start.x || i == end.x || j == start.y || j == end.y || k == start.z || k == end.z){
          data.setPosition(i, j, k, _type);
        }
      }
    }
  }
}
