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
  int worldSize = 10;    //In Chunks^2
  int worldHeight = 1;  //In Chunks

  int renderDistance = 5;

  //Player Position
  glm::vec3 playerPos = glm::vec3(0, 0, 0);  //Position of the player in chunkspace
  glm::vec3 chunkPos = glm::vec3(1, 0, 1);           //Position of the player in worldspace

  //World Information Data
  std::string saveFile;

  //Generate Function / Chunk Handlers
  void generate();
  void loadChunks();  //Reloads all relevant chunks from file
  glm::vec4 getColorByID(BlockType _type);

  //Save and Load Worldfiles
  bool loadWorld();
  bool saveWorld();
  bool saveChunk(std::string fileName, Chunk chunk);
  bool loadChunk(std::string fileName, Chunk &chunk);
};
