#include "bot.h"
#include "population.h"
#include "../world/world.h"
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

/*
=========================================================
                    TASK HANDLING TASKS
=========================================================
*/
//Empty Task
bool Task::null(World &world, Population &population, State &_args){
  //Simply Return True
  return true;
}

//Execute Task Function
bool Task::perform(World &world, Population &population){
  //Change the Name and Execute the Task
  population.bots[botID].task = name;

  //When initially performing a task, make sure to set the animation!
  if(initFlag){
    population.bots[botID].sprite.setAnimation(animation, translate);
  }

  //When the animation is complete, reset the animation.
  if(population.bots[botID].sprite.doAnimationFrame()){
    //We actually want to set the animation after it is completed
    if((*this.*handle)(world, population, args)){
      return true;
    }
  }

  //Task was executed but needs to be repeated, set initFlag
  initFlag = false;
  return false;
}

//Handle the Queue
bool Task::handleQueue(World &world, Population &population){
  //If the Queue isn't empty, handle top task
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.back();
    queue.pop_back();

    //Perform the Task and Put it back on when successful
    if(!newtask.perform(world, population)){
      //We need to repeat the animation
      queue.push_back(newtask);
    }
    //Task was performed successfully, we leave it off.
    return false;
  }

  //Queue is empty, thereby we have been completed.
  initFlag = true;
  return true;
}

//Example - How to Construct a High-Level Task
bool Task::example(World &world, Population &population, State &_args){
  //Check
  if(initFlag){
    Task null("Example", botID, &Task::null);   //Construct the Task "null"
    null.animation = 0;                         //Set the Task's Animation
    queue.push_back(null);                           //Add it to the Queue
  }

  //Handle the Queue appropriately.
  return handleQueue(world, population);
}

/*
=========================================================
                      MOVING TASKS
=========================================================
*/

//Wait - Do a series of Null Tasks
bool Task::wait(World &world, Population &population, State &_args){
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
  return handleQueue(world, population);
}

//Move - Teleport to Location
bool Task::move(World &world, Population &population, State &_args){
  //Do the Value-Change
  population.bots[botID].pos = _args.pos;
  population.bots[botID].path.pop_back();
  return true;
}

//Step - Multiple Moves Along a Path
bool Task::step(World &world, Population &population, State &_args){

  //We now want to step towards our goal. Check if we have a path.
  if(population.bots[botID].path.empty()){
    //Calculate a Path Here.
    population.bots[botID].path = calculatePath(botID, _args.pos, population, world);
    //If the Path is still empty
    if(population.bots[botID].path.empty()){
      //No valid Path!
      //if(debug){std::cout<<botID<<": No valid path."<<std::endl;
      //Make this location unreachable if it is in memory and reachable
      glm::vec3 point = _args.pos;
      Memory query;
      query.state.pos = point;

      //Overwrite Memory, which is unreachable
      Memory memory;
      memory.state.reachable = false;

      //Overwrite any specified points in memory in all memories matching all points in query
      population.bots[botID].updateMemory(query, true, memory);
      return true;
    }
    //We now have a valid path.
  }

  /*Check for path stil valid in this next move!*/

  if(queue.empty()){
    Task move("Move to Position", botID, &Task::move);
    move.args.pos = population.bots[botID].path.back();
    move.animation = 1;
    move.translate = glm::vec3(move.args.pos - population.bots[botID].pos)/glm::vec3(4);
    queue.push_back(move);
  }

  return handleQueue(world, population);
}

//Walk - Multiple Steps with Error Handling
bool Task::walk(World &world, Population &population, State &_args){
  //Goal Position
  glm::vec3 goal = _args.pos;

  /*
  Check if bot is outside position or inside
  Check if goal is outside position or inside

  If goal and start are outside, teleport and wiat
  If otherwise you need to crop the two endpoints and include waiting
  */

  //Check If The Goal Position is Free
  if(world.getBlock(goal) != BLOCK_AIR){
    return true;
  }

  //Check if we're already there
  if(population.bots[botID].pos == goal){
    return true;
  }

  //Do the Stuff
  if(initFlag){
    //Setup some tasks
    Task step("Step", botID, &Task::step);
    step.args.pos = goal;
    queue.push_back(step);
  }

  //Only perform the queue as specified!
  return handleQueue(world, population);
}

//Idle - Move Around Randomly and Wait some time
bool Task::idle(World &world, Population &population, State &_args){
  //Check for initial flag
  if(initFlag){
    Task walk("Walk", botID, &Task::walk);
    int _shift[2] = {rand()%10-5, rand()%10-5};
    glm::vec2 _pos = glm::vec2(population.bots[botID].pos.x,population.bots[botID].pos.z ) + glm::vec2(_shift[0], _shift[1]);
    walk.args.pos = glm::vec3(_pos.x, world.getTop(_pos), _pos.y);

    Task wait("Wait", botID, &Task::wait);
    wait.args.time = _args.time;

    //Add the Tasks for Idle
    queue.push_back(wait);
    queue.push_back(walk); //Walk will be executed first

    //Update the First Flag
    initFlag = false;
  }

  return handleQueue(world, population);
}

