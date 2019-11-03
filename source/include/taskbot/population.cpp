//We need to include headers for all guys if we use their members
#include "bot.h"
#include "../world/world.h"
#include "task.h"
#include "../render/audio.h"
#include "population.h"

void Population::addBot(World world){
  //Add a bot
  Bot bot(bots.size());
  bot.setupSprite();
  bot.pos = glm::vec3(75, 1, 75);//world.getTop(glm::vec2(40, 40));
  //This is the only place I want to allocate the task...
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

void Population::update(World &world, View view, Audio &audio){
  for(Bot& bot : bots){
    //No Execution Conditions
    if(bot.dead) continue;
    if(!helper::inModRange(bot.pos, view.viewPos, view.renderDistance, world.chunkSize)) continue;
    //Execute the Task
    bot.executeTask(world, *this, audio);
  }
}
