#pragma once
#include "../forward/memory.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/task.fwd.h"
#include "../forward/sprite.fwd.h"
#include "state.h"
#include "../forward/item.fwd.h"
#include "../forward/audio.fwd.h"

std::string spriteFiles[4] = {"cowboyfull.png", "hunterfull.png", "nativefull.png", "ladyfull.png"};

struct Memory{
    State state;
    int recallScore;
};

class Bot{
  private:
    unsigned int memorySize;
    unsigned int shortermSize;

  public:
    Bot(int _ID);

    //Meta Data
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 range = glm::vec3(1, 2, 1);
    glm::vec3 viewDistance = glm::vec3(1);

    std::string species;
    bool dead = false;
    int ID;

    //Bot has a sprite
    Sprite sprite;
    void setupSprite();

    //Inventory and Attributes
    Inventory inventory;
    void mergeInventory(Inventory _merge);

    //Bot Activities and Actions
    Task* current;
    bool interrupt = false; //Interrupt flag
    bool tryInterrupt(State _state); //Attempt an Interruption of the Bot
    std::string task = "Idle"; //Task Name
    std::stack<glm::vec3> path;   //Movement Path
    void executeTask(World &world, Population &population, Audio &audio);

    //Memories / Brain
    std::deque<Memory> shorterm; //Shortterm Sensory Memory
    std::deque<Memory> memories; //Longterm Memory
    std::vector<Task*> mandates;  //Mandate Memory
    void addSound(State _state);
    std::deque<Memory> recallMemories(Memory &query, bool all);
    void updateMemory(Memory &query, bool all, Memory &memory);
    inline void addMemory(State &state);
};

//Constructors
Bot::Bot(int _ID){
  viewDistance = glm::vec3(2);
  memorySize = 25;
  shortermSize = 5;
  species = "Human";
  ID = _ID;
  pos = glm::vec3(0);
}
