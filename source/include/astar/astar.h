
#define DEBUG_LISTS 0
#define DEBUG_LIST_LENGTHS_ONLY 0

using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// STL A* Search implementation
// (C)2001 Justin Heyes-Jones
//
// Finding a path on a simple grid maze
// This shows how to do shortest path finding using A*

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>
#include <math.h>

#define DEBUG_LISTS 0
#define DEBUG_LIST_LENGTHS_ONLY 0

using namespace std;

int GetMap(World &world, int x, int y, int z)
{
  //Get the Element we Need
  return (block::moveWeight(world.getBlock(glm::vec3(x,y,z))) + 9 - block::moveWeight(world.getBlock(glm::vec3(x,y-1,z))));
}

// Definitions
class MapSearchNode
{
public:
	int x;	 // the (x,y) positions of the node
	int y;
  int z;

	MapSearchNode() { x = y = z = 0; }
	MapSearchNode( int px, int py, int pz ) { x=px; y=py; z=pz; }

	float GoalDistanceEstimate( MapSearchNode &nodeGoal );
	bool IsGoal( MapSearchNode &nodeGoal, glm::vec3 range );
	bool GetSuccessors(World &world, AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node );
	float GetCost(World &world, MapSearchNode &successor );
	bool IsSameState( MapSearchNode &rhs, glm::vec3 range );

	void PrintNodeInfo();


};

bool MapSearchNode::IsSameState( MapSearchNode &rhs, glm::vec3 range )
{
  if( (x == rhs.x) &&
      (y == rhs.y) &&
      (z == rhs.z))
  {
    return true;
  }

  return false;
}

void MapSearchNode::PrintNodeInfo()
{
	char str[100];
	sprintf( str, "Node position : (%d,%d)\n", x,y );

	cout << str;
}

// Here's the heuristic function that estimates the distance from a Node
// to the Goal.

float MapSearchNode::GoalDistanceEstimate( MapSearchNode &nodeGoal )
{
	return abs(x - nodeGoal.x) + abs(y - nodeGoal.y) + abs(z - nodeGoal.z);
}

bool MapSearchNode::IsGoal( MapSearchNode &nodeGoal, glm::vec3 range )
{
    if( abs(x - nodeGoal.x)<= range.x &&
        abs(y - nodeGoal.y)<= range.y &&
        abs(z - nodeGoal.z)<= range.z)
    {
      return true;
    }

    return false;
}

// This generates the successors to the given Node. It uses a helper function called
// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
// is done for each node internally, so here you just set the state information that
// is specific to the application
bool MapSearchNode::GetSuccessors(World &world, AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node )
{

	int parent_x = -1;
	int parent_y = -1;
  int parent_z = -1;

	if( parent_node )
	{
		parent_x = parent_node->x;
		parent_y = parent_node->y;
    parent_z = parent_node->z;
	}


	MapSearchNode NewNode;

	// push each possible move except allowing the search to go backwards

	if( (GetMap(world, x-1, y, z) < 9)
		&& !((parent_x == x-1) && (parent_y == y) && (parent_z == z))
	  )
	{
		NewNode = MapSearchNode( x-1, y, z );
		astarsearch->AddSuccessor( NewNode );
	}

	if( (GetMap(world, x, y, z-1) < 9)
		&& !((parent_x == x) && (parent_y == y-1) && (parent_z == z))
	  )
	{
		NewNode = MapSearchNode( x, y , z-1);
		astarsearch->AddSuccessor( NewNode );
	}

	if( (GetMap(world, x+1, y, z ) < 9)
		&& !((parent_x == x+1) && (parent_y == y) && (parent_z == z))
	  )
	{
		NewNode = MapSearchNode( x+1, y, z );
		astarsearch->AddSuccessor( NewNode );
	}


	if( (GetMap(world, x, y, z+1 ) < 9)
		&& !((parent_x == x) && (parent_y == y+1) && (parent_z == z))
		)
	{
		NewNode = MapSearchNode( x, y, z+1 );
		astarsearch->AddSuccessor( NewNode );
	}

//Y Stuff
  if( (GetMap(world, x, y+1, z+1 ) < 9)
		&& !((parent_x == x) && (parent_y == y+1) && (parent_z == z+1))
		)
	{
		NewNode = MapSearchNode( x, y+1, z+1 );
		astarsearch->AddSuccessor( NewNode );
	}


  if( (GetMap(world, x, y+1, z-1 ) < 9)
    && !((parent_x == x) && (parent_y == y+1) && (parent_z == z-1))
    )
  {
    NewNode = MapSearchNode( x, y+1, z-1 );
    astarsearch->AddSuccessor( NewNode );
  }

  if( (GetMap(world, x, y-1, z+1 ) < 9)
		&& !((parent_x == x) && (parent_y == y-1) && (parent_z == z+1))
		)
	{
		NewNode = MapSearchNode( x, y-1, z+1 );
		astarsearch->AddSuccessor( NewNode );
	}


  if( (GetMap(world, x, y-1, z-1 ) < 9)
    && !((parent_x == x) && (parent_y == y-1) && (parent_z == z-1))
    )
  {
    NewNode = MapSearchNode( x, y-1, z-1 );
    astarsearch->AddSuccessor( NewNode );
  }

  if( (GetMap(world, x+1, y+1, z ) < 9)
		&& !((parent_x == x+1) && (parent_y == y) && (parent_z == z+1))
		)
	{
		NewNode = MapSearchNode( x+1, y+1, z );
		astarsearch->AddSuccessor( NewNode );
	}


  if( (GetMap(world, x+1, y-1, z ) < 9)
    && !((parent_x == x+1) && (parent_y == y) && (parent_z == z-1))
    )
  {
    NewNode = MapSearchNode( x+1, y-1, z );
    astarsearch->AddSuccessor( NewNode );
  }

  if( (GetMap(world, x-1, y+1, z ) < 9)
		&& !((parent_x == x-1) && (parent_y == y) && (parent_z == z+1))
		)
	{
		NewNode = MapSearchNode( x-1, y+1, z );
		astarsearch->AddSuccessor( NewNode );
	}


  if( (GetMap(world, x-1, y-1, z ) < 9)
    && !((parent_x == x-1) && (parent_y == y) && (parent_z == z-1))
    )
  {
    NewNode = MapSearchNode( x-1, y-1, z );
    astarsearch->AddSuccessor( NewNode );
  }

	return true;
}

// given this node, what does it cost to move to successor. In the case
// of our map the answer is the map terrain value at this node since that is
// conceptually where we're moving

float MapSearchNode::GetCost(World &world, MapSearchNode &successor )
{
	return (float) GetMap(world, x, y, z);
}
