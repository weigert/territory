/*
================================================================================
                            WORLD DATA MANAGEMENT
================================================================================
*/

#ifndef TERRITORY_WORLD
#define TERRITORY_WORLD

using namespace std;
using namespace glm;

class World{
public:

World(string savefile, int _SEED){

  SEED = _SEED;
  savename = savefile;

  RDIM = min(WDIM, RDIM); //Edge Case
  blueprint::RDIM = RDIM;

  if(!init(savefile))
    logg::err("Couldn't Initialize World");
  logg::deb("Initialized World");

  RDIM = min(WDIM, RDIM); //Edge Case
  blueprint::RDIM = RDIM;

  vertexpool.reserve(scene::QUAD, scene::MAXBUCKET);
  vertexpool.index();

  //Reserve Chunkpool Data

  const size_t Nchunks = 32*32*32*4;

  chunkpool = new voxel::block[CVOL*Nchunks]{voxel::BLOCK_AIR}; //This needs deleting
  for(size_t i = 0; i < Nchunks; i++)
    free.push_front(chunkpool+i*CVOL);

}

~World(){
  delete[] chunkpool;
}

int SEED = 0;                         //World Seed
int AGE = 0;
string savename;

static ivec3 WDIM;        //Spatial Information (Size in Chunks)
static ivec3 RDIM;         //Chunks Per Region
static ivec3 CDIM;
static unsigned int CHUNKSIZE;
static unsigned int CVOL;
static unsigned int RVOL;
static unsigned int WVOL;

ivec3 minchunk = ivec3(-1);
ivec3 maxchunk = ivec3(-1);


std::unordered_set<int> groups;


voxel::block* chunkpool;
deque<voxel::block*> free;



voxel::block* getChunkPointer(){
  if(free.empty()) std::cout<<"CAN'T ALLOCATE CHUNK: OUT OF MEMORY"<<std::endl;
  voxel::block* freechunk = free.back();
  free.pop_back();
  return freechunk;
}

void returnChunkPoiner(voxel::block* freechunk){
  free.push_front(freechunk);
}





blueprint::Blueprint blueprint;
blueprint::Blueprint remeshbuf;
vector<voxel::Chunk> chunks;                 //Loaded Chunks
Vertexpool<Vertex> vertexpool;




int sealevel = 16;

void mesh();
void mask();

volatile int time = 0;  //Is set in a separate timed thread.
chrono::milliseconds tickLength = chrono::milliseconds(100);

bool init(std::string savefile);

void blank();

bool lock = false;
void buffer();

bool loadmeta(fs::path);
bool savemeta(fs::path, string);

};

unsigned int World::CHUNKSIZE = 16;
ivec3 World::WDIM = glm::ivec3(16,16,16);        //Spatial Information (Size in Chunks)
ivec3 World::RDIM = glm::ivec3(16,16,16);         //Chunks Per Region
ivec3 World::CDIM = glm::ivec3(World::CHUNKSIZE);
unsigned int World::CVOL = (World::CDIM.x*World::CDIM.y*World::CDIM.z);
unsigned int World::RVOL = (World::RDIM.x*World::RDIM.y*World::RDIM.z);
unsigned int World::WVOL = (World::WDIM.x*World::WDIM.y*World::WDIM.z);

#endif


using namespace std;

