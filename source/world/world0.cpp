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
  Terrain terrain(SEED, ivec3(256, 80, 256));
  terrain.rescale = vec3(4,3,4);
  terrain.generate();

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

        Chunk chunk(vec3(i, j, k), BIOME_VOID);
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

  while(!print.edits.empty()){

    string savefile = "world.region"+regionString(print.edits.back().cpos);

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

      if(fread(chunk.data, sizeof(unsigned char), CHUNKVOL, inFile) < CHUNKVOL)
        logg::err("Read Error");

      chunk.pos = helper::getPos(n, dimr) + (vec3)print.edits.back().rpos*dimr;

      while(!print.edits.empty() && all(equal(chunk.pos, print.edits.back().cpos))){
        chunk.setPosition(mod((vec3)print.edits.back().pos, vec3(chunkSize)), print.edits.back().type);
        print.edits.pop_back();
      }

      fwrite(chunk.data, sizeof(unsigned char), CHUNKVOL, outFile);

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

  timer::benchmark<std::chrono::milliseconds>([&](){

  logg::deb("Buffering Chunks...");

  lock = true;
  evaluateBlueprint(blueprint);


  stack<int> remove;                      //Chunks to Remove
  vector<ivec3> load;                     //Chunks to Load

  vec3 min = floor(cam::look/vec3(chunkSize))-scene::renderdist;  //Extent of Loaded Region
  vec3 max = floor(cam::look/vec3(chunkSize))+scene::renderdist;

  if(any(lessThan(max, vec3(0))) || any(greaterThan(min, dim-vec3(1)))){

    min = clamp(min, vec3(0), dim-vec3(1));
    max = clamp(max, vec3(0), dim-vec3(1));

  }
  else{

    min = clamp(min, vec3(0), dim-vec3(1));
    max = clamp(max, vec3(0), dim-vec3(1));

    for(int i = min.x; i <= max.x; i++)    //All Indices in Region
    for(int j = min.y; j <= max.y; j++)
    for(int k = min.z; k <= max.z; k++)
      load.push_back(ivec3(i, j, k));

  }

  /*
      Goal: Find the chunks which are to be removed and the ones to be loaded.
      This is currently slow (the slowest part!) because I compare all chunks.

      This should be done faster.
  */

  /*
      Remove: All out of the slice.
      Add: All in the slice.
  */

  std::cout<<"Chunk Iteration ";
  timer::benchmark<std::chrono::milliseconds>([&](){

  for(unsigned int i = 0; i < chunks.size(); i++){

    if(any(lessThan(chunks[i].pos, (ivec3)min))        //Remove Out-of-Bounds Chunks
    || any(greaterThanEqual(chunks[i].pos, (ivec3)max))){
      remove.push(i);
      continue;
    }

    for(size_t j = 0; j < load.size(); j++){
      if(all(equal(load[j], chunks[i].pos))){
        load.erase(load.begin()+j);
        j--;
      }
    }                 //Don't Re-Load Existing Chunks

  }

  });


  logg::deb("Unloading ", remove.size(), " chunks");

  while(!remove.empty()){                               //Delete Chunks
    chunks.erase(chunks.begin()+remove.top());
    delete models[remove.top()];
    models.erase(models.begin()+remove.top());
    remove.pop();
  }

  //This is where we should remove chunks that don't deserve loading

  logg::deb("Loading ", load.size(), " chunks");

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

      string savefile = "world.region"+regionString(load.back());

      FILE* inFile = fopen((savedir/savefile).string().c_str(), "rb");
      if(inFile == NULL){
        logg::err("Failed to open region file "+savefile);
        continue;
      }

      int oldn = helper::getIndex(mod((vec3)load.back(), dimr), dimr);
      fseek(inFile, 16*16*16*oldn, SEEK_SET);

      while(savefile == "world.region"+regionString(load.back())){

        Chunk chunk;
        chunk.pos = load.back();
        int newn = helper::getIndex(mod((vec3)chunk.pos, dimr), dimr);

    //    fseek(inFile, 16*16*16*(newn-oldn), SEEK_CUR);
        fseek(inFile, 16*16*16*newn, SEEK_SET);
        if(fread(chunk.data, sizeof(unsigned char), 16*16*16, inFile) < 16*16*16)
          logg::err("Read Fail");
    //    fseek(inFile, -16*16*16, SEEK_CUR);
        oldn = newn;

        chunks.push_back(chunk);
        load.pop_back();

      }

      fclose(inFile);

    }
  }

  lock = false;

  });


//  timer::benchmark<std::chrono::milliseconds>([&](){
    mesh();
//  });


}

/*
================================================================================
                                Mesh Handling
================================================================================
*/

void World::mesh(){

  logg::deb("Meshing Chunks...");

  for(unsigned int i = 0; i < chunks.size(); i++){

    if(i == models.size()){
      Model* model = new Model(chunkmesh::greedy, &chunks[i]);
      models.push_back(model);
      chunks[i].remesh = false;
    }

    models[i]->move((vec3)(chunks[i].pos)*vec3(chunkSize));
  }

  updateLOD = false;

}

void World::remesh(){

  while(!remeshbuf.edits.empty()){

    ivec3 c = remeshbuf.edits.back().cpos;
//    ivec3 p = mod((vec3)remeshbuf.edits.back().pos, vec3(chunkSize));
//    int ind = helper::getIndex(c, dim);

    //Skip non-loaded remeshBuffer objects
    if(!(all(greaterThanEqual(c, ivec3(0))) && all(lessThanEqual(c, (ivec3)dim)))){
      remeshbuf.edits.pop_back();
      continue;
    }

/*
    //Otherwise we are in bounds, so add that cube to the relevant model.
    models[chunk_order[ind]].addCube(p, world.remeshBuffer.editBuffer.back().type);
    models[chunk_order[ind]].update();
*/
    remeshbuf.edits.pop_back();

  }

  //Loop over all chunks, see if they are updated.
  for(unsigned int i = 0; i < chunks.size(); i++){
    if(chunks[i].remesh){
      models[i]->construct(chunkmesh::greedy, &chunks[i]);
      chunks[i].remesh = false;
    }
  }

}
