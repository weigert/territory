#include "bot.h"
#include "population.h"
#include "../world/world.h"
#include "../world/blueprint.h"
#include "state.h"
#include "../game/item.h"
#include "task.h"

/*
=========================================================
                      CONSTRUCTORS
=========================================================
*/

Task::Task(){}

Task::Task(std::string taskName, int taskBotID, TaskHandle _handle){
  name = taskName;
  botID = taskBotID;
  handle = TaskHandles[_handle];
}

Task::Task(std::string taskName, int taskBotID, Handle taskHandle){
  name = taskName;
  botID = taskBotID;
  handle = taskHandle;
}

Task::Task(std::string taskName, int taskBotID, int animationID, glm::vec3 animationTranslate, Handle taskHandle){
  name = taskName;
  botID = taskBotID;
  handle = taskHandle;
  animation = animationID;
  translate = animationTranslate;
}

void Task::set(std::string taskName, int taskBotID, Handle _handle){
  this->initFlag = true;
  this->name = taskName;
  this->botID = taskBotID;
  this->handle = _handle;
}

void Task::set(std::string taskName, int taskBotID, TaskHandle _handle){
  this->initFlag = true;
  this->name = taskName;
  this->botID = taskBotID;
  this->handle = TaskHandles[_handle];
}


/*
=========================================================
                    TASK HANDLING TASKS
=========================================================
*/
//Empty Task
bool Task::null(World &world, Population &population, Audio &audio, State &_args){
  //Simply Return True
  return true;
}

//Execute Task Function
bool Task::perform(World &world, Population &population, Audio &audio){
  //Change the Name and Execute the Task
  population.bots[botID].task = name;

  //When initially performing a task, make sure to set the animation!
  if(initFlag){
    //Set the animation and play the sound associated with the action
    if(sound != SOUND_NONE) audio.unprocessed.push_back(sound);
    population.bots[botID].sprite.setAnimation(animation, translate);
  }

  //When the animation is complete, reset the animation.
  if(population.bots[botID].sprite.doAnimationFrame()){
    //We actually want to set the animation after it is completed
    if((*this.*handle)(world, population, audio, args)){
      return true;
    }
  }

  //Task was executed but needs to be repeated, set initFlag
  initFlag = false;
  return false;
}

//Handle the Queue
bool Task::handleQueue(World &world, Population &population, Audio &audio){
  //If the Queue isn't empty, handle top task
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.back();
    queue.pop_back();

    //Perform the Task and Put it back on when successful
    if(!newtask.perform(world, population, audio)){
      //We need to repeat the animation
      queue.push_back(newtask);
    }

    //Pass the arguments
    if(!queue.empty() && queue.back().pass){
      queue.back().args = newtask.args;
    }

    //Task was performed successfully, we leave it off.
    return false;
  }

  //Queue is empty, thereby we have been completed.
  initFlag = true;
  return true;
}

//Example - How to Construct a High-Level Task
bool Task::example(World &world, Population &population, Audio &audio, State &_args){
  //Check
  if(initFlag){
    Task null("Example", botID, &Task::null);   //Construct the Task "null"
    null.animation = 0;                         //Set the Task's Metadata
    null.sound = SOUND_NONE;
    //...

    Task null2("Other Example", botID, &Task::null);
    null2.animation = 0;
    null2.sound = SOUND_NONE;
    null2.pass = true;
    //...

    //Add more tasks...

    //Add to queue in reverse order...
    queue.push_back(null2);
    queue.push_back(null);
  }

  //Handle the Queue appropriately.
  return handleQueue(world, population, audio);
}

/*
=========================================================
                      MOVING TASKS
=========================================================
*/

//Wait - Do a series of Null Tasks
bool Task::wait(World &world, Population &population, Audio &audio, State &_args){
  //Set the Stuff
  if(initFlag){
    Task null("Null", botID, &Task::null);
    null.animation = 2;
    //Add the Tasks
    for(int i = 0; i < _args.time; i++){
      queue.push_back(null);
    }
  }

  //Handle the Queue
  return handleQueue(world, population, audio);
}