bool World::loadmeta(fs::path savedir){

  ifstream in((savedir/"world.meta").string());
  if(!in.is_open()){
    logg::err("World File Corrupted: No Meta File");
    return false;
  }

  //Load the Meta File!

  string pstring;

  while(!in.eof()){

    getline(in, pstring);
    if(in.eof())
      break;

    if(pstring[0] == '#')	//Skip Commented Lines
      continue;

    char t[32];  //Hard Limit on Total Length
    int m = sscanf(pstring.c_str(), "%s", &t[0]);
    if(m != 1){
      cout<<"Can't parse meta file: "<<pstring<<endl;
      break;
    }

    string key = t;
    pstring = pstring.substr(key.size()+1);

    if(key == "SEED")
      SEED = std::stoi(pstring);

    if(key == "AGE")
      AGE = std::stoi(pstring);

    if(key == "WDIM"){
      sscanf(pstring.c_str(), "%i %i %i", &WDIM[0], &WDIM[1], &WDIM[2]);
      WVOL = WDIM.x*WDIM.y*WDIM.z;
    }

    if(key == "RDIM"){
      sscanf(pstring.c_str(), "%i %i %i", &RDIM[0], &RDIM[1], &RDIM[2]);
      RVOL = RDIM.x*RDIM.y*RDIM.z;
    }

    if(key == "CDIM"){
      sscanf(pstring.c_str(), "%i %i %i", &CDIM[0], &CDIM[1], &CDIM[2]);
      CHUNKSIZE = CDIM.x;
      CVOL = CDIM.x*CDIM.y*CDIM.z;
    }

  }

  blueprint::CHUNKSIZE = CHUNKSIZE;
  blueprint::WDIM = WDIM;
  blueprint::RDIM = RDIM;
  blueprint::CDIM = CDIM;

  in.close();
  return true;

}

bool World::savemeta(fs::path savedir, string savename){

  ofstream out((savedir/"world.meta").string());
  if(!out.is_open()){
    logg::err("Failed to Save World Meta Data. No Saving.");
    return false;
  }

  cout<<"Generating Meta File..."<<endl;

  out<<"# territory world meta file"<<endl;
  out<<"WORLD "<<savename<<endl;
  out<<"SEED "<<SEED<<endl;
  out<<"AGE "<<AGE<<endl;
  out<<"WDIM "<<WDIM.x<<" "<<WDIM.y<<" "<<WDIM.z<<endl;
  out<<"RDIM "<<RDIM.x<<" "<<RDIM.y<<" "<<RDIM.z<<endl;
  out<<"CDIM "<<CDIM.x<<" "<<CDIM.y<<" "<<CDIM.z<<endl;
  out<<"CODEC RAW"<<endl;

  out.close();
  return true;

}

bool World::init(std::string savefile){

  rootdir = fs::path(fs::current_path() / "save");
  if(!fs::exists(rootdir)){
    fs::create_directory(rootdir);
  }
  savedir = rootdir/savefile;

  //Load World

  if(fs::exists(savedir)){
    logg::deb("Found Savefile ", savefile);
    return loadmeta(savedir);
  }

  //Generate World

  logg::deb("Generating New World ", savefile);
  fs::create_directory(savedir);
  savemeta(savedir, savefile);

  //Generate the Blank Region Files
  logg::deb("Generating Blank Region Files");
  blank();
  logg::deb("Finished Initializing World");

  return true;

}

void World::blank(){

  FILE* pFile = NULL;
  string oldsavefile = "";

  voxel::Chunk chunk;

  for(int i = 0; i < WDIM.x; i++){
    for(int j = 0; j < WDIM.y; j++){
      for(int k = 0; k < WDIM.z; k++){

        chunk.pos = ivec3(i, j, k);

        string savefile = "world.region"+math::tostring(chunk.pos/RDIM);
        if(savefile != oldsavefile){
          oldsavefile = savefile;
          if(pFile != NULL){
             fclose(pFile);
             pFile = NULL;
          }
          pFile = fopen((savedir/savefile).string().c_str(), "a+b");
          if(pFile == NULL){
            logg::err("Failed to open region file "+savefile);
            return;
          }
        }
        fwrite(chunk.data, sizeof(unsigned char), CVOL, pFile);

      }
    }
  }

  fclose(pFile);

}

