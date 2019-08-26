//Include Forward Declaration
#pragma once
#include "../forward/world.fwd.h"
//Dependency Forward Declaration
#include "../forward/view.fwd.h"
#include "../forward/shader.fwd.h"
#include "../forward/chunk.fwd.h"
#include "../forward/octree.fwd.h"
#include "../forward/player.fwd.h"

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
  glm::vec3 dim = glm::vec3(20, 5, 20);

  //Movement Weights
  int moveWeight(BlockType _type);
  BlockType getBlock(glm::vec3 _pos);
  int getTop(glm::vec2 _pos);

  //Generate Function / Chunk Handlers
  void generate();
  void generateBlank();
  void generateFlat();
  void generatePerlin();

  //Edit Buffer (Efficient World Editing)
  std::vector<bufferObject> editBuffer;
  bool addEditBuffer(glm::vec3 _pos, BlockType _type);
  bool evaluateEditBuffer();

  //File IO Management
  std::string saveFile;
  bool loadWorld();
  bool saveWorld();
  bool writeChunk(Chunk chunk);
  bool loadChunk(glm::vec3 _c, Chunk &chunk);
  void bufferChunks(View view);    //Reloads all relevant chunks from file
};
