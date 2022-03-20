/*
================================================================================
                          Territory Voxel System
================================================================================
*/

#ifndef TERRITORY_VOXEL
#define TERRITORY_VOXEL

namespace voxel {
using namespace std;
using namespace glm;

unsigned int CHUNKSIZE = 16;
unsigned int CVOL = 16*16*16;
ivec3 CDIM = ivec3(16, 16, 16);

// Block-Types

enum block: unsigned char {
  BLOCK_AIR,
  BLOCK_GRASS,
  BLOCK_DIRT,
  BLOCK_SAND,
  BLOCK_CLAY,
  BLOCK_GRAVEL,
  BLOCK_SANDSTONE,
  BLOCK_STONE,
  BLOCK_WATER,
  BLOCK_LEAVES,
  BLOCK_WOOD,
  BLOCK_PUMPKIN,
  BLOCK_CACTUS,
  BLOCK_PLANKS,
  BLOCK_GLASS,
  BLOCK_VOID,
  BLOCK_CACTUSFLOWER
};

const vector<vec4> skycolors{
  vec4(0.1, 0.1, 0.1, 1.0),            //Dark Grey
  vec4(0.00, 1.00, 1.00, 1.0),         //Blue
  vec4(1.00, 1.00, 1.00, 1.0),         //White
  vec4(0.1, 0.1, 0.1, 1.0)             //Dark Grey
};

const vector<vec4> sandcolors{
  vec4(1.00, 0.75, 0.35, 1.0),
  vec4(0.84, 0.21, 0.21, 1.0),
  vec4(0.93, 0.91, 0.38, 1.0)
};

const vector<vec4> grasscolors{
  vec4(0.65, 0.72, 0.34, 1.0),
  vec4(0.19, 0.45, 0.34, 1.0),
  vec4(0.54, 0.70, 0.34, 1.0)
};

const vector<vec4> stonecolors{
  vec4(0.5, 0.5, 0.5, 1.0),
  vec4(0.6, 0.6, 0.6, 1.0),
  vec4(0.7, 0.7, 0.7, 1.0)
};

const vector<vec4> gravelcolors{
  vec4(0.9, 0.9, 0.9, 1.0),
  vec4(0.8, 0.8, 0.8, 1.0),
  vec4(0.7, 0.7, 0.7, 1.0)
};

const vector<vec4> autumncolors{
  vec4(0.17, 0.40, 0.26, 0.8),
  vec4(1.00, 0.75, 0.35, 1.0),
  vec4(0.84, 0.21, 0.21, 1.0)
};

const vector<vec4> leafcolors{
  vec4(0.16, 0.44, 0.27, 1.0),
  vec4(0.05, 0.31, 0.22, 1.0),
  vec4(0.35, 0.43, 0.13, 1.0)
};

const vector<vec4> claycolors{
  vec4(0.97f, 0.5f, 0.44f, 1.0f),
  vec4(0.75, 0.30, 0.30, 1.0),
};

vec4 get(block type, double hash){
  //Switch the value and return a vector
  switch(type){
    case BLOCK_GRASS:
      return color::bezier(hash, grasscolors);
      break;
    case BLOCK_DIRT:
      return vec4(0.74f, 0.5f, 0.36f, 1.0f);
      break;
    case BLOCK_WATER:
      return vec4(0.3f, 0.57f, 0.67f, 0.8f); //Semi Transparent!
      break;
    case BLOCK_SAND:
      return color::bezier(hash, sandcolors);
      break;
    case BLOCK_CLAY:
      return color::bezier(hash, claycolors);
      break;
    case BLOCK_STONE:
      return color::bezier(hash, stonecolors);
      break;
    case BLOCK_LEAVES:
      return color::bezier(hash, leafcolors);
      break;
    case BLOCK_WOOD:
      return vec4(0.6f, 0.375f, 0.14f, 1.0f);
      break;
    case BLOCK_GRAVEL:
    return color::bezier(hash, gravelcolors);
      break;
    case BLOCK_SANDSTONE:
      return vec4(0.8f, 0.75f, 0.64f, 1.0f);
      break;
    case BLOCK_PUMPKIN:
      return vec4(1.0f, 0.5f, 0.0f, 1.0f);
      break;
    case BLOCK_CACTUS:
      return vec4(0.0f, 0.44f, 0.3f, 1.0f);
      break;
    case BLOCK_PLANKS:
      return vec4(0.75f, 0.6f, 0.28f, 1.0f);
      break;
    case BLOCK_GLASS:
      return vec4(0.8f, 0.9f, 0.95f, 0.2f);
      break;
    default:
      return vec4(1.0f, 1.0f, 1.0f, 0.5f);
      break;
  }
}

// Chunks and Chunk Management

class Chunk{
public:

  Chunk(){
    data = new voxel::block[CVOL]{BLOCK_AIR};
  }
  Chunk(ivec3 p){
    pos = p;
  }
  Chunk(ivec3 p, voxel::block* ndata){
    data = ndata;
    pos = p;
  }

  voxel::block* data;
  ivec3 pos = ivec3(0);

  bool remesh = true;
  bool firstmesh = true;

  int quadsize = 0;
  int quadstart = 0;
  array<uint*, 6> faces;

  void set(ivec3 p, block t){
    data[math::cflatten(p, CDIM)] = t;
  }

  block get(ivec3 p){
    return data[math::cflatten(p, CDIM)];
  }

  block get(int* p){
    return data[math::cflatten(ivec3(p[0], p[1], p[2]), CDIM)];
  }

};

bool operator > (const Chunk& a, const Chunk& b) {

  if(a.pos.x > b.pos.x) return true;    //Sort by Position
  if(a.pos.x < b.pos.x) return false;

  if(a.pos.y > b.pos.y) return true;    //Sort by Position
  if(a.pos.y < b.pos.y) return false;

  if(a.pos.z > b.pos.z) return true;    //Sort by Position
  if(a.pos.z < b.pos.z) return false;

  return false;

}

bool operator < (const Chunk& a, const Chunk& b) {

  if(a.pos.x < b.pos.x) return true;    //Sort by Position
  if(a.pos.x > b.pos.x) return false;

  if(a.pos.y < b.pos.y) return true;    //Sort by Position
  if(a.pos.y > b.pos.y) return false;

  if(a.pos.z < b.pos.z) return true;    //Sort by Position
  if(a.pos.z > b.pos.z) return false;

  return false;
}

// Chunk Pooling

class Chunkpool {
public:

  voxel::block* data = NULL;
  deque<voxel::block*> free;
  size_t SIZE = 0;

  Chunkpool(){}
  Chunkpool(size_t _SIZE){
    reserve(_SIZE);
  }
  ~Chunkpool(){
    if(data != NULL)
      delete[] data;
  }

  void reserve(size_t _SIZE){

    SIZE = _SIZE;
    if(data != NULL)
      delete[] data;

    data = new voxel::block[CVOL*SIZE]{voxel::BLOCK_AIR}; //This needs deleting
    for(size_t i = 0; i < SIZE; i++)
      free.push_front(data+i*CVOL);

  }

  voxel::block* get(){

    if(free.empty()){
      std::cout<<"CAN'T ALLOCATE CHUNK: OUT OF MEMORY"<<std::endl;
      return NULL;
    }

    voxel::block* freechunk = free.back();
    free.pop_back();
    return freechunk;

  }

  void remove(voxel::block* chunk){

    if((chunk - data) < 0 || (size_t)(chunk - data) >= CVOL*SIZE){
      cout<<"TRYING TO FREE OUT OF POOL MEMORY"<<endl;
      return;
    }

    free.push_front(chunk);

  }

};

// Meshing

void mesh(Chunk* c, Vertexpool<Vertex>* vertpool){

  int LOD = scene::LOD;
  int QUAD = scene::QUAD;
  int CHLOD = CHUNKSIZE/LOD;
  vec3 p = c->pos*ivec3(CHUNKSIZE/LOD);

  int u, v, w;
  int n;

  voxel::block* mask = new voxel::block[CHUNKSIZE*CHUNKSIZE/LOD/LOD];
  voxel::block current, facing;
  int s;

  uint* section;

  int quads = 0;

  for(int d = 0; d < 6; d++){

  //  std::cout<<"Chunk D-Loop ";
  //  timer::benchmark<std::chrono::microseconds>([&](){

    u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2      //Dimension m->indices
    v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    c->quadsize = 0;

    section = vertpool->section(QUAD, d, (vec3)c->pos + vec3(0.5, 0.5, 0.5)*vec3(q[0], q[1], q[2]));

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth

      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_AIR;                                                  //Set to Air

          current = c->get(LOD*ivec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_AIR) continue;

          //Basically, we are facing out of the chunk, so we do take over the surface.
          //  if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->get(LOD*ivec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_AIR)
            mask[s] = current;
          // else mask[s] = BLOCK_ANY;

        }
      }

