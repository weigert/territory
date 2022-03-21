/*
================================================================================
                            WORLD DATA MANAGEMENT
================================================================================
*/

#ifndef TERRITORY_WORLD
#define TERRITORY_WORLD

using namespace std;
using namespace glm;

unsigned int CHUNKSIZE = 16;                      //Chunk Side-Length (Power-of-2)
ivec3 WDIM = ivec3(16,16,16);                     //Spatial Information (Size in Chunks)
ivec3 RDIM = min(WDIM, ivec3(16,16,16));          //Chunks Per Region
ivec3 CDIM = ivec3(CHUNKSIZE);
unsigned int CVOL = (CDIM.x*CDIM.y*CDIM.z);
unsigned int RVOL = (RDIM.x*RDIM.y*RDIM.z);
unsigned int WVOL = (WDIM.x*WDIM.y*WDIM.z);

class World{
public:

  string name;
  int SEED = 0;                         //World Seed

  bool compressed = false;

  int AGE = 0;
  volatile int time = 0;  //Is set in a separate timed thread.
  chrono::milliseconds tickLength = chrono::milliseconds(100);

  voxel::Chunkpool chunkpool;
  vector<voxel::Chunk> chunks;                 //Loaded Chunks

  ivec3 minchunk = ivec3(-1);
  ivec3 maxchunk = ivec3(-1);

  Vertexpool<Vertex> vertexpool;
  std::unordered_set<int> groups;

  blueprint::Blueprint blueprint;
  blueprint::Blueprint remeshbuf;

  World(string _name, int _SEED){

    SEED = _SEED;
    name = _name;

    //Reserve Vertexpool Data
    vertexpool.reserve(scene::QUAD, scene::MAXBUCKET);
    vertexpool.index();

    //Reserve Chunkpool Data
    chunkpool.reserve(32*32*32*4);

    if(!init())
      logg::err("Couldn't Initialize World");
    logg::deb("Initialized World");

  }

  bool init();
  bool loadmeta();
  bool savemeta();

  bool lock = false;
  void buffer();
  void mesh();
  void mask();

  void blank();

  void compress();

  /*
    Location Queries
  */

  voxel::block get(ivec3 wpos){

    ivec3 cpos = wpos / CDIM;

    // Out of Loaded-Bounds Check

    if( cpos.x < minchunk.x || cpos.x > maxchunk.x ) return voxel::BLOCK_AIR;
    if( cpos.y < minchunk.y || cpos.y > maxchunk.y ) return voxel::BLOCK_AIR;
    if( cpos.z < minchunk.z || cpos.z > maxchunk.z ) return voxel::BLOCK_AIR;

    // Retrieve Value (Chunk-Index -> Block-Index)

    int cind = math::flatten(cpos - minchunk, maxchunk+ivec3(1)-minchunk);
    return chunks[cind].data[math::cflatten(wpos - cpos*CDIM, CDIM)];

  }

  ivec3 top(ivec2 pos){

    ivec3 wpos = ivec3(pos.x, (maxchunk.y+1)*CHUNKSIZE-1, pos.y);
    voxel::block type = get(wpos);

    while(type == voxel::BLOCK_AIR || type == voxel::BLOCK_LEAVES){
      wpos.y--;
      if(wpos.y < 0) return wpos + ivec3(0,1,0);
      type = get(wpos);
    }

    if(type == voxel::BLOCK_WOOD)
      return wpos + ivec3(0,1,0);

    return wpos + ivec3(0,1,0);

  }

  void set(ivec3 wpos, voxel::block type){

    ivec3 cpos = wpos / CDIM;

    // Out of Loaded-Bounds Check

    if( cpos.x < minchunk.x || cpos.x > maxchunk.x ) return;
    if( cpos.y < minchunk.y || cpos.y > maxchunk.y ) return;
    if( cpos.z < minchunk.z || cpos.z > maxchunk.z ) return;

    // Retrieve Value (Chunk-Index -> Block-Index)

    int cind = math::flatten(cpos - minchunk, maxchunk+ivec3(1)-minchunk);
    chunks[cind].data[math::cflatten(wpos - cpos*CDIM, CDIM)] = type;
    chunks[cind].remesh = true;

  }

};


