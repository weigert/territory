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
  Terrain terrain(SEED, ivec3(512, 120, 512));
  terrain.rescale = vec3(4,4,4);
  terrain.generate(4500);

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

  vec3 min = floor(cam::look/vec3(chunkSize))-scene::renderdist;  //Extent of Loaded Region
  vec3 max = floor(cam::look/vec3(chunkSize))+scene::renderdist;

  /*
      Note: Sorting makes this much faster and more consistent!
  */

/*
  //Sorting Indices
  vector<int> ind(chunks.size());
  for(size_t i = 0; i < chunks.size(); i++)
    ind[i] = i;

  //Sort Indices using Chunk-Order
  sort(ind.begin(), ind.end(),
  [&](const int& a, const int& b) {
    return (chunks[a] > chunks[b]);
  });

  //Sort Chunks and Models
  sort(chunks.begin(), chunks.end(), greater<Chunk>());
  vector<Model*> modelperm;
  for(size_t i = 0; i < models.size(); i++)
    modelperm.push_back(models[ind[i]]);
  swap(models, modelperm);
  */

  //Potential Improvement:
  //  More precise iteration on min, max, minchunk, maxchunk


  //Potential Improvement:
  //  Remove only chunks on the remove slice with a precise iteration!
  //  The chunks are sorted by their position so this works