bool Task::move(World &world, Population &population, Audio &audio, State &_args){
  //Hard set Position
  population.bots[botID].pos = _args.pos;
  return true;
}

//Walk - Multiple Steps with Error Handling
bool Task::walk(World &world, Population &population, Audio &audio, State &_args){

  //Check if we're already there (or in range)
  if(helper::inRange(population.bots[botID].pos, _args.pos, _args.range)){
    _log.debug("Arrived at location within range.");
    return true;
  }

  //Check if we specify that the position must be directly reachable
  if(_args.reachable && !block::isOccupiable(world.getBlock(_args.pos))){
    _log.debug("Target position is occupied or not loaded.");
    return true;
  }

  //Do the Stuff
  if(initFlag || population.bots[botID].path.empty()){
    //Calculate a Path Here.
    _log.debug("Calculating Path.");
    population.bots[botID].path = calculatePath(botID, _args.pos, population, world, _args.range);
    //If the Path is still empty
    if(population.bots[botID].path.empty()){
      _log.debug("No valid path.");

      /*
        Update Memory at Location if Unreachable
      */

      //Memory Query
      //Loop through memories
      for(unsigned int i = 0; i < population.bots[botID].memories.size(); i++){
        //If all matches are required and we have all matches
        if(population.bots[botID].memories[i].state.pos == _args.pos){
          population.bots[botID].memories[i].state.reachable = false;
        }
      }

      return true;
    }
  }



  /*Check for path stil valid in this next move!*/

  if(queue.empty()){
    Task move("Move to Position", botID, &Task::move);
    move.args.pos = population.bots[botID].path.back();
    population.bots[botID].path.pop_back();
    move.animation = 1;
    move.translate = glm::vec3(move.args.pos - population.bots[botID].pos)/glm::vec3(4);
    queue.push_back(move);
  }

  //Only perform the queue as specified!
  return handleQueue(world, population, audio);
}

//Idle - Move Around Randomly and Wait some time
bool Task::idle(World &world, Population &population, Audio &audio, State &_args){
  //Check for initial flag
  if(initFlag){
    Task walk("Walk", botID, &Task::walk);
    int _shift[2] = {rand()%11-5, rand()%11-5};
    glm::vec2 _pos = glm::vec2(population.bots[botID].pos.x, population.bots[botID].pos.z ) + glm::vec2(_shift[0], _shift[1]);
    walk.args.pos = world.getTop(_pos);

    Task wait("Wait", botID, &Task::wait);
    wait.args.time = _args.time;

    Task look("Look", botID, &Task::look);
    look.args.range = population.bots[botID].viewDistance;

    //Add the Tasks for Idle
    queue.push_back(wait);
    queue.push_back(look);
    queue.push_back(walk); //Walk will be executed first
  }

  return handleQueue(world, population, audio);
}

bool Task::follow(World &world, Population &population, Audio &audio, State &_args){
  //Find the position of the bot you want to go to, walk there.
  if(queue.empty()){
    Task walk("Walk to bot's position.", botID, &Task::walk);
    walk.args.pos = population.bots[_args.target].pos;
    walk.args.range = population.bots[botID].range;
    queue.push_back(walk);
  }

  //Just do this (To end the follow guy, you have to be interrupted!).
  handleQueue(world, population, audio);
  return false;
}

/*
================================================================================
                            ITEM MANAGEMENT TASKS
================================================================================
*/

