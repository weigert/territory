//Include Header File
#include "world.h"

void World::generate(){
  //Seed the Random Generator
  srand(time(NULL));

  //Perlin Noise Generator
  //noise::module::Perlin perlin;

  //Loop over all Chunks in the World
  for(int i = 0; i < worldSize; i++){
    for(int j = 0; j < worldHeight; j++){
      for(int k = 0; k < worldSize; k++){
        //Generate a new chunk at a specific location
        Chunk chunk;
        chunk.biome = BIOME_FOREST;
        chunk.size = chunkSize;
        chunk.data.depth = log2(chunkSize); //Set the remaining depth
        chunk.pos = glm::vec3(i, j, k);

        chunk.fillVolume(glm::vec3(0,0,0),glm::vec3(chunkSize-1,0,chunkSize-1), BLOCK_GRASS);

        for(int i = 0; i < 10; i++){
          //Random Locations of Rocks / Trees
          int rock[2] = {rand()%chunkSize, rand()%chunkSize};
          int tree[3] = {rand()%(chunkSize-2)+1, rand()%(chunkSize-2)+1, rand()%(chunkSize/2)+chunkSize/2-1};

          //Place Rocks
          chunk.data.setPosition(rock[0],1,rock[1],BLOCK_STONE);

          //Place Trees
          for(int j = 0; j < tree[2]; j++){
            chunk.data.setPosition(tree[0],j,tree[1],BLOCK_WOOD);
          }
          chunk.data.setPosition(tree[0],tree[2]+1,tree[1],BLOCK_LEAVES);
          chunk.data.setPosition(tree[0]+1,tree[2],tree[1],BLOCK_LEAVES);
          chunk.data.setPosition(tree[0]-1,tree[2],tree[1],BLOCK_LEAVES);
          chunk.data.setPosition(tree[0],tree[2],tree[1]+1,BLOCK_LEAVES);
          chunk.data.setPosition(tree[0],tree[2],tree[1]-1,BLOCK_LEAVES);
        }

        //Try to simplify the chunk
        chunk.data.trySimplify();
        saveChunk(chunk);
      }
    }
  }
}

void World::bufferChunks(){
  //Load / Reload all Visible Chunks

  //Non-Brute Force Method
  //Chunks that should be loaded
  glm::vec3 a = chunkPos - renderDistance;
  glm::vec3 b = chunkPos + renderDistance;


  //Can't exceed a certain size
  a = glm::clamp(a, glm::vec3(0), glm::vec3(worldSize-1, worldHeight-1, worldSize-1));
  b = glm::clamp(b, glm::vec3(0), glm::vec3(worldSize-1, worldHeight-1, worldSize-1));

  //Construct a vector so we can check which guys need loading

  //Chunks that need to be removed
  std::stack<int> remove;
  std::vector<glm::vec3> load;

  //Construct the Vector of guys we should load
  for(int i = a.x; i <= b.x; i ++){
    for(int j = a.y; j <= b.y; j ++){
      for(int k = a.z; k <= b.z; k ++){
        //Add the vector that we should be loading
        load.push_back(glm::vec3(i, j, k));
      }
    }
  }

  //Loop over all existing chunks
  for(unsigned int i = 0; i < chunks.size(); i++){
    //Check if any of these chunks are outside of the limits of a / b
    if(glm::any(glm::lessThan(chunks[i].pos, a)) || glm::any(glm::greaterThan(chunks[i].pos, b))){
      //Check if the chunk was updated
      if(chunks[i].updated){
        //Save chunk to file
        overwriteChunk(chunks[i].pos.x, chunks[i].pos.y, chunks[i].pos.z, chunks[i]);
      }
      //Add the chunk to the erase pile
      remove.push(i);

      //Loop over the load vector, see if the guy we are looking at is identical, remove it in that case
      for(unsigned int j = 0; j < load.size(); j++){
        if(load[j].x == chunks[i].pos.x && load[j].y == chunks[i].pos.y && load[j].z == chunks[i].pos.z){
          //Remove the element from load, as it is already inside this guy
          load.erase(load.begin()+j);
        }
      }
    }
  }

  //Loop over the erase pile, delete the relevant chunks.
  while(!remove.empty()){
    chunks.erase(chunks.begin()+remove.top());
    remove.pop();
  }

  //Loop over the guys we want to load
  while(!load.empty()){
    //Make sure the vector we are trying to load is legit
    Chunk _chunk;
    loadChunk(load.back().x, load.back().y, load.back().z, _chunk);
    //Add the chunk
    chunks.push_back(_chunk);
    load.pop_back();
  }
}

bool World::saveChunk(Chunk chunk){
  //Get the Working Directory
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.region";

  //Save the current loaded chunks to file and the world data
  std::ofstream out(data_dir.string(), std::ofstream::app);
  {
    boost::archive::text_oarchive oa(out);
    oa << chunk;    //Append the Chunk to the Region File
  }
  return true;
}

bool World::loadChunk(int i, int j, int k, Chunk &chunk){
  //Get the Working Directory
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.region";

  //Load the actual worldfile, and the chunks relevant for the player.
  std::ifstream in(data_dir.string());

  //Skip the Lines
  int n = i*worldSize*worldHeight+j*worldSize+k;
  while(n>0){
    in.ignore(100000,'\n');
    n--;
  }

  {
    boost::archive::text_iarchive ia(in);
    ia >> chunk;
  }
  return true;
}

bool World::overwriteChunk(int i, int j, int k, Chunk chunk){
  return true;
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
  ar & _octree.depth;
  ar & _octree.index;
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
  data_dir /= "world.meta";
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
  data_dir /= "world.meta";
  //Make new directory, save stuff to a world file
  std::ofstream out(data_dir.string());
  {
    boost::archive::text_oarchive oa(out);
    oa << *this;
  }
  generate();
  return true;
}
