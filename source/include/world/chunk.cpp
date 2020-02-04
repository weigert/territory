#include "chunk.h"
#include "octree.h"

int Chunk::getIndex(glm::vec3 _p){
  //Return the Correct Index
  if(glm::all(glm::lessThan(_p, glm::vec3(size)))){
    return _p.x*size*size+_p.y*size+_p.z;
  }
  return 0;
}

void Chunk::fromOctree(Octree &octree, glm::vec3 offset){
  int LOD = 0;

  //If we are the lowest level we wish to dive, or can't dive further...
  if(octree.depth == LOD || octree.subTree.empty()){

    //Add the Values
    for(int i = 0; i < 1<<octree.depth; i++)
      for(int j = 0; j < 1<<octree.depth; j++)
        for(int k = 0; k < 1<<octree.depth; k++)
          data[helper::getIndex(offset + glm::vec3(i, j, k), glm::vec3(size))] = octree.type;
          
    return;
  }

  //Iterate over all subtrees
  for(auto &_subtree:octree.subTree)
    fromOctree(_subtree, offset + glm::vec3(1<<(octree.depth-1))*octree.getPos(_subtree.index)); //Add data from the subtrees too
}

void Chunk::setPosition(glm::vec3 _p, BlockType _type){
  data[getIndex(_p)] = (int)_type;
}

BlockType Chunk::getPosition(glm::vec3 _p){
  return (BlockType)data[getIndex(_p)];
}
