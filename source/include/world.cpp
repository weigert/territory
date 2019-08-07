#include "world.h"

void World::generate(){
  //Seed the Random Generator
  srand(SEED);

  //Perlin Noise Generator
  //noise::module::Perlin perlin;

  //Loop over all Chunks in the World
  for(int i = 0; i < worldSize; i++){
    for(int j = 0; j < worldSize; j++){
      for(int k = 0; k < worldHeight; k++){
        //Generate a new chunk at a specific location
        Chunk chunk;
        chunk.biome = BIOME_FOREST;
        chunk.size = chunkSize;
        chunk.data.maxDepth = log2(chunkSize);
        //chunk.maxDepth = log2(chunkSize);
        // = new Chunk(glm::vec3(i, k, j));
        chunk.pos = glm::vec3(i, k, j);
        chunk.data.setRoot();

        //This needs to be done properly!!!!

        if(k == 0){
          chunk.fillVolume(glm::vec3(0,0,0), glm::vec3(chunkSize-1,0,chunkSize-1), BLOCK_GRASS);
        }

        for(int l = 0; l < 5; l++){
          int rock[2] = {rand()%chunkSize, rand()%chunkSize};
          chunk.data.setPosition(rock[0],1,rock[1],BLOCK_STONE);
          int tree[2] = {rand()%chunkSize, rand()%chunkSize};
          chunk.data.setPosition(tree[0],1,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],2,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],3,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],4,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],5,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],6,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],7,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],8,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],9,tree[1],BLOCK_WOOD);
          chunk.data.setPosition(tree[0],10,tree[1],BLOCK_LEAVES);
          chunk.data.setPosition(tree[0]-1,9,tree[1],BLOCK_LEAVES);
          chunk.data.setPosition(tree[0]+1,9,tree[1],BLOCK_LEAVES);
          chunk.data.setPosition(tree[0],9,tree[1]-1,BLOCK_LEAVES);
          chunk.data.setPosition(tree[0],9,tree[1]+1,BLOCK_LEAVES);
        }

        //Save this thing to file
        boost::filesystem::path data_dir(boost::filesystem::current_path());
        data_dir /= "save";
        data_dir /= saveFile;
        std::stringstream ss;
        ss << "00" << std::to_string(i) << "00" << std::to_string(j) << "00" << std::to_string(k) << ".chunk";
        data_dir /= ss.str();
        saveChunk(data_dir.string(), chunk);
      }
    }
  }

  //We need to set the player's sprite to the correct height

}

void World::loadChunks(){
  std::cout<<"loading chunks"<<std::endl;
  //Get the path of the chunks
  chunks.clear();
  //Loop over our chunk renderdistance
  for(int i = chunkPos.x - renderDistance; i <= chunkPos.x + renderDistance; i++){
    for(int j = chunkPos.y - renderDistance; j <= chunkPos.y + renderDistance; j++){
      for(int k = chunkPos.z - renderDistance; k <= chunkPos.z + renderDistance; k++){
        //Generate the supposed file name
        boost::filesystem::path data_dir(boost::filesystem::current_path());
        data_dir /= "save";
        data_dir /= saveFile;
        std::stringstream ss;
        ss << "00" << std::to_string(i) << "00" << std::to_string(k) << "00" << std::to_string(j) << ".chunk";
        data_dir /= ss.str();
        //Check for file name existence
        if(boost::filesystem::is_regular_file(data_dir)){
          //Load the chunk into memory
          Chunk _chunk;
          loadChunk(data_dir.string(), _chunk);
          chunks.push_back(_chunk);
        }
      }
    }
  }
}

bool World::saveChunk(std::string fileName, Chunk chunk){
  //Save the current loaded chunks to file and the world data
  std::ofstream out(fileName);
  {
    boost::archive::text_oarchive oa(out);
    oa << chunk;
  }
  return true;
}

bool World::loadChunk(std::string fileName, Chunk &chunk){
  //Load the actual worldfile, and the chunks relevant for the player.
  std::ifstream in(fileName);
  {
    boost::archive::text_iarchive ia(in);
    ia >> chunk;
  }
  return true;
}

glm::vec4 World::getColorByID(BlockType _type){
  //Switch the value and return a vector
  glm::vec4 color;
  switch(_type){
    case BLOCK_GRASS:
      color = glm::vec4(0.54f, 0.7f, 0.34f, 1.0f);
      break;
    case BLOCK_DIRT:
      color = glm::vec4(0.74f, 0.5f, 0.36f, 1.0f);
      break;
    case BLOCK_WATER:
      color = glm::vec4(0.02f, 0.61f, 0.75f, 1.0f);
      break;
    case BLOCK_SAND:
      color = glm::vec4(0.93f, 0.91f, 0.38f, 1.0f);
      break;
    case BLOCK_CLAY:
      color = glm::vec4(0.97f, 0.5f, 0.44f, 1.0f);
      break;
    case BLOCK_STONE:
      color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
      break;
    case BLOCK_LEAVES:
      color = glm::vec4(0.38f, 0.48f, 0.26f, 0.8f);
      break;
    case BLOCK_WOOD:
      color = glm::vec4(0.6f, 0.375f, 0.14f, 1.0f);
      break;
    default:
      color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
      break;
  }
  return color;
}

namespace boost {
namespace serialization {

//Chunk Serializer
template<class Archive>
void serialize(Archive & ar, Chunk & _chunk, const unsigned int version)
{
  ar & _chunk.pos.x;
  ar & _chunk.pos.y;
  ar & _chunk.pos.z;
  ar & _chunk.size;
  ar & _chunk.biome;
  ar & _chunk.data;
}

//Octree Serializer
template<class Archive>
void serialize(Archive & ar, Octree & _octree, const unsigned int version)
{
  ar & _octree.maxDepth;
  ar & _octree.depth;
  ar & _octree.isRoot;
  ar & _octree.isNode;
  ar & _octree.type;
  ar & _octree.subTree;
}

//World Serializer
template<class Archive>
void serialize(Archive & ar, World & _world, const unsigned int version)
{
  ar & _world.saveFile;
  ar & _world.chunkSize;
  ar & _world.worldSize;
  ar & _world.worldHeight;
  ar & _world.playerPos.x;
  ar & _world.playerPos.y;
  ar & _world.playerPos.z;
  ar & _world.chunkPos.x;
  ar & _world.chunkPos.y;
  ar & _world.chunkPos.z;
}


} // namespace serialization
} // namespace boost

bool World::loadWorld(){
  //Get current path
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  //Make sure savedirectory exists
  if(!boost::filesystem::is_directory(data_dir)){
    //Create the directory upon failure
    boost::filesystem::create_directory(data_dir);
  }

  //Check if the savefile directory exists
  data_dir /= saveFile;
  if(!boost::filesystem::is_directory(data_dir)){
    //Create the directory upon failure
    boost::filesystem::create_directory(data_dir);
    //Save the world here
    saveWorld();
    return true;
  }

  //Load the World
  data_dir /= "world.save";
  std::ifstream in(data_dir.string());
  {
    boost::archive::text_iarchive ia(in);
    ia >> *this;
  }
  return true;
}

bool World::saveWorld(){
  //Get the location
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.save";
  //Make new directory, save stuff to a world file
  std::ofstream out(data_dir.string());
  {
    boost::archive::text_oarchive oa(out);
    oa << *this;
  }
  generate();
  return true;
}
