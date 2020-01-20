/*
Graph Helper Namespace
*/

enum roomtype{
  ROOM_OUTSIDE,
  ROOM_ROOM
};

namespace graph{

  //Different Types of Graphs for Templating
  enum type{
    ROOMGRAPH,
    ROADGRAPH,
    GROUPGRAPH
  };

/*
================================================================================
                              GRAPH DATA-STRUCTURES
================================================================================
*/

  //Create a Graph Node Struct
  template <type G> struct Node{
    int ID;
    Node<G>(int _ID){ ID = _ID; }
  };

  //Comparison Operator
  template <type G> bool operator < (const Node<G>& lhs, const Node<G>& rhs){
    return lhs.ID < rhs.ID;
  }

  //Create an Edge Struct
  template <type G> struct Edge{
    Node<G>* to;
  };

  //Specialize Payloads for types
  template <> struct Node<ROOMGRAPH>{
    int ID;
    Node(int _ID){ ID = _ID; }

    //Payload
    Volume volume;
    roomtype _type;
  };

  template <> struct Edge<ROADGRAPH>{
    Node<ROADGRAPH>* to;
    int distance;
  };
  
  /*
    //Specialize Payloads for types
    template <> struct Node<GROUPGRAPH>{
      int ID;
      Node(int _ID){ ID = _ID; }

      //Payload
      Group group;
    };

    template <> struct Edge<GROUPGRAPH>{
      Node<GROUPGRAPH>* to;
      grouprelationship relation;
    };
    */

  //Define a Nodecontainer
  template <type G> using nodecontainer = std::vector<Node<G>>;
  template <type G> using edgecontainer = std::map<Node<G>, std::vector<Edge<G>>>;

