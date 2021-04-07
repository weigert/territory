/*
================================================================================
                            WORLD DATA MANAGEMENT
================================================================================
*/

#include "world.h"

bool World::init(std::string savefile){

  rootdir = fs::path(fs::current_path() / "save");
  if(!fs::exists(rootdir)){
    fs::create_directory(rootdir);
  }
  savedir = rootdir/savefile;

  //Load World

  if(fs::exists(savedir)){

    logg::deb("Found Savefile ", savefile);

    ifstream in((savedir/"world.meta").string());
    if(!in.is_open()){
      logg::err("World File Corrupted: No Meta File");
    } else {
      boost::archive::text_iarchive ia(in);
      ia >> *this;
      in.close();
    }

    return true;
  }

  //Generate World

  logg::deb("Generating New World ", savefile);

  fs::create_directory(savedir);
  ofstream out((savedir/"world.meta").string());
  if(!out.is_open()){
    logg:: err("Failed to Save World Meta Data. No Saving.");
  }
  else {
    boost::archive::text_oarchive oa(out);
    oa << *this;
  }

  out.close();

  //Set the Blueprint Properties
  blueprint.dim = dim;
  blueprint.chunkSize = chunkSize;

  //Generate the Blank Region Files
  logg::deb("Generating Blank Region Files");
  blank();

  logg::deb("Generating Terrain");

  blueprint::world = this;
  blueprint::eval = &World::evaluateBlueprint;

  //Hydrological Terrain
  Terrain terrain(SEED, ivec3(512, 160, 512));
  terrain.rescale = vec3(4,4,4);
  terrain.generate(1500);

  //Perlin Terrain
//  Terrain terrain(SEED, dim*(float)chunkSize);
//  terrain.perlin(SEED, dim*(float)chunkSize, &World::evaluateBlueprint, this);

  //Flat Terrain
  //evaluateBlueprint(blueprint::flat(dim.x*chunkSize, dim.z*chunkSize));

  logg::deb("Finished Initializing World");

  return true;

}


string World::regionString(vec3 cpos){

  vec3 rpos = floor(cpos/dimr);
  stringstream ss;
  ss << rpos.x << rpos.y << rpos.z;
  return ss.str();

}

void World::blank(){

  FILE* pFile = NULL;
  string oldsavefile = "";

  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      for(int k = 0; k < dim.z; k++){

        Chunk chunk(vec3(i, j, k), tempchunk);
        string savefile = "world.region"+regionString(chunk.pos);
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
        fwrite(chunk.data, sizeof(unsigned char), CHUNKVOL, pFile);

      }
    }
  }

  fclose(pFile);

}

bool World::evaluateBlueprint(Blueprint* print){
  bool success = evaluateBlueprint(*print);
  delete print;
  return success;
}

bool World::evaluateBlueprint(Blueprint& print){

  logg::deb("Evaluating Blueprint [", print.edits.size(), "]");

  if(print.edits.empty())
    return false;

  sort(print.edits.begin(), print.edits.end(), greater<bufferObject>());

  Chunk chunk;
  chunk.data = tempchunk;

  while(!print.edits.empty()){

    ivec3 rpos = print.edits.back().rpos;
    string savefile = "world.region"+regionString(print.edits.back().cpos);;

    FILE* inFile = fopen((savedir/savefile).string().c_str(), "rb");
    if(inFile == NULL){
      logg::err("Failed to open region file "+savefile);
      return false;
    }
    FILE* outFile = fopen((savedir/(savefile+".temp")).string().c_str(), "wb");
    if(inFile == NULL){
      logg::err("Failed to open region file "+savefile);
      return false;
    }

    int n = 0;
    int N = dimr.x*dimr.y*dimr.z;

    while(n < N){  //Region Size

      if(fread(chunk.data, sizeof(BlockType), CHUNKVOL, inFile) < CHUNKVOL)
        logg::err("Read Error");

      chunk.pos = helper::getPos(n, dimr) + (vec3)print.edits.back().rpos*dimr;

      while(!print.edits.empty() && all(equal(chunk.pos, print.edits.back().cpos)) && all(equal(rpos, print.edits.back().rpos))){
        chunk.setPosition(mod((vec3)print.edits.back().pos, vec3(chunkSize)), print.edits.back().type);
        print.edits.pop_back();
      }

      if(fwrite(chunk.data, sizeof(BlockType), CHUNKVOL, outFile) < CHUNKVOL)
        logg::err("Write Error");

      n++;

    }

    fclose(inFile);
    fclose(outFile);

    fs::remove_all(savedir/savefile);
    fs::rename((savedir/(savefile+".temp")), savedir/savefile);

  }

  logg::deb("Success");

  return true;
}

