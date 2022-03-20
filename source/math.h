//Functions to do Arraymath with
namespace math {

using namespace std;
using namespace glm;

  int flatten(ivec2 p, ivec2 s){
    if(!all(lessThan(p, s)) || !all(greaterThanEqual(p, ivec2(0))))
      return -1;
    return p.x*s.y+p.y;
  }

  int flatten(ivec3 p, ivec3 s){
    if(!all(lessThan(p, s)) || !all(greaterThanEqual(p, ivec3(0))))
      return -1;
    return p.x*s.y*s.z+p.y*s.z+p.z;
  }

  ivec3 unflatten(int index, ivec3 s){
    int z = ( index / 1   ) % s.x;
    int y = ( index / s.x ) % s.y;
    int x = ( index / ( s.x * s.y ) );
    return ivec3(x, y, z);
  }

  string tostring(ivec3 v){
    stringstream ss;
    ss << v.x << v.y << v.z;
    return ss.str();
  }

  // Indexing / Flattening / Unflattening in Chunks

  int cflatten(ivec3 p, ivec3 s){
    return libmorton::morton3D_32_encode(p.x, p.y, p.z);
  }

  ivec3 cunflatten(int i){
    long unsigned int x, y, z;
    libmorton::morton3D_32_decode(i, x, y, z);
    return ivec3(x, y, z);
  }

}
