#pragma once
class View;

class World{
public:
  World(std::string _saveFile){
    saveFile = _saveFile;
    //Try loading / saving the world
    loadWorld();
  }
  //Block Data in Octree
  std::vector<Chunk> chunks; //Loaded Chunks
  int SEED = 10;
  int chunkSize = 16;
  int worldSize = 20;    //In Chunks^2
  int worldHeight = 2;  //In Chunks
  glm::vec3 renderDistance = glm::vec3(1, 0, 1);

  //Player Position
  glm::vec3 playerPos = glm::vec3(0, 0, 0);  //Position of the player in chunkspace
  glm::vec3 chunkPos = glm::vec3(1, 0, 1);           //Position of the player in worldspace

  //World Information Data
  std::string saveFile;

  //Generate Function / Chunk Handlers
  void generate();
  void bufferChunks();    //Reloads all relevant chunks from file

  //Save and Load Worldfiles
  bool loadWorld();
  bool saveWorld();
  bool overwriteChunk(int i, int j, int k, Chunk chunk);
  bool saveChunk(Chunk chunk);
  bool loadChunk(int i, int j, int k, Chunk &chunk);
};
