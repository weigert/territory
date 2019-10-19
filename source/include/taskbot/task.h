//Pathfinding Stuff"
#pragma once
#include "../forward/world.fwd.h"
#include "../forward/item.fwd.h"
#include "../forward/population.fwd.h"
#include "../forward/bot.fwd.h"
#include "../forward/state.fwd.h"

using namespace std;

std::vector<glm::vec3> calculatePath(int id, glm::vec3 _dest, Population &population, World &world, glm::vec3 range);
using namespace std::placeholders;

//Add this Typedef for easier reading
typedef bool (Task::*Handle)(World&, Population&, State&);

enum TaskHandle{
  TASK_NULL, TASK_LOOK, TASK_LISTEN, TASK_THINK, TASK_WAIT, TASK_MOVE, TASK_STEP, TASK_WALK, TASK_IDLE, TASK_FOLLOW, TASK_SEEK, TASK_COLLECT,
  TASK_TAKE, TASK_FIND, TASK_SEARCH, TASK_RETRIEVE, TASK_CONVERT, TASK_DECIDE, TASK_REQUEST, TASK_INTERRUPT,
  TASK_TELL, TASK_ASK, TASK_RESPOND, TASK_CONVERSE, TASK_LOCATE
};

std::string TaskName[25] = {
  "NULL", "LOOK", "LISTEN", "THINK", "WAIT", "MOVE", "STEP", "WALK", "IDLE", "FOLLOW", "SEEK", "COLLECT", "TAKE", "FIND", "SEARCH",
  "RETRIEVE", "CONVERT", "DECIDE", "REQUEST", "INTERRUPT", "TELL", "ASK", "RESPOND", "CONVERSE", "LOCATE"
};

//Task Class
class Task{
  public:
    //Members
    std::vector<Task> queue;
    bool initFlag = true;
    State args;

    Handle handle;
    int botID;
    std::string name;
    int animation = 0;
    glm::vec3 translate = glm::vec3(0);

    //Constructor
    Task();
    Task(std::string taskName, int taskBotID, TaskHandle _handle);
    Task(std::string taskName, int taskBotID, Handle taskHandle);
    Task(std::string taskName, int taskBotID, int animationID, glm::vec3 animationTranslate, Handle taskHandle);


    //Task Handling Tasks
    bool perform(World &world, Population &population);
    bool handleQueue(World &world, Population &population);
    bool example(World &world, Population &population, State &_args);

    //Memory Tasks
    bool null(World &world, Population &population, State &_args);  //Do Nothing
    bool look(World &world, Population &population, State &_args);
    bool listen(World &world, Population &population, State &_args);
    bool think(World &world, Population &population, State &_args);

    //Movement Tasks
    bool wait(World &world, Population &population, State &_args);
    bool move(World &world, Population &population, State &_args);  //Move Position
    bool step(World &world, Population &population, State &_args);
    bool walk(World &world, Population &population, State &_args);
    bool idle(World &world, Population &population, State &_args);
    bool follow(World &world, Population &population, State &_args);
    bool seek(World &world, Population &population, State &_args);

    //Item Management Tasks
    bool collect(World &world, Population &population, State &_args);
    bool take(World &world, Population &population, State &_args);
    bool find(World &world, Population &population, State &_args);
    bool search(World &world, Population &population, State &_args);
    bool retrieve(World &world, Population &population, State &_args);
    bool convert(World &world, Population &population, State &_args);

    //Mandate Management Tasks
    bool decide(World &world, Population &population, State &_args);
    bool request(World &world, Population &population, State &_args);

    //Communication Tasks
    bool interrupt(World &world, Population &population, State &_args);
    bool tell(World &world, Population &population, State &_args);
    bool ask(World &world, Population &population, State &_args);
    bool respond(World &world, Population &population, State &_args);
    bool converse(World &world, Population &population, State &_args);
    bool locate(World &world, Population &population, State &_args);

    //Behaviors
    bool Dummy(World &world, Population &population, State &_args);
};

Handle TaskHandles[25] = {
  &Task::null, &Task::look, &Task::listen, &Task::think, &Task::wait, &Task::move, &Task::step, &Task::walk,
  &Task::idle, &Task::follow, &Task::seek, &Task::collect, &Task::take, &Task::find, &Task::search, &Task::retrieve,
  &Task::convert, &Task::decide, &Task::request, &Task::interrupt, &Task::tell, &Task::respond, &Task::converse, &Task::locate
};