void World::buffer(){

  logg::deb("Buffering Chunks...");

  lock = true;
  evaluateBlueprint(blueprint);



  stack<int> remove;                      //Chunks to Remove
  vector<ivec3> load;                     //Chunks to Load

  //Get the LOD-Friendly Camera Position
  ivec3 look = (ivec3)cam::look/ivec3(chunkSize);
  look -= mod((vec3)look, vec3(scene::LOD));

  ivec3 min = look-(ivec3)scene::renderdist;  //Extent of Loaded Region
  ivec3 max = look+(ivec3)scene::renderdist;

  //Correct Upper Bound due to LOD
  max += scene::LOD - 1.0f - mod((vec3)max, vec3(scene::LOD));

  for(size_t i = 0; i < chunks.size(); i++)
  if(any(lessThan(chunks[i].pos, min)) || any(greaterThan(chunks[i].pos, max)))
    remove.push(i);

  min = clamp(min, ivec3(0), (ivec3)dim-ivec3(1-1));
  max = clamp(max, ivec3(0-1), (ivec3)dim-ivec3(1));

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

      if(a.x >> 4 > b.x >> 4) return true;    //Sort by Region (Divide by 16)
      if(a.x >> 4 < b.x >> 4) return false;

      if(a.y >> 4 > b.y >> 4) return true;
      if(a.y >> 4 < b.y >> 4) return false;

      if(a.z >> 4 > b.z >> 4) return true;
      if(a.z >> 4 < b.z >> 4) return false;

      if(a.x > b.x) return true;
      if(a.x < b.x) return false;

      if(a.y > b.y) return true;
      if(a.y < b.y) return false;

      if(a.z > b.z) return true;
      if(a.z < b.z) return false;

      return false;
    });

    while(!load.empty()){

      ivec3 rpos = load.back()/(ivec3)RDIM;
      string savefile = "world.region"+regionString(load.back());

      FILE* inFile = fopen((savedir/savefile).string().c_str(), "rb");
      if(inFile == NULL){
        logg::err("Failed to open region file "+savefile);
        load.pop_back();
        continue;
      }

      int oldn = helper::getIndex(mod((vec3)load.back(), dimr), dimr);
      int newn;
      fseek(inFile, CHUNKVOL*oldn, SEEK_SET);

      while(rpos == load.back()/(ivec3)RDIM){

        chunks.emplace_back(load.back(), getChunkPointer());

        newn = helper::getIndex(mod((vec3)chunks.back().pos, dimr), dimr);

        if(newn > oldn) fseek(inFile, CHUNKVOL*(newn-oldn-1), SEEK_CUR);
        if(fread(chunks.back().data, sizeof(unsigned char), CHUNKVOL, inFile) < CHUNKVOL)
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

  sort(chunks.begin(), chunks.end(), less<Chunk>());

  //What I create chunks which represent merged chunks?

  logg::deb("Meshing Chunks...");

  if(scene::LOD == 1){

    std::cout<<"Chunk Meshing ";
    timer::benchmark<std::chrono::microseconds>([&](){

    for(auto& chunk: chunks){

      if(chunk.remesh){

        if(!chunk.firstmesh)
          for(int d = 0; d < 6; d++)
            vertexpool.unsection(chunk.faces[d]);

        chunkmesh::greedypool(&chunk, &vertexpool);
        chunk.remesh = false;
        chunk.firstmesh = false;

      }
    }

    });

  }



  else {

    ivec3 effchunk = maxchunk-minchunk;
    ivec3 base = mod((vec3)minchunk, vec3(scene::LOD));

    const int CH = CHUNKSIZE;
    const int CHLOD = CHUNKSIZE/scene::LOD;

    for(int i = base.x; i < effchunk.x; i += scene::LOD)
    for(int j = base.y; j < effchunk.y; j += scene::LOD)
    for(int k = base.z; k < effchunk.z; k += scene::LOD){

      //Get the Base Chunk
      Chunk* basechunk = &chunks[helper::getIndex(vec3(i, j, k), effchunk+1)];
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

        Chunk* cur = &chunks[helper::getIndex(vec3(i, j, k) + vec3(ox, oy, oz), effchunk+1)];
        tempchunk[(x+ox*CHLOD)*CH*CH+(y+oy*CHLOD)*CH+(z+oz*CHLOD)] = cur->data[scene::LOD*x*CH*CH+scene::LOD*y*CH+scene::LOD*z];
        cur->remesh = false;

      }

      Chunk newchunk(basechunk->pos, tempchunk);

      scene::SCALE = scene::LOD;
      int tmplod = scene::LOD;
      scene::LOD = 1;
      chunkmesh::greedypool(&newchunk, &vertexpool);
      scene::LOD = tmplod;
      scene::SCALE = 1;

      basechunk->faces = newchunk.faces;
      basechunk->firstmesh = false;

    }

  }

  mask();

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
