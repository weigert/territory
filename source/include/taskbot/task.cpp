#include "bot.h"
#include "population.h"
#include "../world/world.h"
#include "state.h"

#include "task.h"

/*
Pathfinding:
-> For small distance travel, far outside of the players visibility: Simply teleport with a cooldown
-> For large distance travel, outside or within players visibility: Create a road graph network and allow them to travel along the road
  -> Additionally, if the player can't see it, just teleport between road nodes and shit.

-> For small distance travel, within a region of the players visibility: Use regular pathfinding
-> You can also do a truncate move where they exit the player's viewfield

-> How do you make sure their target location is reachable?
-> Something something waypoints as favorable nodes, so that paths are routed through them!

-> What if they randomly want to search for shit? idk fam...
-> When they're not loaded, I guess they won't be doing anything..
-> But all they're really doing now is just walking around...
-> They need much better pathfinding targets anyhow. Right now they don't make a lot of sense.
-> When they have proper pathfinding targets, we can think about other stuff.

-> They need more CONTENT to really showcase their complexity.
-> Follow will also have some complexity.


-> Maybe I should simulate some woodland creatures first....
*/

Task::Task(std::string taskName, int taskBotID, bool (Task::*taskHandle)(World&, Population&, State &_args)){
  name = taskName;
  botID = taskBotID;
  handle = taskHandle;
}

//Execute Task Function
bool Task::perform(World &world, Population &population){
  //if(debug){std::cout<<"Bot with ID: "<<botID<<" performing task: "<<name<<std::endl;}
  //Change the Name and Execute the Task
  population.bots[botID].task = name;

  return (*this.*handle)(world, population, args);
}

//Special Functions
bool Task::Dummy(World &world, Population &population, State &_args){
  //Set Intial Tasks
  if(initFlag){
    //Construct Tasks
    //Add Arguments
    //Push unto task queue
    Task walk("Follow Player", botID, &Task::walk);
    walk.args.pos = glm::vec3(rand()%world.worldSize*world.chunkSize,1,rand()%world.worldSize*world.chunkSize);
    queue.push(walk);

    initFlag = false;
  }

  //Only perform the queue as specified!
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.top();
    queue.pop();

    //If our new Task is not performed successfully
    if(!newtask.perform(world, population)){
      queue.push(newtask);
      return false;
    }
    //If it was successful, we leave it off
    return false;
  }

  //Return Case for Mastertask
  initFlag = true;
  return true;
}

//Primitives (Single Step)
bool Task::step(World &world, Population &population, State &_args){
  /*
  Task: Step

  Description:
    Iteratively move to the specified position with error handling.

  Arguments:
    0 - X
    1 - Y
    2 - Z

  Expansion:

  */

  //We now want to step towards our goal. Check if we have a path.
  //std::cout<<"A:"<<population.bots[botID].path.empty()<<std::endl;
  if(population.bots[botID].path.empty()){
    //Calculate a Path Here.
    population.bots[botID].path = calculatePath(botID, _args.pos, population, world);
    //If the Path is still empty
    if(population.bots[botID].path.empty()){
      //No valid Path!

      //if(debug){std::cout<<botID<<": No valid path."<<std::endl;}
/*
      //Make this location unreachable if it is in memory and reachable
      glm::vec3 point = _args.pos;
      Memory query;
      query.location = point;
      query.queryable[2] = true;

      //Overwrite Memory, which is unreachable
      Memory memory;
      memory.reachable = false;
      memory.queryable[3] = true;

      //Overwrite any specified points in memory in all memories matching all points in query
      population.bots[botID].updateMemory(query, true, memory);
*/
      return true;
    }
    //We now have a valid path.
  }


  //We have a path, but we should check our step validity first.
  /*
  else{
    //Make sure the next step of an OLD PATH is STILL valid
    Point step = population.bots[botID].path.back();
    if(!world.getPassable(step.x, step.y, population.bots[botID].fly)){
      std::cout<<"Path no longer valid. Must calculate new."<<std::endl;
      //Clear the path and let him try to step again.
      population.bots[botID].path.clear();
      std::cout<<"B:"<<population.bots[botID].path.empty()<<std::endl;
      return false;
    }
    //Otherwise we can indeed step there.

  }*/

  //We now have a valid path. Take a step along that path.
  population.bots[botID].pos = population.bots[botID].path.back();
  population.bots[botID].path.pop_back();

  return false;
}

bool Task::consume(World &world, Population &population, State &_args){

  /*
  population.bots[botID].carry = 0;
  */

  return true;
}

bool Task::move(World &world, Population &population, State &_args){
  //Set the Bot Position
  population.bots[botID].pos = _args.pos;
  return true;
}