      int width = 1, height = 1;
      bool quaddone;
      vec3 color;

      for(x[v] = 0; x[v] < CHLOD; x[v]++){            //Evaluate Mask
        for(x[w] = 0; x[w] < CHLOD; x[w] += width){   //Permissible Skip

          width = height = 1;       //Current Quad Dimensions

          s = x[w] + x[v]*CHLOD;    //Current Slice Index
          current = mask[s];        //Current Block Type

          if(current == BLOCK_AIR /* || current == BLOCK_ANY */ )  //We don't mesh air
            continue;

          while(x[w] + width < CHLOD && (mask[s+width] == current /* || mask[s+width] == BLOCK_ANY */ ))
            width++;

          quaddone = false;
          for(height = 1; x[v] + height < CHLOD; height++){   //Find Height

            for(int k = 0; k < width; k++){                   //Iterate Over Width
              if(mask[s+k+height*CHLOD] != current /* && mask[s+k+height*CHLOD] != BLOCK_ANY */ ) {
                quaddone = true;
                break;
              }
            }
            if(quaddone) break;
          }

          for(int l = x[v]; l < x[v] + height; l++)   //Zero the Mask
          for(int k = x[w]; k < x[w] + width; k++)
            mask[k+l*CHLOD] = BLOCK_AIR;

        //  vec3 px = p+vec3(x[0], x[1], x[2]);
          vec3 qq = vec3(q[0], q[1], q[2]);

          int du[3] = {0}; du[v] = height;
          int dv[3] = {0}; dv[w] = width;

          color = voxel::get(current, color::hashrand(math::cflatten(ivec3(x[0], x[1], x[2]), ivec3(16))));

          if(n < 0){

            vertpool->fill(section, c->quadsize*4+0,
              vec3( (p.x-0.5+scene::SCALE*(x[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]))*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+1,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+du[0]+dv[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+du[1]+dv[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+du[2]+dv[2]))*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+2,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+du[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+du[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+du[2]))*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+3,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+dv[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+dv[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+dv[2]))*(float)LOD),
              qq, color);

          }
          else{

            vertpool->fill(section, c->quadsize*4+0,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+y[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+y[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+y[2]))*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+1,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+du[0]+dv[0]+y[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+du[1]+dv[1]+y[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+du[2]+dv[2]+y[2]))*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+2,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+du[0]+y[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+du[1]+y[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+du[2]+y[2]))*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+3,
              vec3( (p.x-0.5+scene::SCALE*(x[0]+dv[0]+y[0]))*(float)LOD,
                    (p.y-0.5+scene::SCALE*(x[1]+dv[1]+y[1]))*(float)LOD,
                    (p.z-0.5+scene::SCALE*(x[2]+dv[2]+y[2]))*(float)LOD),
              qq, color);

          }

          c->quadsize++;
          //Next Quad
        }
      }
      //Next Slice
    }

    vertpool->resize(section, c->quadsize*6);
    c->faces[d] = section;
    quads += c->quadsize;
  //  });


  //Next Surface Orientation
  }

//  std::cout<<"QUADS: "<<quads<<std::endl;

  delete[] mask;

}

/*
================================================================================
            Chunk Compression: Morton Order Run-Length Encoding
================================================================================

Note: rle_num is a datatype that needs to be defined so that a strict number of
bytes is allocated in a file for the number of rle elements in a line (i.e. chunk)
this allows for efficient line skipping / seeking.

*/

using rle_num = size_t;

// RLE Element

struct rlee {
  voxel::block type;
  unsigned int length = 1;
};

// Compress / Uncompress Methods

rle_num compress(rlee* elem, voxel::block* data){

  rle_num N = 0;
  elem->type = data[0];
  elem->length = 1;

  for(size_t i = 1; i < CVOL; i++){
    if(data[i] != elem->type){
      N++; elem++;
      elem->type = data[i];
      elem->length = 1;
    }
    else elem->length++;
  }

  return N;

}

void uncompress(rlee* elem, rle_num N, voxel::block* data){

  size_t R = 0;
  for(rle_num n = 0; n < N; n++){
    for(size_t r = 0; r < elem[n].length; r++)
      data[R+r] = elem[n].type;
    R += elem[n].length;
  }

}

}

#endif
