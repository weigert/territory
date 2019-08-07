# territory
3D rendered proc-gen world test

![Example Rendering](https://octodex.github.com/images/yaktocat.png)

/*
ToDo:
- Octree Try Majority function for map-rendering / LOD functions
-> Higher Efficiency in chunk rendering, by combining vertices and occluded blocks!
-> Higher efficiency in chunk loading, by not reloading everything all the time!
-> Somehow merge chunk files into region files
-> Set the player's sprite to sit in top of the chunk (i.e. player movement)
-> Add a SUB-LOD so that we can have more than single block detail
-> Proper Implementation of LOD so we can zoom out and view the chunk map
- Keep walk orientation after rotating world
- Walking and rotations need to be continuous actions so that you can have animation support
-> "Drop Shadow" effect for the character sprites
-> See if it is possible to draw an outline for the mesh somehow also in post-processing
-> Player proper movement constraints

Renderer:
- World Fog
- Bloom (Lighting Blocks)
- Blur (Characters, DOF)
- Grain (Surfaces)
- Particles
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