bool Task::destroy(World &world, Population &population, Audio &audio, State &_args){

  //Check for Non-Destroy Task
  BlockType _type = world.getBlock(_args.pos);

  if(!block::isDestructable(_type)){
    _log.debug("Block is not destructable.");
    return true;
  }

  if(initFlag){
    //Check if we need to move to the specified position...
    if(_args.reachable && !helper::inRange(population.bots[botID].pos, _args.pos, _args.range)){
      //Add a walk task...
      Task walk("Walk to destruction site.", botID, &Task::walk);
      walk.args.pos = _args.pos;
      walk.args.range = _args.range;  //High vertical capabilities
      queue.push_back(walk);
    }
  }

  //Perform the Queue
  if(!handleQueue(world, population, audio)){
    //We are not yet at the location.
    return false;
  }

  /*

  //Item is within range, check for appropriate tools for destroying
  bool find = true;
  for(unsigned int i = 0; i < _args.inventory.size(); i++){
    find = false;
    //Check if this item is inside the bot's inventory
    for(unsigned int j = 0; j < _args.inventory.size(); j++){
      if(_args.inventory[i] == population.bots[botID].inventory[j]){
        find = true;
        break;
      }
    }
    //Didn't find the item!
    if(!find){
      break;
    }
  }

  //Check if we found the item.
  if(!find){
    _log.debug("Bot doesn't have required items.");
    return true;
  }
  */

  //Inventory of Drops at the specific position
  Inventory _inventory;
  Item _item;

  //Loop over all possible blocks
  glm::vec3 _pseudopos = _args.pos;

  if(block::isFellable(_type)){
    while(_type != BLOCK_AIR){
      //Destroy the Block
      world.setBlock(_pseudopos, BLOCK_AIR, true);

      //Construct the Item
      if(_item.fromTable(_type)){
        _item.pos = _args.pos;
        _item.setupSprite();  //Needs to be because it is placed in the world.
        _inventory.push_back(_item);  //Add the Item to the Drop-Table
      }
      //Move up
      _pseudopos += glm::vec3(0, 1, 0);
      _type = world.getBlock(_pseudopos);
    }
  }
  else{
    //Destroy the Block
    world.setBlock(_args.pos, BLOCK_AIR, true);
    world.blueprint.addEditBuffer(_pseudopos, BLOCK_AIR, false);

    //Construct the Item
    if(_item.fromTable(_type)){
      _item.pos = _args.pos;
      _item.setupSprite();  //Needs to be because it is placed in the world.
      _inventory.push_back(_item);  //Add the Item to the Drop-Table
    }
  }

  if(!_inventory.empty()){
    Inventory _new;
    mergeInventory(_new, _inventory);
    world.drops.insert(world.drops.begin(), _inventory.begin(), _inventory.end());
  }

  //Look to update
  Task inspect("Inspect", botID, &Task::look);
  inspect.args.range = population.bots[botID].viewDistance;
  inspect.perform(world, population, audio);

  return true;
}

bool Task::place(World &world, Population &population, Audio &audio, State &_args){
  //Make sure we are in range.
  if(initFlag){
    //Check if we need to move to the specified position or not...
    if(_args.reachable && !helper::inRange(population.bots[botID].pos, _args.pos, _args.range)){
      //Add a walk task...
      Task walk("Walk to construction site.", botID, &Task::walk);
      walk.args.pos = _args.pos;
      walk.args.range = _args.range;  //High vertical capabilities
      queue.push_back(walk);
    }
  }

  //Walk to the position if requested.
  if(!handleQueue(world, population, audio)) return false;

  //If we require reachability, but couldn't walk there, then we also can't place.
  if(_args.reachable && !helper::inRange(population.bots[botID].pos, _args.pos, _args.range)) return true;

  //Set the Block and add to Blueprint
  world.setBlock(_args.pos, _args.block, false);
  return true;
}

