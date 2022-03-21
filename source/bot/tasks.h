/*
================================================================================
                            Derived / Composed Tasks
================================================================================
*/

#include "astar/sltastar.h"
#include "astar/astar.h"


namespace task {

//List of All Available Tasks

struct Idle;
struct Example;



/*
================================================================================
                              Define the Tasks
================================================================================
*/

struct Idle : public Task {

  string desc = "Idle";

  Idle(){

    handle = [&](Bot* bot){
      cout<<"Handling Queue in Idle"<<endl;
      return true;        //Work off Queue
    };

  };

};

struct Example : public Task {

  string desc = "Example";

  Example(){

    tasks.push_back(new Idle());    //Construct List of Tasks
    tasks.push_back(new Idle());
    tasks.push_back(new Idle());

    handle = [&](Bot* bot){
      cout<<"Handling Queue in Example"<<endl;
      return queue(bot);        //Work off Queue
    };

  };

};

struct Move : public Task {

  string desc = "Move";
  ivec3 to;
  int nsteps = 4;

  Move(ivec3 _to){

    to = _to;

    handle = [&](Bot* bot){

      cout<<"Handling Queue in Move"<<endl;

      if(nsteps-- > 0)
        return false;

      bot->pos = to;
      cam::look = bot->pos;
      cam::update();
      return true;        //Work off Queue

    };

  }

};



struct Path : public Task {

  string desc = "Path";
  stack<ivec3> path;
  ivec3 _to;

  Path(ivec3 to){

    _to = to;

    handle = [&](Bot* bot){

      cout<<"Try Pathing..."<<endl;

      cout<<_to.x<<" "<<_to.y<<" "<<_to.z<<endl;
      cout<<path.size()<<endl;

      if(init && !getPath(bot->pos, _to, ivec3(1,2,1))){

        cout<<"Failed to Find Path"<<endl;
        return true;        //Work off Queue

      }

      cout<<"Pathing..."<<endl;


      if(path.empty())
        return true;

      bot->pos = path.top();
      path.pop();

      cam::look = bot->pos;
      cam::update();

      return false;        //Work off Queue

    };

  }

  bool getPath(ivec3 from, ivec3 to, ivec3 range){

    AStarSearch<MapSearchNode> astarsearch;

    MapSearchNode nodeStart;
    nodeStart.x = from.x;
    nodeStart.y = from.y;
    nodeStart.z = from.z;

    MapSearchNode nodeEnd;
    nodeEnd.x = to.x;
    nodeEnd.y = to.y;
    nodeEnd.z = to.z;

    astarsearch.SetStartAndGoalStates( nodeStart, nodeEnd );

    unsigned int SearchState;
    do{ SearchState = astarsearch.SearchStep(world, range); }
    while( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING );

    if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED ){

        MapSearchNode *node = astarsearch.GetSolutionEnd();
        path.push(ivec3(node->x, node->y, node->z));

        for(;;){
          node = astarsearch.GetSolutionPrev();
          if( !node ) break;
          path.push(ivec3(node->x, node->y, node->z));
        }
        // Once you're done with the solution you can free the nodes up
        astarsearch.FreeSolutionNodes();
        astarsearch.EnsureMemoryFreed();
        return true;

    }

    while(!path.empty())
      path.pop();

    return false;

  }

};

struct Fell : public Task {

  string desc = "Fell Tree";
  ivec3 fellpos;

  ivec3 top(ivec2 pos){

    ivec3 wpos = ivec3(pos.x, (world->maxchunk.y+1)*CHUNKSIZE-1, pos.y);
    voxel::block type = world->get(wpos);

    while(type == voxel::BLOCK_AIR || type == voxel::BLOCK_LEAVES || type == voxel::BLOCK_WOOD){
      wpos.y--;
      if(wpos.y < 0) return wpos + ivec3(0, 1, 0);
      type = world->get(wpos);
    }

    return wpos + ivec3(0,1,0);

  }

  bool find(ivec3 pos, ivec3 radius, ivec3& fpos){

    bool found = false;
    float dist = length(2.0f*vec3(radius));

    for(int x = pos.x - radius.x; x <= pos.x + radius.x; x++)
    for(int z = pos.z - radius.z; z <= pos.z + radius.z; z++){

      ivec3 wpos = top(ivec2(x,z));
      if (world->get(wpos) == voxel::BLOCK_WOOD){
        if(found == false || length(vec3(wpos)-vec3(pos)) < dist){
          dist = length(vec3(wpos)-vec3(pos));
          fpos = wpos;
          found = true;
        }
      }

    }

    return found;

  }


  Fell(ivec3 pos){

    if(!find(pos, ivec3(10), fellpos)) handle = [&](Bot* bot){

      cout<<"No trees in vicinity."<<endl;
      return true;        //Work off Queue

    };

    else {

      cout<<"FOUND TREE POS"<<endl;

      tasks.push_back(new Path( fellpos ));

      handle = [this](Bot* bot){

        if(!queue(bot)) return false;

        function<void(ivec3 pos)> trychopup = [&](ivec3 pos){
          voxel::block type = world->get(pos);
          while(type == voxel::BLOCK_WOOD || type == voxel::BLOCK_LEAVES){
            world->set(pos, voxel::BLOCK_AIR);
            if(world->get(pos + ivec3( 1, 0, 0)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 1, 0, 0));
            if(world->get(pos + ivec3(-1, 0, 0)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3(-1, 0, 0));
            if(world->get(pos + ivec3( 0, 0, 1)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 0, 0, 1));
            if(world->get(pos + ivec3( 0, 0,-1)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 0, 0,-1));
            pos.y++;
            type = world->get(pos);
          }

          if(world->get(pos + ivec3( 0, 1, 0)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 0, 1, 0));
          if(world->get(pos + ivec3( 1, 0, 0)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 1, 0, 0));
          if(world->get(pos + ivec3(-1, 0, 0)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3(-1, 0, 0));
          if(world->get(pos + ivec3( 0, 0, 1)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 0, 0, 1));
          if(world->get(pos + ivec3( 0, 0,-1)) == voxel::BLOCK_LEAVES) trychopup(pos + ivec3( 0, 0,-1));

        };

        trychopup(fellpos);

        world->mesh();

        return true;

      };

    }

  }

};

} //End of Namespace
