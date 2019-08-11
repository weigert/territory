//Include Header File
#include "world.h"

/*
===================================================
          WORLD GENERATING FUNCTIONS
===================================================
*/

void World::generate(){
  //I guess we should split the world-gen into multiple steps?
  std::cout<<"Generating New World"<<std::endl;
  std::cout<<"Seed: "<<SEED<<std::endl;
  //Seed the Random Generator
  srand(SEED);

  //Generate the Blank Region Files
  std::cout<<"Generating Blank World"<<std::endl;
  generateBlank();

  //Generate Height
  std::cout<<"Filling World"<<std::endl;
  generatePerlin();
  //generateFlat();

  //Place the Player
  std::cout<<"Placing Player"<<std::endl;
  //placePlayer();
}

void World::generateBlank(){
  //Add Blank Chunks to Region Files
  //Loop over all Chunks in the World
  for(int i = 0; i < worldSize; i++){
    for(int j = 0; j < worldHeight; j++){
      for(int k = 0; k < worldSize; k++){

        //Generate a new chunk at a specific location
        Chunk chunk;
        chunk.biome = BIOME_VOID;
        chunk.size = chunkSize;
        chunk.data.depth = log2(chunkSize); //Set the remaining depth
        chunk.pos = glm::vec3(i, j, k);

        //Write the Chunk to File
        writeChunk(chunk);
      }
    }
  }
}

void World::generateFlat(){
  //Flat Surface
  std::cout<<"Generating Flat Surface"<<std::endl;
  for(int i = 0; i < worldSize*chunkSize; i++){
    for(int j = 0; j < worldSize*chunkSize; j++){
      //Add to the editBuffer
      addEditBuffer(glm::vec3(i,0,j), BLOCK_GRASS);
    }
  }

  //Evaluate the editBuffer
  evaluateEditBuffer();

  //Rocks
  std::cout<<"Adding Rocks"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int rock[2] = {rand()%(chunkSize*worldSize), rand()%(chunkSize*worldSize)};
    addEditBuffer(glm::vec3(rock[0], 1, rock[1]), BLOCK_STONE);
  }

  //Evaluate the editBuffer
  evaluateEditBuffer();

  //Trees
  std::cout<<"Adding Trees"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int tree[2] = {rand()%(chunkSize*worldSize), rand()%(chunkSize*worldSize)};

    //Add the shit to the editbuffer
    addEditBuffer(glm::vec3(tree[0], 1, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 2, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 3, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 4, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 5, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 6, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 7, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 8, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], 9, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]+1, 8, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]-1, 8, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], 8, tree[1]+1), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], 8, tree[1]-1), BLOCK_LEAVES);
  }

  //Evaluate the Buffer
  evaluateEditBuffer();

}

void World::generatePerlin(){
  //Perlin Noise Generator
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(12);
  perlin.SetFrequency(5);
  perlin.SetPersistence(0.5);

  //Loop over the world-size
  std::cout<<"Generating Perlin Surface"<<std::endl;
  for(int i = 0; i < worldSize*chunkSize; i++){
    for(int k = 0; k < worldSize*chunkSize; k++){

      //Generate the Heightvalue

      //Normalize the Block's x,z coordinates
      float x = (float)(i) / (float)(chunkSize*worldSize);
      float z = (float)(k) / (float)(chunkSize*worldSize);

      float height = perlin.GetValue(x, SEED, z)/5+0.25;
      height *= (worldHeight*chunkSize);

      //Now loop over the height and set the blocks
      for(int j = 0; j < (int)height-1; j++){
        //Add the block to the editBuffer
        addEditBuffer(glm::vec3(i, j, k), BLOCK_DIRT);
      }
      addEditBuffer(glm::vec3(i, (int)height-1, k), BLOCK_GRASS);
    }
  }

  //Evaluate the Guy
  evaluateEditBuffer();

  //Rocks
  std::cout<<"Adding Rocks"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int rock[2] = {rand()%(chunkSize*worldSize), rand()%(chunkSize*worldSize)};
    //Normalize the Block's x,z coordinates
    float x = (float)(rock[0]) / (float)(chunkSize*worldSize);
    float z = (float)(rock[1]) / (float)(chunkSize*worldSize);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (worldHeight*chunkSize);

    addEditBuffer(glm::vec3(rock[0], (int)height, rock[1]), BLOCK_STONE);
  }

  //Evaluate the Guy
  evaluateEditBuffer();

  //Add Trees
  std::cout<<"Adding Trees"<<std::endl;
  for(int i = 0; i < 1000; i++){
    int tree[2] = {rand()%(chunkSize*worldSize), rand()%(chunkSize*worldSize)};

    float x = (float)(tree[0]) / (float)(chunkSize*worldSize);
    float z = (float)(tree[1]) / (float)(chunkSize*worldSize);

    float height = perlin.GetValue(x, SEED, z)/5+0.25;
    height *= (worldHeight*chunkSize);

    //Add the shit to the editbuffer
    addEditBuffer(glm::vec3(tree[0], (int)height, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+1, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+2, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+3, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+4, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+5, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+6, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+7, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+8, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+9, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+10, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+11, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+12, tree[1]), BLOCK_WOOD);
    addEditBuffer(glm::vec3(tree[0], (int)height+13, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]+1, (int)height+12, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0]-1, (int)height+12, tree[1]), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], (int)height+12, tree[1]+1), BLOCK_LEAVES);
    addEditBuffer(glm::vec3(tree[0], (int)height+12, tree[1]-1), BLOCK_LEAVES);
  }

  //Evaluate the Guy
  evaluateEditBuffer();
}