  //Graph Class
  template <type G> class Graph{
  public:
    //Information Storage
    nodecontainer<G> nodes;
    edgecontainer<G> edges;

    //Add Node
    void addNode(){
      //Add the Node to the Vector of Nodes
      Node<G> _node(nodes.size());
      nodes.push_back(_node);
    }

    void addEdge(Node<G> &from, Node<G> &to, bool bidirectional){
      Edge<G> _edge, _edge2;
      _edge.to = &to;
      edges[from].push_back(_edge);

      //Bidirection graph!
      if(bidirectional){
        _edge2.to = &from;
        edges[to].push_back(_edge2);
      }
    }

    //Do special stuff....
    void generate(int n);     //Graph Initialization Function
    int cost();               //Templated Cost Function
    void optimize();          //Perform an Optimization
    bool sanitycheck();       //Check the Results
  };

/*
================================================================================
                        GRAPH GENERATORS & SANITY CHECK
================================================================================
*/

//Graph Generators!
template <> void Graph<ROOMGRAPH>::generate(int n){
  //Clear the Graph!
  nodes.clear();
  edges.clear();


  //Add n nodes to graph
  for(int i = 0; i < n + 1; i++){
    addNode();
  }

  //Set the Room Types
  nodes[0]._type = ROOM_OUTSIDE;

  //Create N Volumes from the Room Types
  for(int i = 1; i < n + 1; i++){
    //New Volume
    nodes[i]._type = ROOM_ROOM;
    int size = 4+rand()%5;
    nodes[i].volume.a = glm::vec3(0);
    nodes[i].volume.b = glm::vec3(size);
  }

  /*
  If you remove all connections from the graph, it will arrange the rooms randomly.
  But still aesthetically. Just no linking inside! Also no conditions.
  */

  //Connect the Rooms According to their Type
  for(int i = 0; i < n; i++){
    addEdge(nodes[i], nodes[i+1], true);
  }
  addEdge(nodes[1], nodes[n], true);
}

template <> bool Graph<ROOMGRAPH>::sanitycheck(){
  //Check for Volume Intersections between any Nodes
  for (auto node : nodes){
    //Check if we have volume intersections or full room detachments.
    if(node.ID == 0) continue;
    for (auto neighbor : nodes){
      if(neighbor.ID == 0) continue;
      if(node.ID == neighbor.ID) continue;
      glm::vec3 overlap = overlapSize(node.volume, neighbor.volume);

      //Check if we have any overlapping rooms! (negative is allowed)
      if(glm::all(glm::greaterThan(overlap, glm::vec3(0)))){
        //Failed Sanity Check: Overlapping Volumes
        return false;
      }
    }
  }

  //Check for Detachment from Neighboring Nodes in Graph
  for (auto [node, _edges] : edges){
    if(node.ID == 0) continue;
    for (auto edge : _edges){
      if(edge.to->ID == 0) continue;
      //Overlap between this node and all of its neighboring guys...
      glm::vec3 overlap = overlapSize(nodes[node.ID].volume, nodes[edge.to->ID].volume);

      if(glm::any(glm::lessThan(overlap, glm::vec3(0)))){
        //Failed Sanity Check: Room Detached from Neighbor
        return false;
      }

      //This guarantees that we have overlap between rooms we need to connect
      if(!(overlap.x == 0 && overlap.y > 3 && overlap.z > 2) &&
         !(overlap.y == 0 && ((overlap.z > 1 && overlap.x > 3) || (overlap.z > 3 && overlap.x > 1))) &&
         !(overlap.z == 0 && overlap.x > 2 && overlap.y > 3)){
           //Failed Sanity Check: Insufficient Overlap
           return false;
      }
    }
  }

  //Sanity Check Completed!
  return true;
}

/*
================================================================================
                              GRAPH COST FUNCTIONS
================================================================================
*/

//Another Possible Cost Function... using the graph!
template <> int Graph<ROOMGRAPH>::cost(){
  //Metric
  int metric[7] = {0};
  //int weight[7] = {2, 1, -2, -2, -1, 2, 1};
  int weight[7] = {2, 4, -2, -2, 0, 5, 1};

  //Check for Surface Contact and Non-Negative Overlap Between Connected Block!
  for (auto const &[node, _edges] : edges){
    if(node.ID == 0) continue; //Ignore the First Guy
    for (auto const &edge : _edges){
      //Overlap and Overlap Areas
      glm::vec3 overlap = overlapSize(node.volume, edge.to->volume);

      //Compute the Overlap Surfaces (actually, both need to be positive for there to be overlap!)
      if(overlap.y == 0 && overlap.x > 0 && overlap.z > 0) metric[2] += overlap.x*overlap.z;  //Vertical Surface Overlap
      if(overlap.x == 0 && overlap.y > 0 && overlap.z > 0) metric[3] += overlap.z*overlap.y;  //Horizontal Surface Overlap
      if(overlap.z == 0 && overlap.x > 0 && overlap.y > 0) metric[3] += overlap.x*overlap.y;  //Horizontal Surface Overlap
    }
  }

  //Check for Volume Intersection of all Rooms!
  for (auto node : nodes){
    if(node.ID == 0) continue;
    for (auto neighbor : nodes){
      if(neighbor.ID == 0) continue;
      if(node.ID == neighbor.ID) continue;
      glm::vec3 overlap = overlapSize(node.volume, neighbor.volume);
      glm::vec3 posOverlap = glm::clamp(overlap, glm::vec3(0), overlap);
      glm::vec3 negOverlap = glm::clamp(overlap, overlap, glm::vec3(0));
      metric[0] += abs(posOverlap.x*posOverlap.y*posOverlap.z); //Positive Surface Overlap
      metric[1] += abs(negOverlap.x*negOverlap.y*negOverlap.z); //Negative Surface Overlap
    }

    //Surface Area Touching Floor
    if(node.volume.a.y == 0) metric[4] += node.volume.a.x*node.volume.a.z;

    //Check if we have volume below the floor!
    if(node.volume.a.y < 0){
      //Check if the whole volume is below the surface
      if(node.volume.b.y < 0){
        metric[5] += abs(node.volume.getVol());
      }
      else{
        metric[5] += abs(node.volume.a.y)*(node.volume.b.z-node.volume.a.z)*(node.volume.b.x-node.volume.a.x);
      }
    }

    //Distance from Floor
    metric[6] = abs(node.volume.a.y);
  }

  return metric[0]*weight[0] + metric[1]*weight[1] + metric[2]*weight[2] + metric[3]*weight[3] + metric[4]*weight[4] + metric[5]*weight[5] + metric[6]*weight[6];
}

/*
================================================================================
                              GRAPH OPTIMIZERS
================================================================================
*/

template <> void Graph<ROOMGRAPH>::optimize(){
  //Possible set of transformations
  std::array<glm::vec3, 7> moves = {
    glm::vec3( 0, 0, 0),  //No move also a possibility!
    glm::vec3( 1, 0, 0),
    glm::vec3(-1, 0, 0),
    glm::vec3( 0, 1, 0),
    glm::vec3( 0,-1, 0),
    glm::vec3( 0, 0, 1),
    glm::vec3( 0, 0,-1)
  };

  //Computational Temperature
  float T = 250;

  while(T > 0.1){
    //Probability of Transformation Array
    std::vector<std::array<double, moves.size()>> probabilities;
    std::array<double, moves.size()> probability;
    int curEnergy = cost();

    //Perform all possible transformations and compute the energy...
    for(int i = 1; i < nodes.size(); i++){
      //Loop over the possible moveset, compute the probability of that transformation
      for(unsigned int m = 0; m < moves.size(); m++){
        //Translate the Room by the move and compute the energy of the new configuration.
        nodes[i].volume.translate(moves[m]);

        //Compute the Energy Difference to the current Energy!
        probability[m] = exp(-(double)(cost() - curEnergy)/T);

        //Translate it Back
        nodes[i].volume.translate(-moves[m]);
      }

      //Add the Guy to the vector
      probabilities.push_back(probability);
    }

    //Sum of States (Normalization Factor)
    float Z = 0;
    for(unsigned int i = 0; i < probabilities.size(); i++){
      for(unsigned int j = 0; j < probabilities[i].size(); j++){
        Z += probabilities[i][j];
      }
    }

    //Normalize the Probabilities
    for(unsigned int i = 0; i < probabilities.size(); i++){
      for(unsigned int j = 0; j < probabilities[i].size(); j++){
        probabilities[i][j] /= Z;
      }
    }

    //Compute the Cumulative Density Function
    std::vector<double> cdf;
    for(unsigned int i = 0; i < probabilities.size(); i++){
      for(unsigned int j = 0; j < probabilities[i].size(); j++){
        if(cdf.empty()) cdf.push_back(probabilities[i][j]);
        else cdf.push_back(probabilities[i][j] + cdf.back());
      }
    }

    //Sample from the CDF
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);
    double uniform = dist(e2);
    int sampled_index = 0;

    //Sample
    for(unsigned int i = 0; i < cdf.size(); i++){
      //If we surpass the value of uniform, we have found the guy...
      if(cdf[i] > uniform){
        sampled_index = i;
        break;
      }
    }

    //Determine Sampled Room and Move
    int _roomindex = sampled_index/moves.size();
    int _moveindex = sampled_index%moves.size();

    //Perform the Move
    nodes[_roomindex+1].volume.translate(moves[_moveindex]);

    T *= 0.95;
    //Repeat!!!
  }
}

//End of Namespace
}
