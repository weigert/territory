//Include Forward Declaration
#include "../game/item.h"
#include "blueprint.h"
#include "chunk.h"

#pragma once
#include "../forward/world.fwd.h"
//Dependency Forward Declaration
#include "../forward/view.fwd.h"
#include "../forward/shader.fwd.h"
#include "../forward/billboard.fwd.h"
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/item.fwd.h"

class World{
public:
  //Constructor
  World(std::string _saveFile){
    saveFile = _saveFile;
    //Try loading / saving the world
    loadWorld();
  }

  //Block Data in Octree
  std::vector<Chunk> chunks; //Loaded Chunks
  std::unordered_map<int, int> chunk_order;
  int SEED = 100;
  int chunkSize = 16;
  int sealevel = 16;
  std::chrono::milliseconds tickLength = std::chrono::milliseconds(1000);
  glm::vec3 dim = glm::vec3(20, 5, 20);
  bool format_octree = false;

  //Min and Max Chunk Positions
  glm::vec3 min = glm::vec3(0);
  glm::vec3 max = dim;

  //Other Stuff
  bool lock = false;  //Lock the World Data for Chunk Loading
  volatile int time = 540;  //Is set in a separate timed thread.

  //Items placed / on the ground
  Inventory placed;
  Inventory drops;
  void drop(Inventory inventory);
  Inventory pickup(glm::vec3 pos);

  //Movement Weights
  int moveWeight(BlockType _type);
  BlockType getBlock(glm::vec3 _pos);
  void setBlock(glm::vec3 _pos, BlockType _type, bool fullremesh);
  glm::vec3 getTop(glm::vec2 _pos);

  //Generate Function / Chunk Handlers
  void generate();
  void generateBlank();
  void generateFlat();
  void generateBuildings();
  void generatePerlin();
  void generateForest();

  //Helpers for Blueprint
  Blueprint blueprint;
  Blueprint remeshBuffer;
  bool evaluateBlueprint(Blueprint &_blueprint);
  void bufferChunks(View &view);

  //File IO Management
  std::string saveFile;
  bool loadWorld();
  bool saveWorld();
};
