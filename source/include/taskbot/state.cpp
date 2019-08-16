#include "../world/world.h"
#include "population.h"
#include "bot.h"
#include "state.h"


State::State(){
  pos = glm::vec3(0);
  task = "";
  carry = 0;
}

State::State(glm::vec3 _loc, std::string _task, int _carry){
  pos = _loc;
  task = _task;
  carry = _carry;
}

void State::retrieveState(World &world, Population &population, int bot){
  pos = population.bots[bot].pos;
  task = population.bots[bot].task;
  carry = population.bots[bot].carry;
}
