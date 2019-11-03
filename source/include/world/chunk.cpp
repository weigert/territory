#include "chunk.h"
#include "octree.h"

int Chunk::getIndex(glm::vec3 _p){
  //Return the Correct Index
  if(glm::all(glm::lessThan(_p, glm::vec3(size)))){
    return _p.x*size*size+_p.y*size+_p.z;
  }
  return 0;
}

void Chunk::setPosition(glm::vec3 _p, BlockType _type){
  data[getIndex(_p)] = (int)_type;
}

BlockType Chunk::getPosition(glm::vec3 _p){
  return (BlockType)data[getIndex(_p)];
}
