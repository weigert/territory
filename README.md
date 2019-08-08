# territory
3D rendered proc-gen world test

## Controls
- Change Position: WASD
- Change View: Arrow Keys
- Zoom: Scroll

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
- Chunks saved to file, loaded dynamically when entering regions (change position with WASD)
- Recursive Octree Data Structure for Chunks
- Octree test for majority contents for LOD handling
- Painting Functions (e.g. "fillVolume") implemented for easy chunk data manipulation

## ToDo:
### Renderer
- Drop-Shadow for sprites
- Sprite animation support
- Higher Efficiency in Chunk Rendering (don't regenerate whole models when moving around)
- Sprite Outlines and Block Outlines (-> picking)
- Shader Effects: Fog, Bloom, Blur, Grain, Particles

### Game
- Proper sprite movement constraints
- Move sprite vertically when possible
- Merge Chunk-Files into more compact region files
- Higher Efficiency in Chunk Loading (don't reload everything all the time)

### Event Handler
- Better Handling of Keyboard Inputs (no key-interrupts)
- Simultaneous Action Handling (kinda done, buggy)
- Multi-tick input consequences (i.e. slow moving)

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
