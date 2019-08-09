#include "chunk.h"

void Chunk::setBlock(int x, int y, int z, BlockType _type){
  data.setPosition(x, y, z, _type);
}

void Chunk::fillHeight(int x, int height, int z, BlockType _type){
  for(int i = 0; i <= height; i++){
    data.setPosition(x, i, z, _type);
  }
}

void Chunk::fillVolume(glm::vec3 start, glm::vec3 end, BlockType _type){
  //Fills the volume
  for(int i = start.x; i <= end.x; i++){
    for(int j = start.y; j <= end.y; j++){
      for(int k = start.y; k <= end.z; k++){
        data.setPosition(i, j, k, _type);
      }
    }
  }
}

void Chunk::hollowBox(glm::vec3 start, glm::vec3 end, BlockType _type){
  //Fills the volume
  for(int i = start.x; i <= end.x; i++){
    for(int j = start.y; j <= end.y; j++){
      for(int k = start.y; k <= end.z; k++){
        if(i == start.x || i == end.x || j == start.y || j == end.y || k == start.z || k == end.z){
          data.setPosition(i, j, k, _type);
        }
      }
    }
  }
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
    default:
      color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
      break;
  }
  return color;
}
