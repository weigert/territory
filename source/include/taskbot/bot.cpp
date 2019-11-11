//Dependencies
#include "../world/world.h"
#include "task.h"
#include "memory.h"
#include "../render/sprite.h"
#include "state.h"
#include "../render/audio.h"
//Link to class
#include "bot.h"

//Test for Interrupt
bool Bot::tryInterrupt(State _state){

  /*
  Currently always returns true - in the future status effects or other world conditions
  might lower the probability that in interruption is successful.
  */

  interrupt = true;
  return true;
}

//Per Tick Executor Task
void Bot::executeTask(World &world, Population &population, Audio &audio){
  //Check for interrupt!
  if(interrupt){
    //Reevaluate Mandates, so we can respond
    current->set("Decide on Action", ID, &Task::decide);
    interrupt = false;
  }
  else{
      //Execute Current Task (Make sure when decide is called, it doesn't reset to itself)
      if(current->handle == &Task::decide && (current->perform)(world, population, audio));
      else if((current->perform)(world, population, audio)){
        current->set("Decide on Action", ID, &Task::decide);
      }
  }
}

//Merge the Inventories
void Bot::mergeInventory(Inventory _merge){
  //Loop over the stuff that we picked up, and sort it into the inventory
  for(unsigned int i = 0; i < _merge.size(); i++){
    bool found = false;
    for(unsigned int j = 0; j < inventory.size(); j++){
      if(_merge[i]._type == inventory[j]._type){
        inventory[j].quantity += _merge[i].quantity;
        found = true;
      }
    }
    //We haven't found the item in the inventory, so add it to the back
    if(!found){
      inventory.push_back(_merge[i]);
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

inline void Bot::addMemory(State &state){
  //Memory already exists, so overwrite relevant portions.
  for(unsigned int i = 0; i < memories.size(); i++){
    if(memories[i].state.pos != state.pos) continue;
    //We already have a memory at this location!

    if(state.block == BLOCK_AIR){
      //This Memory needs to be removed.
      memories.erase(memories.begin()+i);
      return;
    }

    //Update the Information at that position
    memories[i].state.block = state.block;
    memories[i].state.task = state.task;
    return;
  }

  //No New Memories for these Blocks
  if(state.block == BLOCK_AIR) return;
  if(state.block == BLOCK_STONE) return;
  if(state.block == BLOCK_DIRT) return;
  if(state.block == BLOCK_GRASS) return;
  if(state.block == BLOCK_SAND) return;

  Memory memory;
  Memory oldMemory;
  memory.state = state;
  memory.state.reachable = true;

  //Now shuffle the memories around.
  for(unsigned int i = 1; i < memories.size(); i++){
    //Check If A Memory Exists at the location
    if(memories[i].recallScore > memories[i-1].recallScore){
      oldMemory = memories[i-1];
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
  sprite.loadImage("cowboyfull.png");
  sprite.setupBuffer(false);
  sprite.resetModel();
}
