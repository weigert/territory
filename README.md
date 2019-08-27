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
- World Fog
- Bloom (Lighting Blocks)
- Blur (Characters, DOF)
- Grain (Surfaces)
- Particles
- Texture Shadows (i.e. sprites)
- Outlines and Picking (e.g. characters)

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
=======
## Usage
    ./territory [world_name]
If the world name doesn't exist, it will generate a world. If it does, it will load the world. Tested on Ubuntu 18

## Controls
- Change Position: WASD
- Move Character Up / Down: Space / LShift
- Change View Angle: Scroll Left / Right
- Zoom: Scroll Up / Down
- Change LOD: Zoom Out / In

![Example Rendering](https://github.com/weigert/territory/blob/master/territory.png)

## Functionality
### Renderer
- Generation of Voxel Maps based on chunk data
- Proper ignoring of occluded faces for faster VBO (model) construction
- World-Size: 16^3 block-sized chunks, a (10x10x1) chunks ~3.5MB
- Normal Shading
- Shadow Mapping and Transparency
- True 3D Rendering (change angle with arrow keys)
- 2D Camera-Facing Sprites
- Can handle arbitrary LOD for the underlying octree

### World Generator
- Arbitrary Worldsizes
- Chunks saved to file, loaded dynamically when entering regions (change position with WASD), loaded efficiently
- Recursive Octree Data Structure for Chunks, high sparsity of data structure, very compact
- Octree test for majority contents for LOD handling
- Fast macro-world editing using an editBuffer, which sorts changes by their order in file and writes them very quickly
  -> Also tackles the constraint of "chunk-boundaries" and their editing seamlessly for continous objects in world-space
- Simple Perlin Height Generator (for now)

## ToDo:
### Renderer
- Drop-Shadow for sprites
- Sprite animation support
- Sprite Outlines and Block Outlines (-> picking)
- Shader Effects: Fog, Bloom, Blur, Grain, Particles

### Game
- Proper sprite movement constraints

### Event Handler
- Better Handling of Keyboard Inputs (no key-interrupts)
- Simultaneous Action Handling (kinda done, buggy)
- Multi-tick input consequences (i.e. slow moving and animations)

### World Generator
- Geology: Ore Deposits, Stone Types, Solubility / Brittleness, etc / Soil Quality
- Maps: Height, Humidity, Temperature, Volcanism, Drainage, Geology
- Rock: Mountains / Volcanoes / Boulders / Caves / Canyons / Geysers / etc.
- Water: Rivers / Streams / (Hot) Springs / Deltas / Swamps / Islands / Waterfalls
- Biomes: Deserts, Forests, Rainforests, Plains, etc.
- Dynamic Erosion and Flooding
- Dynamic Weather System at chunk resolution(Clouds, Rain, Snow, Hail, Temp, Wind, Lightning, Sunlight)
- Vegetation for soil / water retention, enrichment, climate dependency, cooling
- Dynamic Range of Vegetation, where each plant is a point in some space, then sample near where the state is find nearest plant, place it