/*

  for(size_t i = 0; i < chunks.size(); i++)
  if(any(lessThan((vec3)chunks[i].pos, min)) || any(greaterThan((vec3)chunks[i].pos, max)))
    remove.push(i);

  min = clamp(min, vec3(0), dim-vec3(1-1));
  max = clamp(max, vec3(0-1), dim-vec3(1));

  for(int i = min.x; i < max.x+1; i++)    //All Indices in Region
  for(int j = min.y; j < max.y+1; j++)
  for(int k = min.z; k < max.z+1; k++){
    vec3 c = vec3(i,j,k);
    if(any(greaterThan(c, maxchunk)) || any(lessThan(c, minchunk)))
      load.push_back(c);
  }

  minchunk = min;
  maxchunk = max;

*/

  //Only Load / Unload in Multiples of LOD
  min -= mod(min, vec3(scene::LOD));
  max += ((float)scene::LOD-mod(max, vec3(scene::LOD)));

  min = clamp(min, vec3(0), dim-vec3(1-1));
  max = clamp(max, vec3(0-1), dim-vec3(1));

  for(size_t i = 0; i < chunks.size(); i++)
  if(any(lessThan((vec3)chunks[i].pos, min)) || any(greaterThan((vec3)chunks[i].pos, max)))
    remove.push(i);


  for(int i = min.x; i < max.x+1; i++)    //All Indices in Region
  for(int j = min.y; j < max.y+1; j++)
  for(int k = min.z; k < max.z+1; k++){
    vec3 c = vec3(i,j,k);
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



  //This is where we should remove chunks that don't deserve loading

  logg::deb("Loading ", load.size(), " chunks");

  std::cout<<"Chunk Loading ";
  timer::benchmark<std::chrono::microseconds>([&](){

  if(!load.empty()){                                    //Load New Chunks

    sort(load.begin(), load.end(),                      //Sort Load-Order
    [](const glm::vec3& a, const glm::vec3& b) {

      //Sort Load by Region too!
      if((int)a.x/16 > (int)b.x/16) return true;
      if((int)a.x/16 < (int)b.x/16) return false;

      if((int)a.y/16 > (int)b.y/16) return true;
      if((int)a.y/16 < (int)b.y/16) return false;

      if((int)a.z/16 > (int)b.z/16) return true;
      if((int)a.z/16 < (int)b.z/16) return false;

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
      fseek(inFile, CHUNKVOL*oldn, SEEK_SET);

      while(rpos == load.back()/(ivec3)RDIM){

        chunks.emplace_back(load.back(), getChunkPointer());

        int newn = helper::getIndex(mod((vec3)chunks.back().pos, dimr), dimr);

        if(newn > oldn) fseek(inFile, CHUNKVOL*(newn-oldn-1), SEEK_CUR);
        if(fread(chunks.back().data, sizeof(unsigned char), CHUNKVOL, inFile) < CHUNKVOL)
          logg::err("Read Fail");
        oldn = newn;

        load.pop_back();

      }

      fclose(inFile);

    }
  }

  });

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
        //chunkmesh::occlusionpool(&chunk, &vertexpool);
        chunk.remesh = false;
        chunk.firstmesh = false;

      }
    }

    });

  }

  else if(scene::LOD == 2){

    vec3 effchunk = maxchunk-minchunk;
    vec3 base = glm::mod(minchunk, vec3(2));

    for(size_t i = base.x; i < effchunk.x; i += 2){
      for(size_t j = base.y; j < effchunk.y; j += 2){
        for(size_t k = base.z; k < effchunk.z; k += 2){

          //Identify the 8 chunks we need
          Chunk* basechunk = &chunks[helper::getIndex(vec3(i, j, k), effchunk+1.0f)];

          if(!basechunk->remesh) continue;

          if(!basechunk->firstmesh){
            for(int d = 0; d < 6; d++)
              vertexpool.unsection(basechunk->faces[d]);
          }

          const int CH = CHUNKSIZE;
          const int CHLOD = CHUNKSIZE/2;

          //Merge the Data of these Chunks into a Super Chunk
          for(size_t x = 0; x < CHLOD; x++)
          for(size_t y = 0; y < CHLOD; y++)
          for(size_t z = 0; z < CHLOD; z++){

            //Offset Stuff

            for(size_t ox = 0; ox < 2; ox++)
            for(size_t oy = 0; oy < 2; oy++)
            for(size_t oz = 0; oz < 2; oz++){

              Chunk* cur = &chunks[helper::getIndex(vec3(i, j, k)+vec3(ox, oy, oz), effchunk+1.0f)];
              tempchunk[(x+ox*CHLOD)*CH*CH+(y+oy*CHLOD)*CH+(z+oz*CHLOD)] = cur->data[2*x*CH*CH+2*y*CH+2*z];
              cur->remesh = false;

            }

          }

          Chunk newchunk(basechunk->pos, tempchunk);

          scene::LOD = 1;
          scene::SCALE = 2;
          chunkmesh::greedypool(&newchunk, &vertexpool);
          scene::LOD = 2;
          scene::SCALE = 1;

          basechunk->faces = newchunk.faces;
          basechunk->firstmesh = false;

        }
      }
    }


  }

  else if(scene::LOD == 4){

    vec3 effchunk = maxchunk-minchunk;
    vec3 base = glm::mod(minchunk, vec3(4));

    for(size_t i = base.x; i < effchunk.x-3; i += 4){    //Don't iterate too far, but also increase by a factor of two here
      for(size_t j = base.y; j < effchunk.y-3; j += 4){
        for(size_t k = base.z; k < effchunk.z-3; k += 4){

          //Identify the 8 chunks we need
          Chunk* basechunk = &chunks[helper::getIndex(vec3(i, j, k), effchunk+1.0f)];

          if(!basechunk->remesh) continue;

          const int CH = CHUNKSIZE;
          const int CHLOD = CHUNKSIZE/4;

          //Merge the Data of these Chunks into a Super Chunk
          for(size_t x = 0; x < CHLOD; x++)
          for(size_t y = 0; y < CHLOD; y++)
          for(size_t z = 0; z < CHLOD; z++){

            //Offset Stuff

            for(size_t ox = 0; ox < 4; ox++)
            for(size_t oy = 0; oy < 4; oy++)
            for(size_t oz = 0; oz < 4; oz++){

              Chunk* cur = &chunks[helper::getIndex(vec3(i, j, k)+vec3(ox, oy, oz), effchunk+1.0f)];
              tempchunk[(x+ox*CHLOD)*CH*CH+(y+oy*CHLOD)*CH+(z+oz*CHLOD)] = cur->data[4*x*CH*CH+4*y*CH+4*z];
              cur->remesh = false;

            }

          }

          Chunk newchunk(basechunk->pos, tempchunk);

          if(!basechunk->firstmesh){
            for(int d = 0; d < 6; d++)
              vertexpool.unsection(basechunk->faces[d]);
          }

          scene::LOD = 1;
          scene::SCALE = 4;
          chunkmesh::greedypool(&newchunk, &vertexpool);
          scene::LOD = 4;
          scene::SCALE = 1;

          basechunk->faces = newchunk.faces;
          basechunk->firstmesh = false;

        }
      }
    }





  }


  vertexpool.update();


}
