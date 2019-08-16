#pragma once
#include "../forward/mandate.fwd.h"
#include "../forward/memory.fwd.h"
#include "../forward/world.fwd.h"
#include "../forward/task.fwd.h"
#include "../forward/sprite.fwd.h"

class Bot{
  public:
    //Meta Data
    glm::vec3 home = glm::vec3(0);
    glm::vec3 pos = glm::vec3(0);
    int ID;
    int viewDistance;
    unsigned int memorySize;
    unsigned int subconSize;
    int forage;
    bool trail;
    bool fly;
    std::string species;

    //Bot has a sprite
    Sprite sprite;
    void setupSprite();

    //Raw Constructor
    Bot(int _ID);
    Bot(std::string s, bool t, bool f, int view, int forag, int mem, int id, glm::vec3 _pos, glm::vec3 _home);

    //Inventory and Attributes
    int carry = 0; //What is the Ant Carrying?

    //Bot Activities and Actions
    Task* current;
    bool interrupt = false; //Interrupt flag
    std::string task = "Idle"; //Task Name
    std::vector<glm::vec3> path;   //Movement Path
    void executeTask(World &world, Population &population);

    //Subconscious
    std::deque<Mandate> mandates; //Mandates Memory
    std::deque<Mandate> subconsc; //Subconscious Task Memory
    void addSubcon(Mandate experience);
    void evaluateMandates(World &world, Population &population);

    //Memories / Brain
    std::deque<Memory> memories; //Longterm Memory
    std::deque<Memory> shorterm; //Shortterm Sensory Memory
    std::deque<Memory> recallMemories(Memory &query, bool all);
    void updateMemory(Memory &query, bool all, Memory &memory);
    void addMemory(World world, glm::vec3 _pos);
};
