//Dependencies
#include "mandate.h"
#include "../world/world.h"
#include "task.h"
#include "memory.h"
#include "../render/sprite.h"
#include "state.h"
//Link to class
#include "bot.h"


//Constructor
Bot::Bot(int _ID){
  viewDistance = 2;
  memorySize = 10;
  shortermSize = 5;
  trail = false;
  fly = false;
  species = "Human";
  ID = _ID;
  pos = glm::vec3(0);
  home = glm::vec3(0);
}

Bot::Bot(std::string s, bool t, bool f, int view, int forag, int mem, int id, glm::vec3 _pos, glm::vec3 _home){
  viewDistance = view;
  forage = forag;
  memorySize = mem;
  shortermSize = 5;
  trail = t;
  fly = f;
  species = s;
  ID = id;
  pos = _pos;
  home = _home;
}

bool Bot::tryInterrupt(State _state){
  interrupt = true;
  return true;
}

//Per Tick Executor Task
void Bot::executeTask(World &world, Population &population){

  //Check for interrupt!
  if(interrupt){
    //Reevaluate Mandates, so we can respond
    Task *decide = new Task("Handle Interrupt", ID, &Task::decide);
    current = decide;
    interrupt = false;
  }
  else{
    //Execute Current Task, upon success overwrite.
    if((current->perform)(world, population)){
      Task *decide = new Task("Decide on Action", ID, &Task::decide);
      current = decide;
    }
  }
}

//Find any memories matching a query and overwrite them
void Bot::updateMemory(Memory &query, bool all, Memory &memory){
  //Loop through all existing Memories
  for(unsigned int i = 0; i < memories.size(); i++){
    //If all matches are required and we have all matches
    if(all && (memories[i] == query)){
      //We have a memory that needs to be updated
      memories[i] = memory;
      continue;
    }
    //If not all matches are required and any query elements are contained
    else if(!all && (memories[i] || query)){
      memories[i] = memory;
      continue;
    }
  }
}

std::deque<Memory> Bot::recallMemories(Memory &query, bool all){
  //When we query a memory, it is important if it should fit any criteria or all criteria.
  //New Memory
  std::deque<Memory> recalled;
  //Loop through memories
  for(unsigned int i = 0; i < memories.size(); i++){
    //If all matches are required and we have all matches
    if(all && (memories[i] == query)){
      recalled.push_back(memories[i]);
      memories[i].recallScore++;
      continue;
    }
    //If not all matches are required and any query elements are contained
    else if(!all && (memories[i] || query)){
      recalled.push_back(memories[i]);
      memories[i].recallScore++;
      continue;
    }
  }
  return recalled;
}

void Bot::addMemory(World world, glm::vec3 _pos){
  //Create new memory
  Memory memory;
  memory.state.pos = _pos;
  memory.state.block = world.getBlock(memory.state.pos);
  memory.state.task = task;
  memory.state.reachable = (memory.state.block == BLOCK_AIR)?true:false;

  Memory query;
  query.state.pos = _pos;

  //Overwrite any locations we already have a memory of.
  //Ideally, if an object disappears, and we look at that location,
  //We just change it to air, it doesn't get recalled anymore and pushed out of the queue.
  updateMemory(query, true, memory);

  //Now shuffle the memories around.
  for(unsigned int i = 1; i < memories.size(); i++){
    //Check If A Memory Exists at the location
    if(memories[i].recallScore > memories[i-1].recallScore){
      Memory oldMemory = memories[i-1];
      memories[i-1] = memories[i];
      memories[i] = oldMemory;
    }
  }

  //Add new Memory
  memories.push_front(memory);

  //Remove the Last Element if it is too much
  if(memories.size() > memorySize){
    memories.pop_back();
  }
}

void Bot::addSound(State _state){
  //Create the Auditory Information
  Memory sound;
  sound.state = _state;

  //Add it to the front
  shorterm.push_front(sound);

  //Clip anything hanging off the back
  if(shorterm.size() > shortermSize){
    shorterm.pop_back();
  }
}

void Bot::setupSprite(){
  //Load the Sprite Thing
  sprite.loadImage("hunterfull.png");
  sprite.setupBuffer(false);
  sprite.resetModel();
}
