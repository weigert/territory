/*
Hexelrender Header File
-> Loads Dependendencies
-> Avoids Cyclical Dependencies
*/

#define __PATH__ "/home/nick/Documents/Game Workspace/23_HexelRender/"

//Regular Dependencies
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <noise/noise.h>
#include <fstream>
#include <math.h>
#include <sstream>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include <deque>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

//Include Any Dependencies Here
#include "include/octree.cpp"
#include "include/shader.cpp"
#include "include/model.cpp"
#include "include/chunk.cpp"
#include "include/world.cpp"
#include "include/sprite.cpp"
#include "include/view.cpp"
#include "include/events.cpp"
