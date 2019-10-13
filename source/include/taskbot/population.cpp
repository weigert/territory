//We need to include headers for all guys if we use their members
#include "bot.h"
#include "../world/world.h"
#include "task.h"
#include "population.h"

void Population::addBot(World world){
  //Add a bot
  Bot bot(bots.size());
  bot.setupSprite();
  bot.pos = glm::vec3(0, world.getTop(glm::vec2(0,0)), 0);
  Task *masterTask = new Task("Human Task", bot.ID, &Task::Dummy);
  bot.current = masterTask;
  bots.push_back(bot);
}

void Population::removeBot(int id){
  //Erase the Entry
  if(bots.size() > (unsigned int)id){
    bots.erase(bots.begin() + id);
    //Now we should update all bot IDs
    for(unsigned int i = 0; i < bots.size(); i++){
      bots[i].ID = i;
    }
    std::cout<<"Erased bot with ID: "<<id<<std::endl;
  }
  else{
    //No Entry to Erase
    std::cout<<"Can't delete bot with ID: "<<id<<". Doesn't exist"<<std::endl;
  }
}

Population::Population(World &world){
  //Create some bots
  for(int j = 0; j < 1; j++){
    addBot(world);
  }
}

void Population::update(World &world){
  //Do some Updating Here
  //Check if anyone has died here for instance.
  for(unsigned int i = 0; i < bots.size(); i++){
    if(!bots[i].dead){
      bots[i].executeTask(world, *this);
    }
  }
}
