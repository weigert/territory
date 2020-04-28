#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"

class Octree{
public:
  //Needs to be set initially.
  BlockType type = BLOCK_AIR;
  std::vector<Octree> subTree;  //Sparse SubTree
  unsigned char depth = 4;                    //Current Depth
  unsigned char index = 0;                    //Current Index

  //Back and forth to chunks
  void fromChunk(Chunk &chunk);

  //Filling and Combining Operations for Subtrees
  bool trySimplify();
  bool contains(glm::vec3 _pos);

  //Index Functions
  glm::vec3 getPos(int index);
  int getIndex(glm::ivec3 _pos);

  //We need setters, getters, volume setters, etc.
  bool setPosition(glm::vec3 _pos, BlockType _type);
  BlockType getPosition(glm::vec3 _pos, int LOD);
};
