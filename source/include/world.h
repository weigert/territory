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
  std::stack<int> updateModels;
  int SEED = 10;
  int chunkSize = 16;
  int worldSize = 50;    //In Chunks^2
  int worldHeight = 10;  //In Chunks
  glm::vec3 renderDistance = glm::vec3(1, 2, 1);

  //Player Position
  glm::vec3 playerPos = glm::vec3(0, 0, 0);  //Position of the player in chunkspace
  glm::vec3 chunkPos = glm::vec3(25, 3, 25);           //Position of the player in worldspace

  //World Information Data
  std::string saveFile;

  //Generate Function / Chunk Handlers
  void generate();
  void generateHeight();
  void generateTrees();
  void flatForest();

  //Player Position Functions
  void placePlayer();
  bool getValidMove(glm::vec3 pos, int height); //Get a Valid Movement

  //Save and Load Worldfiles
  bool loadWorld();
  bool saveWorld();
  bool overwriteChunk(int i, int j, int k, Chunk chunk);
  bool saveChunk(Chunk chunk);
  bool loadChunk(int i, int j, int k, Chunk &chunk);
  void bufferChunks();    //Reloads all relevant chunks from file
};
