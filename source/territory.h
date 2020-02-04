enum BiomeType{
  BIOME_VOID = 0,
  BIOME_DESERT = 1,
  BIOME_FOREST = 2,
};

//Dear IMgui Dependencies
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_sdl.h"
#include "include/imgui/imgui_impl_opengl3.h"

//Drawing Dependencies
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

//Containers, Math, Other
#include <stdlib.h>
#include <string>
#include <noise/noise.h>
#include <vector>
#include <deque>
#include <random>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>

//File / General IO
#include <iostream>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

//Logger

//Helper Functions
#include "include/helpers/logger.h"
#include "include/helpers/ease.h"
#include "include/helpers/arraymath.h"
#include "include/helpers/color.h"
#include "include/helpers/volume.h"
#include "include/helpers/graph.h"
#include "include/helpers/timer.h"
#include "include/world/block.h"

//Pathfinding Classes
#include "include/astar/sltastar.h"
#include "include/astar/astar.h"

//World Classes
#include "include/world/blueprint.cpp"
#include "include/world/octree.cpp"
#include "include/world/chunk.cpp"
#include "include/world/world.cpp"

//View Classes
#include "include/render/interface.cpp"
#include "include/render/billboard.cpp"
#include "include/render/view.cpp"
#include "include/render/model.cpp"
#include "include/render/sprite.cpp"
#include "include/render/shader.cpp"
#include "include/render/audio.cpp"

//Game Classes
#include "include/game/event.cpp"
#include "include/game/item.cpp"

//NPC Classes
#include "include/taskbot/group.cpp"
#include "include/taskbot/state.cpp"
#include "include/taskbot/bot.cpp"
#include "include/taskbot/population.cpp"
#include "include/taskbot/task.cpp"
