/*
================================================================================
                      VOLUME FUNCTIONS (FOR BUILDINGS)
================================================================================
*/

struct Volume{
  //Bounding Box Corners
  glm::vec3 a = glm::vec3(0);
  glm::vec3 b = glm::vec3(0);

  void translate(glm::vec3 shift);
  int getVol();

  //Looper
  template<typename FUNCTION>
  void loop(FUNCTION f) {
    for(int i = a.x; i <= b.x; i++){
      for(int j = a.y; j <= b.y; j++){
        for(int k = a.z; k <= b.z; k++){
          f(glm::vec3(i, j, k));
        }
      }
    }
  }
};

//Get the Total Volume
int Volume::getVol(){
  return abs((b.x-a.x)*(b.y-a.y)*(b.z-a.z));
}

//Move the Corners of the Bounding Box
void Volume::translate(glm::vec3 shift){
  a += shift;
  b += shift;
}

Volume boundingBox(Volume v1, Volume v2){
  //New Bounding Volume
  Volume bb;

  //Determine the Lower and Upper Corner
  bb.a = glm::min(v1.a, v2.a);
  bb.b = glm::max(v1.b, v2.b);

  return bb;
}

bool inVol(glm::vec3 pos, Volume v){
  return glm::all(glm::greaterThanEqual(pos, v.a)) && glm::all(glm::lessThanEqual(pos, v.b));
}

//Compute the Volume Overlap in 3 Directions
glm::vec3 overlapSize(Volume v1, Volume v2){
  //Compute Bounding Box for the two volumes
  Volume bb = boundingBox(v1, v2);

  //Compute the Overlap
  glm::vec3 ext1 = glm::abs(v1.b - v1.a);  //Extent of v1 in 3 directions
  glm::vec3 ext2 = glm::abs(v2.b - v2.a);  //Extent of v2 in 3 directions
  glm::vec3 extbb = glm::abs(bb.b - bb.a); //Extent of the bounding box

  //Overlap is given
  return ext1 + ext2 - extbb;
}

Volume intersectionVolume(Volume v1, Volume v2){
  //Intialize Intersection as Bounding Box
  Volume intersection;
  if(glm::any(glm::lessThan(overlapSize(v1, v2), glm::vec3(0)))) return intersection;

  //Get the Intersection Volume
  intersection.a = glm::max(glm::min(v1.a, v1.b), glm::min(v2.a, v2.b));
  intersection.b = glm::min(glm::max(v1.a, v1.b), glm::max(v2.a, v2.b));
  return intersection;
}