bool Task::build(World &world, Population &population, Audio &audio, State &_args){
  /*
  One major problem is the attempt to path-find to the appropriate location.
  This can be mitigated by increasing the possible access range.

  I suspect also that the fact the chunks are being constantly remeshed is what is causing this lag.

  Additionally, this needs to be split into convergence steps so the individual tasks don't take forever.

  If states had a blueprint, then blueprints could also be split up and passed around between bots.


  */

  //Multiple Placement Tasks after designing an appropriate blueprint!
  if(initFlag){
    //Define some blueprint...
    Blueprint _house;
    _house.building<RUSTIC>(4);       //Choose which guy to build
    _house = _house.translate(_args.pos + glm::vec3(0, 1, 0)); //Translate into worldspace and sort
    std::sort(_house.editBuffer.begin(), _house.editBuffer.end(),
            [](const bufferObject& a, const bufferObject& b) {
              if(a.pos.y < b.pos.y) return true;
              if(a.pos.y > b.pos.y) return false;

              //Rest doesn't matter.
              return false;
            });

    //Building Tasks
    Task walk("Walk to construction site.", botID, &Task::walk);
    Task destroy("Destroy occupying block.", botID, &Task::destroy);
    Task place("Construct Building.", botID, &Task::place);

    //Generate a series of walking and building tasks.
    while(!_house.editBuffer.empty()){

      //Destroy and Place Tasks
      destroy.args.pos = _house.editBuffer.back().pos;
      destroy.args.range = population.bots[botID].range+glm::vec3(5, 5, 5);
      destroy.args.reachable = false; //We don't need to walk to the location
      place.args.pos =  _house.editBuffer.back().pos;
      place.args.block = _house.editBuffer.back().type;
      place.args.range = population.bots[botID].range+glm::vec3(5, 5, 5);
      place.args.reachable = false; //We don't need to walk to the location

      BlockType _cur = world.getBlock(_house.editBuffer.back().pos);

      //The required block is already present
      if(_house.editBuffer.back().type == _cur){
        _house.editBuffer.pop_back();
        continue;
      }

      //Check if a block needs to be destroyed...
      if(_house.editBuffer.back().type == BLOCK_AIR){
        queue.push_back(destroy);
        _house.editBuffer.pop_back();
        continue;
      }

      //Otherwise, we need to simply straight up replace the block.
      queue.push_back(place);
      _house.editBuffer.pop_back();
    }
  }

  //Write the Changes to Disk at the End
  return handleQueue(world, population, audio);
}

bool Task::take(World &world, Population &population, Audio &audio, State &_args){
  if(!helper::inRange(population.bots[botID].pos, _args.pos, population.bots[botID].range)){
    _log.debug("Too far away.");
    return true;
  }

  //Pickup everything from a location
  Inventory _inventory = world.pickup(_args.pos);

  if(_inventory.empty()){
    _log.debug("Found nothing.");
    return true;
  }

  /*
  This actually needs to look inside the inventories of other items to see if it is inside somewhere!
  */

  //We don't want specific items
  if(_args.inventory.empty()){
    mergeInventory(population.bots[botID].inventory, _inventory);
    _log.debug("Picked up everything.");
    return true;
  }

  //Loop over the inventory!
  for(unsigned int i = 0; i < _inventory.size(); i++){
    //Compare the item to the desired items
    for(unsigned int j = 0; j < _args.inventory.size(); j++){
      //Compare the item to the desired item...
      if(_inventory[i] == _args.inventory[j]){
        //Bot needs this item!
        population.bots[botID].inventory.push_back(_inventory[i]);
        //Remove this item from the items currently being considered
        _inventory.erase(_inventory.end()+i);

        /*
          Here we should do some handling for the amount of items specified
        */

      }
    }
  }

  //Add the non-picked up items back to the world.
  mergeInventory(world.drops, _inventory);
  _log.debug("Picked up specific items.");
  return true;

  //Add the inventory to the bots inventory
}

bool Task::convert(World &world, Population &population, Audio &audio, State &_args){
  return true;
}