bool Task::take(World &world, Population &population, State &_args){
  /*
  Task: Take

  Description:
    Pick up an item at the current location with error handling.
    If somebody else picks up an item in the same tick we just saw, we will wait a tick.
    Otherwise we will pick up the item.

  Expansion:
    Specify the direction where the character should look, and maybe an item they should search if its a pile
  */

  /*
  if(initFlag){
    //Add a Swap Task
    Task swap("Swap Item", botID, &Task::swap);
    //Add a Wait Task (1 Tick)
    Task wait("Swap Item", botID, &Task::wait);
    wait.args[0] = 1;

    //Check if we are looking at air - then wait one tick.
    if(world.fill[population.bots[botID].pos[0]][population.bots[botID].pos[1]] == 0){
      wait.perform(world, population);
      return true;
    }
    //Otherwise perform the Swap Task DIRECTLY
    swap.perform(world, population);
    return true;
  }*/

  //Queue is Empty
  return true;
}

bool Task::swap(World &world, Population &population, State &_args){
  //Pickup what's on the ground, put what's in your hand on the ground

  return true;
}

bool Task::store(World &world, Population &population, State &_args){
  //Pick up the item at the location

  //Whatever you're carrying, put it in your inventory!


  return true;
}

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

bool Task::wait(World &world, Population &population, State &_args){
  //Wait
  while(_args.time > 0){
    _args.time--;
    return false;
  }
  //Finished Waiting
  return true;
}

//Secondaries (Multi-Step, Multi-Task)
bool Task::search(World &world, Population &population, State &_args){
  //Arguments:
  //id
  //item
  /*

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

        queue.push(inspect);
        queue.push(walk);
      }
      //We have some tasks, so we can skip the repeating this.
    }
    else{
      Task walk("Stroll", botID, &Task::walk);
      walk.args[0] = rand()%100;
      walk.args[1] = rand()%100;

      Task look("Scan", botID, &Task::look);
      look.args[0] = population.bots[botID].viewDistance;

      queue.push(inspect);
      queue.push(look);
      queue.push(walk);

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
    queue.pop();
    //If our new Task is not performed successfully
    if(!newtask.perform(world, population)){
      //Put the Task back on
      queue.push(newtask);
      return false;
    }
    //If it was successful, we leave it off
    return false;
  }

  if(debug){std::cout<<"Bot with ID: "<<botID<<" found nothing. Restarting search."<<std::endl;}

  //Reset For Next Time
  initFlag = true;*/
  return false;
}

bool Task::forage(World &world, Population &population, State &_args){
/*
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
    queue.push(store);
    queue.push(home);
    queue.push(look);
    queue.push(take);
    queue.push(search);

    initFlag = false;
  }

  //Debug Flag
  if(debug){std::cout<<"Bot with ID: "<<botID<<" is foraging."<<std::endl;}

  //Work off our allocated queue.
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.top();
    queue.pop();
    //If our new Task is not performed successfully
    if(!newtask.perform(world, population)){
      //Put the Task back on
      queue.push(newtask);
    }
    //If it was successful, we leave it off
    return false;
  }

  //Return Success Case
  if(debug){std::cout<<"Bot with ID: "<<botID<<" finished foraging."<<std::endl;}
  initFlag = true;*/
  return true;
}

bool Task::walk(World &world, Population &population, State &_args){
  /*
  //Check if they are in an occupied space
  if(!world.getPassable(population.bots[botID].pos[0], population.bots[botID].pos[1], population.bots[botID].fly)){
    //Bot is in an occupied Space, move them to the colony home.
    population.bots[botID].pos[0] = rand()%100;
    population.bots[botID].pos[1] = rand()%100;
    //Log This
    if(debug){std::cout<<"Bot with ID:"<<botID<<" was in occupied space. Shifted position."<<std::endl;}
    //Repeat this Task next tick
    return false;
  }*/

  glm::vec3 goal = _args.pos;

  //Check If The Goal Position is Free
  if(world.getBlock(goal) != BLOCK_AIR){
    //Log this
    std::cout<<"Goal position for bot with ID: "<<botID<<" is occupied."<<std::endl;
    //Terminate this Task
    return true;
  }

  //Check if we're already there
  if(population.bots[botID].pos == goal){
    //Log this
    std::cout<<"Bot with ID: "<<botID<<" - Arrived At: x="<<goal.x<<", y="<<goal.y<<"."<<std::endl;
    //Terminate the Task
    return true;
  }

  Task step("Step", botID, &Task::step);
  step.args.pos = goal;

  if(!step.perform(world, population)){return false;}
  return true;
}

bool Task::idle(World &world, Population &population, State &_args){
  //if(debug){std::cout<<"Bot with ID: "<<botID<<" idling."<<std::endl;}

/*
  Task walk("Walk", botID, &Task::walk);
  walk.args[1] = population.loc[0];
  walk.args[2] = population.loc[1];

  Task wait("Wait", botID, &Task::wait);
  wait.args[0] = arguments[0];

  //Check for initial flag
  if(initFlag){
    //Add the Tasks for Idle
    queue.push(wait);
    queue.push(walk); //Walk will be executed first

    //Update the First Flag
    initFlag = false;
  }

  //Execute the Queue in the Form it was entered.
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.top();
    queue.pop();
    //If our new Task is not performed successfully
    if(!newtask.perform(world, population)){
      queue.push(newtask);
      return false;
    }
    //If it was successful, we leave it off
    return false;
  }
  //Final Sucess of Supertask*/
  return true;
}

// Needs to be expanded to 3D
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
		}
		astarsearch.EnsureMemoryFreed();
    return path;
}
