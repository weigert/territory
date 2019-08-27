#include "../world/world.h"
#include "population.h"
#include "bot.h"
#include "state.h"


State::State(){
  pos = glm::vec3(0);
  task = "";
  carry = 0;
}

void State::retrieveState(World &world, Population &population, int bot){
  pos = population.bots[bot].pos;
  task = population.bots[bot].task;
  carry = population.bots[bot].carry;

  //This needs to be expanded significantly to account for more of the local state.
  
}
