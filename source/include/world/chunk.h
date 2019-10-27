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
  int data[16*16*16] = {0};
  bool refreshModel = false;

  //Get the Flat-Array Index
  int getIndex(glm::vec3 _p);
  void setPosition(glm::vec3 _p, BlockType _type);
  BlockType getPosition(glm::vec3 _p);
  glm::vec4 getColorByID(BlockType _type);
};

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

glm::vec4 Chunk::getColorByID(BlockType _type){
  //Switch the value and return a vector
  glm::vec4 color;
  switch(_type){
    case BLOCK_GRASS:
      color = glm::vec4(0.54f, 0.7f, 0.34f, 1.0f);
      break;
    case BLOCK_DIRT:
      color = glm::vec4(0.74f, 0.5f, 0.36f, 1.0f);
      break;
    case BLOCK_WATER:
      color = glm::vec4(0.02f, 0.61f, 0.75f, 1.0f);
      break;
    case BLOCK_SAND:
      color = glm::vec4(0.93f, 0.91f, 0.38f, 1.0f);
      break;
    case BLOCK_CLAY:
      color = glm::vec4(0.97f, 0.5f, 0.44f, 1.0f);
      break;
    case BLOCK_STONE:
      color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
      break;
    case BLOCK_LEAVES:
      color = glm::vec4(0.38f, 0.48f, 0.26f, 0.8f);
      break;
    case BLOCK_WOOD:
      color = glm::vec4(0.6f, 0.375f, 0.14f, 1.0f);
      break;
    case BLOCK_PUMPKIN:
      color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
      break;
    case BLOCK_CACTUS:
      color = glm::vec4(0.0f, 0.44f, 0.3f, 1.0f);
      break;
    case BLOCK_PLANKS:
      color = glm::vec4(0.75f, 0.6f, 0.28f, 1.0f);
      break;
    case BLOCK_GLASS:
      color = glm::vec4(0.8f, 0.9f, 0.95f, 0.2f);
      break;
    default:
      color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
      break;
  }
  return color;
}
