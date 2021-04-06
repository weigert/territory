/*
================================================================================
                          Chunk Meshing Algorithms
================================================================================
*/

namespace chunkmesh {
using namespace std;

function<void(Chunk*, Vertexpool<Vertex>*)> greedypool = [](Chunk* c, Vertexpool<Vertex>* vertpool){

  int LOD = scene::LOD;
  int QUAD = scene::QUAD;
  int CHLOD = CHUNKSIZE/LOD;
  vec3 p = c->pos*ivec3(CHUNKSIZE/LOD);

  int u, v, w;
  int n;

  BlockType* mask = new BlockType[CHUNKSIZE*CHUNKSIZE/LOD/LOD];
  BlockType current, facing;
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

          current = c->getPosition((float)LOD*vec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_AIR) continue;

          //Basically, we are facing out of the chunk, so we do take over the surface.
          //  if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->getPosition((float)LOD*vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

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

          color = block::getColor(current, 0.0f);

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

};

}
