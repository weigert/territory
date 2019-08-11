enum BiomeType{
  BIOME_VOID = 0,
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
  bool updated = false;  //Has the data been updated?
  bool refreshModel = true;
};
