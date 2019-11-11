#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"

class Octree{
public:
  //Needs to be set initially.
  BlockType type = BLOCK_AIR;
  std::vector<Octree> subTree;  //Sparse SubTree
  int depth = 4;                    //Current Depth
  glm::ivec3 pos = glm::ivec3(0);
  std::byte index = (std::byte)0;                    //Current Index

  //Back and forth to chunks
  void fromChunk(Chunk &chunk);
  Chunk toChunk();

  //Filling and Combining Operations for Subtrees
  bool trySimplify();
  bool contains(glm::vec3 _pos);

  //Index Functions
  glm::vec3 getPos(std::byte index);
  std::byte getIndex(glm::vec3 _pos);

  //We need setters, getters, volume setters, etc.
  bool setPosition(glm::vec3 _pos, BlockType _type);
  BlockType getPosition(glm::vec3 _pos, int LOD);
};
