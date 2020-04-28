#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"
#include "../forward/RLE.fwd.h"

class Chunk{
public:
  //Position information and size information
  glm::ivec3 pos;
  bool remesh = false;
  int size = 16;
  BiomeType biome;

  //Data Storage Member
  unsigned char data[16*16*16] = {0};

  //From Octree Datastructure
  void fromOctree(Octree &octree, glm::vec3 offset);

  //Get the Flat-Array Index
  int getIndex(glm::vec3 _p);
  void setPosition(glm::vec3 _p, BlockType _type);
  BlockType getPosition(glm::vec3 _p);
};
