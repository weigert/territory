#include "geology.h"

void Geology::initialize(){
  //Reset the Seed
  SEED = time(NULL);
  srand(SEED);

  //Perlin Noise Module
  noise::module::Perlin perlin;
  perlin.SetOctaveCount(6);
  perlin.SetFrequency(3);
  perlin.SetPersistence(0.6);

  noise::module::Voronoi voronoi;
  voronoi.SetFrequency(8);

  float a = (float)(rand()%SEED)/((SEED));

  //Seed the Maps
  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.y; j++){
      //Generate a Volcanism Value
      volcanism[helper::getIndex(glm::vec2(i,j), d)] = ((perlin.GetValue((float)i/d.x, (float)j/d.y, SEED))*0.5+1)/2;
      plates[helper::getIndex(glm::vec2(i,j), d)] = ((voronoi.GetValue((float)i/d.x, (float)j/d.y, a)));
      height[helper::getIndex(glm::vec2(i,j), d)] = 0.5;
    }
  }
}

int Geology::clusterPlates(int centers[]){
  //Cluster Storage Thingy
  int nclusters = 0;
  int n[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  //We can reduce overhead
  for(int i = 0; i < size; i++){
    if(centers[i] == 0){
      nclusters++;
      centers[i] = nclusters;
      //Floodset
      std::stack<int> floodSet;
      floodSet.push(i);
      //Loop over the Floodset
      while(!floodSet.empty()){
        //Set the top floodset element to the nclusters value
        int cur = floodSet.top();
        floodSet.pop();
        centers[cur] = nclusters;

        //Add the neighbors
        for(int k = 0; k < 4; k++){
          //Get the neighbor's vector
          int neighbor = helper::getIndex(glm::mod(glm::vec2(cur),d) + glm::vec2(n[k][0], n[k][1]), d);

          //Add the neighbor if it exists, has a 0 value so far and densities are the same
          if(neighbor != -1 && centers[neighbor] == 0 && plates[neighbor] == plates[i]){
            //Add the neighbor to the floodset.
            floodSet.push(neighbor);
          }
        }
      }
    }
  }

  //Return the Number of Clusters
  return nclusters;
}

void Geology::calcOverlap(float overlap[], float newplate[], int labels[], int _nplates){
  //Construct Torque, Movement, Center Vectors for the Various Plates
  std::vector<glm::vec2> centers;
  std::vector<glm::vec2> force;
  std::vector<int> plateSize;

  //Every Plate gets a zero-vector guy
  for(int n = 0; n < _nplates; n++){
    centers.push_back(glm::vec2(0, 0));
    force.push_back(glm::vec2(0, 0));
    plateSize.push_back(1);
  }

  //Loop over all points and change the values of these guys
  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.y; j++){
      //Set the current winner to whatever the plate is
      newplate[helper::getIndex(glm::vec2(i,j), d)] = plates[helper::getIndex(glm::vec2(i,j), d)];

      //Set the overlap to -1
      overlap[helper::getIndex(glm::vec2(i,j), d)] = -1;

      //Get Label
      int x = labels[helper::getIndex(glm::vec2(i, j), d)]-1;
      //Increment the Platesize

      //Move the Centerpoint
      centers[x] = (centers[x]*glm::vec2(plateSize[x]) + glm::vec2(i, j))/glm::vec2(plateSize[x]+1);

      //Move the Force Vector
      force[x] += helper::gradient(volcanism, glm::vec2(i, j), d);

      //Increase the Platesize
      plateSize[x] += 1;
    }
  }

  //Project the Plates and Compute the Overlaps
  glm::vec2 timeStep = d;

  //Get the overlap
  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.y; j++){
      //Point Vector and Label
      glm::vec2 _p = glm::vec2(i, j);
      int x = labels[helper::getIndex(_p, d)]-1;
      //Cap the Force
      force[x] = glm::clamp(force[x], glm::vec2(-1000.0f), glm::vec2(1000.0f));
      //std::cout<<"PlateSize: "<<plateSize[x]<<std::endl;
      glm::vec2 _n = glm::floor(_p + force[x]/glm::vec2(plateSize[x])*timeStep);

      if(helper::getIndex(_n, d) != -1){
        overlap[helper::getIndex(_n, d)]++;
        if(newplate[helper::getIndex(_n, d)] < plates[helper::getIndex(_p, d)]){
          newplate[helper::getIndex(_n, d)] = plates[helper::getIndex(_p, d)];
        }
      }
    }
  }
}

void Geology::doTimeStep(){
  //Compute Temporary Quantities
  int labels[size] = {0};
  int nplates = clusterPlates(labels);

  //Now that we have the number of plates and their labels, compute the overlap map
  float overlap[size] = {-1};
  float winner[size];
  calcOverlap(overlap, winner, labels, nplates); //This means gradients, moving plates, getting {-1,1} value

  //Change Volcanism
  for(int i = 0; i < size; i++){
    volcanism[i] += overlap[i]*plates[i]*0.001f;
  }

  for(int i = 0; i < 10; i++){
    helper::diffuse(volcanism, 0.5, d); //Diffuse the Volcanism
  }

  //Compute the Change to the Height
  float shift[size] = {0};
  float shift2[size] = {0};
  helper::add(shift, height, size);
  //First Term (Low-Land, High Volcanism Eruptions)
  helper::multiply(shift, -1.0, size);
  helper::add(shift, 1.0f, size);
  helper::multiply(shift, volcanism, size);
  helper::multiply(shift, 0.05, size);   //Scaling Factor

  //Second Term, overlap leading to up and down / ridge growth
  helper::shape(shift2, overlap, d);
  helper::diffuse(shift2, 1.0, d);

  //Scale it down
  helper::multiply(shift2, 0.05, size);

  //Add the Shift
  helper::add(shift, shift2, size);
  helper::add(height, shift, size);
  for(int i = 0; i < 3; i++){
    helper::diffuse(height, 0.2, d);
  }

  //Compute the Change to the Density
  helper::set(plates, winner, size);

  CImg<unsigned char> img(50, 50, 1, 3);

  for(int i = 0; i < d.x; i++){
    for(int j = 0; j < d.y; j++){
      //Create the Image
      img(i, j, 0, 0) = height[helper::getIndex(glm::vec2(i, j), d)]*255;
      img(i, j, 0, 1) = height[helper::getIndex(glm::vec2(i, j), d)]*255;
      img(i, j, 0, 2) = height[helper::getIndex(glm::vec2(i, j), d)]*255;
    }
  }
  img.save_png("test_volcanism.png");
}

//Generate the Terrain
void Geology::generate(){
  //Do a series of time steps
  for(int t = 0; t < 20; t++){
    //Do the Time Step
    doTimeStep();
  }
  //Normlize the Height
  helper::normalize(height, size);
}
