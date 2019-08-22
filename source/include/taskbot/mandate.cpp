#include "task.h"
#include "population.h"
#include "../world/world.h"
#include "state.h"

#include "mandate.h"

//Constructor
Mandate::Mandate(short _issuer, short _bot, short _group, const bool _repeat, short _urgency){
  //Set the Quantities
  issuer = _issuer;
  bot = _bot;
  group = _group;
  repeat = _repeat;
  urgency = _urgency;
  viability = 0;
}

//Retreive the suggestions
void Mandate::getSuggest(){
  //Mastertask to hold found tasks
  //Interpret the Goal State
  /*
  If the responsible person for the mandate is not you
    -> If they are lower in the group hierarchy, delegate
    -> If they are unknown / etc, delete it
    -> If they are somewhere above, make a request

  If the bot can't figure a list of task, getSuggest fails.
  It has a choice between deleting the mandate or asking for people how to do it.
  Or delegating.

  If the issuer is not the same bot, then at the end of the task list we will add a "report result" task to the bot, so that they can share the experience of what happened did, and the other bot can associate it with their delegation task.
  */

  /*
  In our memory, what will we have saved?
  -> Do we remember task metadata?
  -> Do we remember the goal state or the current state, or just the effect?

  Simple Example:
    Get Axe -> Chop Wood -> Sell Wood

    Goal State:
      -> Position

    How do we walk to the fucking position?
      By taking the goal state and plugging it directly into a walking action

    How do we know the walking action will succeed for this specific goal state?
      By remembering that

    We need to iterate:
    TaskMeta is a function of the memories and the goal state
      TaskMeta = (Memory, Goal)





    Mandate{
      absolute_goal
      relative_goal

      ternary_indicator_vector[]
    }

    Memory{
      relative_to_start
      relative_to_end
    }

    For every item in ternary indicator vector, if it is -1, it is irrelevant

    if it is zero, we observe it as a relative requirement
    if it is one, we observe it as an absolute requirement

    A lot of tasks take absolute values as their input.





    In our memory, we then have:
    -> What was the difference to our starting state
    -> The difference to the desired end state
    ->

    -> What is our Mandate?
    -> What is in our Memory?
    -> How do we reconstruct a list of tasks?


    Goal -> deltaGold += 1
    To solve this, the bot searches for memories where he remembers deltaGold being +1
    Then he has a task, but lacks context:
      -> What was the task metadata?
      -> What were the previous requirements?


  1. Goal State is Either Differential or Absolute, not Both!
  2. What if two different aspects are differential / absolute?

    e.g. time is differential, so therefore wait
  3.
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
//  masterTask->queue.push(*request);

  //Finally, the suggested task is the mastertask.
  //suggest = masterTask;


  /*
  Future Expansions:
    1. Mandate Responsibility: Delegate tasks if we are not responsible (group)
    2. Don't delegate but remove if person is not member of group / non-friendly.

    1. Some mandate sanity check (not sure if required yet)
  */
}


void Mandate::getViability(){
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
  //For now, shorter list is more viable
  //Otherwise, add the times for each task
  //


  //Put some condition here
  viability = 0;
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