bool World::init(){

  rootdir = fs::path(fs::current_path() / "save");
  if(!fs::exists(rootdir))
    fs::create_directory(rootdir);
  savedir = rootdir/name;

  //Load World
  if(fs::exists(savedir)){
    logg::deb("Found Savefile ", name);
    return loadmeta();
  }

  //Generate World
  logg::deb("Generating New World ", name);
  fs::create_directory(savedir);
  savemeta();

  //Generate the Blank Region Files
  logg::deb("Generating Blank Region Files");
  blank();

  //Optional: Run some terrain generation algorithm here...
  logg::deb("Finished Initializing World");

  return true;

}

bool World::loadmeta(){

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

bool World::savemeta(){

  ofstream out((savedir/"world.meta").string());
  if(!out.is_open()){
    logg::err("Failed to Save World Meta Data. No Saving.");
    return false;
  }

  cout<<"Generating Meta File..."<<endl;

  out<<"# territory world meta file"<<endl;
  out<<"WORLD "<<name<<endl;
  out<<"SEED "<<SEED<<endl;
  out<<"AGE "<<AGE<<endl;
  out<<"WDIM "<<WDIM.x<<" "<<WDIM.y<<" "<<WDIM.z<<endl;
  out<<"RDIM "<<RDIM.x<<" "<<RDIM.y<<" "<<RDIM.z<<endl;
  out<<"CDIM "<<CDIM.x<<" "<<CDIM.y<<" "<<CDIM.z<<endl;
  out<<"CODEC RAW"<<endl;

  out.close();
  return true;

}


void World::blank(){

  FILE* pFile = NULL;
  string oldsavefile = "";

  voxel::Chunk chunk;
  chunk.data = new voxel::block[CVOL]{voxel::BLOCK_AIR};

  ivec3 RPOS = chunk.pos/RDIM;
  string savefile = "world.region"+math::tostring(RPOS);

  pFile = fopen((savedir/savefile).string().c_str(), "w+b");
  if(pFile == NULL){
    cout<<"FAILURE"<<endl;
    logg::err("Failed to open region file "+savefile);
    return;
  }

  for(int i = 0; i < WDIM.x; i++)
  for(int j = 0; j < WDIM.y; j++)
  for(int k = 0; k < WDIM.z; k++){

    chunk.pos = ivec3(i, j, k);

    if(!all(equal(RPOS, chunk.pos/RDIM))){

      RPOS = chunk.pos/RDIM;
      savefile = "world.region"+math::tostring(RPOS);

      fclose(pFile);
      pFile = fopen((savedir/savefile).string().c_str(), "w+b");
      if(pFile == NULL){
        cout<<"FAILURE"<<endl;
        logg::err("Failed to open region file "+savefile);
        return;
      }

    }

    voxel::rle_num nrle = 1;
    voxel::rlee elem;
    elem.type = voxel::BLOCK_AIR;
    elem.length = CVOL;

    if( fwrite(&nrle, sizeof(voxel::rle_num), 1, pFile) < 1 )
      logg::err("Write Error");

    if( fwrite(&elem, sizeof(voxel::rlee), nrle, pFile) < nrle )
      logg::err("Write Error");

  }

  fclose(pFile);
  delete[] chunk.data;

}

void World::buffer(){

  logg::deb("Buffering Chunks...");

  lock = true;
  blueprint.write(name);

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

    chunkpool.remove(chunks[remove.top()].data);         //Return Chunk Pointer

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

      // Divide by Region Size

      if(a.x / RDIM.x > b.x / RDIM.x) return true;
      if(a.x / RDIM.x < b.x / RDIM.x) return false;

      if(a.y / RDIM.y > b.y / RDIM.y) return true;
      if(a.y / RDIM.y < b.y / RDIM.y) return false;

      if(a.z / RDIM.z > b.z / RDIM.z) return true;
      if(a.z / RDIM.z < b.z / RDIM.z) return false;

      if(a.x > b.x) return true;
      if(a.x < b.x) return false;

      if(a.y > b.y) return true;
      if(a.y < b.y) return false;

      if(a.z > b.z) return true;
      if(a.z < b.z) return false;

      return false;

    });

    voxel::rlee rdata[CVOL];

    while(!load.empty()){

      ivec3 rpos = load.back()/RDIM;
      string savefile = "world.region"+math::tostring(rpos);

      FILE* inFile = fopen((savedir/savefile).string().c_str(), "rb");
      if(inFile == NULL){
        logg::err("Failed to open region file "+savefile);
        load.pop_back();
        continue;
      }

      // Current Chunk Index, Next Chunk Index (In Region)

      size_t n;
      int cind = 0;
      int nind = math::flatten(mod((vec3)load.back(), (vec3)RDIM), RDIM);

      // Seek-Operation

      while(cind < nind){
        if( fread(&n, sizeof(voxel::rle_num), 1, inFile) < 1 ) break;
        fseek(inFile, n*sizeof(voxel::rlee), SEEK_CUR);
        cind++;
      }

      while(rpos == load.back()/RDIM){

        chunks.emplace_back(load.back(), chunkpool.get());

        nind = math::flatten(mod((vec3)chunks.back().pos, (vec3)RDIM), RDIM);

        while(cind < nind){
          if( fread(&n, sizeof(voxel::rle_num), 1, inFile) < 1 ) break;
          fseek(inFile, n*sizeof(voxel::rlee), SEEK_CUR);
          cind++;
        }

        if( fread(&n, sizeof(voxel::rle_num), 1, inFile) < 1 ) break;
        if( fread(&rdata[0], sizeof(voxel::rlee), n, inFile) < n) break;
        cind++;

        // Convert the Chunk Data!

        voxel::uncompress(&rdata[0], n, chunks.back().data);

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

      if(!chunk.remesh) continue;

      if(!chunk.firstmesh)
        for(int d = 0; d < 6; d++)
          vertexpool.unsection(chunk.faces[d]);

      chunk.section(&vertexpool);

    }

    std::vector<thread> threads;
    scene::SCALE = scene::LOD;

    for(auto& chunk: chunks){

      if(!chunk.remesh) continue;
      threads.emplace_back(voxel::mesh, &chunk, &vertexpool);
      if(threads.size() >= voxel::NMESHTHREADS){
        for(auto& t: threads)
          t.join();
        threads.clear();
      }

      chunk.remesh = false;
      chunk.firstmesh = false;

      /*
      if(!chunk.remesh) continue;
      scene::SCALE = scene::LOD;
      voxel::mesh(&chunk, &vertexpool);
      chunk.remesh = false;
      chunk.firstmesh = false;
      */

    }

    for(auto& t: threads)
      t.join();

  }

  else {

    // Unsection all chunks...
    /*

    for(auto& chunk: chunks){

      if(!chunk.remesh) continue;

      if(!chunk.firstmesh)
        for(int d = 0; d < 6; d++)
          vertexpool.unsection(chunk.faces[d]);

      chunk.section(&vertexpool);

    }
    */

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

      for(int ox = 0; ox < scene::LOD; ox++)    //Chunk Offset
      for(int oy = 0; oy < scene::LOD; oy++)
      for(int oz = 0; oz < scene::LOD; oz++){

        voxel::Chunk* cur = &chunks[math::flatten(ivec3(i, j, k) + ivec3(ox, oy, oz), effchunk+1)];



        int typescount[17]{0};


        for(int kx = 0; kx < scene::LOD; kx++)    //Offset
        for(int ky = 0; ky < scene::LOD; ky++)
        for(int kz = 0; kz < scene::LOD; kz++)
          typescount[cur->data[math::cflatten(ivec3(x,y,z)*scene::LOD+ivec3(kx,ky,kz), ivec3(CH))]]++;

        voxel::block type = voxel::BLOCK_AIR;
        int max = 0;
        for(int m = 1; m < 17; m++){
          if(typescount[m] > max){
              type = (voxel::block)m;
              max = typescount[m];
          }
        }

        /*




        voxel::block type = cur->data[math::cflatten(ivec3(x+1,y+1,z+1)*scene::LOD-ivec3(1), ivec3(CH))];
        if(type == voxel::BLOCK_AIR)
          type = cur->data[math::cflatten(ivec3(x,y,z)*scene::LOD, ivec3(CH))];

          */

        tempchunk[math::cflatten(ivec3(x,y,z) + ivec3(ox,oy,oz)*CHLOD, ivec3(CH))] = type;

        cur->remesh = false;

      }

      voxel::Chunk newchunk(basechunk->pos, tempchunk);

      scene::SCALE = scene::LOD;
      newchunk.section(&vertexpool);
      voxel::mesh(&newchunk, &vertexpool);

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

#endif