/*
===================================================
          OPTIMIZING UTILITY FUNCTIONS
===================================================
*/

bool World::addEditBuffer(glm::vec3 _pos, BlockType _type){
  //Check validity
  if(glm::any(glm::lessThan(_pos, glm::vec3(0))) || glm::any(glm::greaterThanEqual(_pos, glm::vec3(chunkSize)*glm::vec3(worldSize, worldHeight, worldSize)))){
    //Invalid Position
    return false;
  }

  //Add a new bufferObject
  editBuffer.push_back(bufferObject());
  editBuffer.back().pos = _pos;
  editBuffer.back().cpos = glm::floor(_pos/glm::vec3(chunkSize));
  editBuffer.back().type = _type;

  //Push it on
  return true;
}

//Sorting Operator for bufferObjects
bool operator<(const bufferObject& a, const bufferObject& b) {
  if(a.cpos.x < b.cpos.x) return true;
  if(a.cpos.x > b.cpos.x) return false;

  if(a.cpos.y < b.cpos.y) return true;
  if(a.cpos.y > b.cpos.y) return false;

  if(a.cpos.z < b.cpos.z) return true;
  if(a.cpos.z > b.cpos.z) return false;

  return false;
}

//Sorting Operator for bufferObjects
bool operator>(const bufferObject& a, const bufferObject& b) {
  if(a.cpos.x > b.cpos.x) return true;
  if(a.cpos.x < b.cpos.x) return false;

  if(a.cpos.y > b.cpos.y) return true;
  if(a.cpos.y < b.cpos.y) return false;

  if(a.cpos.z > b.cpos.z) return true;
  if(a.cpos.z < b.cpos.z) return false;

  return false;
}

bool World::evaluateEditBuffer(){
  //Check if the editBuffer isn't empty!
  if(editBuffer.empty()){
    std::cout<<"editBuffer is empty."<<std::endl;
    return false;
  }

  //Sort the editBuffer
  std::sort(editBuffer.begin(), editBuffer.end(), std::greater<bufferObject>());

  //Open the File
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;

  //Load File and Write File
  std::ifstream in((data_dir/"world.region").string());
  std::ofstream out((data_dir/"world.region.temp").string());

  //Chunk for Saving Data
  Chunk _chunk;

  int n_chunks = 0;

  //While there is still stuff inside the editBuffer...
  while(!editBuffer.empty()){
    //Read the File into the Chunk

    {
      boost::archive::text_iarchive ia(in);
      ia >> _chunk;
    }

    //If the chunk is not equal to the editbuffer's element
    while(_chunk.pos != editBuffer.back().cpos){
      //Make sure there is an endoffile criterion.
      if(in.eof()){
        std::cout<<"Error: Reached end of file."<<std::endl;
        return false;
      }

      //Create the Thing
      {
        boost::archive::text_oarchive oa(out);
        oa << _chunk;
        n_chunks++;
        boost::archive::text_iarchive ia(in);
        ia >> _chunk;
      }
    }

    //Now we have a chunk that corresponds the the editBuffer element
    while(!editBuffer.empty() && _chunk.pos == editBuffer.back().cpos){
      //Set the block in the chunk.
      _chunk.data.setPosition(glm::mod(editBuffer.back().pos, glm::vec3(chunkSize)), editBuffer.back().type);

      //Erase the first element
      editBuffer.pop_back();
    }

    //Write the guy to file
    _chunk.data.trySimplify();
    //Write the chunk to file
    {
      boost::archive::text_oarchive oa(out);
      oa << _chunk;
      n_chunks++;
    }
  }

  //Fill up the rest
  while(n_chunks < worldSize*worldSize*worldHeight){
    boost::archive::text_iarchive ia(in);
    boost::archive::text_oarchive oa(out);
    oa << _chunk;
    ia >> _chunk;
    n_chunks++;
  }


  //Close the fstream and ifstream
  in.close();
  out.close();

  //Delete the first file, rename the temp file
  boost::filesystem::remove_all((data_dir/"world.region").string());
  boost::filesystem::rename((data_dir/"world.region.temp").string(),(data_dir/"world.region").string());

  //Success!
  return true;
}

/*
===================================================
          FILE IO HANDLING FUNCTIONS
===================================================
*/

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
    }
    //Make sure that the chunk that we determined will not be removed is also
    for(unsigned int j = 0; j < load.size(); j++){
      if(load[j].x == chunks[i].pos.x && load[j].y == chunks[i].pos.y && load[j].z == chunks[i].pos.z){
        //Remove the element from load, as it is already inside this guy
        load.erase(load.begin()+j);
      }
    }
  }

  updateModels = remove;

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

bool World::writeChunk(Chunk chunk){
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
