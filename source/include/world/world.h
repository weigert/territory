//Include Forward Declaration
#pragma once
#include "../forward/world.fwd.h"
//Dependency Forward Declaration
#include "../forward/view.fwd.h"
#include "../forward/geology.fwd.h"
#include "../forward/shader.fwd.h"
#include "../forward/billboard.fwd.h"
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"
#include "../forward/player.fwd.h"
#include "../forward/item.fwd.h"

//EditBuffer Struct
struct bufferObject {
  int chunkSize;
  glm::vec3 pos;
  glm::vec3 cpos;
  BlockType type;
};

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
  std::stack<int> updateModels;
  int SEED = 10;
  int chunkSize = 16;
  int tickLength = 1;
  glm::vec3 dim = glm::vec3(10, 5, 10);

  //Items placed / on the ground
  std::vector<Item>placed;
  std::vector<Item>drops;

  //Movement Weights
  int moveWeight(BlockType _type);
  BlockType getBlock(glm::vec3 _pos);
  void setBlock(glm::vec3 _pos, BlockType _type);
  int getTop(glm::vec2 _pos);

  //Generate Function / Chunk Handlers
  void generate();
  void generateBlank();
  void generateTectonic();
  void generateFlat();
  void generatePerlin();

  //Edit Buffer (Efficient World Editing)
  std::vector<bufferObject> editBuffer;
  bool addEditBuffer(glm::vec3 _pos, BlockType _type);
  bool evaluateEditBuffer();
  void bufferChunks(View view);    //Reloads all relevant chunks from file

  //File IO Management
  std::string saveFile;
  bool loadWorld();
  bool saveWorld();
};
