#ifndef TERRITORY_BLUEPRINT
#define TERRITORY_BLUEPRINT

namespace blueprint {
using namespace glm;
using namespace std;


const size_t MAXEDITS = 2 << 24;
unsigned int CHUNKSIZE = 16;                      //Chunk Side-Length (Power-of-2)
ivec3 WDIM = ivec3(16,16,16);                     //Spatial Information (Size in Chunks)
ivec3 RDIM = min(WDIM, ivec3(16,16,16));          //Chunks Per Region
ivec3 CDIM = ivec3(CHUNKSIZE);
unsigned int CVOL = (CDIM.x*CDIM.y*CDIM.z);
unsigned int RVOL = (RDIM.x*RDIM.y*RDIM.z);
unsigned int WVOL = (WDIM.x*WDIM.y*WDIM.z);

/*
================================================================================
                      Blueprint World Editing System
================================================================================
*/

struct bufferObject {

  bufferObject(){}

  bufferObject(vec3 p, voxel::block t){
    set(p, t);
  }
  bufferObject(ivec3 c){
    cpos = c;
    rpos = cpos/RDIM;
  }

  void set(vec3 p, voxel::block t){
    pos = p; type = t;
    cpos = pos/CDIM;
    rpos = cpos/RDIM;
  }

  ivec3 pos = ivec3(0);        //Block Position in Chunk
  ivec3 cpos = ivec3(0);       //Block Position in Chunk
  ivec3 rpos = ivec3(0);       //Chunk Position in Region
  voxel::block type = voxel::BLOCK_AIR; //Metadata

};

bool operator > (const bufferObject& a, const bufferObject& b) {

  if(a.rpos.x > b.rpos.x) return true;    //Sort by Region (3D)
  if(a.rpos.x < b.rpos.x) return false;

  if(a.rpos.y > b.rpos.y) return true;
  if(a.rpos.y < b.rpos.y) return false;

  if(a.rpos.z > b.rpos.z) return true;
  if(a.rpos.z < b.rpos.z) return false;

  if(a.cpos.x > b.cpos.x) return true;    //Sort by Chunk (3D)
  if(a.cpos.x < b.cpos.x) return false;

  if(a.cpos.y > b.cpos.y) return true;
  if(a.cpos.y < b.cpos.y) return false;

  if(a.cpos.z > b.cpos.z) return true;
  if(a.cpos.z < b.cpos.z) return false;

  return false;

}

class Blueprint{
public:

  vector<bufferObject> edits;

  bool add(ivec3 _pos, voxel::block _type, bool negative = false);  //Add Block
  bool write(string savename);

  Blueprint& operator += (Blueprint& lhs){              //Merge Blueprints
    for(bufferObject& obj : lhs.edits)
      this->edits.push_back(obj);
    return *this;
  }

  Blueprint& operator + (Blueprint& lhs){
    for(bufferObject& obj : lhs.edits)
      this->edits.push_back(obj);
    return *this;
  }

  Blueprint& operator + (const vec3 shift){                  //Shift Blueprints
    size_t n = this->edits.size();
    while(n > 0){
      bufferObject& obj = this->edits[0];
      this->add(obj.pos + (ivec3)shift, obj.type, false);
      this->edits.erase(this->edits.begin(), this->edits.begin()+1);
      n--;
    }
    return *this;
  }

};

bool Blueprint::add(ivec3 pos, voxel::block type, bool negative){

  if(!negative && (any(lessThan(pos, ivec3(0)))          //Check Boundary Conditions
  || any(greaterThanEqual(pos, CDIM*WDIM))))
    return false;

  edits.emplace_back(pos, type);  //Add to Edit Buffer
  return true;

}

bool Blueprint::write(string s){

  logg::deb("Evaluating Blueprint [", edits.size(), "]");

  if(edits.empty())
    return false;

  sort(edits.begin(), edits.end(), greater<bufferObject>());

  voxel::Chunk chunk;
  chunk.data = new voxel::block[CVOL]{voxel::BLOCK_AIR};

  voxel::rlee rledata[CVOL];              //RLE Data
  voxel::rle_num nrle = 1;                    //Number of Elements in Line

  while(!edits.empty()){

    ivec3 rpos = edits.back().rpos;
    string savefile = "world.region"+math::tostring(rpos);

    FILE* inFile = fopen((savedir/savefile).string().c_str(), "rb");
    if(inFile == NULL){
      logg::err("Failed to open region file "+savefile);
      return false;
    }

    FILE* outFile = fopen((savedir/(savefile+".temp")).string().c_str(), "wb");
    if(outFile == NULL){
      logg::err("Failed to open region file "+savefile);
      return false;
    }

    for(size_t n = 0; n < RVOL; n++){  //Region Size

      if( fread(&nrle, sizeof(voxel::rle_num), 1, inFile) < 1 )
        logg::err("Read Error");

      if( fread(&rledata[0], sizeof(voxel::rlee), nrle, inFile) < nrle)
        logg::err("Read Error");

      // Comporess

      voxel::uncompress(rledata, nrle, chunk.data); //Uncompress

      chunk.pos = math::unflatten(n, RDIM) + edits.back().rpos*RDIM;

      while(!edits.empty() && all(equal(chunk.pos, edits.back().cpos)) && all(equal(rpos, edits.back().rpos))){

        chunk.set(mod((vec3)edits.back().pos, vec3(CHUNKSIZE)), edits.back().type);
        edits.pop_back();

      }

      nrle = voxel::compress(rledata, chunk.data);  //Compress

      if( fwrite(&nrle, sizeof(voxel::rle_num), 1, outFile) < 1 )
        logg::err("Write Error");

      if( fwrite(&rledata[0], sizeof(voxel::rlee), nrle, outFile) < nrle )
        logg::err("Write Error");

    }

    fclose(inFile);
    fclose(outFile);

    fs::remove_all(savedir/savefile);
    fs::rename((savedir/(savefile+".temp")), savedir/savefile);

  }

  delete[] chunk.data;

  logg::deb("Success");
  return true;

}

}

#endif