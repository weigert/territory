enum BiomeType{
  BIOME_OCEAN = 0,
  BIOME_DESERT = 1,
  BIOME_FOREST = 2,
};

class Chunk{
public:
  //Position information and size information
  glm::vec3 pos;
  int size;
  BiomeType biome;

  //Data Storage Member
  Octree data;  //Raw Data

  //Chunk Manipulation Functions
  void fillVolume(glm::vec3 start, glm::vec3 end, BlockType _type);
  void hollowBox(glm::vec3 start, glm::vec3 end, BlockType _type);
};