bool Task::seek(World &world, Population &population, Audio &audio, State &_args){

  //Do this guy here
  if(!handleQueue(world, population, audio)){
    return false;
  }

  if(initFlag){
    //Execute a look task!
    Task inspect("Inspect", botID, &Task::look);
    inspect.args.range = population.bots[botID].viewDistance;
    inspect.perform(world, population, audio);

    //Memory Query
    std::deque<Memory> recalled;
    //Loop through memories
    for(unsigned int i = 0; i < population.bots[botID].memories.size(); i++){
      //If all matches are required and we have all matches
      if(population.bots[botID].memories[i].state.block == _args.block && population.bots[botID].memories[i].state.reachable){
        recalled.push_back(population.bots[botID].memories[i]);
        population.bots[botID].memories[i].recallScore++;
      }
    }

    if(recalled.empty()){
      //Pick a random location and walk there...
      _log.debug("No relevant location found in memory. Walking to random location nearby.");
      Task walk("Stroll", botID, &Task::walk);
      walk.args.pos = world.getTop(glm::vec2(population.bots[botID].pos.x, population.bots[botID].pos.z) + glm::vec2(rand()%11-5, rand()%11-5));
      walk.args.range = population.bots[botID].range;
      _args.pos = walk.args.pos;

      //Random Walk!
      queue.push_back(walk);
    }
    else{

      //Find the Clostest Memory available to us...
      std::sort(recalled.begin(), recalled.end(),
              [this, population](const Memory& a, const Memory& b) {
                if(a.state.pos.x - population.bots[botID].pos.x > b.state.pos.x - population.bots[botID].pos.x) return true;
                if(a.state.pos.x - population.bots[botID].pos.x < b.state.pos.x - population.bots[botID].pos.x) return false;

                if(a.state.pos.y - population.bots[botID].pos.y > b.state.pos.y - population.bots[botID].pos.y) return true;
                if(a.state.pos.y - population.bots[botID].pos.y < b.state.pos.y - population.bots[botID].pos.y) return false;

                if(a.state.pos.z - population.bots[botID].pos.z > b.state.pos.z - population.bots[botID].pos.z) return true;
                if(a.state.pos.z - population.bots[botID].pos.z < b.state.pos.z - population.bots[botID].pos.z) return false;
                return false;
              });

      //Walk to the location we have in memory (within range!)
      Task walk("Seek", botID, &Task::walk);
      walk.args.pos = recalled.back().state.pos;
      walk.args.range = population.bots[botID].range;
      walk.args.reachable = false;  //Spot must not be directly reachable
      _args.pos = walk.args.pos;

      //Check if we are within range of the memory location.
      if(helper::inRange(population.bots[botID].pos, walk.args.pos, population.bots[botID].range)){
        //We are within range of a validated location
        _log.debug("Accessed location in range of seek objective.");
        return true;
      }

      _log.debug("Walking within range of location in memory.");
      queue.push_back(walk);
    }
  }

  return false;
}

/*
=========================================================
                    MEMORY TASKS
=========================================================
*/

//Look - Scan surroundings and write to memory queue
bool Task::look(World &world, Population &population, Audio &audio, State &_args){
  //Look State (don't reinitialize every time...)
  State state;

  //Character Searches on Grid and Adds what it finds.
  for(int i = population.bots[botID].pos.x-_args.range.x; i <= population.bots[botID].pos.x+_args.range.x; ++i){
    for(int j = population.bots[botID].pos.y-_args.range.y; j <= population.bots[botID].pos.y+_args.range.y; ++j){
      for(int k = population.bots[botID].pos.z-_args.range.z; k <= population.bots[botID].pos.z+_args.range.z; ++k){

        //Get the ChunkPosition
        glm::vec3 c = glm::floor(glm::vec3(i,j,k)/glm::vec3(world.chunkSize));
        if(!(glm::all(glm::greaterThanEqual(c, world.min)) && glm::all(glm::lessThanEqual(c, world.max)))) continue;

        //Set Properties
        state.pos = glm::vec3(i, j, k);
        state.task = population.bots[botID].task;
        state.block = world.getBlock(state.pos);
        population.bots[botID].addMemory(state);
      }
    }
  }
  return true;
}

//Think - Query Memory
bool Task::think(World &world, Population &population, Audio &audio, State &_args){
  //Generate Memory Query
  Memory query;
  query.state = _args;

  //Get the Memories
  std::deque<Memory> memories = population.bots[botID].recallMemories(query, false);

  //Check if we have no memories about that query
  if(!memories.empty()){
    int i = rand()%memories.size();
    _args = memories[i].state;
  }

  //Pick a random memory and set it to the argument
  return true;
}

//Think - Write Short-Term to Long-Term Memory
bool Task::listen(World &world, Population &population, Audio &audio, State &_args){
  //Add listen elements directly to long-term memory stack
  while(!population.bots[botID].shorterm.empty()){
    //Check if we already know the information
    std::deque<Memory> memories = population.bots[botID].recallMemories(population.bots[botID].shorterm.back(), true);

    //When memories are empty, we don't have an exact matching memory
    if(memories.empty()){
      population.bots[botID].memories.push_front(population.bots[botID].shorterm.back());
    }

    population.bots[botID].shorterm.pop_back();
  }
  return true;
}

/*
=========================================================
                MANDATE HANDLING TASKS
=========================================================
*/

