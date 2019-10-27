#pragma once
#include "../forward/mandate.fwd.h"
#include "../forward/memory.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/task.fwd.h"
#include "../forward/sprite.fwd.h"
#include "../forward/state.fwd.h"
#include "../forward/item.fwd.h"
#include "../forward/audio.fwd.h"

class Bot{
  public:
    //Meta Data
    glm::vec3 home = glm::vec3(0);
    glm::vec3 pos = glm::vec3(0);
    glm::vec3 range = glm::vec3(1, 2, 1);
    glm::vec3 viewDistance = glm::vec3(1);

    int ID;
    unsigned int memorySize;
    unsigned int shortermSize;
    int forage;
    bool trail;
    bool fly;
    std::string species;
    bool dead = false;

    //Bot has a sprite
    Sprite sprite;
    void setupSprite();

    //Raw Constructor
    Bot(int _ID);
    Bot(std::string s, bool t, bool f,int forag, int mem, int id, glm::vec3 _pos, glm::vec3 _view, glm::vec3 _home);

    //Inventory and Attributes
    Inventory inventory;
    void mergeInventory(Inventory _merge);

    //Bot Activities and Actions
    Task* current;
    bool interrupt = false; //Interrupt flag
    bool tryInterrupt(State _state); //Attempt an Interruption of the Bot
    std::string task = "Idle"; //Task Name
    std::vector<glm::vec3> path;   //Movement Path
    void executeTask(World &world, Population &population, Audio &audio);

    //Memories / Brain
    std::deque<Memory> shorterm; //Shortterm Sensory Memory
    std::deque<Memory> memories; //Longterm Memory
    std::vector<Task*> mandates;  //Mandate Memory
    void addSound(State _state);
    std::deque<Memory> recallMemories(Memory &query, bool all);
    void updateMemory(Memory &query, bool all, Memory &memory);
    void addMemory(World world, glm::vec3 _pos);
};
