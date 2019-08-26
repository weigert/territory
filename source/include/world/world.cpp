//Include Header File
//Dependencies
#include "chunk.h"
#include "octree.h"
#include "../game/player.h"
#include "../render/shader.h"
#include "../render/view.h"
#include "world.h"

/*
===================================================
          WORLD GENERATING FUNCTIONS
===================================================
Proposed Pipeline:
  -> Generate Blank Chunks

  -> Generate Base Height-Map
    -> Low lying areas, flat and! hilly
    -> High lying areas, flat and! hilly

  -> Add Geological Deposits based on elemental distribution maps in 3D!
    -> Generate Perlin Maps for all Elements, then cluster to common minerals and rare stuff too!
    -> Scale the perlin maps by the relative abundance of elements on earth for instance
    -> Mineral Types Need: Brittleness, Solubility, Porosity,
    -> This is still deterministic based on the perlin map!

  -> Generate a Volcanism Map Randomly, with varying degrees of intensity in certain spots
    -> Underground temperature field comes from volcanism
    -> From this is derived a pressure field, which moves groundwater around
    -> Groundwater is empty at first, but is filled later

  -> Generate large bodies of water
    -> Randomly pick minima on the map and grow them up
    -> Sufficiently large volumes (i.e. > critical mass) are then filled
    -> If it reaches the edge, it is flood-filled upto "sea-level" with salt-water

  -> Alternatingly Perform:
    -> Weather Cycle
    -> Vegetation Cycle
    -> Erosion Cycle

  -> Weather Cycle (@ chunk-resolution):
    -> Compute Wind-Speed Map
      -> Slope of Land affects wind Speed
      -> Plant Coverage hinders winds
      -> Heavy wind dropping / rising in P / T very rapidly causes tornadoes?
    -> Compute Humidity Map
      -> Humidity is moved around by wind and diffuses
      -> Bodies of Water add humidity in dependency of temp
      -> Plants remove humidity from the air
      -> Rain removes humidity from the air in dependency of temp
      -> Snow removes humidity from the air
      -> Evaporation lowers the water level of lakes
    -> Compute Temperature Map
      -> Air temperature is moved around by wind and diffuses
      -> Sunlight adds Temperature
      -> Plants block sunlight
      -> Higher elevation has less temperature
      -> Evaporation from water sources cools the temp
      -> Temperature / sunlight will melt snow and ice
    -> Compute Clouds
      -> Temperature and Humidity give cloud-coverage
      -> Critical Values then give rain vs. snow in some areas
      -> Heavy rain causes lightning in hot areas
      -> Clouds reflect sunlight

  -> Vegation Cycle:
    -> Plants have a feature vector for their desired weather values (wind / temp / humidity / elevation / sunlight, etc.)
    -> Plants also have a vector of how they affect those values in turn (wind-blockage, sunlight blockage, soil retention)
    -> Depending on the predictor vectors in a chunk, we can sample from a distribution of plants given by distance to the plant prototypes
    -> Place the plants in appropriate locations near other plants of the same type on subsequent cycles

  -> Erosion / Sedimenation Cycle
    -> Water Erosion:
      -> Rain spawns particles that move down the map at the steepest slope
      -> Particles are eroded based on their solubility
      -> Particles have a certain mass and certain amount of soil the can carry based on mass / speed
      -> Deposited soil becomes dirt / mud
      -> Steeper Slopes / Faster Speeds means more sediment capacity
      -> Particles have inertia based on mass, which moves them along a direction
      -> When particles (nearly) stop and they still have volume, they spawn an amount of water
      -> Particles lose mass over time as they move over porous rock / soil

      How does this make rivers as blocks though?
        -> Track particle frequency of paths and the most frequent areas become rivers based on some width?
        -> Particles inside rivers lose momentum much less?
        -> Something like that (i.e. if they hit a body of water, they are propelled forward / lose inertia much less!!)

    -> Glacial Erosion:
      -> Snow deposits where it falls, lots of snow compacts to ice
      -> Ice needs to move downhill and away from peaks of course, so you get nice exposed ridges!
      -> Sunlight will melt snow and ice and spawn particles too
      -> Ice will also move downhill slowly, leading to erosion based on rock brittleness
      -> Large rock amounts will be broken off and placed into the ice
      -> As the ice melts in lower / hotter regions, these large rocks are placed at the mouth of the glacier

    -> Wind Erosion:
      -> Based on wind-speeds particles are removed and made into a fine sand.
      -> Sand and loose particles are moved around appropriately
      -> Wind carry-capacity is dependent on the speed, as it drops, particles leave the wind.

    -> Ground Water Cycle
      -> Water from the particles of sedimentation will move into porous ground and add to the water amount
      -> High pressure regions will move to low-pressure regions, based on rock porosity
      -> Temperature increases the pressure
      -> Underground pressure is given by the volcanism
      -> Temperature /pressure above a critical amount will lead to geysering / dissolution of rock
      -> Also leads to springs when water is moved to certain regions!

    -> Erosion Cycle / Water / Soil Movement
      -> Water also seeps into the ground when rain falls on flat areas that are porous
      -> This leads to a larger amount of groundwater
      -> Water underground flows to more porous areas
      -> When pressure is above a certain value, you can get geyser formation
      -> Otherwise groundwater can be moved upwards to natural springs
      -> Once lakes are overfilled, they can spill over
      -> Rivers and streams are created by tracking a drainage map over certain areas and growing it!
      -> Fast winds can move loose soil and erode the sides of rock-faces it runs past.
      -> Depending on wind-speed model will incentivize different erosion types
      -> Glaciers move downhill and actively grind the mountain, brittle rocks will be moved by the glacier
      -> Glaciers melt from sunlight and form strong rivers, they move large rocks down the mountain too
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
  perlin.SetOctaveCount(10);
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
      _chunk.setPosition(glm::mod(editBuffer.back().pos, glm::vec3(chunkSize)), editBuffer.back().type);

      //Erase the first element
      editBuffer.pop_back();
    }

    //Write the guy to file
    //_chunk.data.trySimplify();
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
          MOVEMENT RELATED FUNCTIONS
===================================================
*/

