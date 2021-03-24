#ifndef TERRITORY_BLUEPRINT
#define TERRITORY_BLUEPRINT

using namespace glm;
using namespace std;

/*
================================================================================
                      Blueprint World Editing System
================================================================================
*/

struct bufferObject {

  bufferObject(){}

  bufferObject(vec3 p, BlockType t){
    set(p, t);
  }

  void set(vec3 p, BlockType t){
    pos = p; type = t;
    cpos = floor((vec3)pos/cdim);
    rpos = floor((vec3)cpos/rdim);
  }

  ivec3 pos;      //Block Position in Chunk
  ivec3 cpos;     //Block Position in Chunk
  ivec3 rpos;     //Chunk Position in Region
  BlockType type; //Metadata

  static vec3 rdim;
  static vec3 wdim;
  static vec3 cdim;

};

vec3 bufferObject::rdim = vec3(16,16,16);
vec3 bufferObject::wdim = vec3(64,16,64);
vec3 bufferObject::cdim = vec3(16,16,16);

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

  static int chunkSize;                                 //Size of Chunk
  static vec3 dim;                                      //Number of Chunks in Region

  vector<bufferObject> edits;

  bool add(vec3 _pos, BlockType _type, bool negative);  //Add Block
  void clean(bool later);                               //Remove Duplicates

  bool save(string savefile);                           //File Saving / Loading
  bool load(string loadfile);

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

int Blueprint::chunkSize = CHUNKSIZE;
glm::vec3 Blueprint::dim = glm::vec3(64,16,64);

/*
================================================================================
                         Blueprint Editing Functions
================================================================================
*/


bool Blueprint::add(vec3 pos, BlockType type, bool negative){

  if(!negative && (any(lessThan(pos, vec3(0)))          //Check Boundary Conditions
  || any(greaterThanEqual(pos, bufferObject::cdim*dim))))
    return false;

  edits.emplace_back(pos, type);  //Add to Edit Buffer
  return true;

}

void Blueprint::clean(bool later){

  Blueprint newprint;

  for(unsigned int i = 0; i < edits.size(); i++){

    bufferObject* buf;
    if(!later) buf = &edits[i];
    else buf = &edits[edits.size()-1 - i];

    bool duplicate = false;

    for(unsigned int j = 0; j < newprint.edits.size(); j++){
      if(!all(equal(buf->pos+buf->cpos*ivec3(16), newprint.edits[j].pos+newprint.edits[j].cpos*ivec3(16))))
        continue;
      duplicate = true;
      break;
    }

    if(!duplicate)  //Add First Occurence
      newprint.edits.push_back(edits[i]);

  }

  *this = newprint;

}


/*
================================================================================
                 Blueprint Serialization / Saving / Loading
================================================================================
*/

bool Blueprint::save(string savefile){
  fs::path bpdir = rootdir/"../resource/blueprint";
  if(!fs::exists(bpdir)){
    logg::deb("Creating Blueprint Resource Directory");
    fs::create_directory(bpdir);
  }

  ofstream out((bpdir/savefile).string());
  if(!out.is_open()){
    logg::err("Failed to Open Blueprint for Saving");
    return false;
  } else {
    boost::archive::binary_oarchive oa(out);
    oa << *this;
  }
  return true;
}

bool Blueprint::load(string loadfile){
  fs::path bpdir = rootdir/"../resource/blueprint";
  if(!fs::exists(bpdir)){
    logg::err("Blueprint Directory does not exist!");
    return false;
  }

  ifstream in((bpdir/loadfile).string());
  if(!in.is_open()){
    logg::err("Failed to Oepen Blueprint for Loading");
    return false;
  } else {
    boost::archive::binary_iarchive ia(in);
    ia >> *this;
  }
  return true;
}

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, bufferObject &_buf, const unsigned int version)
{
  ar & _buf.pos.x;
  ar & _buf.pos.y;
  ar & _buf.pos.z;
  ar & _buf.cpos.x;
  ar & _buf.cpos.y;
  ar & _buf.cpos.z;
  ar & _buf.type;
}

template<class Archive>
void serialize(Archive & ar, Blueprint &_blueprint, const unsigned int version)
{
  ar & _blueprint.chunkSize;
  ar & _blueprint.dim.x;
  ar & _blueprint.dim.y;
  ar & _blueprint.dim.z;
  ar & _blueprint.edits;
}

} // namespace serialization
} // namespace boost

#endif