void World::buffer(){

  logg::deb("Buffering Chunks...");

  lock = true;
  blueprint.write(savename);

  stack<int> remove;                      //Chunks to Remove
  vector<ivec3> load;                     //Chunks to Load

  //Get the LOD-Friendly Camera Position
  ivec3 look = (ivec3)cam::look/ivec3(CHUNKSIZE);
  look -= mod((vec3)look, vec3(scene::LOD));

  ivec3 min = look-(ivec3)scene::renderdist;  //Extent of Loaded Region
  ivec3 max = look+(ivec3)scene::renderdist;

  //Correct Upper Bound due to LOD
  max += scene::LOD - 1.0f - mod((vec3)max, vec3(scene::LOD));

  for(size_t i = 0; i < chunks.size(); i++)
  if(any(lessThan(chunks[i].pos, min)) || any(greaterThan(chunks[i].pos, max)))
    remove.push(i);

  min = clamp(min, ivec3(0), WDIM-ivec3(1-1));
  max = clamp(max, ivec3(0-1), WDIM-ivec3(1));

  for(int i = min.x; i <= max.x; i++)    //All Indices in Region
  for(int j = min.y; j <= max.y; j++)
  for(int k = min.z; k <= max.z; k++){
    ivec3 c = ivec3(i,j,k);
    if(any(greaterThan(c, maxchunk)) || any(lessThan(c, minchunk)))
      load.push_back(c);
  }

  minchunk = min;
  maxchunk = max;








  logg::deb("Unloading ", remove.size(), " chunks");

  while(!remove.empty()){                               //Delete Chunks

    free.push_front(chunks[remove.top()].data);         //Return Chunk Pointer

    if(!chunks[remove.top()].firstmesh)
    for(int d = 0; d < 6; d++)
      vertexpool.unsection(chunks[remove.top()].faces[d]);

    swap(chunks[remove.top()], chunks.back());
    chunks.pop_back();

    remove.pop();

  }






  if(!load.empty()){                                    //Load New Chunks

    logg::deb("Loading ", load.size(), " chunks");
    float loadsize = load.size();

    std::cout<<"Chunk Loading ";
    float loadtime = timer::benchmark<std::chrono::microseconds>([&](){

    sort(load.begin(), load.end(),                      //Sort Load-Order
    [](const ivec3& a, const ivec3& b) {

      if(a.x >> RDIM.x > b.x >> RDIM.x) return true;    //Sort by Region (Divide by 16)
      if(a.x >> RDIM.x < b.x >> RDIM.x) return false;

      if(a.y >> RDIM.y > b.y >> RDIM.y) return true;
      if(a.y >> RDIM.y < b.y >> RDIM.y) return false;

      if(a.z >> RDIM.z > b.z >> RDIM.z) return true;
      if(a.z >> RDIM.z < b.z >> RDIM.z) return false;

      if(a.x > b.x) return true;
      if(a.x < b.x) return false;

      if(a.y > b.y) return true;
      if(a.y < b.y) return false;

      if(a.z > b.z) return true;
      if(a.z < b.z) return false;

      return false;
    });

    while(!load.empty()){

      ivec3 rpos = load.back()/RDIM;
      string savefile = "world.region"+math::tostring(rpos);

      FILE* inFile = fopen((savedir/savefile).string().c_str(), "rb");
      if(inFile == NULL){
        logg::err("Failed to open region file "+savefile);
        load.pop_back();
        continue;
      }

      int oldn = math::flatten(mod((vec3)load.back(), (vec3)RDIM), RDIM);
      int newn;
      fseek(inFile, CVOL*oldn, SEEK_SET);

      while(rpos == load.back()/RDIM){

        chunks.emplace_back(load.back(), getChunkPointer());

        newn = math::flatten(mod((vec3)chunks.back().pos, (vec3)RDIM), RDIM);

        if(newn > oldn) fseek(inFile, CVOL*(newn-oldn-1), SEEK_CUR);
        if(fread(chunks.back().data, sizeof(unsigned char), CVOL, inFile) < CVOL)
          logg::err("Read Fail");
        oldn = newn;

        load.pop_back();

      }

      fclose(inFile);

    }

    });

    std::cout<<"Load-Time Per Chunk: "<<loadtime/loadsize<<std::endl;

  }

  lock = false;

}

