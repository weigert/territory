#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"

class Octree{
public:
  Octree(){
    type = BLOCK_AIR;
    index = 0;
  }

  //Needs to be set initially.
  BlockType type;               //Type
  std::vector<Octree> subTree;  //Sparse SubTree
  int depth = 4;                    //Current Depth
  glm::vec3 pos;

  //Stuff that is set live!
  int index;                    //Current Index

  //Back and forth to chunks
  void fromChunk(Chunk &chunk);
  Chunk toChunk();

  //Filling and Combining Operations for Subtrees
  bool trySimplify();
  bool contains(glm::vec3 _pos);

  //Index Functions
  glm::vec3 getPos(int index);
  int getIndex(glm::vec3 _pos);

  //We need setters, getters, volume setters, etc.
  bool setPosition(glm::vec3 _pos, BlockType _type);
  BlockType getPosition(glm::vec3 _pos, int LOD);
};
