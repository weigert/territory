/*
================================================================================
                      VOLUME FUNCTIONS (FOR BUILDINGS)
================================================================================
*/

struct Volume{
  //Bounding Box Corners
  glm::vec3 a;
  glm::vec3 b;

  void translate(glm::vec3 shift);
  int getVol();
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

  //Determine the Lower Corner
  bb.a.x = (v1.a.x < v2.a.x)?v1.a.x:v2.a.x;
  bb.a.y = (v1.a.y < v2.a.y)?v1.a.y:v2.a.y;
  bb.a.z = (v1.a.z < v2.a.z)?v1.a.z:v2.a.z;

  //Determine the Upper Corner
  bb.b.x = (v1.b.x > v2.b.x)?v1.b.x:v2.b.x;
  bb.b.y = (v1.b.y > v2.b.y)?v1.b.y:v2.b.y;
  bb.b.z = (v1.b.z > v2.b.z)?v1.b.z:v2.b.z;

  return bb;
}

//Compute the Volume Overlap in 3 Directions
glm::vec3 overlapVolumes(Volume v1, Volume v2){
  //Compute Bounding Box for the two volumes
  Volume bb = boundingBox(v1, v2);

  //Compute the Overlap
  glm::vec3 ext1 = glm::abs(v1.b - v1.a);  //Extent of v1 in 3 directions
  glm::vec3 ext2 = glm::abs(v2.b - v2.a);  //Extent of v2 in 3 directions
  glm::vec3 extbb = glm::abs(bb.b - bb.a); //Extent of the bounding box

  //Overlap is given
  return ext1 + ext2 - extbb;
}
