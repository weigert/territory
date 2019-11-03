/*
-> Function to Generate Bezier Coefficients (as vectors)
-> Function to quickly evaluate polynomials
-> Function to to convert HSV to RGB and vice versa

Can we discuss why one thing or the other is faster?



*/

/*
  High order curves lack numerical stability because
  of the evaluation of the inverse of a factorial - this can become very large
  and thereby give you inaccurate results.
  These ratios of factorials are not good. It provides an in-between step that moves over
  vastly different orders of magnitude, which causes errors in the machine precision.
*/

namespace color{

  std::vector<glm::vec4> skycolors{
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0),
    glm::vec4(0.93, 0.91, 0.38, 1.0)
  };

  //Get some Sand-Colors for the Bezier Curve
  std::vector<glm::vec4> sandcolors{
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0),
    glm::vec4(0.93, 0.91, 0.38, 1.0)
  };

  std::vector<glm::vec4> grasscolors{
    glm::vec4(0.65, 0.72, 0.34, 1.0),
    glm::vec4(0.19, 0.45, 0.34, 1.0),
    glm::vec4(0.54, 0.70, 0.34, 1.0)
  };

  std::vector<glm::vec4> leafcolors{
    glm::vec4(0.17, 0.40, 0.26, 0.8),
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0)
  };

  std::vector<glm::vec4> claycolors{
    glm::vec4(0.97f, 0.5f, 0.44f, 1.0f),
    glm::vec4(0.75, 0.30, 0.30, 1.0),
  //  glm::vec4(0.9, 0.65, 0.28, 1.0)
  };

  //Multiply Color by Constant Factor
  glm::vec4 operator*(const double &d, const glm::vec4& vec) {
    return glm::vec4(d)*vec;
  }

  //Use the Position Hash (Generates Hash from Index)
  std::hash<unsigned int> position_hash;
  double hashrand(unsigned int i){
    return (double)(position_hash(i)%1000)/1000.0;
  }

  //De-Casteljau Algorithm (Templated)
  template <typename T>
  T bezier(double t, std::vector<T> points){
    //Recursively do Linear Interpolations
    while(points.size() > 1){
      std::vector<T> next;
      for(int i = 0; i < points.size()-1; i++)
        next.push_back( (1-t)*points[i] + t*points[i+1] );
      points = next;
    }
    return points[0];
  }

  /*
  Bezier Takes as its input a number, which I currently generate randomly.
  This number should actually be hashed, not random.
  */

  glm::vec4 getColor(BlockType _type, double t){
    //Switch the value and return a vector
    switch(_type){
      case BLOCK_GRASS:
        return bezier(t, grasscolors);
        break;
      case BLOCK_DIRT:
        return glm::vec4(0.74f, 0.5f, 0.36f, 1.0f);
        break;
      case BLOCK_WATER:
        return glm::vec4(0.3f, 0.57f, 0.67f, 1.0f);
        break;
      case BLOCK_SAND:
        return bezier(t, sandcolors);
        break;
      case BLOCK_CLAY:
        return bezier(t, claycolors);
        break;
      case BLOCK_STONE:
        return glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        break;
      case BLOCK_LEAVES:
        return bezier(t, leafcolors);
        break;
      case BLOCK_WOOD:
        return glm::vec4(0.6f, 0.375f, 0.14f, 1.0f);
        break;
      case BLOCK_GRAVEL:
        return glm::vec4(0.08f, 0.32f, 0.43f, 1.0f);
        break;
      case BLOCK_SANDSTONE:
        return glm::vec4(0.8f, 0.75f, 0.64f, 1.0f);
        break;
      case BLOCK_PUMPKIN:
        return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
        break;
      case BLOCK_CACTUS:
        return glm::vec4(0.0f, 0.44f, 0.3f, 1.0f);
        break;
      case BLOCK_PLANKS:
        return glm::vec4(0.75f, 0.6f, 0.28f, 1.0f);
        break;
      case BLOCK_GLASS:
        return glm::vec4(0.8f, 0.9f, 0.95f, 0.2f);
        break;
      default:
        return glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
        break;
    }
  }
}
