# territory
3D rendered proc-gen world test
=======
## Usage
    ./territory [world_name]
If the world name doesn't exist, it will generate a world. If it does, it will load the world. Tested on Ubuntu 18

### Controls
		- Change Camera Position: WASD
		- Move Camera Up / Down: Space / LShift
		- Change View Angle: Scroll Left / Right
		- Zoom: Scroll Up / Down
		- Manipulate the Simulation through the control panel

### Panel
	- Simulation: Control the Simulation Speed
- Population
	- Observe individual bots
		- Follow Bots
		- Check their Task Queue
		- Check the Memory
	- Interrupt Bots
	- Kill Bots
- View
	- Change to perspective projection
	- FPS Counter
	- Change Skybox Color
	- Change Lighting Color

![Example Rendering](https://github.com/weigert/territory/blob/master/territory.png)

## Functionality
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
- Geology: Ore Deposits, Stone Types, Solubility / Brittleness, etc / Soil Quality
- Maps: Height, Humidity, Temperature, Volcanism, Drainage, Geology
- Rock: Mountains / Volcanoes / Boulders / Caves / Canyons / Geysers / etc.
- Water: Rivers / Streams / (Hot) Springs / Deltas / Swamps / Islands / Waterfalls
- Biomes: Deserts, Forests, Rainforests, Plains, etc.
- Dynamic Erosion and Flooding
- Dynamic Weather System at chunk resolution(Clouds, Rain, Snow, Hail, Temp, Wind, Lightning, Sunlight)
- Vegetation for soil / water retention, enrichment, climate dependency, cooling
- Dynamic Range of Vegetation, where each plant is a point in some space, then sample near where the state is find nearest plant, place it