/*
================================================================================
                                Mesh Handling
================================================================================
*/

void World::mesh(){

  timer::benchmark<std::chrono::microseconds>([&](){

  sort(chunks.begin(), chunks.end(), less<voxel::Chunk>());

  //What I create chunks which represent merged chunks?

  logg::deb("Meshing Chunks...");

  if(scene::LOD == 1){

    for(auto& chunk: chunks){

      if(chunk.remesh){

        if(!chunk.firstmesh)
          for(int d = 0; d < 6; d++)
            vertexpool.unsection(chunk.faces[d]);

        voxel::mesh(&chunk, &vertexpool);
        chunk.remesh = false;
        chunk.firstmesh = false;

      }
    }

  }

  else {

    ivec3 effchunk = maxchunk-minchunk;
    ivec3 base = mod((vec3)minchunk, vec3(scene::LOD));

    const int CH = CHUNKSIZE;
    const int CHLOD = CHUNKSIZE/scene::LOD;

    voxel::block* tempchunk = new voxel::block[CVOL]{voxel::BLOCK_AIR};

    for(int i = base.x; i < effchunk.x; i += scene::LOD)
    for(int j = base.y; j < effchunk.y; j += scene::LOD)
    for(int k = base.z; k < effchunk.z; k += scene::LOD){

      //Get the Base Chunk
      voxel::Chunk* basechunk = &chunks[math::flatten(ivec3(i, j, k), effchunk+1)];
      if(!basechunk->remesh) continue;

      //Check if it already has a mesh -> unsection
      if(!basechunk->firstmesh)
      for(int d = 0; d < 6; d++)
        vertexpool.unsection(basechunk->faces[d]);

      //Merge the Data of these Chunks into a Super Chunk
      for(int x = 0; x < CHLOD; x++)
      for(int y = 0; y < CHLOD; y++)
      for(int z = 0; z < CHLOD; z++)

      for(int ox = 0; ox < scene::LOD; ox++)    //Offset
      for(int oy = 0; oy < scene::LOD; oy++)
      for(int oz = 0; oz < scene::LOD; oz++){

        voxel::Chunk* cur = &chunks[math::flatten(ivec3(i, j, k) + ivec3(ox, oy, oz), effchunk+1)];
        tempchunk[(x+ox*CHLOD)*CH*CH+(y+oy*CHLOD)*CH+(z+oz*CHLOD)] = cur->data[scene::LOD*x*CH*CH+scene::LOD*y*CH+scene::LOD*z];
        cur->remesh = false;

      }

      voxel::Chunk newchunk(basechunk->pos, tempchunk);

      scene::SCALE = scene::LOD;
      int tmplod = scene::LOD;
      scene::LOD = 1;
      voxel::mesh(&newchunk, &vertexpool);
      scene::LOD = tmplod;
      scene::SCALE = 1;

      basechunk->faces = newchunk.faces;
      basechunk->firstmesh = false;

    }

    delete[] tempchunk;

  }

  mask();

});


}

void World::mask(){
  groups.clear();
  if(cam::pos.x < 0) groups.insert(0);
  if(cam::pos.x > 0) groups.insert(1);
  if(cam::pos.y < 0) groups.insert(2);
  if(cam::pos.y > 0) groups.insert(3);
  if(cam::pos.z < 0) groups.insert(4);
  if(cam::pos.z > 0) groups.insert(5);
  vertexpool.mask([&](DAIC& cmd){
    return groups.contains(cmd.group);
  });

  vertexpool.order([&](const DAIC& a, const DAIC& b){
    if(dot(b.pos - a.pos, cam::pos) < 0) return true;
    if(dot(b.pos - a.pos, cam::pos) > 0) return false;
    return (a.baseVert < b.baseVert);
  });

  vertexpool.update();
}
