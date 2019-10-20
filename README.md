# territory
3D rendered proc-gen world test

/*
Todo:
- Add run-length-encoding for flat-array
- Multithread Saving / Loading / Population Updates / Rendering
- Possibly convert to octree for saving, after multithreading
- Reduce flat array size by using bytes?
- Reimplement LOD for flat-array

Renderer:
- Bloom (Lighting Blocks)
- Particles
- Texture Shadows (i.e. sprites)

//3.5 MB for a HIGH-DETAIL (10*16)^2 * 10 World

World Generator:
Features:
	-> Geology: Ore Deposits, Stone Types, Solubility / Brittleness, etc / Soil Quality
	-> Maps: Height, Humidity, Temperature, Volcanism, Drainage, Geology
	-> Rock: Mountains / Volcanoes / Boulders / Caves / Canyons / Geysers / etc.
	-> Water: Rivers / Streams / (Hot) Springs / Deltas / Swamps / Islands / Waterfalls
	-> Biomes: Deserts, Forests, Rainforests, Plains
	-> Dynamic Erosion and Flooding
	-> Dynamic Weather System (Clouds, Rain, Snow, Hail, Temp, Wind, Lightning, Sunlight)
	-> Vegetation for soil / water retention, enrichment, climate dependency, cooling
	-> Dynamic Range of Vegetation, where each plant is a point in some space, then sample near where the state is find nearest plant, place it

DONE:
- Animation Pipeline
- Keep rotation when loading new chunks
- Shadow Mapping!!!!
- Save World File Structure / File Folder Structure
- World generates and saves to file
- World loads chunks around a player
- Arbitrary world sizes!
-> PLAYER POSITION AND CHUNK LOADING
- Chunks have a position in chunk-space
- Some chunks are loaded, some are not, and which ones are depends on the player position
- If chunks are loaded, they are simply rendered at appropriate position relative to the player
- World can save and load chunks into an array
- Chunks are rendered as single objects
- Chunks have normal maps and we can compute simple shading
- Colors for chunk faces are loaded dynamically
- Setup Basic Shadow Mapping and Normal Mapping / Ambient Lighting, etc.
- Chunks should construct a VBO, so that we render chunks, not individual cubes.
- Pick the block color based on the ID value in the chunk data.
*/

## Usage
    ./territory [world_name]
If the world name doesn't exist, it will generate a world. If it does, it will load the world. Tested on Ubuntu 18

### Controls
Mouse and Keyboard Inputs:

	- Change Camera Position: WASD
	- Move Camera Up / Down: Space / LShift
	- Change View Angle: Scroll Left / Right
	- Zoom: Scroll Up / Down

### Panel
In-simulation control panel allows for manipulating the simulation:

	- Tab: Simulation: Control the Simulation Speed
	- Tab: Population
		- Observe individual bots
			- Follow Bots
			- Check their Task Queue
			- Check the Memory
		- Interrupt Bots
		- Kill Bots
	- Tab: View
		- Change to perspective projection
		- FPS Counter
		- Change Skybox Color
		- Change Lighting Color

![Example Rendering](https://github.com/weigert/territory/blob/master/resource/test.png)
![Control Panel](https://github.com/weigert/territory/blob/master/resource/test2.png)

## Source
### Reading

	- Dependencies: Read *source/territory.h* and the makefile
	- Structure: Read *source/territory.cpp*
	
I also make heavy usage of forward declarations to avoid cyclical dependencies in this project. To understand how the forward declaration structure works, read:

	- Forward Declarations: *source/include/forward/structure.md*
	
You may not be interested in all aspects of this project. In that case, reading certain portions of the source code can give you insight into the system.

	- Rendering Engine: Read *source/include/render/view.h*
	- Taskbot System: Read *source/include/taskbot/population.h*
	- World Generation / Storage: Read *source/include/world/world.h*
	- 3D Pathfinding: Read *source/include/taskbot/task.cpp* (bottom) and *source/include/astar/**

## Features (including planned)
### General
**To-Do:**
- Better Input Handling for simultaneous and multi-tick inputs

### Renderer
**Done:**
- Generation of voxel maps based on chunk data
- Full 3D rendering of scene
- 2D Camera-Facing Sprites with animation support
- Normal Shading, Shadow Mapping and Transparency
- Full Dear ImGui Control Panel for simulation
- Renderdistance based viewing of entities and chunks
- FPS Calculator

**To-Do:**
- Drop-Shadow for sprites
- Sprite and Block Outlines (-> picking)
- Shader Effects: Fog, Bloom, Blur, Grain, Particles

### Population
**Done:**
- Can robustly execute arbitrary tasks in a hierarchical manner
- Can respond to interrupt events
- Can 3D Pathfind using A* in the world and walk around autonomously
- Can have animations associated with their tasks, which they must complete before executing the next task

**To-Do**:
- Expand task library with more basic behavior elements.
	- Tasks for: Skills/Crafts, Economy, Resources, Inventory, Fighting, Movement, Mandates, Communication, Memory, etc.
- Implement a group structure with land-ownership rights
- Add items for interaction, as well as storage of tasks and memories inside items

### World Generator
**Done:**
- Arbitrary Worldsizes, Chunks saved to file, loaded efficiently based on position and renderdistance
- Fast Macro-World editing using an editBuffer, for single-savefile-access changing the world map.
	- Also tackles "chunk-boundary" constraint seamlessly
- Deprecated Octree Datastructure (might be used later again)
	- High sparsity of data structure, very compact for savefiles!
	- Test-Majority function for nodes for LOD Handling
- A few very simple world generators, including a flat one and a perlin heightmap one.

**To-Do:**
- Multithread chunk saving and loading
- Add realism to the height map generation
- Add run-length-encoding for the flat array in the world's virtual memory of currently visible chunks
