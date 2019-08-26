#include "bot.h"
#include "population.h"
#include "../world/world.h"
#include "state.h"

#include "task.h"

/*
=========================================================
                      CONSTRUCTORS
=========================================================
*/
Task::Task(std::string taskName, int taskBotID, bool (Task::*taskHandle)(World&, Population&, State &_args)){
  name = taskName;
  botID = taskBotID;
  handle = taskHandle;
}

Task::Task(std::string taskName, int taskBotID, int animationID, glm::vec3 animationTranslate, bool (Task::*taskHandle)(World&, Population&, State &_args)){
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
    Task newtask = queue.top();
    queue.pop();

    //Perform the Task and Put it back on when successful
    if(!newtask.perform(world, population)){
      //We need to repeat the animation
      queue.push(newtask);
    }
    //Task was performed successfully, we leave it off.
    return false;
  }

  //Queue is empty, thereby we have been completed.
  initFlag = true;
  return true;
}

//Empty Task
bool Task::null(World &world, Population &population, State &_args){
  //Simply Return True
  return true;
}

/*
=========================================================
                      MOVING TASKS
=========================================================
*/

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
    queue.push(move);
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
    queue.push(step);
  }

  //Only perform the queue as specified!
  return handleQueue(world, population);
}

/*
=========================================================
                      OTHER TASKS
=========================================================
*/

//Special Functions
bool Task::Dummy(World &world, Population &population, State &_args){
  //Set Intial Tasks
  if(initFlag){
    Task idle("Walk Around", botID, &Task::idle);
    idle.args.time = 5;
    queue.push(idle);
  }

  //Only perform the queue as specified!
  return handleQueue(world, population);
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
  //Set the Stuff
  if(initFlag){
    Task null("Wait", botID, &Task::null);
    null.animation = 2;
    //Add the Tasks
    for(int i = 0; i < _args.time; i++){
      queue.push(null);
    }
  }

  //Handle the Queue
  return handleQueue(world, population);
}

//Secondaries (Multi-Step, Multi-Task)
bool Task::search(World &world, Population &population, State &_args){
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
    queue.push(wait);
    queue.push(walk); //Walk will be executed first

    //Update the First Flag
    initFlag = false;
  }

  return handleQueue(world, population);
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
      path.clear();
		}
		astarsearch.EnsureMemoryFreed();
    return path;
}
