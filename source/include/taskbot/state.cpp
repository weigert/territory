#include "../world/world.h"
#include "population.h"
#include "bot.h"
#include "state.h"
#include "../game/item.h"

void State::retrieveState(World &world, Population &population, int bot){
  pos = population.bots[bot].pos;
  task = population.bots[bot].task;
  //This needs to be expanded significantly to account for more of the local state.
}