int World::moveWeight(BlockType _type){
  switch(_type){
    case BLOCK_AIR:
      return 1;
    case BLOCK_WATER:
      return 5;
    default:
      return 10;
  }
}

BlockType World::getBlock(glm::vec3 _pos){
  //Check if the position is inside, if not return 0, otherwise return the block
  for(unsigned int i = 0; i < chunks.size(); i++){
    glm::vec3 c = glm::floor(_pos/glm::vec3(chunkSize));
    //Check the Chunkpos
    if(c == chunks[i].pos){
      //Read the Goy
      glm::vec3 p = glm::mod(_pos, glm::vec3(chunkSize));
      return chunks[i].getPosition(p, 0);
    }
  }
  return BLOCK_STONE;
}

//Get the Top-Free space position in the x-z position
int World::getTop(glm::vec2 _pos){
  //Highest Block you can Stand On
  int max = 0;

  //Loop over the height
  for(int i = 1; i < worldHeight*chunkSize; i++){
    //Check if we satisfy the conditions
    if(getBlock(glm::vec3(_pos.x, i, _pos.y)) == BLOCK_AIR && getBlock(glm::vec3(_pos.x, i-1, _pos.y)) != BLOCK_AIR){
      max = i;
    }
  }
  return max;
}

/*
===================================================
          FILE IO HANDLING FUNCTIONS
===================================================
*/

void World::bufferChunks(View view){
  //Load / Reload all Visible Chunks

  //Non-Brute Force Method
  //Chunks that should be loaded
  glm::vec3 a = glm::floor(view.viewPos/glm::vec3(chunkSize))-view.renderDistance;
  glm::vec3 b = glm::floor(view.viewPos/glm::vec3(chunkSize))+view.renderDistance;

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
        //Add the updated stuff the the buffer and evaluate it
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

  //Theoretically, we only need to load the file once.
  //we can sort the load vector, then access the relevant chunks ONCE and close the file!
  //Sort the editBuffer

  //Loop over the guys we want to load
  while(!load.empty()){
    //Make sure the vector we are trying to load is legit
    Chunk _chunk;
    loadChunk(load.back(), _chunk);
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

bool World::loadChunk(glm::vec3 _c, Chunk &chunk){
  //Get the Working Directory
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  data_dir /= "save";
  data_dir /= saveFile;
  data_dir /= "world.region";

  //Load the actual worldfile, and the chunks relevant for the player.
  std::ifstream in(data_dir.string());

  //Skip the Lines
  int n = _c.x*worldSize*worldHeight+_c.y*worldSize+_c.z;
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
}

} // namespace serialization
} // namespace boost