bool Task::decide(World &world, Population &population, Audio &audio, State &_args){

  //Perform a Listen
  Task listen("Listen to surroundings.", botID, &Task::listen);
  listen.perform(world, population, audio);
  population.bots[botID].current->set("Harvest Cactus", botID, &Task::Dummy);

  return true;
}

bool Task::request(World &world, Population &population, Audio &audio, State &_args){
  //Write a mandate request to a different bot
  //something something...

  return true;
}

/*
=========================================================
                  COMMUNICATION TASKS
=========================================================
*/

bool Task::interrupt(World &world, Population &population, Audio &audio, State &_args){
  //Attempt to set an interrupt on a different bot
  if(population.bots[botID].tryInterrupt(_args)){ //!!!!Note this uses botID
    return true;
  }
  return false;
}

bool Task::tell(World &world, Population &population, Audio &audio, State &_args){
  if(initFlag){
    //Write to the bots shortterm memory
    population.bots[_args.target].addSound(_args);

    //Do an interrupt Task Repeatedly until success (i.e. persistent approach)
    Task interrupt("Interrupt Bot", _args.target, &Task::interrupt);
    interrupt.args = _args;
    queue.push_back(interrupt);
  }

  return handleQueue(world, population, audio);
}

bool Task::ask(World &world, Population &population, Audio &audio, State &_args){
  return true;
}

bool Task::respond(World &world, Population &population, Audio &audio, State &_args){
  return true;
}

bool Task::converse(World &world, Population &population, Audio &audio, State &_args){
  return true;
}

/*
=========================================================
                      BEHAVIORS
=========================================================
*/

//Special Functions
bool Task::Dummy(World &world, Population &population, Audio &audio, State &_args){
  if(initFlag){
    Task seek("Seek Cactus", botID, &Task::seek);
    seek.args.block = BLOCK_CACTUS;
    seek.args.range = population.bots[botID].range;

    //Use the outputs from the previous task for these tasks.
    Task destroy("Destroy Cactus", botID, &Task::destroy);
    destroy.sound = SOUND_HIT;
    destroy.pass = true;
    destroy.animation = 3;                         //Set the Task's Animation

    Task take("Take Cactus", botID, &Task::take);
    take.pass = true;

    //Add them to the queue
    queue.push_back(take);
    queue.push_back(destroy);
    queue.push_back(seek);
  }

  return handleQueue(world, population, audio);
}

/*
=========================================================
                      PATHFINDING
=========================================================
*/

//Calculatepath - Return a path that the bots should follow along until a goal in 3D
std::vector<glm::vec3> calculatePath(int id, glm::vec3 _dest, Population &population, World &world, glm::vec3 range){
    //Vector to Return
    std::vector<glm::vec3> path;

  	AStarSearch<MapSearchNode> astarsearch;

		// Create a start state
		MapSearchNode nodeStart;
		nodeStart.x = population.bots[id].pos.x;
		nodeStart.y = population.bots[id].pos.y;
    nodeStart.z = population.bots[id].pos.z;

		// Define the goal state
		MapSearchNode nodeEnd;
		nodeEnd.x = _dest.x;
		nodeEnd.y = _dest.y;
    nodeEnd.z = _dest.z;

		// Set Start and goal states
		astarsearch.SetStartAndGoalStates( nodeStart, nodeEnd );

		unsigned int SearchState;
		do{ SearchState = astarsearch.SearchStep(world, range); }
		while( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING );

		if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED ){
				MapSearchNode *node = astarsearch.GetSolutionEnd();
        path.push_back(glm::vec3(node->x, node->y, node->z));

        for(;;){
          node = astarsearch.GetSolutionPrev();
          if( !node )
          {
            break;
          }
          path.push_back(glm::vec3(node->x, node->y, node->z));
        }
				// Once you're done with the solution you can free the nodes up
				astarsearch.FreeSolutionNodes();
		}
		else if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_FAILED ){
      //  if(debug){cout << "Search terminated. Did not find goal state\n";}
      path.clear();
		}
		astarsearch.EnsureMemoryFreed();

    //Remove the First Guy!
    if(!path.empty()){
      path.pop_back();
    }
    return path;
}