/*
================================================================================
                            ITEM MANAGEMENT TASKS
================================================================================
*/

bool Task::collect(World &world, Population &population, State &_args){
  //If the bot is within range
  if(glm::all(glm::lessThanEqual(population.bots[botID].pos - _args.pos, population.bots[botID].range))){
    //Item is within range, check for appropriate tools for collection
    bool find = true;
    for(unsigned int i = 0; i < _args.inventory.size(); i++){
      find = false;
      //Check if this item is inside the bot's inventory
      for(unsigned int j = 0; j < _args.inventory.size(); j++){
        if(_args.inventory[i]._type == population.bots[botID].inventory[j]._type){
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
      return true;
    }

    //Found the item, drop it
    BlockType _type = world.getBlock(_args.pos);

    if(_type != BLOCK_AIR){
      //Destroy the Block
      world.setBlock(_args.pos, BLOCK_AIR);
      world.addEditBuffer(_args.pos, BLOCK_AIR);

      //Drop the Item
      Item _item;
      _item.fromTable(_type);
      _item.pos = _args.pos;
      _item.setupSprite();  //Needs to be because it is placed in the world.
      world.drops.push_back(_item);  //Add the Item to the Drop-Table
    }
  }
  return true;
}

bool Task::take(World &world, Population &population, State &_args){
  //Check if Bot is in Range
  if(!glm::all(glm::lessThanEqual(population.bots[botID].pos - _args.pos, population.bots[botID].range))){
    return true;
  }
  return true;
}

bool Task::find(World &world, Population &population, State &_args){
  return true;
}

bool Task::search(World &world, Population &population, State &_args){
  return true;
}

bool Task::retrieve(World &world, Population &population, State &_args){
  return true;
}

bool Task::convert(World &world, Population &population, State &_args){
  return true;
}

/*
=========================================================
                    MEMORY TASKS
=========================================================
*/

//Look - Scan surroundings and write to memory queue
bool Task::look(World &world, Population &population, State &_args){
  //Parse Arguments
  int rad = _args.dist; //View Square Size

  //Character Searches on Grid and Adds what it finds.
  for(int i = population.bots[botID].pos.x-rad; i <= population.bots[botID].pos.x+rad; i++){
    for(int j = population.bots[botID].pos.y-rad; j <= population.bots[botID].pos.y+rad; j++){
      for(int k = population.bots[botID].pos.z-rad; k <= population.bots[botID].pos.z+rad; k++){
        //Form new Memories from Surroundings
        population.bots[botID].addMemory(world, glm::vec3(i, j, k));
      }
    }
  }
  return true;
}

//Think - Query Memory
bool Task::think(World &world, Population &population, State &_args){
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
bool Task::listen(World &world, Population &population, State &_args){
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

  //Pick a random memory and set it to the argument
  return true;
}

/*
=========================================================
                MANDATE HANDLING TASKS
=========================================================
*/

bool Task::decide(World &world, Population &population, State &_args){
  //Check if we have mandates to go
  /*
  if(population.bots[botID].mandates.empty()){
    //Set the Current Task to Something Abitrary
    Task *masterTask = new Task("Human Task", botID, &Task::Dummy);
    population.bots[botID].current = masterTask;
  }
  else{
    //Determine a viable task from the list of tasks to perform
    population.bots[botID].current = population.bots[botID].mandates.back();
    population.bots[botID].mandates.pop_back();
    //For now, we are just taking the back element
  }
  */
  //Take whatever mandate is available
  return false;
}

bool Task::request(World &world, Population &population, State &_args){
  //Write a mandate request to a different bot
  //something something...

  return true;
}

/*
=========================================================
                  COMMUNICATION TASKS
=========================================================
*/

bool Task::interrupt(World &world, Population &population, State &_args){
  //Attempt to set an interrupt on a different bot
  if(population.bots[_args.target].tryInterrupt(_args)){ //!!!!Note this uses botID
    return true;
  }
  return false;
}

bool Task::tell(World &world, Population &population, State &_args){
  if(initFlag){
    //Write to the bots shortterm memory
    population.bots[_args.target].addSound(_args);

    //Do an interrupt Task Repeatedly until success (i.e. persistent approach)
    Task interrupt("Interrupt Bot", _args.target, &Task::interrupt);
    interrupt.args = _args;
    queue.push_back(interrupt);
  }

  return handleQueue(world, population);
}

bool Task::ask(World &world, Population &population, State &_args){
  return true;
}

bool Task::respond(World &world, Population &population, State &_args){
  return true;
}

bool Task::converse(World &world, Population &population, State &_args){
  return true;
}

/*
=========================================================
                      BEHAVIORS
=========================================================
*/

//Special Functions
bool Task::Dummy(World &world, Population &population, State &_args){
  //Set Intial Tasks
  if(initFlag){
    Task idle("Walk Around", botID, &Task::idle);
    idle.args.time = 5;
    queue.push_back(idle);
  }

  //Only perform the queue as specified!
  return handleQueue(world, population);
}

//Secondaries (Multi-Step, Multi-Task)
/*
bool Task::search(World &world, Population &population, State &_args){

  Task inspect("Inspect", botID, &Task::look);
  inspect.args[0] = 0;

  //If we are on the item we are looking for
  if(world.fill[population.bots[botID].pos[0]][population.bots[botID].pos[1]] == arguments[0]){
    if(debug){std::cout<<"Found Item: "<<world.getFillName(arguments[0])<<std::endl;}
    return true;
  }

  if(initFlag){
    //Memory Query
    //Recal the Memories
    //Recall Memory
    Memory query;
    query.object = world.getFillName(arguments[0]);
    query.reachable = true;
    query.queryable[0] = true;
    query.queryable[3] = true;
    //Query the memory requiring all components to be exact matches
    std::deque<Memory> memories = population.bots[botID].recallMemories(query, true);

    //If memories isn't empty, we have to add some move to and analyze tasks.
    if(!memories.empty()){
      //We need to go look in these places!!
      //Pick a random memory from here
      for(unsigned int i = memories.size(); i > 0; i--){
        Task walk("Seek", botID, &Task::walk);
        walk.args[0] = memories[i-1].location.x;
        walk.args[1] = memories[i-1].location.y;

        queue.push_back(inspect);
        queue.push_back(walk);
      }
      //We have some tasks, so we can skip the repeating this.
    }
    else{
      Task walk("Stroll", botID, &Task::walk);
      walk.args[0] = rand()%100;
      walk.args[1] = rand()%100;

      Task look("Scan", botID, &Task::look);
      look.args[0] = population.bots[botID].viewDistance;

      queue.push_back(inspect);
      queue.push_back(look);
      queue.push_back(walk);

      //We didn't find anything. Therefore we must search!
      //Walk to a random location and look
    }
    //Do not repeat this step yet.
    initFlag = false;
  }

  //Work off our allocated queue.
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.top();
    queue.pop_back();
    //If our new Task is not performed successfully
    if(!newtask.perform(world, population)){
      //Put the Task back on
      queue.push_back(newtask);
      return false;
    }
    //If it was successful, we leave it off
    return false;
  }

  if(debug){std::cout<<"Bot with ID: "<<botID<<" found nothing. Restarting search."<<std::endl;}

  //Reset For Next Time
  initFlag = true;
  return false;
}*/

bool Task::follow(World &world, Population &population, State &_args){
  return true;
}

bool Task::locate(World &world, Population &population, State &_args){
  return true;
}

/*
bool Task::forage(World &world, Population &population, State &_args){
  //Task Starting from the Beginning
  if(initFlag){
    //Define our Tasks
    Task search("Locate Food", botID, &Task::search);
    search.args[0] = arguments[0];

    Task look("Look for more Food", botID, &Task::look);
    look.args[0] = 2;

    Task take("Take Food", botID, &Task::take);

    Task home("Walk Home", botID, &Task::walk);
    home.args[0] = population.loc[0];
    home.args[1] = population.loc[1];

    Task store("Store", botID, &Task::store);

    //Add in reverse order!
    queue.push_back(store);
    queue.push_back(home);
    queue.push_back(look);
    queue.push_back(take);
    queue.push_back(search);

    initFlag = false;
  }

  //Debug Flag
  if(debug){std::cout<<"Bot with ID: "<<botID<<" is foraging."<<std::endl;}

  //Work off our allocated queue.
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.top();
    queue.pop_back();
    //If our new Task is not performed successfully
    if(!newtask.perform(world, population)){
      //Put the Task back on
      queue.push_back(newtask);
    }
    //If it was successful, we leave it off
    return false;
  }

  //Return Success Case
  if(debug){std::cout<<"Bot with ID: "<<botID<<" finished foraging."<<std::endl;}
  initFlag = true;
  return true;
}
*/
/*
=========================================================
                      PATHFINDING
=========================================================
*/

//Calculatepath - Return a path that the bots should follow along until a goal in 3D
std::vector<glm::vec3> calculatePath(int id, glm::vec3 _dest, Population &population, World &world){
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
		do{ SearchState = astarsearch.SearchStep(world); }
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
    path.pop_back();
    return path;
}
