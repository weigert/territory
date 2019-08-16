//Forward Declarations
#pragma once
#include "../forward/player.fwd.h"
//Depdendencies

class Player{
public:
  //Player Position
  glm::vec3 playerPos = glm::vec3(8, 0, 8); //Position of the player in chunkspace
  glm::vec3 chunkPos = glm::vec3(5, 0, 5);  //Position of the player in worldspace
  glm::vec3 renderDistance = glm::vec3(1, 2, 1);
};
