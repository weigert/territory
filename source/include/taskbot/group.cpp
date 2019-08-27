#include "task.h"
#include "population.h"
#include "../world/world.h"
#include "state.h"

#include "mandate.h"

//Constructor
Mandate::Mandate(short _issuer, short _bot, short _group, const bool _repeat, Task *_request, short _urgency){
  //Set the Quantities
  issuer = _issuer;
  bot = _bot;
  group = _group;
  repeat = _repeat;
  request = _request;
  urgency = _urgency;
  viability = 0;
  //The goal and current states will thereby be identical.
  //The learning effect will be null, unless a goal state is added.
}

void Mandate::initStates(World &world, Population &population, int bot){
    //Add the current and shit here
    curr.retrieveState(world, population, bot);
    goal.retrieveState(world, population, bot);
}

//Retreive the suggestions
void Mandate::getSuggest(){
  //Mastertask to hold found tasks
  Task *masterTask = new Task("Execute Mandate.", (int)bot, &Task::Dummy);

  //Interpret the Goal State
  /*
  1. Identify unsatisfied conditions (now - goal)
  2. Create an order of precedence for state variables
  3. For every unsatisfied condition in precedence order:
    1. Search subconscious for tasks that either make dGoal of that state 0, or would change it towards (using a cost function minimizer)
    2. Add that dCurr to the current state, and reevaluate the precedence order (incase tasks need to be interjected)
    3. Add the associated task with correct metadata to the list
    4. Repeat until the difference to goal disappears, or until 10 tasks are allocated.
  4. Add the requested task at the and, so all preconditions are fulfilled.
  */

/*
Example: Drink something specific at location -> get item, go to location,
Example: Pick something up at location -> go to location, execute grab task
Example: Find some water to drink

Examples we want: Find food, eat.
                  Find water, drink.
                  Speak with people (makes you happy)
                  Find / do things that make you happy.
                  Sleep based on some day cycle.
*/

  //Requested Task is added at the end
  masterTask->queue.push(*request);

  //Finally, the suggested task is the mastertask.
  suggest = masterTask;


  /*
  Future Expansions:
    1. Mandate Responsibility: Delegate tasks if we are not responsible (group)
    2. Don't delegate but remove if person is not member of group / non-friendly.

    1. Some mandate sanity check (not sure if required yet)
  */
}

//Set the Goal State so it is ready for saving.
void Mandate::setGoal(World &world, Population &population, int bot){
  //Get the now state
  State now;
  now.retrieveState(world, population, bot);

  //Overloaded Operators Required
  dCurr = now - curr;
  dGoal = goal - now;

  //Now Update Goal and Curr to Reflect
  curr = now; //Curr becomes what it ended up as
}


short Mandate::getViability(){
  //This returns the probability of executing a task chain
  //Multiply with some property of the bot.
/*
  what role does time play?
    for a current time very close to goal time, viability becomes extremely low.
    this way you can raise probability of executing at a time.
    you can also remove this.


      Take the urgency and multiply it by the viability.
      This is the score.

*/
  //Put some condition here
  return -1;
}

bool Mandate::completed(World &world, Population &population, int bot){
  //Retrieve Now State
  State now;
  now.retrieveState(world, population, bot);

  //Check for Goal Condition
  if(now == goal){
    return true;
  }

  //We have not fulfilled our condition.
  return false;
}
