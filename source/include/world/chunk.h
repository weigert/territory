//
#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"

class Chunk{
public:
  //Position information and size information
  glm::ivec3 pos;
  bool remesh = false;
  int size = 16;
  BiomeType biome;

  //Data Storage Member
  int data[16*16*16] = {0};

  //Get the Flat-Array Index
  int getIndex(glm::vec3 _p);
  void setPosition(glm::vec3 _p, BlockType _type);
  BlockType getPosition(glm::vec3 _p);
};
