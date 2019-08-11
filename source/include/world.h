#pragma once
class View;

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
  int worldSize = 20;    //In Chunks^2
  int worldHeight = 5;  //In Chunks
  glm::vec3 renderDistance = glm::vec3(1, 2, 1);

  //Player Position
  glm::vec3 playerPos = glm::vec3(8, 0, 8); //Position of the player in chunkspace
  glm::vec3 chunkPos = glm::vec3(5, 0, 5);  //Position of the player in worldspace

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
  bool loadChunk(int i, int j, int k, Chunk &chunk);
  bool overwriteChunk(int i, int j, int k, Chunk chunk);
  void bufferChunks();    //Reloads all relevant chunks from file
};
