#pragma once
#include "../forward/octree.fwd.h"

class Octree{
public:
  Octree(){
    type = BLOCK_AIR;
    index = 0;
  }

  //Some Information contained by the octree element
  std::vector<Octree> subTree;
  int depth;    //Gives us the remaining depth levels of the octree
  int index;

  //By default type is 0, and subTree is empty.
  BlockType type;

  //Filling and Combining Operations for Subtrees
  bool trySimplify();
  bool contains(glm::vec3 _pos);

  //Index Functions
  glm::vec3 getPos(int index);
  int getIndex(glm::vec3 _pos);

  //Get Color Data
  glm::vec4 getColorByID(BlockType _type);

  //We need setters, getters, volume setters, etc.
  bool setPosition(glm::vec3 _pos, BlockType _type);
  BlockType getPosition(glm::vec3 _pos, int LOD);
};
