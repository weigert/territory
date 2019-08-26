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

  if(population.bots[botID].sprite.doAnimationFrame()){

    //Return what happens after the task executes
    population.bots[botID].sprite.setAnimation(0);
    auto start = std::chrono::high_resolution_clock::now();
    bool a = (*this.*handle)(world, population, args);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << botID<<", "<<name << ": " << duration.count() << std::endl;
    return a;
  }
  return false;
}

//Special Functions
bool Task::Dummy(World &world, Population &population, State &_args){
  //Set Intial Tasks
  if(initFlag){
    //Construct Tasks
    //Add Arguments
    //Push unto task queue
    Task walk("Walk Around", botID, &Task::walk);
    int _pos[2] = {rand()%(world.worldSize*world.chunkSize), rand()%(world.worldSize*world.chunkSize)};
    walk.args.pos = glm::vec3(_pos[0], world.getTop(glm::vec2(_pos[0], _pos[1])), _pos[1]);
    queue.push(walk);
    initFlag = false;
  }

  //Only perform the queue as specified!
  if(!queue.empty()){
    //Get the Top Task
    Task newtask = queue.top();
    queue.pop();

    if(!newtask.perform(world, population)){
      queue.push(newtask);
      return false;
    }
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
  Task move("Move to Position", botID, &Task::move);
  move.args.pos = population.bots[botID].path.back();

  //Set the Animation
  if(initFlag){
    population.bots[botID].sprite.setAnimation(1);
    population.bots[botID].sprite.animation.translate = glm::vec3(move.args.pos - population.bots[botID].pos)/glm::vec3(4);
    initFlag = false;
  }

  if(move.perform(world, population)){
    //Remove that path element
    population.bots[botID].path.pop_back();
  }
  //We now have a valid path. Take a step along that path.
  return false;
}

bool Task::move(World &world, Population &population, State &_args){
  //Set the bots position
  population.bots[botID].pos = _args.pos;
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
  //Goal Position
  glm::vec3 goal = _args.pos;

  /*
  What happens if this task is run every tick? Does it check if bot passes through??
  How do we do this??
  */

  //Check if bot is outside position or inside
  //Check if Goal is outside the position or inside

  //If Goal and Start are outside
    //Simply teleport + cooldown

  //Check If The Goal Position is Free
  if(world.getBlock(goal) != BLOCK_AIR){
    //Terminate this Task
    return true;
  }

  //Check if we're already there
  if(population.bots[botID].pos == goal){
    //Log this
    //std::cout<<"Bot with ID: "<<botID<<" - Arrived At: x="<<goal.x<<", y="<<goal.y<<"."<<std::endl;
    //Terminate the Task
    return true;
  }

  //Do the Stuff
  if(initFlag){
    //Setup some tasks
    Task step("Step", botID, &Task::step);
    step.args.pos = goal;
    queue.push(step);
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
  return true;

  /*
  glm::vec3 start = population.bots[botID].pos;

  //Replace Formally with viewdistance.
  //This needs the player's position.

  glm::vec3 a = glm::floor(start/glm::vec3(world.chunkSize)) - glm::vec3(1,2,1);
  glm::vec3 b = glm::floor(start/glm::vec3(world.chunkSize)) + glm::vec3(1,2,1);

  //Can't exceed a certain size
  a = glm::clamp(a, glm::vec3(0), glm::vec3(world.worldSize-1, world.worldHeight-1, world.worldSize-1));
  b = glm::clamp(b, glm::vec3(0), glm::vec3(world.worldSize-1, world.worldHeight-1, world.worldSize-1));

  if(glm::any(glm::lessThan(start, a)) || glm::any(glm::greaterThan(start, b))){
    //Goal is outside
    std::cout<<"Start is outside distance"<<std::endl;
  }

  if(glm::any(glm::lessThan(goal, a)) || glm::any(glm::greaterThan(goal, b))){
    //Goal is outside
    std::cout<<"Goal is outside distance"<<std::endl;
  }*/

  /*
  If the goal is outside the field, cut the goal path, to that edge point
  sliced = true;

  This way we can chack if bots are coming in or leaving or what.

  Try to find any valid path in that general direction that works, i.e. slice it
  Walk to that position
  If the player moves, this is recalculated every single frame, doesn't matter.
  If they are at that position, then they immediately teleport to the location and wait there for the remaining manhattan distance.

  If they bot is outside the area, then they should wait the manhattan distance to the nearest point in the frame
  Then they should teleport immediately in and walk to the designated position.

  If they are out and the place is out, they just cooldown then teleport
  */
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

    std::cout<<"X: "<<_dest.x<<", Y: "<<_dest.y<<", Z: "<<_dest.z<<std::endl;

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
    auto start = std::chrono::high_resolution_clock::now();
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
      std::cout<<"Search Failed"<<std::endl;
      path.clear();
		}
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Calculating Path: " << duration.count() << std::endl;

		astarsearch.EnsureMemoryFreed();
    return path;
}
