//Dependencies
#include "mandate.h"
#include "../world/world.h"
#include "task.h"
#include "memory.h"
#include "../render/sprite.h"
//Link to class
#include "bot.h"


//Constructor
Bot::Bot(int _ID){
  viewDistance = 2;
  memorySize = 10;
  subconSize = 5;
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
  subconSize = 5;
  trail = t;
  fly = f;
  species = s;
  ID = id;
  pos = _pos;
  home = _home;
}

//Per Tick Executor Task
void Bot::executeTask(World &world, Population &population){
  //Check for interrupt!
  if(interrupt){
    //Reevaluate Mandates, so we can respond
    //Here, you could put some way to save the game state?
    //This is where error handling would happen if you observe bugs
    //We have been interrupted and should check if we change our state.
    //Potentially do a listen task?
    evaluateMandates(world, population);
    interrupt = false;
  }

  //Here we should do an animation frame

  //Execute Current Task, upon success overwrite.
  if((current->perform)(world, population)){
    //Task is successful, we can reevaluate our mandates.
    evaluateMandates(world, population);
  }
}

void Bot::evaluateMandates(World &world, Population &population){
  //This should account for fulfilled mandate goals!!
  //Check for No Mandates (this should be done somewhere else)
  if(mandates.empty()){
    //The bot should never have no mandates. It will die at this point.
    //Or we load a set of standard mandates that is specific to the bot's species


    //We construct a new mandate and add it to our stack
    //This should be some general species initializer.
    Task *masterTask = new Task("Do Ant Stuff.", ID, &Task::Dummy);

    //Construct a new blank species mandate
    Mandate *mandate = new Mandate(ID, ID, 0, true, masterTask, 1);
    (mandate->initStates)(world, population, ID);
    mandates.push_back(*mandate);
  }

  short tempViab = 0;

  //Cycle through the Mandates
  for(unsigned int i = 0; i < mandates.size(); i++){

    //Check for complete condition
    if(mandates[i].completed(world, population, ID)){

      //Improve our mandate information
      mandates[i].setGoal(world, population, ID);
      //Add the Mandate Result to our subconscious
      addSubcon(mandates[i]);

      //Check if this mandate should be repeated
      if(!mandates[i].repeat){
        //Remove Mandate from Queue
        mandates.erase(mandates.begin() + i);
      }
    }

    //Get the Suggested Task for every mandate.
    mandates[i].getSuggest();

    //Calculate the viability
    if(mandates[i].getViability() < tempViab){
      //For a lower viability, change our current executing task
      current = mandates[i].suggest;
      tempViab = mandates[i].getViability();

      //Update the Mandate Start State before executing task
      //This makes cause and effect more coherent!
      mandates[i].curr.retrieveState(world, population, ID);
    }
  }
}

void Bot::addSubcon(Mandate experience){
  //Add the Experience to Subconscious
  subconsc.push_front( experience );

  //Cap the Queue
  if(subconsc.size() > subconSize){
    subconsc.pop_back();
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
  memory.location = _pos;
  memory.object = world.getBlock(memory.location);
  memory.task = task;
  memory.reachable = (memory.object == BLOCK_AIR)?true:false;

  Memory query;
  query.location = _pos;
  query.queryable[2] = true;

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

void Bot::setupSprite(){
  //Load the Sprite Thing
  sprite.loadImage("hunter.png");
  sprite.setupBuffer();
  sprite.resetModel();
}
