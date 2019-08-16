//
#pragma once
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"

class Chunk{
public:
  //Position information and size information
  glm::vec3 pos;
  int size;
  BiomeType biome;

  //Data Storage Member
  Octree data;  //Raw Data
  bool updated = false;  //Has the data been updated?
  bool refreshModel = true;
};
